#include "stdafx.h"
#include "DBLib/dbLib.h"
#include "zlib/zlib.h"
#include "Common/MemGuard.h"
#include "base/WorkQueue.h"
#include "Common/PacketType.h"
#include "WinTCP/IPacket.h"
#include "WinTCP/dtServerSocket.h"
#include "WorldServer.h"
#include "EventProcess.h"
#include "PlayerMgr.h"
#include "ServerMgr.h"
#include "SavePlayerJob.h"
#include "Common/TeamBase.h"
#include "BanList.h"
#include "SocialManager.h"

#include "LockCommon.h"
#include "Base/Log.h"
#include "Common/DumpHelper.h"
#include "AccountHandler.h"
#include "ChatServer/PlayerMgr.h"
#include "ChatHelper.h"
#include "DBLayer/Data/TBLAccount.h"
#include "DBLayer/Data/TBLWorld.h"
#include "DBLayer/Data/TBLMailList.h"
#include "Common/ZoneSetting.h"
#include "DBLayer\Data\TBLZoneSetting.h"
#include "QuickLz/quicklz.h"
#include "Common/CommonClient.h"
#include "Common/YYDef.h"
#include "DBLayer/Data/TBLPlayer.h"
#include "DBContext.h"
#include "CommLib/PacketFactoryManager.h"
#include "Common/BonusBase.h"
#include "Common/RALibDef.h"
#include "common/cfgbuilder.h"
#include "Common/BanInfoData.h"
#include "Common/CollectItemData.h"
//#include "Common/ItemData.h"
#include "Common/OrgDepotData.h"
#include <algorithm>
#include "DB_SetAccountStatus.h"
#include "DB_CheckOldPlayerLogin.h"
#include "Script/lua_tinker.h"
#include "Common/UtilString.h"
#include "DB_PlayerPurchase .h"
#include "DB_ReNameAccount.h"
#include "Script/lua_tinker.h"
//IdAccountMap g_loginPending;
#include "Room/ChessTable.h"
#include "DBLayer/Data/TBLAccount.h"
#include "Common/FilterOpt.h"
#include "Logic/ItemMgr.h"

using namespace std;

extern CMyLog g_gmLog;

//typedef bool (*NetEventFunction)(int ,stPacketHead *,Base::BitStream *);
//NetEventFunction NetFNList[END_NET_MESSAGE];

//��ز���
static hash_map<U32, U32> g_NetFNListTotalTick;
static hash_map<U32, U32> g_NetFNListTotalFreq;

//��Ӫһϵ�л

#define OLD_PLAYER_BONUS				 SERVER->GetServerManager()->GetWorldSetting().reserve[0]
#define VIP_NETBAR_ACTIVITY				 SERVER->GetServerManager()->GetWorldSetting().reserve[1]
#define VIP_ACCOUNT						 SERVER->GetServerManager()->GetWorldSetting().reserve[2]
#define OLD_PLAYER_AND_VIP_ACCOUNT		 SERVER->GetServerManager()->GetWorldSetting().reserve[3]
#define LOGIN_COUNT_ACTIVITY			 SERVER->GetServerManager()->GetWorldSetting().ActivityFlag
#define OPEN_ACTIVITY 1
#define CLOSE_ACTIVITY 0
	 
bool _isNeedRemote()
{
	if(OPEN_ACTIVITY==VIP_NETBAR_ACTIVITY || OPEN_ACTIVITY==VIP_ACCOUNT  \
		|| OPEN_ACTIVITY==OLD_PLAYER_AND_VIP_ACCOUNT || OPEN_ACTIVITY==LOGIN_COUNT_ACTIVITY)
		return true;
	else
		return false;
}


AccountRef _GetAccount(int id)
{
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(id);

	if (!pAccount)
		pAccount = SERVER->GetPlayerManager()->GetAccount(id);

	return pAccount;
}

