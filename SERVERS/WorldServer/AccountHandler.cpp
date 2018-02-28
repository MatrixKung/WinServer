//#include "Common/CheckCodeCache.h"
#include "stdafx.h"
#include "BanList.h"
#include "DBLib/dbLib.h"
#include "wintcp/dtServerSocket.h"
#include "AccountHandler.h"
#include "WorldServer.h"
#include "ServerMgr.h"
#include "PlayerMgr.h"
#include "zlib/zlib.h"
#include "SocialManager.h"
#include "LockCommon.h"
#include "AccountServer/PlayerMgr.h"
#include <mmsystem.h>
#include "DBLayer\Data\TBLPlayer.h"
#include "Common\FilterOpt.h"
#include "Common\dbStruct.h"
#include "ChatHelper.h"
#include "QuickLz/quicklz.h"
#include "Common/CommonClient.h"
#include "base/WorkQueue.h"
#include "DB_UpdateAccountLogoutTime.h"
#include "DB_UpdateAccountTime.h"
#include "DB_UpdatePlayerDeleteTime.h"
#include "Common/Common.h"
#include "Script/lua_tinker.h"
#include "Common/UtilString.h"
//#include "Room/RoomMgr.h"
#ifdef MODEL_TEST
#include "DB_MailList.h"
#include "DB_ReadMail.h"
#include "DB_TryGetMailItem.h"
#endif

#pragma comment(lib, "Winmm.lib")

Player::Player(void):
m_status(this)
{
	mSocketId		  = 0;
	m_createInfo.Clear();
    m_newPlayerId     = 0;
    m_isFirstLoginGame= false;
		

	__InitStatus();

	m_status.SetRawState(0);   
}

Player::~Player(void)
{
	/*if (0 != m_pSMData)
	{
		SERVER->GetPlayerManager()->GetAccountSMPool()->Free(m_pSMData);
		m_pSMData = 0;
	}*/

	SERVER->GetPlayerManager()->DelOnlineAccount(GetAccountId());
}

void Player::__InitStatus(void)
{
	m_status.m_onStateChanged           = &Player::__OnStateChangedHandler;

	m_status[STATUS_LOGIN].onEnter		= &Player::__OnEnterLoginHandler;

	m_status[STATUS_IN_SELECT].onEnter  = &Player::__OnEnterSelectHandler;
	m_status[STATUS_IN_SELECT].onLeave  = &Player::__OnLeaveSelectHandler;

	m_status[STATUS_IN_GAME].onEnter    = &Player::__OnEnterGameHandler;
	m_status[STATUS_IN_GAME].onLeave    = &Player::__OnLeaveGameHandler;

	m_status[STATUS_LOGOUT].onEnter		= &Player::__OnEnterLogoutHandler;
}

bool Player::Kick(void)
{
	SetBit(EVENT_WAIT_KICKPLAYER, true);
	//�ߵ���ǰ��Ϸ�е��û�
	U32 playerId = m_playerId;

	if(0 != playerId)
	{
		CHAT_HELPER->clearAccount(GetAccountId());
	}

	m_kickTimer.Start();
	return true;
}

void Player::__OnEnterLogoutHandler(void)
{
	ClrBit();

	UpdateAccountLogoutTime();
	m_offlineTimer.Start();

	if (!IsGM())
	{
		//֪ͨ�ʺŷ�������ɫ�˳�
		char buf[256];
		Base::BitStream sendPacket(buf,sizeof(buf));
		stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket,"WORLD_ACCOUNT_PlayerLogout",GetAccountId(),SERVICE_ACCOUNTSERVER);
		pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);

		int svrSocket = SERVER->GetServerManager()->GetServerSocket(SERVICE_ACCOUNTSERVER);

		if(svrSocket > 0)
		{
			SERVER->GetServerSocket()->Send(svrSocket,sendPacket);
		}
	}

	//��gateɾ��ͨ��
	{
		char buf[256];
		Base::BitStream sendPacket(buf,sizeof(buf));
		stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket,"GATE_WORLD_ClientLogoutResponse",GetAccountId(),SERVICE_GATESERVER);
		pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);

		SERVER->GetServerSocket()->Send(GetGateSocketId(), sendPacket);
	}

	SERVER->GetPlayerManager()->DelOnlineAccount(GetAccountId());

    CHAT_HELPER->removeAccountPlayer(GetAccountId());
    //�Ƴ�������Ҽ���
    //SERVER->GetPlayerManager()->removeLinePlayerCount(GetPlayerId(),GetLastLineId() >= CCopyMapManager::COPYMAP_LINE ? GetSelLineId() : GetLastLineId());
}

