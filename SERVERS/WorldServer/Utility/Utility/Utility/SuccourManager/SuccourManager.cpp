#include "SuccourManager.h"
#include "../../AccountHandler.h"
#include "../../WorldServer.h"
#include "../../PlayerMgr.h"
#include "../platform/platform.h"
#include "Common/MemGuard.h"
#include "WINTCP/dtServerSocket.h"
#include "../../db/Souccour/DB_DelSuccour.h"
#include "../../db/Souccour/DB_SaveSuccour.h"
#include "../../db/Souccour/DB_UpdateFlagSuccour.h"
#include "../../DBContext.h"
#include "DBLayer/common/DBUtility.h"
#include "data/TBLBase.h"
#include "CommLib/CommLib.h"
#include "DBLayer/Common/Define.h"
#include "Common/OrgBase.h"
#include "Common/mRandom.h"
#include "../../MailManager.h"
#include "Common/PlayerStruct.h"
#include "../../DB_UpdatePlayerFlag.h"

CSuccourManager::CSuccourManager()
{
	registerEvent(CW_SUCCOUR_PUBLISH_AWARD_REQUEST,					&CSuccourManager::HandlePlayerSuccourPublish);
	registerEvent(CW_SUCCOUR_GET_PLAYERLIST_REQUEST,				&CSuccourManager::HandlePlayerSuccourGetPlayerList);
	registerEvent(CW_SUCCOUR_SAVE_PLAYER_REQUEST,					&CSuccourManager::HandlePlayerSavePlayer);
	registerEvent(CW_SUCCOUR_SAVE_RESULT_REQUEST,					&CSuccourManager::HandlePlayerSaveResult);

	JUST_ASSERT(loadDB() == false);
	m_60Timer.Start();
}

CSuccourManager::~CSuccourManager()
{
	for(SUCCOURITR itr = m_SurrourMap.begin(); itr != m_SurrourMap.end(); ++itr)
	{
		SAFE_DELETE(itr->second);
	}
	m_SurrourMap.clear();
}

void CSuccourManager::Update()
{
	//��������ʱ
	if(m_60Timer.CheckTimer())
	{
		U32 nCurTime = _time32(NULL);
		for(SUCCOURITR itr = m_SurrourMap.begin(); itr != m_SurrourMap.end();)
		{
			SuccourData* pData = itr->second;
			if(!pData)
			{
				 ++itr;
				continue;
			}
			
			if(nCurTime - pData->createTime  >= SUCCOUR_REALIVE_TIME)
			{
				//ɾ�����ݿ�
				itr = m_SurrourMap.erase(itr);
				delToDB(pData->PlayerId);
				//�����������״̬
				DB_UpdatePlayerFlag* pDBHandle = new DB_UpdatePlayerFlag;
				pDBHandle->mPlayerId = pData->PlayerId;
				pDBHandle->mPlayerFlag = PLAYER_FLAG_DEATH;
				SERVER->GetSavePlayerDBManager()->SendPacket(pDBHandle);
				continue;
			}
			else if(nCurTime - pData->saveTime >= SUCCOUR_TIMEOUT && (pData->Flag != SUCCOUR_FLAG_CANSAVE))
			{
				//��վ�����־
				pData->saveTime = 0;
				pData->Flag = SUCCOUR_FLAG_CANSAVE;
				updatePlayerFlag(pData->PlayerId, SUCCOUR_FLAG_CANSAVE);
			}
			++itr;
		}
	}
}

void CSuccourManager::TimeProcess(U32 bExit)
{
	Update();
	//SERVER->GetWorkQueue()->PostEvent(3,0,0,0,WQ_TIMER);
	//return true;
}

bool CSuccourManager::loadDB()
{
	DBError err = DBERR_UNKNOWERR;
	CDBConnPool* pDBPool = SERVER->GetActorDB()->GetPool();
	if(pDBPool)
	{
		CDBConn* pDBConn = pDBPool->Lock();
		if(pDBConn)
		{
			do 
			{
				try
				{
					M_SQL(pDBConn, "SELECT PlayerID, GateID, Flag, CreateTime, PlayerName FROM Tbl_Succour");
					while(pDBConn->More())
					{
						SuccourData* pData = new SuccourData();
						pData->PlayerId	   = pDBConn->GetInt();
						pData->GateId	   = pDBConn->GetInt();
						pData->Flag		   = pDBConn->GetInt();
						pData->createTime  = pDBConn->GetTime();
						const char* tempStr	   = pDBConn->GetString();
						memcpy(pData->PlayerName, tempStr, strlen(tempStr));
						insertToMap(pData, false);
					}

					for(SUCCOURITR itr = m_SurrourMap.begin(); itr != m_SurrourMap.end(); ++itr)
					{
						if(itr->second)
						{
							M_SQL(pDBConn, "SELECT Item FROM Tbl_Succour WHERE PlayerID=%d", itr->second->PlayerId);
							if(pDBConn->Eval())
							{
								pDBConn->GetBlob( itr->second->itemLength, (PBYTE)itr->second->item);
							}
						}
					}

					err = DBERR_NONE;
				}
				DBECATCH()
			} while (false);
			pDBPool->Unlock(pDBConn);
		}
	}

	if(err == DBERR_NONE)
		return true;

	return false;
}

