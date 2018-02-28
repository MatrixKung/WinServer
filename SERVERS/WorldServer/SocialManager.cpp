#include "dblib/dbCore.h"
#include "Common/CommonServerClient.h"
#include "PlayerMgr.h"
#include "WorldServer.h"
#include "ServerMgr.h"
#include "common/omputils.h"
#include "zlib/zlib.h"
#include <deque>
#include "Common/ChatBase.h"
#include "QuickLz/quicklz.h"
#include "DBLayer/Data/TblSocial.h"
#include "DBLayer/Data/TBLMPList.h"
#include "Common/OrgBase.h"
#include "SocialManager.h"
#include "base/WorkQueue.h"
#include "DBLayer/Data/TBLPlayer.h"
#include "common/PlayerStruct.h"
#include "Common/MemGuard.h"

#ifdef _NTJ_UNITTEST
#include "DBlib/dbPool.h"
#endif

void CSocialManager::ProcessEvents(int event,Base::BitStream& recv)
{
    switch (event)
    {
    /*case OWQ_QueryFriendRequest:
        {
            SERVER->GetSocialManager()->FindFriendRequest(&recv);
        }
        break;*/
    case OWQ_EnterGame:
        {
            int playerID = recv.readInt(Base::Bit32);
            SERVER->GetSocialManager()->onPlayerEnter(playerID);
        }
        break;
    case OWQ_LeaveGame:
        {
            int playerID = recv.readInt(Base::Bit32);
            SERVER->GetSocialManager()->onPlayerQuit(playerID);
        }
        break;
    /*case OWQ_ChangeSocialState:
        {
            int playerID = recv.readInt(Base::Bit32);
            PlayerStatusType::Type statusType = (PlayerStatusType::Type)recv.readInt(Base::Bit8);
            SERVER->GetSocialManager()->changePlayerStatus(playerID, statusType);
        }
        break;*/
    case OWQ_DistorySocialLink:
        {
            int playerID = recv.readInt(Base::Bit32);
            SERVER->GetSocialManager()->destoryAllLink(playerID);
        }
        break;
    /*case OWQ_CleanupSocialCache:
        {
            int playerID = recv.readInt(Base::Bit32);
            int targetID = recv.readInt(Base::Bit32);
            SERVER->GetSocialManager()->cleanupSocialCache(playerID, targetID);
        }
        break;*/
    default:
        JUST_ASSERT("WQ_SOCIAL: not handle condition.");
        break;
    }
}

void CSocialManager::OnTimer()
{
    //updateInfo();
}

void CSocialManager::OnQuit()
{
    updateDb();
}

bool CSocialManager::readDb(int playerId)
{
	if(0 == playerId)
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
		return false;
	}

	SocialMap::iterator it = mSocialMap.find(playerId);
	if(it != mSocialMap.end())
		return true;

	SocialItemMap playerSocialMap;

	TBLSocial tbSocial(0);
	tbSocial.AttachConn(m_pDBConn);
	DBError dbErr = tbSocial.Load(playerId,playerSocialMap);

	if(DBERR_NONE != dbErr)
	{
		g_Log.WriteError("�������playerId=%d����ϵ,err=%d",playerId,getDBErr(dbErr));
		return false;
	}

	if(playerSocialMap.empty())
		return false;

	mSocialMap[playerId] = playerSocialMap;
	return true;
}

bool CSocialManager::saveDb(int playerId)
{
	SocialMap::iterator it = mSocialMap.find(playerId);
	if(it == mSocialMap.end())
		return false;

	SocialItemMap itemMap = it->second;
	SocialItemMap::iterator itr = itemMap.begin();
	//��ʱ���Ѳ��豣�����ݿ�
	while (itr != itemMap.end())
	{
		if(itr->second.type == SocialType::Temp)
			itr = itemMap.erase(itr);
		else
			++itr;
	}

	if(itemMap.empty())
		return false;

	TBLSocial tbSocial(0);
	tbSocial.AttachConn(m_pDBConn);
	DBError dbErr = tbSocial.Save(playerId,itemMap);
	if(DBERR_NONE == dbErr)
		return true;

	//��¼��־
	g_Log.WriteWarn("��������ϵplayerId=%d,err=%d",playerId,getDBErr(dbErr));
	return false;
}

bool CSocialManager::saveDb(int playerId,const stSocialItem& info)
{
	if(info.type == SocialType::Temp)
		return true;

	TBLSocial tbSocial(0);
	tbSocial.AttachConn(m_pDBConn);
	DBError dbErr = tbSocial.Save(playerId,info);
	if(DBERR_NONE == dbErr)
		return true;

	//��¼��־
	g_Log.WriteError("��������ϵplayerId=%d,destPlayerId=%d,type=%d,err=%d",
		playerId,info.playerId,info.type,getDBErr(dbErr));
	return false;
}

bool CSocialManager::updateDb(int playerId,const stSocialItem& info)
{
	if(info.type == SocialType::Temp)
		return true;

	TBLSocial tbSocial(0);
	tbSocial.AttachConn(m_pDBConn);
	DBError dbErr = tbSocial.Update(playerId,info);
	if(DBERR_NONE == dbErr)
		return true;

	//��¼��־
	g_Log.WriteError("��������ϵplayerId=%d,destPlayerId=%d,newType=%d,err=%d",
		playerId,info.playerId,info.type,getDBErr(dbErr));
	return false;
}

bool CSocialManager::updateDb(int playerId,const stSocialItem& info,int destType)
{
	if(info.type == SocialType::Temp)
		return true;

	TBLSocial tbSocial(0);
	tbSocial.AttachConn(m_pDBConn);
	DBError dbErr = tbSocial.Update(playerId,info,destType);
	if(DBERR_NONE == dbErr)
		return true;

	//��¼��־
	g_Log.WriteError("�޸�����ϵplayerId=%d,destPlayerId=%d,thisType=%d,destType=%d,err=%d",
		playerId,info.playerId,info.type,destType,getDBErr(dbErr));
	return false;
}

void CSocialManager::updateInfo()
{
//	DO_LOCK( Lock::Player);

	if (!m_60Timer.CheckTimer())
		return;

	//���º��Ѷ�
	if(updateDb())
	{
		// �����ݴ���5����ʱ����ʼ����
		if(mSocialMap.size() > 50000)
		{
			g_Log.WriteWarn("����ϵ����[%d]��,��ʼ��������������ϵ����,%s,%d��",50000,__FUNCTION__,__LINE__);
			QuitList::iterator it = mQuitList.begin();
			for(; it != mQuitList.end();++it)
			{
				mSocialMap.erase(*it);
			}
		}
	}

	tm systime;
	__time32_t time32 = _time32(NULL);
	if(!_localtime32_s(&systime,&time32) && mCurrentDay != systime.tm_mday)
	{
		mLimitTeam.clear();
		mLimitMission.clear();
		mCurrentDay = systime.tm_mday;
	}
}

void CSocialManager::cleanupSocialCache(int playerId,int destPlayerId)
{
//	DO_LOCK( Lock::Player);

	//���˳��б������
	mQuitList.erase(playerId);
	mQuitList.erase(destPlayerId);

	//�����ڴ��е�����ϵ
	mSocialMap.erase(playerId);
	mSocialMap.erase(destPlayerId);

	//�����ڴ��еĺ��Ѷ�
	mFriendValueMap.erase(PlayerIdPair(playerId, destPlayerId));
}

void CSocialManager::TimeProcess(U32 bExit)
{
	updateInfo();
}

bool CSocialManager::deleteSocial(int playerId,int destPlayerId)
{
	TBLSocial tbSocial(0);
	tbSocial.AttachConn(m_pDBConn);
	DBError dbErr = tbSocial.DeleteTarget(playerId,destPlayerId);
	if(DBERR_NONE == dbErr)
		return true;

	//��¼��־
	g_Log.WriteError("ɾ������ϵplayerId=%d,destPlayerId=%d,err=%d",
		playerId,destPlayerId,getDBErr(dbErr));
	return false;
}

bool CSocialManager::isDataLoading(int playerId)
{
//	DO_LOCK(Lock::Player);
	
	return readDb(playerId);
}

