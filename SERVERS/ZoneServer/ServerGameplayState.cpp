#include <winsock2.h>
#include "ServerGameplayState.h"
//#include "Common/LogHelper.h"
#include "CommLib/CommLib.h"
// ========================================================================================================================================
//	ȫ�ֶ���
// ========================================================================================================================================
ServerGameplayState* g_ServerGameplayState = NULL;
ServerGameplayState gServerGameplayState;
// ========================================================================================================================================
//	Constructor & Destructor
// ========================================================================================================================================
ServerGameplayState::ServerGameplayState()
{
	g_ServerGameplayState	= this;
	mZoneId					= 0;
	//m_pDataAgentHelper		= new CDataAgentHelper;
    m_pLogHelper            = new CLogHelper();
	m_bIsRobotTest			= false;

	mInitFlag				= 0;
	mInited					= false;
}

ServerGameplayState::~ServerGameplayState()
{
	
}

U32 g_nZoneId = 0;

void ServerGameplayState::Initialize()
{
	//�����ȵȴ�GATE�������������
	while(!EventGuard::CheckExist("_gatestarted_"))
	{
		g_Log.WriteLog("���ڵȴ�gate�������������...");
		Sleep(1000);
		continue;
	}

	//g_nZoneId = mZoneId;

	//���õ�ͼ����������̨���ڱ���
	{
		char szTitle[128];
		char moduleName[MAX_PATH];

		GetModuleFileNameA( NULL, moduleName, MAX_PATH );
		const char* endChar = strrchr( moduleName, '\\' );

		char name[128];
		strncpy_s( name, sizeof( name ), moduleName, endChar - moduleName );

		const char* rendChar = strrchr( name, '\\' );

		sprintf_s(szTitle, sizeof(szTitle),"MapID[%d] S: %s P: %d, V: [%s]-%s", 
						mZoneId, rendChar + 1,END_NET_MESSAGE, G_PVERSION, G_VERSION_BUILD
						);
		SetConsoleTitleA(szTitle);
	}


    //m_pLogHelper->connect( Con::getVariable("$Pref::Net::LogIp"), atoi( Con::getVariable("$Pref::Net::LogPort") ) );

	
	// -------------------------------------- ��ʼ��data --------------------------------------
	m_TimeSignal.notify(this, &ServerGameplayState::CheckPlayerQuit, 1000 );
}

bool  ServerGameplayState::InitSM()
{
    //��ʼ��sm
	if(!m_smPool.Init(SMKEY_DATAAGENT_PLAYER))
	{
		return false;
	}
	
	m_smPool.WaitSaved(this->getZoneId());
    return true;
}

void ServerGameplayState::preShutdown()
{
}

void ServerGameplayState::Shutdown()
{
	m_TimeSignal.remove( this, &ServerGameplayState::CheckPlayerQuit );
	char buf[128];
	sprintf_s( buf, sizeof( buf ), "%d", getZoneId() );

	HANDLE handle = OpenMutexA( MUTEX_ALL_ACCESS, FALSE, buf );
	if( handle != NULL && handle != INVALID_HANDLE_VALUE )
		CloseHandle( handle );

	//delete m_pDataAgentHelper;
    delete m_pLogHelper;
}

void ServerGameplayState::TimeLoop()
{	
	if(m_TimeSignal.mTargetTime <= GetTickCount())
	{
		m_TimeSignal.trigger();
	}

	//�ƽ�SM
	m_smPool.Update();

    U64 memoryUsed = GetUsePhysMemory();

    if (memoryUsed > 1024 * 1024)
    {
        g_Log.WriteError("��ͼ[%d]�ڴ泬��1G,���ڴ�С[%I64d]",getZoneId(),memoryUsed);
    }
}

bool ServerGameplayState::DeletePlayerLoginData(int PlayerId)
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(PlayerId);

	if(it == m_PlayerLoginData.end())
		return false;
    {
        bool ret = false;
        if( it->second.pPlayerData )
        {
            it->second.FreeSM(&m_smPool);
            delete it->second.pPlayerData;
            ret = true;
        }

        m_PlayerLoginData.erase(it);
        return ret;
    }
}