void CSuccourManager::saveToDB(SuccourData* pData)
{
	DB_SaveSuccour* pDBHandle = new DB_SaveSuccour;
	pDBHandle->pData = pData;
	SERVER->GetMailDBManager()->SendPacket(pDBHandle);
}

void CSuccourManager::delToDB(U32 nPlayerId)
{
	DB_DelSuccour* pDBHandle = new DB_DelSuccour;
	pDBHandle->mPlayerID = nPlayerId;
	SERVER->GetMailDBManager()->SendPacket(pDBHandle);
}

void CSuccourManager::updateFlagToDB(U32 nPlayerId, U8 nFlag)
{
	DB_UpdateFlagSuccour* pDBHandle = new DB_UpdateFlagSuccour;
	pDBHandle->mPlayerID = nPlayerId;
	pDBHandle->mFlag	 = nFlag;
	SERVER->GetMailDBManager()->SendPacket(pDBHandle);
}

void CSuccourManager::insertToMap(SuccourData* pData, bool bSaveToDB/*= true*/)
{
	if(!pData || pData->PlayerId == 0)
		return;

	SUCCOURITR itr = m_SurrourMap.find(pData->PlayerId);
	if(itr != m_SurrourMap.end())
	{
		m_SurrourMap.erase(pData->PlayerId);
	}

	m_SurrourMap.insert(SUCCOURMAP::value_type(pData->PlayerId, pData));

	if(bSaveToDB)
	{
		saveToDB(pData);
	}
}

void CSuccourManager::delMap(U32 nPlayerId)
{	
	m_SurrourMap.erase(nPlayerId);
	delToDB(nPlayerId);
}

void CSuccourManager::updatePlayerFlag(U32 nPlayerId, U8 nFlag)
{	
	SUCCOURITR itr = m_SurrourMap.find(nPlayerId);
	if(itr != m_SurrourMap.end())
	{
		itr->second->Flag = nFlag;
		updateFlagToDB(nPlayerId, nFlag);
	}
}

U32  CSuccourManager::getCanSuccourSize(U32 nPlayerId)
{
	U32 nSize = 0;
	for(SUCCOURITR itr = m_SurrourMap.begin(); itr != m_SurrourMap.end(); ++itr)
	{
		if(itr->second && itr->second->Flag == SUCCOUR_FLAG_CANSAVE && itr->second->PlayerId != nPlayerId)
		{
			++nSize;
		}
	}

	return nSize;
}
//---------------------------������Ϣ����----------------------------
void CSuccourManager::HandlePlayerSuccourPublish(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::Player);
	U32	nPlayerId = pHead->Id;
	std::string  UID;
	U32 nError = 0;//0û�д���1��ʾ�Ѿ�������2������Ʒ����

	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);
	if(!pAccount)
		return;

	PlayerDataRef pPlayer = pAccount->GetPlayerData();
	if(!pPlayer)
		return;

	do 
	{
		//������ط�������
		SUCCOURITR itr  = m_SurrourMap.find(nPlayerId);
		if(itr != m_SurrourMap.end())
		{
			nError = 1;
			break;
		}

		//UID����ȷ
		{
			char strUID[MAX_UUID_LENGTH] = "";
			Packet->readString(strUID, MAX_UUID_LENGTH);
			UID = strUID;
			if(UID.empty())
			{
				nError = 2;
				break;
			}
		}

		//��ȡ����
		stItemInfo* pItem = pAccount->getInventory(UID);
		if(!pItem)
		{
			nError = 2;
			break;
		}

		SuccourData* pData = new SuccourData();
		pData->PlayerId    = nPlayerId;
		pData->GateId      = pPlayer->MainData.PartData.GateId;
		pData->createTime  = _time32(NULL);
		pData->Flag		   = SUCCOUR_FLAG_CANSAVE;
		memcpy(pData->PlayerName, pAccount->GetPlayerName(), strlen(pAccount->GetPlayerName()));
		memcpy(pData->item, pItem, sizeof(stItemInfo));

		//���������ɾ������Ʒ
		pAccount->ItemSaveToDB(pItem, 0, SHORTCUTTYPE_INVENTORY, false, true);
		pAccount->SendClientItem(pItem, SocketHandle, 0, NOTIFY_OPERATER_DEL, SHORTCUTTYPE_INVENTORY);
		pItem->InitData();

		insertToMap(pData);
	}while(false);

	//�ᷢ���ͳɹ����
	{
		CMemGuard Buffer(64 MEM_GUARD_PARAM);
		Base::BitStream SendPacket(Buffer.get(), 64);
		stPacketHead* pSendHead = IPacket::BuildPacketHead(SendPacket, WC_SUCCOUR_PUBLISH_AWARD_RESPONSE, pAccount->GetAccountId(), SERVICE_CLIENT, nError);
		pSendHead->PacketSize = SendPacket.getPosition() - IPacket::GetHeadSize();
		SERVER->GetServerSocket()->Send(SocketHandle, SendPacket);
	}

	return;
}