void CSocialManager::RemoveLotList(int playerId,int sex /* = 0 */)
{
//	DO_LOCK(Lock::Player);

	if(SEX_BOTH == sex)
	{
		SimplePlayerData playerData;

		if(!SERVER->GetPlayerManager()->AnsyGetPlayerData(playerId,playerData))
		{
			return;
		}
		else
		{
			sex = playerData.Sex;
		}
	}
	
	PlayerList* playerList = NULL;
	if (SEX_MALE == sex) // �е�
	{
		playerList = &mMaleList;
	}
	else if (SEX_FEMALE == sex)// Ů��
	{
		playerList = &mFemaleList;
	}

	if(NULL == playerList)
		return;

	PlayerList::iterator it = playerList->begin();
	for (; it != playerList->end(); ++it)
	{
		if((*it) == playerId)
		{
			playerList->erase(it);
			return;
		}
	}
}

void CSocialManager::onPlayerEnter(int playerId)
{
	//���״̬
	PlayerStatusType::Type statusType = PlayerStatusType::Idle;

	mQuitList.erase(playerId);
	mPlayerStatusMap[playerId] = statusType;

	bool bFindDb = false;
	if(readDb(playerId))
	{
		bFindDb = true;
		notifyPlayerStatusChange(playerId, statusType);	
	}
}

void CSocialManager::onPlayerQuit(int playerId)
{
	mPlayerStatusMap[playerId] = PlayerStatusType::Offline;

	SocialMap::iterator it = mSocialMap.find(playerId);
	if(it == mSocialMap.end())
		return;

	mQuitList.insert(playerId);
	notifyPlayerStatusChange(playerId, PlayerStatusType::Offline);

	//����Ե�����Ƴ�
	RemoveLotList(playerId);
}

stSocialInfo CSocialManager::buildInfo(const stSocialItem& item,SimplePlayerData* data)
{
	stSocialInfo info;
	info.id          = item.playerId;
	info.type        = item.type;
	info.friendValue = item.friendValue;
	info.status      = mPlayerStatusMap[item.playerId];
	info.teamId      = 0;

	if(data)
	{
		info.sex       = data->Sex;
		info.level     = data->Level;
		info.family    = data->Family;
		info.job       = data->FirstClass;
		info.faceImage = 0;
		dStrcpy(info.name,sizeof(info.name),data->PlayerName);
	}
	return info;
}

void CSocialManager::changePlayerStatus(int playerId, PlayerStatusType::Type status)
{
//	DO_LOCK( Lock::Player);

	mPlayerStatusMap[playerId] = status;
}

bool CSocialManager::updateDb()
{
//	DO_LOCK( Lock::Player);

	if(mFriendValueMap.empty())
		return true;

	TBLSocial tbSocial(0);
	tbSocial.AttachConn(m_pDBConn);
	DBError dbErr = tbSocial.Update(mFriendValueMap);
	if(DBERR_NONE == dbErr)
	{
		mFriendValueMap.clear();
		return true;
	}

	g_Log.WriteWarn("���º��Ѷ� err = %d",getDBErr(dbErr));
	return false;
}

void CSocialManager::sendSocial(int playerId,bool bFlag /* = true */)
{
	//	DO_LOCK(Lock::Player);
	AnsyPlayerInfo playerInfo;

	if (!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
		return;
	}


	SocialMap::iterator it = mSocialMap.find(playerId);

	if(it != mSocialMap.end())
	{
		S32 iTotalNum = (S32)it->second.size() - 1;
		S32 iSocialNum = 0;

		{
			CMemGuard  Buffer(2016 MEM_GUARD_PARAM);
			Base::BitStream sendPacket(Buffer.get(), 2016);
			stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,"WC_SocialInfoResponse",playerInfo.accountId,SERVICE_CLIENT);
			sendPacket.writeInt(iTotalNum,  Base::Bit16);
			int nHeadPos = sendPacket.getPosition();

			SocialItemMap::iterator itr = it->second.begin();
			for (; itr != it->second.end(); ++itr, iSocialNum++)
			{
				if(iSocialNum == 0)
				{
					sendPacket.writeInt(iSocialNum, Base::Bit16);
				}

				int destPlayerId = itr->second.playerId;

				SimplePlayerData playerData;
				bool hasPlayerData = SERVER->GetPlayerManager()->AnsyGetPlayerData(destPlayerId,playerData);

				stSocialInfo info = buildInfo(itr->second,hasPlayerData ? &playerData : 0);
				info.WriteData(&sendPacket);

				//��Ϊ���Ѽ��
				/*if (isFriendType(info.type) && info.friendValue > 0)
				{
					int playerId1 = playerId;
					if( playerId1 > destPlayerId )
						std::swap( playerId, destPlayerId );

					sendPacket.writeInt(mLimitTeam[PlayerIdPair(playerId1,destPlayerId)],Base::Bit16);
					sendPacket.writeInt(mLimitMission[PlayerIdPair(playerId1,destPlayerId)],Base::Bit16);
				}*/

				//20��Ϊһ����
				if(iSocialNum % 20 == 0 && iSocialNum != 0)
				{
					pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
					SERVER->GetServerSocket()->Send(playerInfo.socket, sendPacket );
					sendPacket.setPosition(nHeadPos);
					sendPacket.writeInt(iSocialNum, Base::Bit16);
					info.WriteData(&sendPacket);
				}
			}

			if(iSocialNum > iTotalNum)
			{
				pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
				SERVER->GetServerSocket()->Send(playerInfo.socket, sendPacket );
			}
		}
	}
}

void CSocialManager::notifyPlayerStatusChange(int playerId,PlayerStatusType::Type status)
{
//	DO_LOCK( Lock::Player);

	//��������ϵ
	SocialMap::iterator it = mSocialMap.find(playerId);

	if(it == mSocialMap.end())
		return;

	SocialItemMap::iterator itr = it->second.begin();

	for(; itr != it->second.end(); itr++)
	{
        AnsyPlayerInfo playerInfo;
		if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(itr->first,playerInfo))
			continue;

		/*char buf[200];
		Base::BitStream sendPacket(buf,sizeof(buf));
        stPacketHead* pHead = IPacket::BuildPacketHead(sendPacket,WORLD_CLIENT_PlayerStatusChange,playerInfo.accountId,SERVICE_CLIENT,playerId,status);

		pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
        SERVER->GetServerSocket()->Send(playerInfo.socket,sendPacket);*/
	}
}

enSocialError CSocialManager::onNewAddPlayer(int playerId1,int playerId2,SocialType::Type type)
{
//	DO_LOCK( Lock::Player);

	stSocialItem stInfo;		
	stInfo.playerId = playerId2;
	stInfo.type = type;
	stInfo.friendValue = 0;

	bool bIsFind = false;
	SocialItemMap::iterator itr;
	if (isDataLoading(playerId2))
	{
		SocialMap::iterator it = mSocialMap.find(playerId2);
		if (it != mSocialMap.end())
		{
			itr = it ->second.find(playerId1);
			if (itr != it->second.end())
			{
				bIsFind = true;
				if(isFriendType(itr->second.type) && isFriendType(type))
					stInfo.friendValue = 1;
			}	
		}
	}
	
	if(saveDb(playerId1,stInfo))
	{
		SocialItemMap& itemMap = mSocialMap[playerId1];
		itemMap[playerId2] = stInfo;
		sendFriendInfo(playerId1,playerId2);

		if(bIsFind)
		{
			itr->second.friendValue = stInfo.friendValue;
			sendFriendInfo(playerId2,playerId1);
		}

		return SocialError_None;
	}

	return SocialError_DbError;
}