//������Ϸ����
//��ʱ����Ѿ�������zone,��������ҹ���
void Player::__OnEnterGameHandler(void)
{
    if (IsFakePlayer())
    {
        ClrBit();

        //��������fake player�������Ϣ
		//SERVER->GetPlayerManager()->ResetFakeInfo(GetPlayerId(),GetLastLineId(),GetLastZoneId());

		//SERVER->GetPlayerManager()->addLinePlayerCount(GetPlayerId(), GetLastLineId(), true);

		//�����ҵ�1�ε�½��ͼ,���͵�ǰ�ĵ�¼����
		/*if (m_isFirstLoginGame && GetLastZoneId() < 9999)
		{
			SERVER->GetPlayerManager()->AddLoginList(GetLastLineId(),GetPlayerId(),GetPlayerName());
		}
		*/
		m_isFirstLoginGame = false;
        return;
    }

	U32 curPlayerId = GetPlayerId();

    g_Log.WriteLog("[%d] enter begin",curPlayerId);

	for( int i = 0; i < MAX_ACTOR_COUNT; ++i)
	{
		int playerId = m_playerIdList[i].playerID;

		if(0 != playerId && playerId != curPlayerId)
		{
			g_Log.WriteLog("ChatLog:__OnEnterGameHandler player[%d]", playerId);
			SERVER->GetPlayerManager()->RemovePlayer(playerId,"");
		}
	}

	m_newPlayerId  = 0;

	ClrBit();

	/*if (m_status.GetPreState() != m_status.GetState() && m_status.GetPreState() != STATUS_LOGIN)
	{
		//��1�ν�����Ϸ
		//SERVER->GetSocialManager()->onPlayerEnter(curPlayerId);
		{
			MAKE_WORKQUEUE_PACKET(sendPacket, 256);
			sendPacket.writeInt(curPlayerId, Base::Bit32);
			SEND_WORKQUEUE_PACKET(sendPacket, WQ_SOCIAL, OWQ_EnterGame);
		}

		//������Ϣ
		{
			MAKE_WORKQUEUE_PACKET(sendPacket, 256);
			sendPacket.writeInt(curPlayerId, Base::Bit32);
			SEND_WORKQUEUE_PACKET(sendPacket, WQ_ORG, OWQ_EnterGame);
		}
	}*/


	//��������ϵ��Ϣ
	/*{
		MAKE_WORKQUEUE_PACKET(sendPacket, 256);
		sendPacket.writeInt(curPlayerId, Base::Bit32);
		SEND_WORKQUEUE_PACKET(sendPacket, WQ_SOCIAL, OWQ_NotifyZone);
	}


    //ˢ�µ�chat������
    CHAT_HELPER->addAccountPlayer(curPlayerId,GetAccountId(),GetPlayerName(curPlayerId),GetAccountName(),GetGateSocketId());*/

	//֪ͨPlayerBase
	{
		MAKE_WORKQUEUE_PACKET(sendPacket, 32);
		stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WW_Player_Enter_To_PlayerInfoMgr", curPlayerId);
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
		SEND_WORKQUEUE_PACKET(sendPacket, WQ_NORMAL, OWQ_EnterGame);
	}
    m_isFirstLoginGame = false;

    g_Log.WriteLog("[%d] enter end",curPlayerId);
	/*CChessTable* nTable = ROOMMGR->FindChessTableByPlayerId(curPlayerId);
	if(nTable)
	{
		U16 nSlot = nTable->GetPlayerSlot(curPlayerId);
		nTable->OnEventSendGameScene(nSlot,false,nTable->GetId(),curPlayerId);
	}*/

}