void CSuccourManager::HandlePlayerSuccourGetPlayerList(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::Player);
	U32	nPlayerId = pHead->Id;
	U32 nPlayerNum = getCanSuccourSize(nPlayerId);
	if(nPlayerNum == 0)
		return;

	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);
	if(!pAccount)
		return;

	PlayerDataRef pPlayer = pAccount->GetPlayerData();
	if(!pPlayer)
		return;

	CMemGuard Buffer(2048 MEM_GUARD_PARAM);
	Base::BitStream SendPacket(Buffer.get(), 2048);
	stPacketHead* pSendHead = IPacket::BuildPacketHead(SendPacket, WC_SUCCOUR_GET_PLAYERLIST_RESPONSE, pAccount->GetAccountId(), SERVICE_CLIENT);
	if(nPlayerNum <= MAX_SUCCOUR_SIZE)
	{
		SendPacket.writeInt(nPlayerNum, Base::Bit32);
		for(SUCCOURITR itr = m_SurrourMap.begin(); itr != m_SurrourMap.end(); ++itr)
		{
			if(itr->second && itr->second->Flag == SUCCOUR_FLAG_CANSAVE)
			{
				itr->second->WriteData(&SendPacket);
			}
		}
	}
	else//���ȡ�ĸ������б�
	{
		std::set<U32> NumSet;
		U32 nCurNum = 0;
		do 
		{
			U32 nRandomVal = gRandGen.randI(0, nPlayerNum -1);
			std::set<U32>::iterator itr = NumSet.find(nRandomVal);
			if(itr == NumSet.end())
			{
				NumSet.insert(nRandomVal);
			}
		} while (NumSet.size() < MAX_SUCCOUR_SIZE);

		SendPacket.writeInt(MAX_SUCCOUR_SIZE, Base::Bit32);
		for(SUCCOURITR itr = m_SurrourMap.begin(); itr != m_SurrourMap.end(); ++itr)
		{
			if(itr->second && itr->second->Flag == SUCCOUR_FLAG_CANSAVE)
			{
				std::set<U32>::iterator iter = NumSet.find(nCurNum);
				if(iter != NumSet.end())
				{
					itr->second->WriteData(&SendPacket);
					NumSet.erase(nCurNum);
					if(NumSet.empty())
					{
						break;
					}
				}
				++nCurNum;
			}
		}
	}
	pSendHead->PacketSize = SendPacket.getPosition() - IPacket::GetHeadSize();
	SERVER->GetServerSocket()->Send(SocketHandle, SendPacket);
}