enSocialError CSocialManager::makeLink( int playerId1, int playerId2, SocialType::Type type )
{
	if(type >= SocialType::Count)
		return SocialError_Unknown;

	if(isBestFriendType(type))
		return SocialError_Unallowed;

	if(playerId1 == playerId2)
		return SocialError_Self;

//	DO_LOCK(Lock::Player);

	// �����Ȳ���һ��,���δ�ҵ�����ֱ�Ӳ�ѯһ�����ݿ�
	// ��worldServer�������رպ�δ�����������
	if (isDataLoading(playerId1))
	{
		SocialMap::iterator it = mSocialMap.find(playerId1);
		if (it != mSocialMap.end())
		{
			U8 nCount = 0;
			bool bIsFind = false;
			int  firstPlayerId = 0;
			stSocialItem findSocialItem;

			SocialItemMap& itemMap = it->second;
			SocialItemMap::iterator itr = itemMap.begin();
			for (; itr != itemMap.end(); ++itr)
			{
				if (!bIsFind && itr->second.playerId == playerId2)
				{
					bIsFind = true;
					findSocialItem = itr->second;
				}

				if (itr->second.type == type)
				{
					nCount++;
					if (firstPlayerId == 0)
						firstPlayerId = itr->second.playerId;
				}
			}

			// type is full, send error
			if (nCount >= SocialTypeMaxCount[type])
			{
				if( type != SocialType::Enemy && type != SocialType::Temp )
				{
					return SocialError_MaxCount; 
				}
				else
				{
					// �����˻���ʱ���������ʱ��ɾ��һ�����˻���ʱ����
					if( firstPlayerId != 0 && SocialError_None != destoryLink( playerId1, firstPlayerId ))
						return SocialError_DbError;
				}
			}

			if (bIsFind)
			{
				if (findSocialItem.type == type)
				{
					return SocialError_Existed;
				}
				else
				{
					if(!hasMakeLink(findSocialItem.type,type))
						return SocialError_Unallowed;
				}

				stSocialItem tempItem;
				tempItem.playerId = playerId2;
				tempItem.type = type;
				tempItem.friendValue = 0;

				bIsFind = false;
				if(isDataLoading(playerId2))
				{
					SocialMap::iterator tempIt = mSocialMap.find(playerId2);
					if (tempIt != mSocialMap.end())
					{
						itr = tempIt->second.find(playerId1);
						if (itr != tempIt->second.end())
						{
							bIsFind = true;
							if(isFriendType(itr->second.type) && isFriendType(type))
								tempItem.friendValue = 1;
						}	
					}
				}

				bool bDbNone = true;
				if(findSocialItem.type > SocialType::Temp)
					bDbNone = updateDb(playerId1,tempItem);
				else
					bDbNone = saveDb(playerId1,tempItem);

				if(bDbNone)
				{
					itemMap[playerId2] = tempItem;
					sendFriendInfo(playerId1,playerId2);

					if(bIsFind)
					{
						itr->second.friendValue = tempItem.friendValue;
						sendFriendInfo(playerId2,playerId1);
					}

					return SocialError_None;
				}

				return SocialError_DbError;
			}
		}
	}


	enSocialError enError = onNewAddPlayer(playerId1,playerId2,type);

	if(!SERVER->GetPlayerManager()->IsPlayerOnline(playerId2))
	{
		mQuitList.insert(playerId2);
	}

	return enError;
}

enSocialError CSocialManager::destoryLink(int playerId1,int playerId2)
{
	if (playerId1 ==  playerId2)
		return SocialError_Self;

//	DO_LOCK(Lock::Player);


	// �����Ȳ���һ��,���δ�ҵ�����ֱ�Ӳ�ѯһ�����ݿ�
	// ��worldServer�������رպ�δ�����������
	if (isDataLoading(playerId1))
	{
		SocialMap::iterator it = mSocialMap.find(playerId1);
		if(it == mSocialMap.end())
			return SocialError_NotFound;

		SocialItemMap::iterator itr = it->second.find(playerId2);
		if(itr == it->second.end())
			return SocialError_NotFound;

		SocialType::Type type = (SocialType::Type) itr->second.type;
		if(type == SocialType::Friend || type == SocialType::Mute || type == SocialType::Temp || type == SocialType::Enemy)
		{
			if(deleteSocial(playerId1,playerId2))
			{
				it->second.erase(itr);
				sendFriendInfo(playerId1,playerId2);
				if (setFriendValue(playerId2,playerId1,0))
					sendFriendInfo(playerId2,playerId1);

				return SocialError_None;
			}

			return SocialError_DbError;
		}
	}
	
	return SocialError_Unallowed;
}

void CSocialManager::destoryAllLink(int playerId)
{
//	DO_LOCK(Lock::Player);

	if (isDataLoading(playerId))
	{
		SocialMap::iterator it = mSocialMap.find(playerId);
		if(it == mSocialMap.end())
			return;

		SocialItemMap::iterator itr = it->second.begin();
		for (; itr != it->second.end(); ++itr)
		{
			int destPlayerId = itr->second.playerId;
			if (isDataLoading(destPlayerId))
			{
				SocialMap::iterator it1 = mSocialMap.find(destPlayerId);
				if (it1 != mSocialMap.end())
				{
					SocialItemMap::iterator itr1 = it1->second.find(playerId);
					if (itr1 != it1->second.end())
					{
						it1->second.erase(itr1);
						continue;
					}
				}
			}

		}

		mSocialMap.erase(it);

		TBLSocial tbSocial(0);
		tbSocial.AttachConn(m_pDBConn);
		DBError dbErr = tbSocial.DeleteAll(playerId);
		if(DBERR_NONE == dbErr)
			return;

		//��¼��־
		g_Log.WriteError("���ɾ�Ŵ���,ɾ�����Ѻ��Լ���������������ϵplayerId=%d,err=%d",
			playerId,getDBErr(dbErr));
	}
}

int CSocialManager::addFriendValue(int playerId1,int playerId2,int value)
{
//	DO_LOCK( Lock::Player);

	SocialMap::iterator it1,it2;
	it1 = mSocialMap.find(playerId1);
	it2 = mSocialMap.find(playerId2);
	if(it1 == mSocialMap.end() || it2 == mSocialMap.end())
		return 0;

	SocialItemMap::iterator itr1,itr2;
	itr1 = it1->second.find(playerId2);
	itr2 = it2->second.find(playerId1);
	if(itr1 == it1->second.end() || itr2 == it2->second.end())
		return 0;

	if(!isFriendType(itr1->second.type)|| !isFriendType(itr2->second.type))
		return 0;

	itr1->second.friendValue += value;
	itr2->second.friendValue = itr1->second.friendValue;

	
	//��¼�б�,�Ա�ͳһ����
	if( playerId1 > playerId2 )
		std::swap( playerId1, playerId2 );

	mFriendValueMap[PlayerIdPair(playerId1, playerId2)] = itr1->second.friendValue;
	return value;
}

int CSocialManager::addFriendValueV1(int playerId1,int playerId2,int value)
{
//	DO_LOCK( Lock::Player );

	if( playerId1 > playerId2 )
		std::swap( playerId1, playerId2 );

	int& oldValue = mLimitTeam[PlayerIdPair(playerId1, playerId2)];

	if( oldValue <= LimintFriendValue_Kill )
	{
		int tempValue = oldValue;
		oldValue = mClamp(oldValue+value,0,LimintFriendValue_Kill);
		return addFriendValue( playerId1, playerId2, oldValue - tempValue );
	}

	return 0;
}

int CSocialManager::addFriendValueV2(int playerId1,int playerId2,int value)
{
//	DO_LOCK( Lock::Player );

	if( playerId1 > playerId2 )
		std::swap( playerId1, playerId2 );

	int& oldValue = mLimitMission[PlayerIdPair(playerId1, playerId2)];

	if( oldValue <= LimintFriendValue_Mission )
	{
		int tempValue = oldValue;
		oldValue = mClamp(oldValue+value,0,LimintFriendValue_Mission);
		return addFriendValue( playerId1, playerId2, oldValue - tempValue );
	}

	return 0;
}

bool CSocialManager::hasFriend(const SocialItemMap& itemMap,int destPlayerId)
{
//	DO_LOCK( Lock::Player);
	SocialItemMap::const_iterator it = itemMap.find(destPlayerId);
	if(it != itemMap.end())
		return isFriendType(it->second.type);

	return false;
}

bool CSocialManager::hasFriend(int playerId1,int playerId2)
{
//	DO_LOCK(Lock::Player);
	SocialMap::iterator it1,it2;
	it1 = mSocialMap.find(playerId1);
	it2 = mSocialMap.find(playerId2);
	if(it1 == mSocialMap.end() || it2 == mSocialMap.end())
		return false;

	SocialItemMap::iterator itr1,itr2;
	itr1 = it1->second.find(playerId2);
	itr2 = it2->second.find(playerId1);
	if(itr1 == it1->second.end() || itr2 == it2->second.end())
		return false;

	return true;
}