int SERVER_CLASS_NAME::EventProcess(LPVOID Param)
{
	WorkQueueItemStruct *pItem = (WorkQueueItemStruct*)Param;

	switch(pItem->opCode)
	{
	case WQ_CONNECT:
		break;
	case WQ_DISCONNECT:
        {
            try
            {
                SERVER->GetServerManager()->ReleaseServerMap(pItem->Id);
            }
            catch(...)
            {
                g_Log.WriteError("Error on connection disconnected!!!");
            }
        }
		break;
	case WQ_STARTED:
		{
			lua_tinker::dofile(L,"script/main.lua");
		}
		break;
	case WQ_SCRIPT:
		{
			lua_tinker::dostring(L,pItem->Buffer);
		}
		break;
	case WQ_LOGIC:
		{
			bool bNeedRemove = true ;
			ThreadBase* pDBHandle = (ThreadBase*)pItem->Buffer;

			_MY_TRY
			{
				U32 uret = pDBHandle->Execute(DB_CONTEXT_LOGIC,0,0,0);

				if(uret == PACKET_NOTREMOVE)
				{
					bNeedRemove = false;
				}
			}
			_MY_CATCH
			{
				g_Log.WriteError("WQ_LOGIC ִ���쳣 [%s]", typeid(*pDBHandle).name());
			}

			//������Ϣ
			if(bNeedRemove)
			{
				SAFE_REMOVE_PACKET(pDBHandle);
			}

			pItem->Buffer = 0;
		}
		break;
	case WQ_TIMER:
		{
		}
		break;
	case WQ_SOCIAL:
		{
			Base::BitStream recv(pItem->Buffer,pItem->size);
			SERVER->GetSocialManager()->ProcessEvents(pItem->Id,recv);
		}
		break;
	case WQ_NORMAL:
		{
			stPacketHead* pHead = (stPacketHead*)pItem->Buffer;
			char *pData	= (char *)(pHead) + IPacket::GetHeadSize();
			Base::BitStream RecvPacket(pData,pItem->size-IPacket::GetHeadSize());
			TRIGGER_EVENT(pItem->Id,pHead,RecvPacket);
		}
		break;
	case WQ_COMMAND:
		{
			//const char* cmd = pItem->Buffer;
			//// ִ��������
			//try
			//{
			//	executeCmd( cmd );
			//}
			//catch( SHException& e )
			//{
			//	g_Log.WriteError( "���ش����׳��ṹ���쳣[WQ_COMMAND: cmd=%s][ERRCODE=%d]",cmd, e.getCode());
			//	//g_dumper.writeDump( e.getExceptionPointer(),TINY_DUMP );
			//}
			//catch( ... )
			//{
			//	g_Log.WriteError( "���ش���δ�����쳣[WQ_COMMAND: cmd=%s]" , cmd);					
			//}
		}
		break;
	case WQ_PACKET:
		{
			stPacketHead* pHead = (stPacketHead*)pItem->Buffer;
			char *pData	= (char *)(pHead) + IPacket::GetHeadSize();
			Base::BitStream RecvPacket(pData,pItem->size-IPacket::GetHeadSize());

			Base::BitStream switchPacket(pItem->Buffer,pItem->size);
			switchPacket.setPosition(pItem->size);

			if (pHead->DestServerType == SERVICE_REMOTESERVER)
				EventFn::SwitchSendToRemote(pHead, switchPacket);
			else if (pHead->DestServerType == SERVICE_ACCOUNTSERVER)
				EventFn::SwitchSendToAccount(pHead, switchPacket);
			//else if(pHead->DestServerType == SERVICE_ZONESERVER)
				//EventFn::SwitchSendToZone(pHead, switchPacket);
			else if (pHead->DestServerType == SERVICE_CLIENT)
				EventFn::SwitchSendToClient(pHead, switchPacket);

			else if(ISVALID_EVENT(pHead->Message))
			{
				try
				{
					//ע��:catch SE�쳣����������translator
					//_set_se_translator( (_se_translator_function)&CDumpHelper::Exception );

					U64 startTick = GetTickCount64(); 
					bool rslt = !TRIGGER_EVENT(pItem->Id,pHead,RecvPacket);
					U64 endTick = GetTickCount64();

					g_NetFNListTotalTick[pHead->Message] += endTick - startTick;
					g_NetFNListTotalFreq[pHead->Message] ++;
					return rslt;
				}
				catch( SHException& e )
				{
					g_Log.WriteError( "���ش����׳��ṹ���쳣[WQ_PACKET: MSGCODE=%s][ERRCODE=%d]",
						SHOWMSG_EVENT(pHead->Message), e.getCode());
                    
                    static int maxdump = 5;

                    if (maxdump-- >= 0)
                    {
					    g_dumper.writeDump(GetCurrentThreadId(), e.getExceptionPointer(), TINY_DUMP);
                    }
				}
				catch( ... )
				{
					g_Log.WriteError( "���ش���δ�����쳣[WQ_PACKET: MSGCODE=%s]", SHOWMSG_EVENT(pHead->Message));
				}
			}
			else
			{
				U64 startTick = GetTickCount64(); 

				//�����߼��¼�
				SERVER->GetSocialManager()->HandleEvent( AccountRef(), pItem->Id, pHead, RecvPacket );
				CHAT_HELPER->HandleEvent(AccountRef(),pItem->Id, pHead, RecvPacket);
				U64 endTick = GetTickCount64();

				g_NetFNListTotalTick[pHead->Message] += endTick - startTick;
				g_NetFNListTotalFreq[pHead->Message] ++;
			}
		}
		break;
    case WQ_CONFIGMONITOR:
        {
            char* fileName = (char*)pItem->Buffer;

            //�������������ļ�
            if (0 == _stricmp(fileName,MONITOR_CONFIGFILE))
            {
                CFG_BUIILDER->Reload();
            }
			else
			{
				lua_tinker::call<void>(L, "ConFigmonitor", fileName);
			}

   //         if (0 == _stricmp(fileName,MONITOR_BANNAME) || 0 == _stricmp(fileName,MONITOR_BANWORD))
   //         {
   //             g_BanInfoRepository.read();
   //         }

   //         if (0 == _stricmp(fileName,MONITOR_COLLECTITEMDATA))
   //         {
   //             g_CollectItemRepository.read();
   //         }

   //         /*if (0 == _stricmp(fileName,MONITOR_ITEMDATA))
   //         {
   //             ITEMDATA->Reload();
   //         }*/

			//if (0 == _stricmp(fileName,MONITOR_PLAYERNAME))
			//{
			//	PlayerNameMgr::getInstance()->Reload();
			//}

			////������lua�ű�
			//if(0 == _stricmp(fileName, MONITOR_SCRIPT_MAINLUA))
			//{
			//	lua_tinker::dofile(L,fileName);
			//}

   //         //����Ǽ������ݸ��ģ���Ҫ���͵�����Ĵ����߳�����ȥ��������data
   //         if (0 == _stricmp(fileName,MONITOR_ORGDEPOTDATA))
   //         {
   //             MAKE_WORKQUEUE_PACKET(sendPacket, 64);
   //             sendPacket.writeFlag(true);
   //             SEND_WORKQUEUE_PACKET(sendPacket, WQ_ORG,OWQ_ReloadData);
   //         }

   //         if (0 == _stricmp(fileName,MONITOR_ORGBOSSDATA))
   //         {
   //             MAKE_WORKQUEUE_PACKET(sendPacket, 64);
   //             sendPacket.writeFlag(false);
   //             SEND_WORKQUEUE_PACKET(sendPacket, WQ_ORG,OWQ_ReloadData);
   //         }
        }
        break;
	}

	return false;
}

void DumpRecvPackets(void)
{
	for (hash_map<U32, U32>::iterator itr = g_NetFNListTotalFreq.begin(); itr != g_NetFNListTotalFreq.end(); ++itr)
	{
		if (0 == itr->second)
			continue;

		g_Log.WriteLog("�յ���Ϣ [%d] ����[%d] ��ʱ��[%d] ƽ��ʱ��[%d]",itr->first,itr->second,g_NetFNListTotalTick[itr->first],itr->second ? (g_NetFNListTotalTick[itr->first] / itr->second) : 0);

		itr->second = 0;
		g_NetFNListTotalTick[itr->first] = 0;
    }
}

/*int g_accountSocket = 0;*/


namespace EventFn
{
	void Initialize()
	{
		//����������Ϣ
		REGISTER_EVENT_FUNCTION("COMMON_RegisterRequest", &EventFn::HandleRegisterRequest);
		REGISTER_EVENT_FUNCTION("ACCOUNT_WORLD_ClientLoginRequest", &EventFn::HandleClientLoginAccountRequest);
		REGISTER_EVENT_FUNCTION("GATE_WORLD_ClientLost", &EventFn::HandleGateClientLost);
		REGISTER_EVENT_FUNCTION("GATE_WORLD_ACCOUNTRELINK", &EventFn::HandleGateWorldAccountRelink);

		REGISTER_EVENT_FUNCTION("GATE_WORLD_ClientLoginRequest", &EventFn::HandlePlayerLoginGate);
		REGISTER_EVENT_FUNCTION("GATE_WORLD_ClientLogoutRequest", &EventFn::HandlePlayerLogoutGate);
		REGISTER_EVENT_FUNCTION("CLIENT_WORLD_SelectPlayerRequest", &EventFn::HandleSelectPlayerRequest);
		REGISTER_EVENT_FUNCTION("CLIENT_WORLD_DeletePlayerRequest", &EventFn::HandleDeletePlayerRequest);
		REGISTER_EVENT_FUNCTION("CLIENT_WORLD_CreatePlayerRequest", &EventFn::HandleCreatePlayerRequest);
		REGISTER_EVENT_FUNCTION("CLIENT_WORLD_AutoPlayerNameRequest", &EventFn::HandleAutoPlayerNameRequest);
		REGISTER_EVENT_FUNCTION("WORLD_REMOTE_CreatePlayer", &EventFn::HandleRWCreatePlayerRespond);
		REGISTER_EVENT_FUNCTION("WORLD_REMOTE_AutoPlayerName", &EventFn::HandleRWAutoPlayerNameRespond);
		REGISTER_EVENT_FUNCTION("CLIENT_GAME_LoginRequest", &EventFn::HandleLoginGameRequest);
		REGISTER_EVENT_FUNCTION("CLIENT_GAME_LogoutRequest", &EventFn::HandleClientLogout);
		REGISTER_EVENT_FUNCTION("GATE_WORLD_ClientReLoginRequest", &EventFn::HandlePlayerGateReLogin);
		REGISTER_EVENT_FUNCTION("CLIENT_Bind_AccountResponse", &EventFn::HandlePlayerBindAccount);

		REGISTER_EVENT_FUNCTION("WORLD_REMOTE_PlayerLogin", &EventFn::HandleRWLoginPlayerResp);
		REGISTER_EVENT_FUNCTION("CW_GETLINEPLAYERCOUNT", &EventFn::HandleGetLinePlayerCount);

		REGISTER_EVENT_FUNCTION("Alipay_Charge", &EventFn::HandleAlipayCharge);
		REGISTER_EVENT_FUNCTION("RW_PURCHASE_RESPONSE", &EventFn::HandlePlayerPurchaseResponse);
		REGISTER_EVENT_FUNCTION("RW_RECHARGE_RESPONSE", &EventFn::HandleRechargeResponse);
		REGISTER_EVENT_FUNCTION("RW_RECHARGE1_RESPONSE", &EventFn::HandleRecharge1Response);
		REGISTER_EVENT_FUNCTION("RW_CODE_RESPONSE", &EventFn::HandleCodeResponse);
	}