void CSuccourManager::HandlePlayerSavePlayer(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::Player);
	U32	nPlayerId = pHead->Id;//�������
	U32 nTargetId = pHead->DestZoneId;//�����������
	U32 nError = 0;//0û�д���1��ʾΪ�ҵ���������ң�2����������ѱ����˾�����
	U32 nGateId = 0;
	char TargetName[COMMON_STRING_LENGTH] = "";

	//�Լ����Լ�
	if(nPlayerId == nTargetId)
		return;

	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);
	if(!pAccount)
		return;

	PlayerDataRef pPlayer = pAccount->GetPlayerData();
	if(!pPlayer)
		return;

	do 
	{
		//δ�ҵ��������
		SUCCOURITR itr = m_SurrourMap.find(nTargetId);
		if(itr == m_SurrourMap.end())
		{
			nError =  1;
			break;
		}

		SuccourData* pData = itr->second;
		if(!pData)
			break;

		//���ܱ�����
		if(pData->Flag != SUCCOUR_FLAG_CANSAVE)
		{
			nError =  2;
			break;
		}

		//��������
		{
			nGateId = pData->GateId;
			memcpy(TargetName, pData->PlayerName, strlen(pData->PlayerName));
			pData->Flag = SUCCOUR_FLAG_SAVEING;
			pData->saveTime	= _time32(NULL);//��ʼ����ʱ
			updateFlagToDB(nTargetId, SUCCOUR_FLAG_SAVEING);
			pAccount->setPlayerFlag(PLAYER_FLAG_SUCCOUR);//��Ԯ�߿�ʼ���Ͼ�����־
		}
	} while (false);

	{
		CMemGuard Buffer(128 MEM_GUARD_PARAM);
		Base::BitStream SendPacket(Buffer.get(), 128);
		stPacketHead* pSendHead = IPacket::BuildPacketHead(SendPacket, WC_SUCCOUR_SAVE_PLAYER_RESPONSE, pAccount->GetAccountId(), SERVICE_CLIENT, nError, nGateId);
		SendPacket.writeInt(nTargetId, Base::Bit32);
		SendPacket.writeString(TargetName, COMMON_STRING_LENGTH);
		pSendHead->PacketSize = SendPacket.getPosition() - IPacket::GetHeadSize();
		SERVER->GetServerSocket()->Send(SocketHandle, SendPacket);
	}
}

void CSuccourManager::HandlePlayerSaveResult(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::Player);
	U32	nPlayerId = pHead->Id;//�������
	U32 nTargetId = pHead->DestZoneId;//�����������
	bool bSucess  = (bool)pHead->SrcZoneId;//�������
	U32 nError = 0;//0û�д���1��ʾΪ�ҵ���������ң�2�����û�о���״̬�Ѿ����

	//�Լ����Լ�
	if(nPlayerId == nTargetId)
		return;

	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);
	if(!pAccount)
		return;

	PlayerDataRef pPlayer = pAccount->GetPlayerData();
	if(!pPlayer)
		return;

	do 
	{
		//δ�ҵ��������
		SUCCOURITR itr = m_SurrourMap.find(nTargetId);
		if(itr == m_SurrourMap.end())
		{
			nError =  1;
			break;
		}

		SuccourData* pData = itr->second;
		if(!pData)
			break;

		//���ܱ�����
		if(pData->Flag != SUCCOUR_FLAG_SAVEING)
		{
			nError =  2;
			break;
		}

		U32 nCurTime = _time32(NULL);
		if(nCurTime - pData->saveTime > SUCCOUR_TIMEOUT)
		{
			nError = 2;
			break;
		}

		//�����ɹ�,��ȡ����
		if(bSucess)
		{
			//ɾ��������Ϣ
			delMap(nTargetId);
			//�����ȡ����
			stItemInfo pItem;
			memcpy(&pItem, pData->item, pData->itemLength);

			stMailItem mailItem;
			mailItem.nSender	= 0;
			mailItem.nRecver	= nPlayerId;
			mailItem.isSystem	= 1;
			char* tempTitle = "�����ɹ���ע�����";
			memcpy(mailItem.title, tempTitle, strlen(tempTitle));
			SERVER->GetMailManager()->sendMailEx(nPlayerId, mailItem, pItem);
		}
		else
		{
			pData->Flag = SUCCOUR_FLAG_CANSAVE;
			pData->saveTime	= 0;//��յ���ʱ
			updateFlagToDB(nTargetId, SUCCOUR_FLAG_CANSAVE);
		}
	} while (false);

	pAccount->clearePlayerFlag(PLAYER_FLAG_SUCCOUR);//ȥ��������־

	{
		CMemGuard Buffer(64 MEM_GUARD_PARAM);
		Base::BitStream SendPacket(Buffer.get(), 64);
		stPacketHead* pSendHead = IPacket::BuildPacketHead(SendPacket, WC_SUCCOUR_SAVE_RESULT_RESPONSE, pAccount->GetAccountId(), SERVICE_CLIENT, nError);
		pSendHead->PacketSize = SendPacket.getPosition() - IPacket::GetHeadSize();
		SERVER->GetServerSocket()->Send(SocketHandle, SendPacket);
	}
}