bool CSocialManager::buddyFriend(const SocialItemMap& itemMap,int destPlayerId)
{
//	DO_LOCK( Lock::Player);
	SocialItemMap::const_iterator it = itemMap.find(destPlayerId);
	if(it != itemMap.end())
	{
		if (!isFriendType(it->second.type))
			return false;

		if(it->second.friendValue > 0)
			return true;
	}

	return false;
}
bool CSocialManager::buddyFriend(int playerId1,int playerId2)
{
//	DO_LOCK(Lock::Player);
	SocialMap::iterator it1,it2;
	it1 = mSocialMap.find(playerId1);
	it2 = mSocialMap.find(playerId2);
	if(it1 == mSocialMap.end() || it2 == mSocialMap.end())
		return false;

	SocialItemMap::iterator itr1,itr2;
	itr1 = it1->second.find(playerId2);
	itr2 = it2->second.find(playerId1);
	if(itr1 == it1->second.end() || itr2 == it2->second.end())
		return false;

	//����Ѷ�����һ��
	if(itr1->second.friendValue > 0)
		return true;

	return false;
}

int CSocialManager::queryFriendValue(int playerId1,int playerId2)
{
//	DO_LOCK(Lock::Player);

	SocialMap::iterator it = mSocialMap.find(playerId1);
	if(it == mSocialMap.end())
		return 0;

	SocialItemMap::iterator itr = it->second.find(playerId2);
	if(itr == it->second.end())
		return 0;

	return itr->second.friendValue;
}

bool CSocialManager::isPlayerBlackListed(int playerId1,int playerId2)
{
//	DO_LOCK(Lock::Player);

	SocialMap::iterator it = mSocialMap.find(playerId1);
	if(it == mSocialMap.end())
		return false;

	SocialItemMap::iterator itr = it->second.find(playerId2);
	if(itr == it->second.end())
		return false;

	if(itr->second.type == SocialType::Mute)
		return true;

	return false;
}

bool CSocialManager::setFriendValue(int playerId1,int playerId2,int value)
{
	if(value < 0)
		return false;

//	DO_LOCK( Lock::Player);

	// �����Ȳ���һ��,���δ�ҵ�����ֱ�Ӳ�ѯһ�����ݿ�
	// ��worldServer�������رպ�δ�����������
	if(!isDataLoading(playerId1) || !isDataLoading(playerId2))
	{
		g_Log.WriteWarn("���id1[%d]��id2[%d]δ�ɹ��޸ĺ��Ѷ�,���Ҳ�����Ӧ��ϵ",playerId1,playerId2);
		return false;
	}

	SocialMap::iterator it1,it2;
	it1 = mSocialMap.find(playerId1);
	it2 = mSocialMap.find(playerId2);
	if(it1 == mSocialMap.end() || it2 == mSocialMap.end())
		return false;

	SocialItemMap::iterator itr1,itr2;
	itr1 = it1->second.find(playerId2);
	itr2 = it2->second.find(playerId1);
	if(itr1 == it1->second.end() || itr2 == it2->second.end())
		return false;

	if(!isFriendType(itr1->second.type)|| !isFriendType(itr2->second.type))
		return false;

	LimitMap tempMap;
	tempMap[PlayerIdPair(playerId1, playerId2)] = value;

	TBLSocial tbSocial(0);
	tbSocial.AttachConn(m_pDBConn);
	DBError dbErr = tbSocial.Update(tempMap);
	if(DBERR_NONE == dbErr)
	{
		itr1->second.friendValue = value;
		itr2->second.friendValue = value;
		return true;
	}

	g_Log.WriteError("�޸����playerId=[%d]��destPlayerId=[%d]���Ѷ�ʧ�� err = %d",\
		playerId1,playerId2,getDBErr(dbErr));

	return false;
}