//IsFirst ��һСʱ�ڵ�һ�ε��볡��
//IsFirstLogin�����������޸����ݺ��һ�ε�¼��־
bool ServerGameplayState::AddPlayerLoginData(T_UID UID,
								stPlayerStruct *pPlayerData)
{
    {
        //Ϊ��ǰ���ӷ���SM�������
        SM_PLAYER_DATA* pSMPlayerData = m_smPool.Alloc(pPlayerData->BaseData.PlayerId,getZoneId());

        if (0 == pSMPlayerData)
        {
           g_Log.WriteError("�޷�Ϊ���[%d]����Share Memory����",pPlayerData->BaseData.PlayerId);
            return false;
        }

        //�����µ����ݿ�
        PlayerLoginBlock block;
        block.UID			= UID;
        block.pPlayerData	= pPlayerData;;

        pSMPlayerData->Attach(pPlayerData);
        block.pSMPlayerData = pSMPlayerData;

        AddPlayerLoginData(block);
    }
	return true;
}

void ServerGameplayState::AddPlayerLoginData(PlayerLoginBlock &block )
{
	stPlayerStruct *pPlayerData = block.pPlayerData;
	ASSERT(pPlayerData,"ServerGameplayState::AddPlayerLoginData Why?!");

	block.Time			= time(NULL);
	memcpy( &(m_PlayerLoginData[pPlayerData->BaseData.PlayerId]), &block, sizeof( PlayerLoginBlock ) );
	memcpy( &(m_PlayerLoginDataTimeout[pPlayerData->BaseData.PlayerId]), &block, sizeof( PlayerLoginBlock ) );

	SetPlayerSID( pPlayerData->BaseData.PlayerId, pPlayerData->BaseData.SaveUID + 100 );
}

stPlayerStruct *ServerGameplayState::GetPlayerLoginData(T_UID UID,int PlayerId)
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(PlayerId);

	if(it == m_PlayerLoginData.end())
		return NULL;

	PlayerLoginBlock& block = it->second;

	if(block.UID == UID)
		return block.pPlayerData;

	return NULL;
}

PlayerLoginBlock ServerGameplayState::DetachPlayerLoginData(int PlayerId)
{
	PlayerLoginBlock block;
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(PlayerId);

	if(it != m_PlayerLoginData.end())
	{
		block = it->second;
		m_PlayerLoginData.erase(it);
	}

	return block;
}


stPlayerStruct* ServerGameplayState::GetPlayerLoginData( int playerId )
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(playerId);
	if(it == m_PlayerLoginData.end())
		return NULL;

	PlayerLoginBlock& block = it->second;

	return block.pPlayerData;
}
 
void ServerGameplayState::SavePlayerData(U32 playerId,bool isLastSave)
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginData.find(playerId);

	if(it == m_PlayerLoginData.end())
		return;

	PlayerLoginBlock& block = it->second;

    if (0 != block.pSMPlayerData && 0 != block.pPlayerData)
	{
        //assert(0 != block.pPlayerData->BaseData.PlayerId);
		block.pSMPlayerData->Save(true,isLastSave);
	}
}

bool ServerGameplayState::LoadPlayerDataFromSM(U32 PlayerId,stPlayerStruct* pData)
{
	if (0 == pData)
		return false;

	return m_smPool.LoadCache(PlayerId,pData);
}

void ServerGameplayState::RemovePlayerLoginDataTimeout(int PlayerId)
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginDataTimeout.find(PlayerId);

	if(it != m_PlayerLoginDataTimeout.end())
	{
		m_PlayerLoginDataTimeout.erase(it);
	}
}

void ServerGameplayState::FilterPlayerLoginDataTimeout(int curTime)
{
	HashPlayerLoginBlock::iterator it = m_PlayerLoginDataTimeout.begin();

	while(it!=m_PlayerLoginDataTimeout.end())
	{
		PlayerLoginBlock& block = it->second;

		if(curTime - block.Time> 5*60 ) //5����
		{
			DeletePlayerLoginData(it->first);		//Ray: ���ֹ�m_PlayerLoginDataTimeout�д���ȴ��m_PlayerLoginData�в����ڵ����������޽�
			m_PlayerLoginDataTimeout.erase(it++);
		}
		else
			it++;
	}

}