void Player::__OnLeaveGameHandler(void)
{
	U32 playerId = GetPlayerId();
   // SERVER->GetPlayerManager()->removeLinePlayerCount(playerId,GetLastLineId() >= CCopyMapManager::COPYMAP_LINE ? GetSelLineId() : GetLastLineId());
    
    if(!GetBit(Player::EVENT_TRANSPORT))
    {
		{
			MAKE_WORKQUEUE_PACKET(sendPacket, 256);
			sendPacket.writeInt(playerId, Base::Bit32);
			SEND_WORKQUEUE_PACKET(sendPacket, WQ_SOCIAL, OWQ_LeaveGame);
		}
		{
			MAKE_WORKQUEUE_PACKET(sendPacket, 32);
			stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WW_Player_Leave_To_PlayerInfoMgr", playerId);
			pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
			SEND_WORKQUEUE_PACKET(sendPacket, WQ_NORMAL, OWQ_LeaveGame);
		}

		//��������
		//֪ͨ���������,�Ӵ���Ҷ��ĵ�����Ƶ����,ɾ����������
		CHAT_HELPER->clearAccount(GetAccountId());
       
    }


	g_Log.WriteLog("ChatLog:__OnLeaveGameHandler player[%d, %s]", playerId, GetPlayerName(playerId));
	//SERVER->GetPlayerManager()->RemovePlayer(playerId,GetPlayerName(playerId));

	//SERVER->GetCombatManager()->DelWaitPlayer(playerId);
	//SetBit(Player::EVENT_TRANSPORT,false);
}

void Player::__OnStateChangedHandler(void)
{
	//UpdateSMData();
}

void Player::__OnEnterLoginHandler(void)
{	
	SendGateLoginRespond(0);

	if (!IsGM())
	{
		SERVER->GetPlayerManager()->AddOnlineAccount(GetAccountId());
	}
}

void Player::__OnEnterSelectHandler(void)
{
	SetBit(EVENT_WAIT_PLAYERDATA,false);
}

void Player::__OnLeaveSelectHandler(void)
{	
	SetBit(EVENT_WAIT_PLAYERDATA,false);
	SetBit(EVENT_WAIT_CREATEPLAYER,false);
	SetBit(EVENT_WAIT_ENTERGAME,false);
}

void Player::Update( U32 curTime )
{
	m_status.Update(curTime);
	if (GetBit(EVENT_WAIT_KICKPLAYER) && m_kickTimer.CheckTimer())
	{	
		SetState(STATUS_LOGOUT);
		SetBit(EVENT_WAIT_KICKPLAYER, false);
	}

	//��⵽���ӶϿ���Ҫ�ȴ�,����ͻ���û����������Ϊ����״̬
	if (GetBit(EVENT_CLIENTLOST))
	{
		if (m_lostTimer.CheckTimer())
		{
			if (STATUS_IN_GAME == GetState())
			{
				UpdateLogoutTime();
			}

            g_Log.WriteLog("���[%d]��ʱ����",GetPlayerId());

            if (STATUS_LOGOUT != GetState())
            {
			    SetState(STATUS_LOGOUT);
            }
		}
	}

	//��������logout״̬,���1��ʱ����л���offline״̬,��ʱɾ����ɫ��������
	if (STATUS_LOGOUT == GetState() && m_offlineTimer.CheckTimer())
	{
		SetState(STATUS_OFFLINE);
		return;
	}

   /* if (CanSendLogicData())
    {
        SafePacket::Instance()->Flush(this);
    }*/
}

void Player::ResetLostTimer(void)
{
	m_lostTimer.Start();
}

U32 Player::GetLogoutTime(void)
{
	return m_offlineTimer.GetLastTime();
}

bool Player::VerifyUID(U32 id,int SocketHandle)
{
	return id >= UID;
}