void CSocialManager::sendFriendInfo(int playerId, int destPlayerId)
{
	if(playerId == destPlayerId)
		return;

    AnsyPlayerInfo playerInfo;

	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
		return;

	SocialMap::iterator it = mSocialMap.find(playerId);
	MAKE_PACKET(sendPacket, "WC_FriendInfo", playerInfo.accountId, SERVICE_CLIENT);
	if(it != mSocialMap.end())
	{
		SocialItemMap::iterator itr = it->second.find(destPlayerId);
		if(itr != it->second.end())
		{
			sendPacket.writeFlag(true);
			sendPacket.writeInt(destPlayerId,Base::Bit32);

			SimplePlayerData playerData;
			bool hasPlayerData = SERVER->GetPlayerManager()->AnsyGetPlayerData(itr->second.playerId,playerData);

			stSocialInfo info = buildInfo(itr->second,hasPlayerData ? &playerData : 0);
			info.WriteData(&sendPacket);

			SEND_PACKET(sendPacket, playerInfo.socket);
			return;
		}
	}

	sendPacket.writeFlag(false);
	sendPacket.writeInt(destPlayerId, Base::Bit32);
	SEND_PACKET(sendPacket, playerInfo.socket);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           ��Ϣ����                                                                            //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSocialManager::CSocialManager(CDBConnPool* pConnPool) 
:  m_pDBConn(pConnPool->Lock())
{
	tm systime;
	__time32_t time32 = _time32(NULL);
	if(!_localtime32_s(&systime,&time32))
		mCurrentDay = systime.tm_mday;

	registerEvent("CW_MakeLinkRequest",				    &CSocialManager::HandleMakeRequest );
	registerEvent("CW_SocialInfoRequest",				&CSocialManager::HandleWorldSocialInfoRequest	);
	registerEvent("CW_DestoryLinkRequest",			    &CSocialManager::HandleDestoryRequest );
	/*registerEvent(ZONE_WORLD_QueryFriendRequest,	    &CSocialManager::HandleZoneWorldFindFriendRequest	);
	registerEvent(ZONE_WORLD_FindPlayerRequest,	        &CSocialManager::HandleZoneWorldFindPlayerRequest );
	registerEvent(ZONE_WORLD_ChangeLinkRequest,  	    &CSocialManager::HandleZoneWorldChangeLinkReuquest	);
	registerEvent(ZONE_WORLD_AddFriendValue,		    &CSocialManager::HandleZoneWorldAddFriendValue		);
	registerEvent(ZONE_WORLD_PlayerStatusChangeRequest, &CSocialManager::HandleZoneWorldPlayerStatusChangeRequest);
	registerEvent(ZONE_WORLD_LotRequest,                &CSocialManager::HandleZoneWorldLotRequest );
	registerEvent(ZONE_WORLD_AddToLotRequest,           &CSocialManager::HandleZoneWorldAddLotRequest );
	registerEvent(ZONE_WORLD_RemoveFromLotRequest,      &CSocialManager::HandleZoneWorldRemoveLotRequest );
	registerEvent(ZONE_WORLD_MasterSocialRequest,       &CSocialManager::HandleZoneWorldMasterSocialRequest );
	registerEvent(ZONE_WORLD_PlayerMasterSaveRequest,   &CSocialManager::HandleZoneWorldPlayerMasterSaveRequest );
	registerEvent(ZONE_WORLD_PrenticeListRequest,       &CSocialManager::HandleZoneQueryPrenticeListRequest );
	registerEvent(Zone_WORLD_AddFriendRequest,          &CSocialManager::HandleZoneAddFriendRequest );
	registerEvent(ZONE_WORLD_UseItem_AddFriendValueReq, &CSocialManager::HandleZoneUseItemAddFriendValueRequest);
	registerEvent(ZONE_WORLD_List_AddFriendValue,       &CSocialManager::HandleZonePlayerListAddFriendRequest);
	registerEvent(ZONE_WORLD_NotFriendClearRequest,     &CSocialManager::HandleZoneNotFriendClearRequest);*/

	m_60Timer.Start();
}

CSocialManager::~CSocialManager(void)
{

}

// ����Zone�ı����״̬������
/*void CSocialManager::EVENT_FUNCTION( HandleZoneWorldPlayerStatusChangeRequest )
{
//	DO_LOCK( Lock::Player);

	int playerId = pHead->Id;
	U8 statusType = (U8)pHead->DestZoneId;

	changePlayerStatus(playerId,(PlayerStatusType::Type)statusType);
}

void CSocialManager::EVENT_FUNCTION(HandleZoneAddFriendRequest)
{
//	DO_LOCK( Lock::Player);
	int playerId = pHead->Id;
	int destPlayerId = pHead->DestZoneId;

	SERVER->GetPlayerManager()->SendSimpleInfoPacket(destPlayerId,WORLD_CLIENT_SocialAdded,playerId,AddFriendRequest);
}

void CSocialManager::EVENT_FUNCTION(HandleZoneUseItemAddFriendValueRequest)
{
//	DO_LOCK(Lock::Player);
	int playerId     = pHead->Id;
	int itemId       = Packet->readInt(Base::Bit32);
	int iFriendValue = Packet->readInt(Base::Bit32);
	int destPlayerId = Packet->readInt(Base::Bit32);

	bool bFlag = false;
	if(isDataLoading(playerId) && isDataLoading(destPlayerId))
	{
		int iCurrFriendValue = queryFriendValue(playerId,destPlayerId);
		if(buddyFriend(playerId,destPlayerId) && iCurrFriendValue >0)
		{
			bFlag = setFriendValue(playerId,destPlayerId,iCurrFriendValue + iFriendValue);
		}
	}

	if(!bFlag)
	{
		//��Ʒ���ʼ��Ļ�
		char* title = "ϵͳ�ʼ�";
		char* szMsg = "���øж�ʧ�ܣ����ص���";
		SERVER->GetMailManager()->sendMail(0,playerId,0,itemId,1,title,szMsg,true);

		g_Log.WriteWarn("ʹ����Ʒ���Ӻøж�,��Ҳ������Ҳ���ʧ��,��Ʒ���ʼ���ʽ���� \
			playerId=%d,destPlayerId=%d,itemId=%d,friendValue=%d",playerId,destPlayerId,itemId,iFriendValue);

		return;
	}
	
	int ZoneId = 0;
	int socketId = 0;
	char buf[256];
	Base::BitStream sendPacket(buf,sizeof(buf));

	//֪ͨ�Լ�
    AnsyPlayerInfo playerInfo;

	if(SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
        ZoneId   = playerInfo.zoneId;
        socketId = SERVER->GetServerManager()->GetGateSocket(playerInfo.lineId,ZoneId);

		stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,WORLD_ZONE_UseItem_AddFriendValueRes,playerId,SERVICE_ZONESERVER,ZoneId);
		sendPacket.writeFlag(true);
		sendPacket.writeInt(iFriendValue,Base::Bit32);
		sendPacket.writeInt(destPlayerId,Base::Bit32);

		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send(socketId, sendPacket);
	}

	//֪ͨ��Ŀ�����
    AnsyPlayerInfo destAccount;

	if(SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(destPlayerId,destAccount))
	{
        ZoneId   = destAccount.zoneId;
        socketId = SERVER->GetServerManager()->GetGateSocket(destAccount.lineId,ZoneId);
		sendPacket.setPosition(0);

		stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,WORLD_ZONE_UseItem_AddFriendValueRes,destPlayerId,SERVICE_ZONESERVER,ZoneId);
		sendPacket.writeFlag(false);
		sendPacket.writeInt(iFriendValue,Base::Bit32);
		sendPacket.writeInt(playerId,Base::Bit32);

		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send(socketId, sendPacket);
	}

	g_Log.WriteLog("ʹ����Ʒ���Ӻ��Ѷ�playerId=%d,destPlayerId=%d,itemId=%d,friendValue=%d,bFlag=%d",
		playerId,destPlayerId,itemId,iFriendValue);
}

void CSocialManager::EVENT_FUNCTION( HandleZonePlayerListAddFriendRequest )
{
	int playerId = pHead->Id;
	int iSize = pHead->DestZoneId;
	
	if(!isDataLoading(playerId))
		return;

	for (int i=0; i<iSize; i++)
	{
		int destPlayerId = Packet->readInt(Base::Bit32);
		int iType        = Packet->readInt(Base::Bit8);
		int iFriendValue = Packet->readInt(Base::Bit32);
		if(iFriendValue == 0 || iType < FriendValue_Non || iType > FriendValue_Mission)
			continue;

		if(!isDataLoading(destPlayerId))
			continue;

		if (iType == FriendValue_Kill)
			addFriendValueV1( playerId, destPlayerId, iFriendValue );
		else if (iType == FriendValue_Mission)
			addFriendValueV2( playerId, destPlayerId, iFriendValue );
		else
			addFriendValue( playerId, destPlayerId, iFriendValue );
	}
}

void CSocialManager::EVENT_FUNCTION( HandleZoneNotFriendClearRequest )
{
	int playerId = pHead->Id;
	int iSocialType = pHead->DestZoneId;

//	DO_LOCK(Lock::Player);
	if (isFriendType(iSocialType) || !isDataLoading(playerId))
		return;

	SocialMap::iterator it = mSocialMap.find(playerId);
	if (it == mSocialMap.end())
		return;

	SocialItemMap::iterator itr = it->second.begin();
	while(itr != it->second.end())
	{
		if (itr->second.type == iSocialType)
			itr = it->second.erase(itr);
		else
			++itr;
	}
}*/

bool CSocialManager::EVENT_FUNCTION( HandleMakeRequest )
{
//	DO_LOCK(Lock::Player);

	int playerId = pHead->Id;
	int destPlayerId = pHead->DestZoneId;
    
    AnsyPlayerInfo playerInfo;

	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
		return false;
	}

	int nError = SocialError_NotFound;
	SocialType::Type type = (SocialType::Type) pHead->SrcZoneId;
	
	SimplePlayerData playerData;
	bool bFlag = SERVER->GetPlayerManager()->AnsyGetPlayerData(destPlayerId,playerData);

	if(bFlag)
	{
		nError = makeLink(playerId, destPlayerId, type);
	}

	//��ʱ��δ�ҵ��Ļ���������
	if(type == SocialType::Temp && nError == SocialError_NotFound)
		return false;

	char buf[256];
	Base::BitStream sendPacket(buf,sizeof(buf));
    stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WC_MakeLinkResponse", playerInfo.accountId, SERVICE_CLIENT,nError, pHead->DestZoneId, type);
	
	//��������
	if(nError == SocialError_None)
	{
		if(sendPacket.writeFlag(bFlag))
		{
			sendPacket.writeString(playerData.PlayerName,MAX_NAME_LENGTH);
		}
	}

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
    SERVER->GetServerSocket()->Send(playerInfo.socket,sendPacket);

	if(nError == SocialError_None && type == SocialType::Friend)
	{
		if(!SERVER->GetPlayerManager()->IsPlayerOnline(destPlayerId))
			return false;

		if(buddyFriend(playerId,destPlayerId))
			return false;

		SERVER->GetPlayerManager()->SendSimpleInfoPacket(destPlayerId, "WC_SocialAdded", playerId, AddFriendnotify);
	}
	return true;
}


bool CSocialManager::EVENT_FUNCTION( HandleDestoryRequest )
{
//	DO_LOCK(Lock::Player);

	int playerId = pHead->Id;
	int destPlayerId = pHead->DestZoneId;

	AnsyPlayerInfo playerInfo;

	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
		return false;
	}

	int nError = destoryLink(playerId, destPlayerId);

	char buf[256];
	Base::BitStream sendPacket(buf,sizeof(buf));
    stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WC_DestoryLinkResponse",playerInfo.accountId,SERVICE_CLIENT,nError);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

    SERVER->GetServerSocket()->Send(playerInfo.socket, sendPacket);
	return true;
}

// �����������ϵ�б�����
bool CSocialManager::EVENT_FUNCTION( HandleWorldSocialInfoRequest )
{
	//	DO_LOCK( Lock::Player);

	int playerId = pHead->Id;

	mQuitList.erase(playerId);
	if(isDataLoading(playerId))
	{
		sendSocial(playerId,false);
	}

	return true;
}