void ServerGameplayState::AddInPlayerManager(Player *pPlayer)
{
	if( !pPlayer )
		return ;

	int PlayerId = pPlayer->getPlayerID();

	//Ray: ����ܿ��ܻ���AddPlayerLoginDataʱ�̱�©�ߵ���ʱû�������������
	HashPlayer::iterator itp = m_PlayerManager.find(PlayerId);
	if(itp != m_PlayerManager.end())
	{
		Player *pOld = itp->second;
        GameConnection *pCon = pOld->getControllingClient();
        if(pCon)
        {
			PlayerLoginBlock block = DetachPlayerLoginData( PlayerId );
            pCon->deleteObject();		// �ɵ�������

			if(block.pPlayerData)
			{
				//Ray: ���BUFF�ض����������ӹ����ˣ����ñ���detach��
				AddPlayerLoginData( block );
			}
        }
    }

	//Ray: �����ݳ�ʱ�����ȥ��
	RemovePlayerLoginDataTimeout(PlayerId);

	m_PlayerManager[pPlayer->getPlayerID()] = pPlayer;

	m_AccountMap[pPlayer->getPlayerID()] = pPlayer->getAccountID();

	// ֪ͨWORLDSERVER�������ȷ���ѵ�½��ZONE����������
	char buf[32];
	Base::BitStream sendPacket(buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_PlayerLogin, pPlayer->getPlayerID(), SERVICE_WORLDSERVER, getZoneId());

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	GetPacketProcess()->Send( sendPacket );

	addBattleField(pPlayer);
}

void ServerGameplayState::DeleteFromPlayerManager(Player *pPlayer)
{
	if( !pPlayer )
		return ;
    
    //��WORLDͬ���¾���������
    GetPacketProcess()->Send_World_ChallengeData(pPlayer);

	ZONE_TEAM_MGR->OnPlayerLeaveZone( pPlayer->getPlayerID() );
	BFCORPSMGR.DelPlayer(pPlayer->getPlayerID());

	removeBattleField(pPlayer);
	if( GetPlayer( pPlayer->getPlayerID()) == NULL )
		return ;

	if( !pPlayer->isTransporting() )
	{
		// �޸�Ϊֱ�ӱ��浽��������� [6/23/2010 Administrator]
		pPlayer->saveToDataAgent(true);
	}	

	m_PlayerManager.erase(pPlayer->getPlayerID());

	m_AccountMap.erase( pPlayer->getAccountID() );

	g_ServerGameplayState->DeletePlayerLoginData( pPlayer->getPlayerID() );
/*
	GameConnection *pCon = pPlayer->getControllingClient();
	if(pCon)
	{
		pCon->deleteObject();		// �ɵ�������
	}*/
}

Player *ServerGameplayState::GetPlayer(int PlayerId)
{
	HashPlayer::iterator itp = m_PlayerManager.find(PlayerId);
	if(itp != m_PlayerManager.end())
	{
		return itp->second;
	}

	return NULL;
}

void ServerGameplayState::AddPlayerSaveData( stPlayerStruct* pPlayer )
{
	HashPlayerSaveMap::iterator it;
	it = m_PlayerSaveMap.find( pPlayer->BaseData.PlayerId );
	if( it != m_PlayerSaveMap.end() )
	{
		delete it->second;
		it->second = pPlayer;
	}
	else
	{
		m_PlayerSaveMap[pPlayer->BaseData.PlayerId] = pPlayer;
	}
}

void ServerGameplayState::RemovePlayerSaveData( U32 playerId )
{
	HashPlayerSaveMap::iterator it;
	it = m_PlayerSaveMap.find( playerId );
	if( it == m_PlayerSaveMap.end() )
		return ;

	delete it->second;

	m_PlayerSaveMap.erase( it );
}

Player * ServerGameplayState::GetPlayerByAccount( int accountId )
{
	HashAccount::iterator it;
	it = m_AccountMap.find( accountId );
	if( it == m_AccountMap.end() )
		return NULL;

	return GetPlayer( it->second );
}