	void SwitchSendToRemote(stPacketHead *pHead,Base::BitStream &switchPacket,int ctrlType)
	{
        g_Log.WriteLog("RA Switch message[%s]", SHOWMSG_EVENT(pHead->Message));

		SERVER->GetRemoteAccountSocket()->Send(switchPacket,ctrlType);
	}

	void SwitchSendToAccount(stPacketHead *pHead,Base::BitStream &switchPacket,int ctrlType)
	{
		int svrSocket = SERVER->GetServerManager()->GetServerSocket(SERVICE_ACCOUNTSERVER);

		if (svrSocket > 0)
        {
			SERVER->GetServerSocket()->Send(svrSocket, switchPacket,ctrlType);
        }
	}

	/*void SwitchSendToZone(stPacketHead *pHead,Base::BitStream &switchPacket,int ctrlType)
	{
		int playerId = pHead->Id;
		AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(playerId);
		if(NULL == pAccount)
			return;

		int iLineId = pAccount->GetLastLineId();
		int ZoneId = pAccount->GetLastZoneId();
		pHead->DestZoneId = ZoneId;
		int socketId = SERVER->GetServerManager()->GetGateSocket(iLineId,ZoneId);
		if(socketId > 0)
		{
			SERVER->GetServerSocket()->Send(socketId, switchPacket,ctrlType);
		}
	}*/

	void SwitchSendToClient(stPacketHead* pHead, Base::BitStream& switchPacket, int ctrlType)
	{
		AccountRef pAccount = SERVER->GetPlayerManager()->GetAccount(pHead->Id);
		if (NULL == pAccount)
			return;

		SERVER->GetServerSocket()->Send(pAccount->GetGateSocketId(), switchPacket, ctrlType);
	}

	bool HandleRegisterRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
        DO_LOCK(Lock::Server);

		stServerInfo ServerInfo;
		ServerInfo.SocketId = SocketHandle;
		ServerInfo.Type		= Packet->readInt(Base::Bit8);
		ServerInfo.LineId   = Packet->readInt(Base::Bit8);
		ServerInfo.GateId	= Packet->readInt(Base::Bit32);
		int ConnectTimes	= Packet->readInt(Base::Bit32);
        
		int IP = 0,Port = 0;

		if(Packet->readFlag())
		{
			ServerInfo.Ip[0] = Packet->readInt(Base::Bit32);
			ServerInfo.Port	= Packet->readInt(Base::Bit16);
		}

		if(Packet->readFlag())
		{
			ServerInfo.Ip[1] = Packet->readInt(Base::Bit32);
		}

		SERVER->GetServerManager()->AddServerMap(ServerInfo);

        //ע������
        if (SERVICE_CHATSERVER == ServerInfo.Type)
        {
            SERVER->GetPlayerManager()->RegisterChat();

            MAKE_WORKQUEUE_PACKET(sendPacket,256);
			//SEND_WORKQUEUE_PACKET(sendPacket,WQ_ORG,OWQ_RegisterChat);

            //����ע�����Ƶ��
            //SERVER->GetTeamManager()->RegisterChat();
            //SERVER->GetCorpsManager()->RegisterChannel();

			//ͬ��banlist
			//CBanlist::Instance()->SendBanListToChatServer();
        }