/*
void CSocialManager::EVENT_FUNCTION( HandleZoneWorldMasterSocialRequest )
{
	int playerId = pHead->Id;

	AnsyPlayerInfo playerInfo;

	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
		return;
	}

	//�ݸ��ò�ѯ����ϵ�ṹ
	int destPlayerId = pHead->DestZoneId;
	int nError       = SocialError_NotFound;
	char* szbuf      = new char[MAX_PACKET_SIZE];

	Base::BitStream sendPacket(szbuf, MAX_PACKET_SIZE);
    stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, WORLD_CLIENT_MasterSocialResponse, playerInfo.accountId, SERVICE_CLIENT);

	//��ȡָ���������
	bool bIsLoad = isDataLoading(destPlayerId);
	if(bIsLoad || (playerId == destPlayerId))
	{
		nError = SocialError_None;

		int nFriendValue = 0;
		std::vector<int> tempPlayerList;
		tempPlayerList.push_back(destPlayerId);
	
		SocialMap::iterator it = mSocialMap.find(destPlayerId);
		if(it != mSocialMap.end())
		{
			SocialItemMap::iterator itr = it->second.begin();
			while(itr != it->second.end())
			{
				//ͳ��ͽ������
				if(itr->second.type == SocialType::Prentice)
					tempPlayerList.push_back(itr->second.playerId);

				//�鿴ͽ��,������Ѷ�
				if(itr->second.type == SocialType::Master)
					nFriendValue = itr->second.friendValue;

				++itr;
			}
		}

		int size = MAX_PACKET_SIZE;
		char* buf = new char[MAX_PACKET_SIZE];
		char* zipBuf = new char[MAX_PACKET_SIZE];
		Base::BitStream dataStream(buf, MAX_PACKET_SIZE);

		int nSize = (int)tempPlayerList.size();
		dataStream.writeInt(nSize - 1,Base::Bit8);//ͽ������
		dataStream.writeInt(nSize,Base::Bit8);
		dataStream.writeInt(nFriendValue,Base::Bit32);

		//��ȡ����
		PlayerSimpleDataRef stPlayerData;
		for(int i=0; i<nSize; i++)
		{
			int playerId = tempPlayerList[i];

			SimplePlayerData playerData;
			bool flage = SERVER->GetPlayerManager()->AnsyGetPlayerData(playerId,playerData);

			if (dataStream.writeFlag(flage))
			{
				playerData.WriteData(&dataStream);

				//״̬
				dataStream.writeInt(mPlayerStatusMap[playerId],Base::Bit8);
			}
		}

		int iret = SXZ_COMPRESS(zipBuf, &size, buf, dataStream.getPosition());
		if(sendPacket.writeFlag(!iret))
		{
			sendPacket.writeInt(size,Base::Bit32);
			sendPacket.writeBits(size << 3, zipBuf);
		}

		SAFE_DELETE_ARRAY(buf);
		SAFE_DELETE_ARRAY(zipBuf)
	}

	pSendHead->DestZoneId = nError;
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
    SERVER->GetServerSocket()->Send(playerInfo.socket,sendPacket);

	//�Ƴ�����
	if(playerId != destPlayerId)
	{
		if(!SERVER->GetPlayerManager()->IsPlayerOnline(destPlayerId))
		{
			mQuitList.insert(destPlayerId);
		}
	}

	SAFE_DELETE_ARRAY(szbuf);
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldAddLotRequest )
{
//	DO_LOCK(Lock::Player);

	int nPlayerId = pHead->Id;
	int iSex = pHead->DestZoneId;

	PlayerList* playerList = NULL;
	if (SEX_MALE == iSex) // �е�
	{
		playerList = &mMaleList;
	}
	else if (SEX_FEMALE == iSex)// Ů��
	{
		playerList = &mFemaleList;
	}

	if(NULL == playerList)
		return;

	PlayerList::iterator it = playerList->begin();
	for (; it != playerList->end(); ++it)
	{
		if((*it) == nPlayerId)
			return;
	}

	playerList->push_back(nPlayerId);
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldRemoveLotRequest )
{
	int nPlayerId = pHead->Id;
	int iSex = pHead->DestZoneId;
	RemoveLotList(nPlayerId,iSex);
}

// ����������Ե������
void CSocialManager::EVENT_FUNCTION( HandleZoneWorldLotRequest )
{
	// ����ҵ�ĳ���ȼ���ʱ��Ѱ��������Ե��
	int nPlayerId = pHead->Id;
	int iSex      = pHead->DestZoneId;
    
    AnsyPlayerInfo playerInfo;

	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(nPlayerId,playerInfo))
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",nPlayerId,__FUNCTION__,__LINE__);
		return;
	}

	PlayerList* playerList = NULL;

	if (SEX_MALE == iSex) // �е�
	{
		playerList = &mFemaleList;
	}
	else if (SEX_FEMALE == iSex)// Ů��
	{
		playerList = &mMaleList;
	}

	if( NULL == playerList || playerList->empty())
		return;

	int nDestPlayerId = 0;
	bool bFindPlayer = false;

	PlayerList::iterator it = playerList->begin();
	for (; it != playerList->end(); ++it)
	{
		nDestPlayerId = (*it);
        
		if(!SERVER->GetPlayerManager()->IsPlayerOnline(nDestPlayerId))
			continue;

		// �������˻����Ϊ����
		enSocialError error = SocialError_None;
		if(!buddyFriend(nPlayerId,nDestPlayerId))
		{
			if(!hasFriend(nPlayerId,nDestPlayerId))
			{
				error = makeLink(nPlayerId, nDestPlayerId, SocialType::Friend);
				if(error != SocialError_None)
					continue;
			}

			if(!hasFriend(nDestPlayerId,nPlayerId))
			{
				error = makeLink(nDestPlayerId, nPlayerId, SocialType::Friend);
				if(error != SocialError_None)
				{
					continue;
				}
				else
				{
					bFindPlayer = true;
					break;
				}
			}
			else
			{
				bFindPlayer = true;
				break;
			}
		}
		else
		{
			bFindPlayer = true;
			break;
		}
	}

	//δƥ��ɹ�
	if(!bFindPlayer)
		return;

	//���ú��Ѷ�
	setFriendValue(nPlayerId,nDestPlayerId,100);

	// ֪ͨ�ߵȼ����;
    AnsyPlayerInfo destAccount;

	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(nDestPlayerId,destAccount))
		return;

    int nZoneId  = destAccount.zoneId;
    int socketId = SERVER->GetServerManager()->GetGateSocket(destAccount.lineId,nZoneId);

	char buf[256];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, ZONE_WORLD_LotResponse, nDestPlayerId, SERVICE_ZONESERVER, nZoneId, nPlayerId);

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send(socketId, sendPacket);

	//֪ͨ�͵ȼ����
    socketId = SERVER->GetServerManager()->GetGateSocket(playerInfo.lineId,playerInfo.zoneId);

	sendPacket.setPosition(0);
	pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_LotNotify, nPlayerId, SERVICE_ZONESERVER, playerInfo.zoneId, nDestPlayerId );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send(socketId, sendPacket);

	//��ɶԳɹ�������ɾ��
	playerList->erase(it);
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldFindFriendRequest )
{
//	DO_LOCK(Lock::Player);

	
	MAKE_WORKQUEUE_PACKET(sendPacket, 256);
	sendPacket.writeInt(pHead->Id, Base::Bit32);
	sendPacket.writeInt(pHead->DestZoneId, Base::Bit32);
	sendPacket.writeInt(pHead->SrcZoneId, Base::Bit8);

        
    //д����ҵľ���������
    ChallengeMember* pMember = SERVER->GetChallengeManager()->GetMember(pHead->DestZoneId);

    if (0 != pMember)
    {
        sendPacket.writeInt(pMember->rank + 1, Base::Bit32);
    }
    else
    {
        sendPacket.writeInt(9999, Base::Bit32);
    }

	SEND_WORKQUEUE_PACKET(sendPacket, WQ_ORG, OWQ_QueryFriendRequest);
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldAddFriendValue )
{
	int playerId1 = pHead->Id;
	int playerId2 = pHead->DestZoneId;
	int value     = pHead->SrcZoneId;

	if(!SERVER->GetPlayerManager()->IsPlayerOnline(playerId1) || 
       !SERVER->GetPlayerManager()->IsPlayerOnline(playerId2))
	{
		return;
	}

	// �����Ȳ���һ��,���δ�ҵ�����ֱ�Ӳ�ѯһ�����ݿ�
	// ��worldServer�������رպ�δ�����������
	if(!isDataLoading(playerId1) || !isDataLoading(playerId2))
	{
		g_Log.WriteWarn("���id1[%d]��id2[%d]δ���Ϻ��Ѷ�,���Ҳ�����Ӧ��ϵ",playerId1,playerId2);
		return;
	}

	int zoneId = Packet->readInt(Base::Bit32);

	int iAddValue = iAddValue = addFriendValue(playerId1,playerId2,value);

	if(iAddValue == 0)
		return;

	MAKE_PACKET( sendPacket, ZONE_WORLD_AddFriendValue, playerId1, SERVICE_ZONESERVER,zoneId, playerId2  );
	sendPacket.writeInt( iAddValue, 32 );
	SEND_PACKET( sendPacket, SocketHandle );

	MAKE_PACKET( sendPacket1, ZONE_WORLD_AddFriendValue, playerId2, SERVICE_ZONESERVER, zoneId, playerId1  );
	sendPacket1.writeInt( iAddValue, 32 );
	SEND_PACKET( sendPacket1, SocketHandle );
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldChangeLinkReuquest )
{
//	DO_LOCK( Lock::Player);

	int playerId = pHead->Id;
	int destPlayerId = pHead->DestZoneId;

	bool bClearValue = Packet->readFlag();
	int thisNewType = Packet->readInt(Base::Bit8);
	int destNewType = Packet->readInt(Base::Bit8);
	//���ڼ�¼��־
	int thisOldType = SocialType::Temp;
	int destOldType = SocialType::Temp;

	enSocialError error = SocialError_NotFound;
	// �����Ȳ���һ��,���δ�ҵ�����ֱ�Ӳ�ѯһ�����ݿ�
	// ��worldServer�������رպ�δ�����������
	if (isDataLoading(playerId))
	{
		SocialMap::iterator it = mSocialMap.find(playerId);
		if (it != mSocialMap.end())
		{
			SocialItemMap::iterator itr = it->second.find(destPlayerId);
			if (itr != it->second.end())
			{
				thisOldType = itr->second.type;
				stSocialItem stInfo;
				if(isBestFriendType(itr->second.type) && thisNewType == SocialType::Friend)
				{
					stInfo.playerId = destPlayerId;
					stInfo.type = thisNewType;
					if(bClearValue)
						stInfo.friendValue = 1;
					else
						stInfo.friendValue = itr->second.friendValue;

					error = SocialError_None;

					if (updateDb(playerId,stInfo,destNewType))
					{
						itr->second = stInfo;

						SocialMap::iterator tempIt = mSocialMap.find(destPlayerId);
						if (tempIt != mSocialMap.end())
						{
							SocialItemMap::iterator tempItr = tempIt->second.find(playerId);
							if (tempItr != tempIt->second.end())
							{
								destOldType = tempItr->second.type;
								tempItr->second.type = destNewType;
								tempItr->second.friendValue = stInfo.friendValue;
							}
						}
					}
					else
					{
						error = SocialError_DbError;
					}
				}
				else
				{
					stInfo.playerId = destPlayerId;
					stInfo.type = thisNewType;
					stInfo.friendValue = itr->second.friendValue;
					error = SocialError_None;

					if (updateDb(playerId,stInfo,destNewType))
					{
						itr->second = stInfo;

						SocialMap::iterator tempIt = mSocialMap.find(destPlayerId);
						if (tempIt != mSocialMap.end())
						{
							SocialItemMap::iterator tempItr = tempIt->second.find(playerId);
							if (tempItr != tempIt->second.end())
							{
								destOldType = tempItr->second.type;
								tempItr->second.type = destNewType;
							}
						}
					}
					else
					{
						error = SocialError_DbError;
					}
				}
			}
		}
	}

	//��¼��־
	GameplayLog::GL_PlayerChangeSocial(playerId,destPlayerId,thisOldType,thisNewType,destOldType,destNewType);
	
	char buff[1024];
	stPacketHead* pSendHead = NULL;
	Base::BitStream sendPacket(buff,sizeof(buff));

	SimplePlayerData playerData;

    AnsyPlayerInfo playerInfo;

	if(SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
        int zoneId = playerInfo.zoneId;
		stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,ZONE_WORLD_ChangeLinkResponse,playerId,SERVICE_ZONESERVER,zoneId,destPlayerId,error);

		sendPacket.writeFlag(true);   //���÷���������
		sendPacket.writeFlag(bClearValue);
		sendPacket.writeInt(thisNewType,Base::Bit8);

		bool hasPlayerData = SERVER->GetPlayerManager()->AnsyGetPlayerData(destPlayerId,playerData);

		if(!hasPlayerData)
			sendPacket.writeString("",COMMON_STRING_LENGTH);
		else
			sendPacket.writeString(playerData.PlayerName,COMMON_STRING_LENGTH);

		pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);

		SERVER->GetServerSocket()->Send(SocketHandle,sendPacket);
	}
	else
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
	}

	if(SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(destPlayerId,playerInfo))
	{
		sendPacket.setPosition(0);
        int zoneId   =  playerInfo.zoneId;
        int socketId = SERVER->GetServerManager()->GetGateSocket(playerInfo.lineId,zoneId);
		pSendHead = IPacket::BuildPacketHead(sendPacket,ZONE_WORLD_ChangeLinkResponse,destPlayerId,SERVICE_ZONESERVER,zoneId,playerId,error);

		sendPacket.writeFlag(false);   //���÷���������
		sendPacket.writeFlag(bClearValue);
		sendPacket.writeInt(destNewType,Base::Bit8);

		bool hasPlayerData = SERVER->GetPlayerManager()->AnsyGetPlayerData(playerId,playerData);

		if(!hasPlayerData)
			sendPacket.writeString("",Base::Bit32);
		else
			sendPacket.writeString(playerData.PlayerName,Base::Bit32);

		pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
		SERVER->GetServerSocket()->Send(socketId,sendPacket);
	}
	else
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
	}
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldFindPlayerRequest )
{
	int playerId     = pHead->Id;
	int destPlayerId = pHead->DestZoneId;
	int iAction      = Packet->readInt(Base::Bit8);

    AnsyPlayerInfo playerInfo;

	if (!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
		return ;
	}

	bool isLoad = false;

	SimplePlayerData playerData;

	if (destPlayerId == 0)
	{
		char playerName[COMMON_STRING_LENGTH];
		Packet->readString( playerName,COMMON_STRING_LENGTH );

		isLoad = SERVER->GetPlayerManager()->AnsyGetPlayerData(playerName,playerData);
	}
	else
	{
		isLoad = SERVER->GetPlayerManager()->AnsyGetPlayerData(destPlayerId,playerData);
	}

	char buf[512] = {0,};
	Base::BitStream sendPacket( buf, sizeof(buf));
    stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WORLD_CLIENT_FindPlayerResponse, playerInfo.accountId, SERVICE_CLIENT, SocialError_None );

	if(isLoad)
	{
		sendPacket.writeInt(iAction,Base::Bit8);
		sendPacket.writeInt(playerData.PlayerId,Base::Bit32);
		sendPacket.writeInt(mPlayerStatusMap[playerData.PlayerId],Base::Bit8);
		sendPacket.writeString(playerData.PlayerName,COMMON_STRING_LENGTH);
	}
	else
	{
		pSendHead->DestZoneId = SocialError_NotFound;
	}

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
    SERVER->GetServerSocket()->Send( playerInfo.socket, sendPacket );
}

void CSocialManager::EVENT_FUNCTION(HandleZoneWorldPlayerMasterSaveRequest)
{
//	DO_LOCK(Lock::Player);

	int nError = SocialError_None;
	int playerId = pHead->Id;
	int masterPlayerId = pHead->DestZoneId;

	//���ҹ�ϵ��û�н��
	 if(isDataLoading(playerId))
	 {
		 SocialMap::iterator it = mSocialMap.find(playerId);
		 if(it != mSocialMap.end())
		 {
			 SocialItemMap::iterator itr = it->second.find(masterPlayerId);
			 if(itr != it->second.end() && itr->second.type == SocialType::Master)
				 return;
		 }
	 }

	if(playerId != masterPlayerId)
	{
		TBLMPList tblMPList(0);
		tblMPList.AttachConn(m_pDBConn);
		DBError dbErr = tblMPList.Save(playerId,masterPlayerId);
		if(DBERR_NONE != dbErr)
		{
			nError = SocialError_DbError;
			g_Log.WriteError("��ʦplayerId=%d,masterPlayerId=%d,err=%d",playerId,masterPlayerId,getDBErr(dbErr));
		}
		else
		{
			MAKE_WORKQUEUE_PACKET(sendPacket, 128);
			sendPacket.writeInt(masterPlayerId, Base::Bit32);
			SEND_WORKQUEUE_PACKET(sendPacket, WQ_NORMAL, OWQ_SocialNotifyTopRank);
		}
	}
	else
	{
		nError = SocialError_Self;
	}

	AnsyPlayerInfo playerInfo;

	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
		return;
	}

    int nZoneId = playerInfo.zoneId;

	char buf[512];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,ZONE_WORLD_PlayerMasterSaveResponse,playerId,SERVICE_ZONESERVER,nZoneId,masterPlayerId);
	sendPacket.writeInt(nError,Base::Bit32);

	pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
	SERVER->GetServerSocket()->Send(SocketHandle,sendPacket);
}

void CSocialManager::EVENT_FUNCTION(HandleZoneQueryPrenticeListRequest)
{
	int playerId = pHead->Id;
    
    AnsyPlayerInfo playerInfo;
	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
		return;
	}

	VecLearnPrentice vecInfo;
	int nError = SocialError_None;

	TBLMPList tblMPList(0);
	tblMPList.AttachConn(m_pDBConn);
	DBError dbErr = tblMPList.Load(playerId,vecInfo);
	if(dbErr != DBERR_NONE)
	{
		nError = SocialError_DbError;
		g_Log.WriteWarn("��ȡplayerId=%d��ʦͽ���б�ʧ��err=%d",playerId,getDBErr(dbErr));
	}

	char* buf = new char[MAX_PACKET_SIZE];
	Base::BitStream sendPacket(buf,MAX_PACKET_SIZE);
    stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,WORLD_CLIENT_PrenticeListResponse,playerInfo.accountId,SERVICE_CLIENT,nError);
	
	int nSize = (int)vecInfo.size();
	//��������400���Ժ���÷�ҳ
	if(nSize > 400)
		nSize = 400;

	int zipSize   = MAX_PACKET_SIZE;
	char* zipBuf  = new char[MAX_PACKET_SIZE];
	char* dataBuf = new char[MAX_PACKET_SIZE];

	Base::BitStream dataStream(dataBuf,MAX_PACKET_SIZE);
	dataStream.writeInt(nSize,Base::Bit16);

	for(int i=0; i<nSize; i++)
	{
		stLearnPrentice stInfo = vecInfo[i];

		SimplePlayerData playerData;
        bool hasPlayerData = SERVER->GetPlayerManager()->AnsyGetPlayerData(stInfo.playerId,playerData);

		if(!hasPlayerData)
			continue;

		dStrcpy(stInfo.playerName,sizeof(stInfo.playerName),playerData.PlayerName);
		stInfo.level  = playerData.Level;
		stInfo.family = playerData.Family;
		stInfo.job    = playerData.FirstClass;
		stInfo.status = mPlayerStatusMap[stInfo.playerId];
		stInfo.WriteData(&dataStream);
	}

	int iret = SXZ_COMPRESS(zipBuf, &zipSize, dataBuf, dataStream.getPosition());

	if(sendPacket.writeFlag(!iret))
	{
		sendPacket.writeInt(zipSize,Base::Bit32);
		sendPacket.writeBits(zipSize << 3, zipBuf);
	}

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
	SERVER->GetServerSocket()->Send(playerInfo.socket,sendPacket);

	SAFE_DELETE_ARRAY(buf);
	SAFE_DELETE_ARRAY(zipBuf);
	SAFE_DELETE_ARRAY(dataBuf);
}*/