bool Player::SendPlayerList(void)
{
	bool isAllDataReady = true;

	//��ȡ�������
	int nPlayerSize = GetPlayerCount();
	CMemGuard Buffer(512 MEM_GUARD_PARAM);
	Base::BitStream sendPacket(Buffer.get(), 512);
	stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,"CLIENT_WORLD_SelectPlayerResponse", GetAccountId(), SERVICE_CLIENT);
	//---------ylb web-------//
	sendPacket.writeInt(0, Base::Bit16);
	//---------ylb web-------//
	sendPacket.writeInt(nPlayerSize, Base::Bit16);
	for(int i = 0; i < nPlayerSize; ++i)
	{
		PlayerSimpleDataRef spd = SERVER->GetPlayerManager()->GetPlayerData(m_playerIdList[i].playerID);
		if(sendPacket.writeFlag(spd != NULL))
		{
			//spd->WriteData(&sendPacket);
			sendPacket.writeInt(spd->PlayerId, Base::Bit32);
			char* tempName = Util::MbcsToUtf8(spd->PlayerName);
			sendPacket.writeString(tempName, 32);
			sendPacket.writeInt(spd->Gold, Base::Bit32);
			delete[] tempName;
		}
	}
	pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();

	SERVER->GetServerSocket()->Send(GetGateSocketId(),sendPacket);
	return true;
}

int Player::CheckDeletePlayer(U32 playerId,bool& canDeleted,int& deleteTime)
{
    return 0;
}

//ɾ�����,�������ɾ����ʱ��ȥworld db����
void Player::DeletePlayer(U32 playerId)
{
}

void Player::SendPlayerDeleteRespond(U32 playerId,int error,int deleteTime)
{
    char buf[128];
	Base::BitStream sendPacket(buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,"CLIENT_WORLD_DeletePlayerResponse",GetAccountId(),SERVICE_CLIENT);

	sendPacket.writeInt(UID, UID_BITS);
	sendPacket.writeInt(playerId,Base::Bit32);
    sendPacket.writeInt(error,Base::Bit8);
    sendPacket.writeInt(deleteTime,Base::Bit32);

    SERVER->GetServerSocket()->Send(GetGateSocketId(),sendPacket);
}

//�յ�remote account��������ķ���
int Player::CreatePlayer(U32 playerId)
{
	//if (m_createInfo.m_Family >= Family_Max)
	//	return UNKNOW_ERROR;

	if (PlayerNum >= MAX_CREATE_ACTOR_COUNT)
		return PLAYER_CREATE_FULL;

	int error = SERVER->GetPlayerManager()->CreatePlayer(
		m_createInfo.m_CreateUID, AccountId, m_createInfo.m_CreateName, m_createInfo.m_CreateSex, m_createInfo.m_CreateBody,
		m_createInfo.m_CreateFace, m_createInfo.m_CreateHair, m_createInfo.m_CreateHairColor, m_createInfo.m_CreateItemId, m_createInfo.m_CreateFaceImage, m_createInfo.m_Family,
		0, playerId);

	return error;
}

bool Player::SetPlayerId(U32 playerId)
{
	for (int i = 0; i < MAX_ACTOR_COUNT; ++i)
	{
		if (m_playerIdList[i].playerID == playerId)
		{
			m_playerId = playerId;

			return true;
		}
	}

	return false;
}

const char* Player::GetPlayerName(void)
{
	return GetPlayerName(GetPlayerId());
}

const char* Player::GetPlayerName(U32 playerId)
{
    if (0 == playerId)
        return "";

	for (int i = 0; i < MAX_ACTOR_COUNT; ++i)
	{
		if (m_playerIdList[i].playerID == playerId)
		{
			PlayerSimpleDataRef spd = SERVER->GetPlayerManager()->GetPlayerData(playerId);

			if (spd)
			{
				return spd->PlayerName;
			}

			return "";
		}
	}

	return "";
}

U32 Player::GetPlayerCount(void) const
{
	U32 count = 0;

	for (int i = 0; i < MAX_ACTOR_COUNT; ++i)
	{
		if (0 != m_playerIdList[i].playerID)
		{
			count++;
		}
	}

	return count;
}