		CMemGuard Buffer(MAX_PACKET_SIZE MEM_GUARD_PARAM);
		Base::BitStream sendPacket(Buffer.get(),MAX_PACKET_SIZE);
		stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,"COMMON_RegisterResponse");

		switch(ServerInfo.Type)
		{
		case SERVICE_ACCOUNTSERVER:
			SERVER->SetAccountSocket(SocketHandle);
			pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(SocketHandle,sendPacket);
			break;
		case SERVICE_GATESERVER:
			//SERVER->GetServerManager()->PackZoneData(sendPacket);
			pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(SocketHandle,sendPacket);
			break;
		default:
			break;
		}

		return true;
	}

	bool HandleClientLoginAccountRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		DO_LOCK( Lock::Player | Lock::Server );

		Player AccountInfo;
        
		bool isFirstLogin	  = Packet->readFlag();
		AccountInfo.UID		  = Packet->readInt(UID_BITS);
		AccountInfo.AccountId = Packet->readInt(Base::Bit32);
		AccountInfo.isGM	  = Packet->readInt(Base::Bit8);
		AccountInfo.status    = Packet->readInt(Base::Bit32);
		//AccountInfo.loginType = (LOGIN_TYPE)Packet->readInt(Base::Bit8);

		AccountInfo.isAdult	  = Packet->readFlag();
		AccountInfo.loginTime  = Packet->readInt(32);
        AccountInfo.logoutTime = Packet->readInt(32);

		//�����ṩ��
		//AccountInfo.netProvider = Packet->readInt(Base::Bit8);

        char ip[32];
        Packet->readString(ip,sizeof(ip));
        dStrcpy(AccountInfo.loginIP,sizeof(AccountInfo.loginIP),ip);

		Packet->readString(AccountInfo.AccountName,ACCOUNT_NAME_LENGTH);

        int playerCount = Packet->readInt(32);
        Players playerList;
        
        for (int i = 0; i < playerCount; ++i)
        {
			PlayerInfo info;
			info.ReadData(Packet);
            if (0 != info.playerID)
            {
                playerList.push_back(info);
            }
        }

        g_Log.WriteLog("�յ��ʺ�[%d] name[%s]�ĵ�¼����",AccountInfo.AccountId,AccountInfo.AccountName);

		ERROR_CODE error = NONE_ERROR;

		AccountRef pAccount = SERVER->GetPlayerManager()->GetAccount(AccountInfo.AccountId);
			
		do
		{
			//ͬ�ʺŵ�½����
			if (!pAccount)
				break;

			if (pAccount->IsOffline() || pAccount->IsLogout())
				break;

			if (0 == _stricmp(pAccount->loginIP,AccountInfo.loginIP))
			{
				break;
			}

			//int remainTime = 2*60*1000;

            //g_Log.WriteLog("�ʺ�[%d]���ڵ�¼������,��¼ʧ��,ʣ��ʱ��[%d](s)",AccountInfo.AccountId,remainTime / 1000);

			//��������ʺ��ڱ𴦵�½
			/*{
				MAKE_PACKET(sendPacket, WORLD_CLIENT_NOTIFY_SAMEACCOUNTLOGIN, pAccount->AccountId, SERVICE_CLIENT);
				SEND_PACKET(sendPacket, pAccount->GetGateSocketId());
			}*/

			//������ҵȴ�һ��ʱ���½
			/*{
				MAKE_PACKET(sendPacket, "ACCOUNT_WORLD_ClientLoginResponse",isFirstLogin);
				sendPacket.writeInt(AccountInfo.UID, UID_BITS);
				sendPacket.writeInt(AccountInfo.AccountId, Base::Bit32);
				//�����ṩ��
				sendPacket.writeInt(ACCOUNT_IN_USEING, Base::Bit16);
				sendPacket.writeInt(remainTime, Base::Bit32);
				SEND_PACKET(sendPacket, SocketHandle);
			}

			if(!pAccount->GetBit(Player::EVENT_WAIT_KICKPLAYER))
			{
				pAccount->Kick();
			}

			return true;*/
		}
		while(false);
		
		//�Ŷ�ϵͳ
       /* if(!SERVER->GetPlayerManager()->HasOnlineAccount(AccountInfo.AccountId) &&
            SERVER->GetPlayerManager()->GetOnlineAccountCount() >= SERVER->GetServerManager()->GetMaxPlayerCount() &&
            !AccountInfo.IsGM())
		{
            g_Log.WriteLog("�����������Ѿ���,����[%d],�ʺ�[%d]�޷���½",SERVER->GetServerManager()->GetMaxPlayerCount(),
                AccountInfo.AccountId);

			error = GAME_WORLD_FULL;
		}*/

		int punishRemainTime = 5*60;
		
		do 
		{
			if (error != NONE_ERROR)
				break;

			error = SERVER->GetPlayerManager()->AddAccount(AccountInfo, playerList);

			if (NONE_ERROR != error)
				break;

			pAccount = SERVER->GetPlayerManager()->GetAccount(AccountInfo.AccountId);

		} 
		while (false);

		if (pAccount->status & ACCOUNT_BAND){
			error = ACCOUNT_FREEZE;
		}

        /*if (NONE_ERROR == error)
        {
            time_t curTime = time(0);

            //�жϷ������Ƿ��Ѿ�����
            if (!SERVER->GetServerManager()->IsOpenTime(curTime))
            {
                //�ж��Ƿ��������¼��ʱ����
                if (!SERVER->GetServerManager()->IsLoginTime(curTime))
                {
                    error = SERVER_NOT_OPENED;
                }
            }
        }*/

		if (NONE_ERROR != error)
		{
			char buf[64];
			Base::BitStream sendPacket(buf,sizeof(buf));
			stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,"ACCOUNT_WORLD_ClientLoginResponse",isFirstLogin,AccountInfo.isAdult);

			sendPacket.writeInt(AccountInfo.UID, UID_BITS);
			sendPacket.writeInt(AccountInfo.AccountId, Base::Bit32);
			sendPacket.writeInt(error, Base::Bit16);

			/*if (GAME_WORLD_FULL == error)
			{
				sendPacket.writeInt(AccountInfo.loginType, Base::Bit8);
				sendPacket.writeInt(AccountInfo.isAdult, Base::Bit32);
			}
            
            //�·�����������ʱ��
            if (SERVER_NOT_OPENED == error)
            {
                time_t openTime = SERVER->GetServerManager()->GetOpenTime();
                sendPacket.writeInt((int)openTime,Base::Bit32);
            }
			
			if (error == PLAYER_PUNISH)
            {
				sendPacket.writeInt(punishRemainTime, Base::Bit32);
            }*/

			SERVER->GetServerSocket()->Send(SocketHandle,sendPacket);
		}
		else
		{
			//�ѵ�ǰ��gate list�б��͸��ͻ���
			char buf[2048];
			Base::BitStream sendPacket(buf,sizeof(buf));
			stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,"ACCOUNT_WORLD_ClientLoginResponse",isFirstLogin,AccountInfo.isAdult);

			sendPacket.writeInt(AccountInfo.UID,UID_BITS);
			sendPacket.writeInt(AccountInfo.AccountId,Base::Bit32);
			sendPacket.writeInt(0,Base::Bit16);
			//for(int i = 0; i < ACCOUNTFLAG_MAX; i++)
			//	sendPacket.writeInt(AccountInfo.mAccountFlag[i], Base::Bit8);

			//�������������б������
			SERVER->GetServerManager()->PackGateData(sendPacket, 0);

			pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(SocketHandle,sendPacket);

			g_Log.WriteLog( "���ʺ�[%d]���͵�½��Ӧ",AccountInfo.AccountId);


            if (pAccount)
            {
                pAccount->SetFirstLoginFlag(true);
            }

		}

		return true;
	}

	bool HandlePlayerLogoutGate(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if(!pAccountInfo)
		{
			return false;
		}

		T_UID UID = Packet->readInt(UID_BITS);

		if (!pAccountInfo->VerifyUID(UID,SocketHandle))
			return false;

		g_Log.WriteLog("�յ��ʺ�[%d]�ǳ�gate����Ϣ",pHead->Id);

		if (Player::STATUS_IN_GAME == pAccountInfo->GetState())
		{
			pAccountInfo->UpdateLogoutTime();
		}

		pAccountInfo->SetState(Player::STATUS_LOGOUT);

		return true;
	}

	bool HandlePlayerLoginGate(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if(!pAccountInfo)
		{
			g_Log.WriteWarn("�ʺ�[%d]��¼Gate,���޷��ҵ���Ӧ���ʺ�����",pHead->Id);
			return false;
		}

        g_Log.WriteLog("�ʺ�[%d]�����¼����",pHead->Id);

		T_UID UID = Packet->readInt(UID_BITS);
		bool isLineSwitching = Packet->readFlag();

		int error = NONE_ERROR;

		if(pAccountInfo->UID != UID)
		{
			g_Log.WriteWarn("�ʺ�[%d]��¼Gate,��UID����Ӧ s:[%d] c:[%d]",pHead->Id,pAccountInfo->GetUID(),UID);
			error = GAME_UID_ERROR;
		}

		if (NONE_ERROR != error)
		{	
			return true;
		}

        int lineId = SERVER->GetServerManager()->GetLineId(SocketHandle);


		if (NONE_ERROR == error)
		{
            //���ﲻ������ҵ�½����ʱ���ڵ���,��������˳���ʱ��,����֮ǰ���ڵ���
			pAccountInfo->SetGateSocketId(SocketHandle);
			pAccountInfo->SetUID(UID);
		}

		//�������������ȴ�����
		if(pAccountInfo->GetBit(Player::EVENT_WAIT_KICKPLAYER))
		{
			g_Log.WriteLog("�ʺ�[%d]�Ѿ�������,��Ҫ�ȴ����ز��ܼ�����¼",pHead->Id);
			return true;
		}

        if (isLineSwitching)
        {
            g_Log.WriteLog("�ʺ�[%d]�����л�����[%d]",pHead->Id,lineId);

            if (!pAccountInfo->GetBit(Player::EVENT_TRANSPORT))
            {
                isLineSwitching = false;
            }
        }

		//����ʺ�����Ϸ��,������
        
        if (!isLineSwitching)
        {
            pAccountInfo->ClrBit();
        }
        
#ifdef DATA_OVER_TCP
        //���븱��ĿǰҲ��Ҫ�л�gate,��ʱ��������Ӧ��login��Ϣ
        {
            pAccountInfo->SetState(Player::STATUS_LOGIN);
        }
#else
        pAccountInfo->SetState(Player::STATUS_LOGIN);
#endif
		return true;
	}

	//��ҵ���
	bool HandleGateClientLost( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if(!pAccountInfo)
		{
			return false;
		}

		T_UID UID = Packet->readInt(UID_BITS);

		if (!pAccountInfo->VerifyUID(UID,SocketHandle))
			return false;

		if (pAccountInfo->GetGateSocketId() != SocketHandle)
		{
			return false;
		}


		//��־�ͻ�������,�������һ��ʱ��û�����µ�½,�����
		pAccountInfo->SetBit(Player::EVENT_CLIENTLOST,true);
		pAccountInfo->ResetLostTimer();

		//�ж����Ƿ�����л�,���������������Ҵ�gate��ʧ������������?
		if (pAccountInfo->GetBit(Player::EVENT_TRANSPORT))
			return false;

		SERVER->GetPlayerManager()->DelOnlineAccount(pHead->Id);
		return true;
	}
    bool HandleGateWorldAccountRelink( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
    {
		DO_LOCK(Lock::Player);
		T_UID UID = Packet->readInt(UID_BITS);
		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);
		if(!pAccountInfo)
		{
			int AccountId = pHead->Id;
			TBLAccount::stAccount accountInfo;
			TBLAccount tbl_account(SERVER->GetActorDB());
			DBError err = tbl_account.Load(AccountId, &accountInfo);
			if (err != DBERR_NONE)
				return false;

			Player AccountInfo;
			AccountInfo.AccountId = AccountId;
			AccountInfo.isGM = accountInfo.GMFlag;
			AccountInfo.status = accountInfo.Status;
			AccountInfo.isAdult = accountInfo.IsAdult;
			AccountInfo.loginTime = accountInfo.LoginTime;
			AccountInfo.logoutTime = accountInfo.LogoutTime;
			dStrcpy(AccountInfo.loginIP, sizeof(accountInfo.LoginIP), accountInfo.LoginIP);
			dStrcpy(AccountInfo.AccountName, sizeof(accountInfo.AccountName), accountInfo.AccountName);

			Players playerList;
			err = tbl_account.LoadPlayerList(accountInfo.AccountID, playerList);
			if (err != DBERR_NONE)
				return false;

			SERVER->GetPlayerManager()->AddAccount(AccountInfo, playerList);
			pAccountInfo = SERVER->GetPlayerManager()->GetAccount(AccountId);
			if (!pAccountInfo)
			{
				return false;
			}

			pAccountInfo->SetGateSocketId(SocketHandle);
			SERVER->GetPlayerManager()->AddOnlineAccount(AccountId);
			pAccountInfo->ClrBit();
			pAccountInfo->SetState(Player::STATUS_IN_SELECT);
			pAccountInfo->SendPlayerList();
			return true;
		}

		if (!pAccountInfo->VerifyUID(UID,SocketHandle))
            return false;

        pAccountInfo->SetGateSocketId(SocketHandle);
        return true;
    }
    
	bool HandleSelectPlayerRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if(!pAccountInfo)
		{
			return false;
		}

		T_UID UID = Packet->readInt(UID_BITS);

		if (!pAccountInfo->VerifyUID(UID,SocketHandle))
            return false;
		
		pAccountInfo->SetState(Player::STATUS_IN_SELECT);
		pAccountInfo->SendPlayerList();
		return true;
	}

	bool HandleDeletePlayerRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if	(!pAccountInfo)
			return false;

		T_UID UID = Packet->readInt(UID_BITS);

		if (!pAccountInfo->VerifyUID(UID,SocketHandle))
            return false;

		pAccountInfo->DeletePlayer(Packet->readInt(Base::Bit32));

		return true;
	}
	
	bool HandleCreatePlayerRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if (!pAccountInfo)
		{
			return false;
		}

		if(pAccountInfo->PlayerNum >= MAX_CREATE_ACTOR_COUNT)
			return false;

		//pAccountInfo->m_createInfo.ReadData(Packet);
		pAccountInfo->m_createInfo.m_CreateUID = Packet->readInt(Base::Bit32);
		Packet->readString(pAccountInfo->m_createInfo.m_CreateName, sizeof(pAccountInfo->m_createInfo.m_CreateName));
		pAccountInfo->m_createInfo.m_Family = Packet->readInt(Base::Bit32);
		pAccountInfo->m_createInfo.m_CreateFace = Packet->readInt(Base::Bit32);

		char* tempName = Util::Utf8ToMbcs(pAccountInfo->m_createInfo.m_CreateName);
		memset(pAccountInfo->m_createInfo.m_CreateName, 0, sizeof(pAccountInfo->m_createInfo.m_CreateName));
		dMemcpy(pAccountInfo->m_createInfo.m_CreateName, tempName, strlen(tempName));
		delete tempName;

		if (FilterOpt::isBanName(pAccountInfo->m_createInfo.m_CreateName))
		{
			char buf[64];
			Base::BitStream sendPacket(buf, sizeof(buf));
			stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "CLIENT_WORLD_CreatePlayerResponse", pHead->Id, SERVICE_CLIENT);
			sendPacket.writeInt(79, Base::Bit16);
			pSendHead->PacketSize = sendPacket.getPosition() - IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(pAccountInfo->GetGateSocketId(), sendPacket);
			return false;
		}

		if (!pAccountInfo->VerifyUID(pAccountInfo->m_createInfo.m_CreateUID, SocketHandle))
            return false;

		{
			//��������Ϣ
			char buf[1024];
			Base::BitStream sendPacket(buf, sizeof(buf));
			stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WORLD_REMOTE_CreatePlayer", pAccountInfo->GetAccountId());
			pSendHead->DestZoneId = SERVER->currentAreaId;
			pAccountInfo->m_createInfo.WriteData(&sendPacket);
			pSendHead->PacketSize = sendPacket.getPosition() - IPacket::GetHeadSize();

			SERVER->GetRemoteAccountSocket()->Send(sendPacket);

			pAccountInfo->SetBit(Player::EVENT_WAIT_CREATEPLAYER, true);
			pAccountInfo->SetBit(Player::EVENT_WAIT_ENTERGAME, false);
		}

		return true;
	}

	bool HandleAutoPlayerNameRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if (!pAccountInfo)
		{
			return false;
		}

		{
			//��������Ϣ
			char buf[256];
			Base::BitStream sendPacket(buf, sizeof(buf));
			stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WORLD_REMOTE_AutoPlayerName", pAccountInfo->GetAccountId());
			const char* playerName = "";//PlayerNameMgr::GetRandomName();
			pSendHead->DestZoneId = SERVER->currentAreaId;
			sendPacket.writeString(playerName, COMMON_STRING_LENGTH);
			pSendHead->PacketSize = sendPacket.getPosition() - IPacket::GetHeadSize();

			SERVER->GetRemoteAccountSocket()->Send(sendPacket);
		}

		return true;
	}

	bool HandleRWAutoPlayerNameRespond(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if (!pAccountInfo)
		{
			return false;
		}

		int error = NONE_ERROR;

		if (0 != pHead->DestZoneId)
		{
			error = PLAYER_NAME_ERROR;
		}

		if (NONE_ERROR == error)
		{
			char tempName[COMMON_STRING_LENGTH] = "";
			Packet->readString(tempName, COMMON_STRING_LENGTH);
			char* u8Name = Util::MbcsToUtf8(tempName);

			char buf[256];
			Base::BitStream sendPacket(buf,sizeof(buf));
			stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "CLIENT_WORLD_AutoPlayerNameResponse", pHead->Id, SERVICE_CLIENT);
			
			sendPacket.writeString(u8Name, COMMON_STRING_LENGTH);
			pSendHead->PacketSize = sendPacket.getPosition() - IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(pAccountInfo->GetGateSocketId(), sendPacket);
			delete[] u8Name;
		}

		if (error != NONE_ERROR)
		{
			//�Զ�����
			{
				//��������Ϣ
				char buf[256];
				Base::BitStream sendPacket(buf, sizeof(buf));
				stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WORLD_REMOTE_AutoPlayerName", pAccountInfo->GetAccountId());
				const char* playerName = "";//PlayerNameMgr::GetRandomName();
				pSendHead->DestZoneId = SERVER->currentAreaId;
				sendPacket.writeString(playerName, COMMON_STRING_LENGTH);
				pSendHead->PacketSize = sendPacket.getPosition() - IPacket::GetHeadSize();

				SERVER->GetRemoteAccountSocket()->Send(sendPacket);
			}
		}

		return true;
	}
	
	bool HandleRWCreatePlayerRespond(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if (!pAccountInfo)
		{
            if (0 != pHead->DestZoneId)
            {
                g_Log.WriteLog("RA DeletePlayer");

                char buf[128];
                Base::BitStream remotePacket(buf, sizeof(buf));
                stPacketHead* pSendHead = IPacket::BuildPacketHead(remotePacket, "WORLD_REMOTE_DeletePlayer", pHead->DestZoneId);
                SERVER->GetRemoteAccountSocket()->Send(remotePacket);
            }

            return false;
        }

		int error = NONE_ERROR;

		if (0 == pHead->DestZoneId)
		{
			error = PLAYER_NAME_ERROR;
		}

		pAccountInfo->m_createInfo.ReadData(Packet);
		if (NONE_ERROR == error)
		{
			error = pAccountInfo->CreatePlayer(pHead->DestZoneId);
            
            //��Ҫɾ����remote account�ϴ����ɹ��Ľ�ɫ
            if (error != NONE_ERROR)
            {
                g_Log.WriteLog("RA DeletePlayer2");

                char buf[128];
                Base::BitStream remotePacket(buf, sizeof(buf));
                stPacketHead* pSendHead = IPacket::BuildPacketHead(remotePacket, "WORLD_REMOTE_DeletePlayer", pHead->DestZoneId);
                SERVER->GetRemoteAccountSocket()->Send(remotePacket);
            }
        }

        //��������ɹ�����ȴ�DB��������
        if (error != NONE_ERROR)
        {

			{
				char buf[MAX_PACKET_SIZE];
				Base::BitStream sendPacket(buf,sizeof(buf));
				stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "CLIENT_WORLD_CreatePlayerResponse", pHead->Id, SERVICE_CLIENT);
				//---------ylb web-------//
				sendPacket.writeInt(error, Base::Bit16);
				sendPacket.writeInt(pAccountInfo->UID, UID_BITS);
				sendPacket.writeInt(0, Base::Bit32);
				/*sendPacket.writeInt(pAccountInfo->UID, UID_BITS);
				sendPacket.writeInt(error, Base::Bit16);
				sendPacket.writeInt(0, Base::Bit32);*/
				//---------ylb web-------//

				SERVER->GetServerSocket()->Send(pAccountInfo->GetGateSocketId(), sendPacket);
			}
        }

		return true;
	}
	
	bool HandleLoginGameRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if(!pAccountInfo)
		{
			return false;
		}

		T_UID UID = Packet->readInt(UID_BITS);

		if (!pAccountInfo->VerifyUID(UID,SocketHandle))
			return false;

		U32	 playerId      = Packet->readInt(Base::Bit32);
		
        g_Log.WriteLog("�յ��ʺ�[%d]���[%d]�ĵ�½����",pHead->Id,playerId);

		if (!pAccountInfo->SetPlayerId(playerId))
		{
			g_Log.WriteError("�ʺ�[%d]��������[%d]������",pHead->Id,playerId);
			return false;
		}

        time_t curTime = time(0);

        //�жϷ������Ƿ��Ѿ�����
        /*if (!SERVER->GetServerManager()->IsOpenTime(curTime))
        {
            char buf[64]; 
            Base::BitStream sendPacket(buf,sizeof(buf));
            stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,CLIENT_GAME_LoginResponse,pHead->Id,SERVICE_CLIENT);
            sendPacket.writeInt(SERVER_NOT_OPENED,Base::Bit16);
            time_t openTime = SERVER->GetServerManager()->GetOpenTime();
            sendPacket.writeInt((int)openTime,Base::Bit32);

            pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
            SERVER->GetServerSocket()->Send(SocketHandle,sendPacket);
            return false;
        }*/

		{

			int error = NONE_ERROR;
			U32 playerId = pAccountInfo->GetPlayerId();

			if(error == NONE_ERROR)
			{
				error = SERVER->GetPlayerManager()->AddPlayerMap(pAccountInfo,playerId,pAccountInfo->GetPlayerName(playerId));
			}

			/*if (NONE_ERROR == error && pAccountInfo->IsRobot())
			{
				PlayerDataRef spPlayerData = pAccountInfo->GetPlayerData();

				if (spPlayerData)
				{
					if (Packet->getReadByteSize() > 4)
					{
						//�����˵�½ʱ,ָ����½�ĵ�ͼ��λ��
						spPlayerData->BaseData.ZoneId = Packet->readInt(Base::Bit32);
					}
				}
			}*/
			
			/*{
				char buf[128]; 
				Base::BitStream sendPacket(buf,sizeof(buf));
				stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,"CLIENT_GAME_LoginResponse",pHead->Id,SERVICE_CLIENT);
				sendPacket.writeInt(error,Base::Bit16);
				sendPacket.writeInt(S32(_time32(0)), Base::Bit32);
				pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
				SERVER->GetServerSocket()->Send(SocketHandle,sendPacket);
			}*/

			pAccountInfo->SetState(Player::STATUS_IN_GAME);
		}

		return true;
	}
    
    //����뿪��ͼ
	bool HandleClientLogout(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if(!pAccountInfo)
		{
			return false; 
		}

		pAccountInfo->SetGateSocketId(SocketHandle);
		pAccountInfo->SetBit(Player::EVENT_CLIENTLOST, false);
		return true;
	}

	bool HandlePlayerGateReLogin(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);

		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if(!pAccountInfo)
		{
			return false;
		}

		T_UID UID = Packet->readInt(UID_BITS);

		if (pAccountInfo->GetUID() != UID)
			return false;

		pAccountInfo->SetGateSocketId(SocketHandle);
		pAccountInfo->SetBit(Player::EVENT_CLIENTLOST,false);

        if (pAccountInfo->GetState() != Player::STATUS_LOGOUT)
        {
		    SERVER->GetPlayerManager()->AddOnlineAccount(pHead->Id);
        }

		g_Log.WriteLog( "�ʺ�[%d]��������",pHead->Id);

		char buf[64];
		Base::BitStream sendPacket(buf,sizeof(buf));
		stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,"GATE_WORLD_ClientReLoginResponse",pHead->Id);
		sendPacket.writeInt( 0,Base::Bit16);
		pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
		SERVER->GetServerSocket()->Send(SocketHandle,sendPacket);

		return true;
	}

	bool HandlePlayerBindAccount(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		U32 nAccountId = pHead->Id;
		U32 nError = pHead->DestZoneId;
		char AccountName[50] = "";
		char PassWord[32] = "";
		Packet->readString(AccountName, 50);
		Packet->readString(PassWord, 32);
		if(nError == 0)
		{
			DB_ReNameAccount* pDBHandle = new DB_ReNameAccount();
			pDBHandle->m_accountId = nAccountId;
			dMemcpy(pDBHandle->m_AccountName, AccountName, strlen(AccountName));
			SERVER->GetCommonDBManager()->SendPacket(pDBHandle);
		}

		{
			DO_LOCK(Lock::Player);
			AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(nAccountId);
			if(!pAccountInfo)
				return false;

			if(nError == 0)
			{
				pAccountInfo->SetAccountName(AccountName);
			}

			char buf[256];
			Base::BitStream sendPacket(buf,sizeof(buf));
			stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,"CLIENT_Bind_AccountResponse",nAccountId, SERVICE_CLIENT,nError);
			sendPacket.writeString(AccountName, 50);
			sendPacket.writeString(PassWord, 32);
			pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(pAccountInfo->GetGateSocketId(), sendPacket);
		}
		return true;
	}

	bool HandleRWLoginPlayerResp(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if (!pAccountInfo)
		{
			return false;
		}

        g_Log.WriteLog("�յ��ʺ�[%d]Զ�̷�������¼����", pHead->Id);

		int iBonusCount = Packet->readInt(32);
		int NetBarLv = Packet->readInt(32);               
		int accountVipLv = Packet->readInt(32);
		int defrayExp = Packet->readInt(32);
		int sponsorActivity = Packet->readInt(32);
							
		//��֤�ɹ����½Zone
		int error = NONE_ERROR;
		U32 playerId = pAccountInfo->GetPlayerId();

		int zoneSocket = 0;

		if (NONE_ERROR != error)
		{
			char buf[64]; 
			Base::BitStream sendPacket(buf, sizeof(buf));
			stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket, "CLIENT_GAME_LoginResponse", pHead->Id, SERVICE_CLIENT);
			sendPacket.writeInt(error, Base::Bit16);
			sendPacket.writeInt(S32(_time32(0)), Base::Bit32);
			pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(pAccountInfo->GetGateSocketId(), sendPacket);
			return true;
		}
		else
		{
			//��¼�ɹ�
		}

		return true;
	}

    bool HandleGetLinePlayerCount(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
    {
        AccountRef pAccountInfo = SERVER->GetPlayerManager()->GetAccount(pHead->Id);

		if(!pAccountInfo)
		{
			return false;
		}
        
        std::vector<int> lines;
        SERVER->GetServerManager()->GetAllLines(lines);

        U32 totalLineCount = 0;

        for (size_t i = 0; i < lines.size(); ++i)
        {  
            totalLineCount++;
        }

        char buf[1024];
        Base::BitStream sendPacket(buf,sizeof(buf));
        stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,"WC_GETLINEPLAYERCOUNT_RESP",pHead->Id,SERVICE_CLIENT);
        
        int totalOnline = 397 * SERVER->GetServerManager()->GetNormalLineCount() + SERVER->GetPlayerManager()->GetOnlineAccountCount();
        sendPacket.writeInt(totalOnline,Base::Bit32);
        sendPacket.writeInt(totalLineCount,Base::Bit32);

        for (size_t i = 0; i < lines.size(); ++i)
        {
            sendPacket.writeInt(lines[i],Base::Bit32);

            U32 mode = 1;
            U32 playerCount =  0;

            float percent = 0;
            
            if (0 != g_maxLinePlayers)
            {
                percent = (float)playerCount / (float)g_maxLinePlayers;
            }
            
            float status1 = float(atoi(CFG_BUIILDER->Get("status1").c_str())) / 100.0f;
            float status2 = float(atoi(CFG_BUIILDER->Get("status2").c_str())) / 100.0f;

            if (0 == status1)
            {
                status1 = 0.25f;
            }

            if (0 == status2)
            {
                status2 = 0.50f;
            }

            if(percent < status1)
				mode = 1;
			else if(percent < status2)
				mode = 2;
			else 
				mode = 3;

            sendPacket.writeInt(mode,Base::Bit32);
        }
        
        pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
        SERVER->GetServerSocket()->Send(pAccountInfo->GetGateSocketId(),sendPacket);
        return true;
    }

	bool HandleAlipayCharge(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
	{
		DO_LOCK(Lock::Player);
		AccountRef account = SERVER->GetPlayerManager()->GetOnlinePlayer(pHead->Id);
		if (!account)
			return false;
		int price = Packet->readInt(Base::Bit32);
		char buf[512];
		Base::BitStream sendPacket(buf, sizeof(buf));
		stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "Alipay_Charge", pHead->Id);
		sendPacket.writeInt(account->GetAccountId(), Base::Bit32);
		sendPacket.writeString(account->GetAccountName());
		sendPacket.writeInt(SERVER->currentAreaId, Base::Bit32);
		sendPacket.writeString(account->GetLoginIP());
		sendPacket.writeInt(price, Base::Bit32);
		SERVER->GetRemoteAccountSocket()->Send(sendPacket);

		return true;
	}

	bool HandlePlayerPurchaseResponse(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet)
	{
		DO_LOCK(Lock::Player);
		U32 nPlayerId = pHead->Id;
		U32 nGold	  =   Packet->readInt(Base::Bit32);
		U32 nMoney    =	  Packet->readInt(Base::Bit32);
		U32 nCopperCoil = Packet->readInt(Base::Bit32);
		U32 nSilverCoil = Packet->readInt(Base::Bit32);
		U32 nGoldCoil   = Packet->readInt(Base::Bit32);
		AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);
		if(pAccount)
		{
		}
		else
		{
			DB_PlayerPurchase* pDBHandle = new DB_PlayerPurchase();
			pDBHandle->nPlayerId = nPlayerId;
			pDBHandle->nGold = nGold;
			pDBHandle->nMoney = nMoney;
			SERVER->GetCommonDBManager()->SendPacket(pDBHandle);
		}
		return true;
	}

	bool HandleRechargeResponse(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet)
	{
		DO_LOCK(Lock::Player);
		U32 nPlayerId = pHead->Id;
		PlayerSimpleDataRef spd = SERVER->GetPlayerManager()->GetPlayerData(nPlayerId);
		if (spd)
		{
			stOrder OrderInfo;
			OrderInfo.ReadData(Packet);
			std::string UID = Packet->readString(50);
			if (OrderInfo.buytype >= 9 && OrderInfo.buytype <= 12) {

			}
			else {
				SERVER->GetPlayerManager()->AddDrawGold(nPlayerId, OrderInfo.itemcount);
				SERVER->GetPlayerManager()->AddGold(nPlayerId, OrderInfo.itemcount);
			}

			CMemGuard buf(256 MEM_GUARD_PARAM);
			Base::BitStream sendPacket(buf.get(), 256);
			stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WR_RECHARGE_RESPONSE", pHead->Id);
			sendPacket.writeString(UID, 50);
			SERVER->GetRemoteAccountSocket()->Send(sendPacket);

			AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);
			if (pAccount)
			{
				CMemGuard Buffer(256 MEM_GUARD_PARAM);
				Base::BitStream sendPacket(Buffer.get(), 256);
				stPacketHead* pHead = IPacket::BuildPacketHead(sendPacket, "WC_RECHARGE_SUCESSS_NOTIFY", pAccount->GetAccountId(), SERVICE_CLIENT);
				sendPacket.writeInt(OrderInfo.id, Base::Bit32);
				pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
				SERVER->GetServerSocket()->Send(pAccount->GetGateSocketId(), sendPacket);
			}

			auto ActivityMgrNofity = [](int nPlayerID, int buytype, int nPrince)
			{
				{
					MAKE_WORKQUEUE_PACKET(sendPacket, 256);
					stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WW_ACTIVITY_Notify");
					sendPacket.writeInt(ACTIVITY_NOTIFY_GOLD, Base::Bit8);
					sendPacket.writeInt(nPlayerID, Base::Bit32);
					sendPacket.writeInt(buytype, Base::Bit32);
					sendPacket.writeInt(nPrince, Base::Bit32);
					sendPacket.writeFlag(true);
					pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
					SEND_WORKQUEUE_PACKET(sendPacket, WQ_NORMAL, OWQ_EnterGame);
				}

				{
					MAKE_WORKQUEUE_PACKET(sendPacket, 256);
					stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WW_ORGGIFT_Notify");
					sendPacket.writeInt(ACTIVITY_NOTIFY_GOLD, Base::Bit8);
					sendPacket.writeInt(nPlayerID, Base::Bit32);
					sendPacket.writeInt(buytype, Base::Bit32);
					sendPacket.writeInt(nPrince, Base::Bit32);
					sendPacket.writeFlag(true);
					pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
					SEND_WORKQUEUE_PACKET(sendPacket, WQ_NORMAL, OWQ_EnterGame);
				}	
			};

			ActivityMgrNofity(nPlayerId, OrderInfo.buytype, OrderInfo.price);
		}
		
		return true;
	}

	bool HandleRecharge1Response(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet) {
		DO_LOCK(Lock::Player);
		U32 nPlayerId = pHead->Id;
		PlayerSimpleDataRef spd = SERVER->GetPlayerManager()->GetPlayerData(nPlayerId);
		if (spd)
		{
			stOrder OrderInfo;
			OrderInfo.ReadData(Packet);
			std::string UID = Packet->readString(50);
			if (OrderInfo.buytype >= 9 && OrderInfo.buytype <= 12) {

			}
			else {
				SERVER->GetPlayerManager()->AddDrawGold(nPlayerId, OrderInfo.itemcount);
				SERVER->GetPlayerManager()->AddGold(nPlayerId, OrderInfo.itemcount);
			}

			AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);
			if (pAccount)
			{
				CMemGuard Buffer(256 MEM_GUARD_PARAM);
				Base::BitStream sendPacket(Buffer.get(), 256);
				stPacketHead* pHead = IPacket::BuildPacketHead(sendPacket, "WC_RECHARGE_SUCESSS_NOTIFY", pAccount->GetAccountId(), SERVICE_CLIENT);
				sendPacket.writeInt(OrderInfo.id, Base::Bit32);
				pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
				SERVER->GetServerSocket()->Send(pAccount->GetGateSocketId(), sendPacket);
			}

			auto ActivityMgrNofity = [](int nPlayerID, int buytype, int nPrince)
			{
				{
					MAKE_WORKQUEUE_PACKET(sendPacket, 256);
					stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WW_ACTIVITY_Notify");
					sendPacket.writeInt(ACTIVITY_NOTIFY_GOLD, Base::Bit8);
					sendPacket.writeInt(nPlayerID, Base::Bit32);
					sendPacket.writeInt(buytype, Base::Bit32);
					sendPacket.writeInt(nPrince, Base::Bit32);
					sendPacket.writeFlag(false);
					pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
					SEND_WORKQUEUE_PACKET(sendPacket, WQ_NORMAL, OWQ_EnterGame);
				}

				{
					MAKE_WORKQUEUE_PACKET(sendPacket, 256);
					stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WW_ORGGIFT_Notify");
					sendPacket.writeInt(ACTIVITY_NOTIFY_GOLD, Base::Bit8);
					sendPacket.writeInt(nPlayerID, Base::Bit32);
					sendPacket.writeInt(buytype, Base::Bit32);
					sendPacket.writeInt(nPrince, Base::Bit32);
					sendPacket.writeFlag(true);
					pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
					SEND_WORKQUEUE_PACKET(sendPacket, WQ_NORMAL, OWQ_EnterGame);
				}
			};
			ActivityMgrNofity(nPlayerId, OrderInfo.buytype, OrderInfo.price);
		}

		return true;
	}

	bool HandleCodeResponse(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet) {
		DO_LOCK(Lock::Player);
		U32 nPlayerId = pHead->Id;
		PlayerSimpleDataRef spd = SERVER->GetPlayerManager()->GetPlayerData(nPlayerId);
		if (spd)
		{
			std::vector<S32> ItemIdVec, ItemNumVec;
			S32 nLen = Packet->readInt(Base::Bit32);
			for (auto i = 0; i < nLen; ++i) {
				ItemIdVec.push_back(Packet->readInt(Base::Bit32));
				ItemNumVec.push_back(Packet->readInt(Base::Bit32));
			}
			std::string UID = Packet->readString(50);
			auto nSlot = 0;
			for (auto itr = ItemIdVec.begin(); itr != ItemIdVec.end(); ++itr, ++nSlot) {
				ITEMMGR->AddItem(nPlayerId, *itr, ItemNumVec[nSlot]);
			}

			CMemGuard buf(256 MEM_GUARD_PARAM);
			Base::BitStream sendPacket(buf.get(), 256);
			stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WR_CODE_RESPONSE", pHead->Id);
			sendPacket.writeString(UID, 50);
			SERVER->GetRemoteAccountSocket()->Send(sendPacket);

			/*AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);
			if (pAccount)
			{
				CMemGuard Buffer(256 MEM_GUARD_PARAM);
				Base::BitStream sendPacket(Buffer.get(), 256);
				stPacketHead* pHead = IPacket::BuildPacketHead(sendPacket, "WC_CODE_SUCESSS_NOTIFY", pAccount->GetAccountId(), SERVICE_CLIENT);
				pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
				SERVER->GetServerSocket()->Send(pAccount->GetGateSocketId(), sendPacket);
			}*/
		}
		return true;
	}
}