void CSocialManager::FindFriendRequest(Base::BitStream* Packet)
{
	int playerId = Packet->readInt(Base::Bit32);
	int destPlayerId = Packet->readInt(Base::Bit32);
	enQueryPlayerInfo queryType = (enQueryPlayerInfo)(Packet->readInt(Base::Bit8));
    int rank         = Packet->readInt(Base::Bit32);

	AnsyPlayerInfo playerInfo;
	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
	{
		g_Log.WriteWarn("��ѯ���id[%d]����Ϊ��,%s,%d��",playerId,__FUNCTION__,__LINE__);
		return;
	}

	char* buf = new char[MAX_PACKET_SIZE];
	/*Base::BitStream sendPacket(buf,MAX_PACKET_SIZE);
    stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, WORLD_CLIENT_QueryFriendResonse, playerInfo.accountId, SERVICE_CLIENT);


    SimplePlayerData playerData;
	bool isLoad = SERVER->GetPlayerManager()->AnsyGetPlayerData(destPlayerId,playerData);

	if(isLoad)
	{
		isLoad = false;

		AnsyPlayerInfo destPlayer;

		stPlayerStruct playerStruct;
		stPlayerInfoExt playerInfoEX;
		playerInfoEX.PlayerId = destPlayerId;

		TBLPlayer tbPlayer(0);
		tbPlayer.AttachConn(m_pDBConn);
		DBError dbErr = tbPlayer.LoadPlayerInfoExt(destPlayerId,&playerInfoEX);

		if (dbErr != DBERR_NONE)
		{
			g_Log.WriteError("��ѯĿ�����װ��Ʒ��ֵ%dʧ��,%s,%d��",destPlayerId,__FUNCTION__,__LINE__);
			return;
		}

		sendPacket.writeInt(playerInfoEX.EquipQualitySum, Base::Bit32);

		if (SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(destPlayerId,destPlayer))
		{
            sendPacket.writeInt(destPlayer.gateLineId,Base::Bit8);
		}
		else
		{
			sendPacket.writeInt(0,Base::Bit8);
		}

		// ��������
		char orgName[COMMON_STRING_LENGTH] = {0,};
		Packet->readString(orgName, sizeof(orgName));
		// ��������
		char leagueName[COMMON_STRING_LENGTH] = {0,};
		Packet->readString(leagueName, sizeof(leagueName));

		sendPacket.writeString(orgName, sizeof(orgName));
		sendPacket.writeString(leagueName,sizeof(leagueName));

        //��������
        sendPacket.writeInt(rank,Base::Bit32);

		if (queryType == QueryTypeFriend)
		{
			// �����Ȳ���һ��,���δ�ҵ�����ֱ�Ӳ�ѯһ�����ݿ�
			// ��worldServer�������رպ�δ�����������
			if (isDataLoading(playerId))
			{
				SocialMap::iterator it = mSocialMap.find(playerId);

				if (it != mSocialMap.end())
				{
					SocialItemMap::iterator itr = it->second.find(destPlayerId);

					if (itr != it->second.end())
					{
						isLoad = true;
						stSocialInfo stInfo = buildInfo(itr->second,&playerData);
						stInfo.WriteData(&sendPacket);
					}
				}
			}
		}
		else if (queryType == QueryTypePlayer)
		{
			isLoad = true;
			playerData.WriteData(&sendPacket);
		}
	}

	pSendHead->DestZoneId = isLoad;
	pSendHead->SrcZoneId  = queryType;
    SERVER->GetServerSocket()->Send(playerInfo.socket,sendPacket);
	*/
	SAFE_DELETE_ARRAY(buf);
}