void Player::UpdateLogoutTime(void)
{
	if (GetBit(Player::EVENT_LOGOUTED))
	{
		return;
	}

	U32 playerId = GetPlayerId();

	if (0 == playerId)
		return;

	int lastTime = (int)_time32( NULL ) - this->LastTime;

	if( lastTime < 0 )
		lastTime = 0;

    //�����ʺŵ�����ʱ��ͷ�����ʱ��
    DB_UpdateAccountTime* pDBHandle = new DB_UpdateAccountTime;

    pDBHandle->m_accountId          = GetAccountId();
    pDBHandle->m_lastTime           = lastTime;
    pDBHandle->m_dTotalOfflineTime  = 0;

    this->LastTime         = (int)_time32(NULL);

    SERVER->GetCommonDBManager()->SendPacket(pDBHandle);

	PlayerSimpleDataRef spPlayerData = SERVER->GetPlayerManager()->GetPlayerData(playerId);

	if(spPlayerData)
	{
		spPlayerData->LastLogoutTime = (unsigned int)_time32( NULL );
	}

	SetBit(Player::EVENT_LOGOUTED,true);
}

void Player::UpdateAccountLogoutTime(void)
{
    logoutTime = _time32(0);

    DB_UpdateAccountLogoutTime* pDBHandle = new DB_UpdateAccountLogoutTime;

    pDBHandle->m_accountId = GetAccountId();
    pDBHandle->m_time	   = logoutTime;

    SERVER->GetCommonDBManager()->SendPacket(pDBHandle);
}

void Player::ClearAllSpPlayerData()
{
	for (int i = 0; i < MAX_ACTOR_COUNT; i++)
	{
	}
}

int Player::ReloadPlayerList(Players& playerList, AccountRef spThis)
{
	PlayerNum = 0;
    memset(m_playerIdList, 0, sizeof(m_playerIdList));

	for(size_t i = 0; i < playerList.size(); ++i)
	{
		m_playerIdList[PlayerNum] = playerList[i];
		PlayerNum++;
		SERVER->GetPlayerManager()->m_PlayerMap[playerList[i].playerID] = spThis;
	}

	return NONE_ERROR;
}

void Player::SendClientErrorCode(Base::BitStream& sendPacket)
{
    stPacketHead* pHead = (stPacketHead*)sendPacket.getBuffer();
    pHead->Id = GetAccountId();

	SERVER->GetServerSocket()->Send(GetGateSocketId(),sendPacket);
}

void Player::SendGateLoginRespond(int error)
{
    //���Ϳͻ��˵�¼��Ӧ
	char buf[64];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead *pSendHead = IPacket::BuildPacketHead(sendPacket,"GATE_WORLD_ClientLoginResponse",GetAccountId());

	sendPacket.writeInt(error,Base::Bit16);
	sendPacket.writeFlag(IsGM());
	pSendHead->PacketSize = sendPacket.getPosition()-IPacket::GetHeadSize();
	//int gateSocketId = SERVER->GetServerManager()->GetServerSocket(SERVICE_GATESERVER, 1, 1);
	//if(gateSocketId)
		SERVER->GetServerSocket()->Send(GetGateSocketId(),sendPacket);
}

U8 Player::GetLevel(void)
{
    PlayerSimpleDataRef spData = SERVER->GetPlayerManager()->GetPlayerData(GetPlayerId());

    if (!spData)
        return 0;

    return spData->Level;
}

bool Player::CanSendLogicData(void)
{
    return GetState() == STATUS_IN_GAME && !GetBit(EVENT_CLIENTLOST) && !GetBit(EVENT_TRANSPORT);
}

//----------------------------����lua������Ҳ���---------------------------------------
bool Player::isVaild()
{
	if(GetAccountId() == 0)
		return false;
	
	return true;
}


Player Player::getPlayer(U32 nPlayerId)
{
	static Player s_NilPlayer;//lua��ʶ��յ��û�����
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);
	if(pAccount)
		return *(pAccount.get());
	
	return s_NilPlayer;
}


void Player::Export(struct lua_State* L)
{
	lua_tinker::class_add<Player>(L, "Player");
	lua_tinker::class_def<Player>(L, "isNil", &Player::isVaild);
	lua_tinker::def(L, "getPlayer", &Player::getPlayer);
	//lua_tinker::set<CMissionDataRes>(L, "MISSIONDATA", *g_MissionDataMgr);
}