void CSocialManager::NotifyPlayerOnline(int playerId, stSocialItem& socialItem)
{
    AnsyPlayerInfo playerInfo;

	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
		return;

    SimplePlayerData playerData;

	if(!SERVER->GetPlayerManager()->AnsyGetPlayerData(socialItem.playerId,playerData))
		return;

	char buf[512];
/*	Base::BitStream SendPacket(buf,sizeof(buf));
    stPacketHead* pHead = IPacket::BuildPacketHead(SendPacket, CLIENT_TEAM_PlayerOnline, playerInfo.accountId, SERVICE_CLIENT);

	socialItem.WriteData(&SendPacket);
	SendPacket.writeString(playerData.PlayerName,MAX_NAME_LENGTH);

	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
    SERVER->GetServerSocket()->Send( playerInfo.socket, SendPacket);*/
}

void CSocialManager::NotifyPlayerOffline(int playerId, stSocialItem& socialItem)
{
	AnsyPlayerInfo playerInfo;

	if(!SERVER->GetPlayerManager()->AnsyGetOnlinePlayer(playerId,playerInfo))
		return;

	SimplePlayerData playerData;

	if(!SERVER->GetPlayerManager()->AnsyGetPlayerData(socialItem.playerId,playerData))
		return;

	char buf[512];
/*	Base::BitStream SendPacket(buf,sizeof(buf));
    stPacketHead* pHead = IPacket::BuildPacketHead(SendPacket, CLIENT_TEAM_PlayerOffline,playerInfo.accountId,SERVICE_CLIENT);

	socialItem.WriteData(&SendPacket);
	SendPacket.writeString(playerData.PlayerName,MAX_NAME_LENGTH);

	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send(playerInfo.socket,SendPacket);*/
}