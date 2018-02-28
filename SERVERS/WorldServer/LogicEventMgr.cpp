/*
#include "dblib/dbLib.h"
#include "Event.h"
#include "WorldServer.h"
#include "ServerMgr.h"
#include "PlayerMgr.h"
#include "Common/LogicEventBase.h"
#include "Common/dbStruct.h"
#include "DBLayer/Data/TBLEventList.h"

#include "DB_SendLogicEvents.h"
#include "DB_ZoneLogicEvent.h"
#include "DB_CheckLogicEvent.h"

CLogicEventManager::CLogicEventManager()
{
	registerEvent( ZONE_WORLD_SENDEVENT,		&CLogicEventManager::HandleZoneSendEvent );
	registerEvent( ZONE_WORLD_RECEIVEEVENT,		&CLogicEventManager::HandleZoneReceiveEvent );
}

CLogicEventManager::~CLogicEventManager()
{
}

// ----------------------------------------------------------------------------
// ��ZoneServer��ĳ��ȡ�˷�������ϵͳ�¼�����
void CLogicEventManager::SendAllEventToZone(int nPlayerId)
{
	DO_LOCK( Lock::LogicEvent);

    AccountRef pInfo = SERVER->GetPlayerManager()->GetOnlinePlayer(nPlayerId);

	if(!pInfo)
		return;

    int LineId = pInfo->GetLastLineId();
    int ZoneId = pInfo->GetLastZoneId();

	int svrSocket = SERVER->GetServerManager()->GetGateSocket( LineId, ZoneId );

    if (0 == svrSocket)
    {
        return;
    }

	DB_SendLogicEvents* pDBHandle = new DB_SendLogicEvents;

    pDBHandle->m_playerId     = nPlayerId;
    pDBHandle->m_zoneId	      = ZoneId;
    pDBHandle->m_zoneSocketId = svrSocket;
    
    SERVER->GetCommonDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// ��������ZoneServer����ϵͳ�߼��¼�������ͬһZone�����
void CLogicEventManager::HandleZoneSendEvent(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
{
	DO_LOCK( Lock::LogicEvent);

	stEventItem item;
	item.UnpackData(Packet);

    DB_ZoneLogicEvent* pDBHandle = new DB_ZoneLogicEvent;

    pDBHandle->m_playerId     = item.recver;
    pDBHandle->m_event	      = item;
    
    AccountRef pInfo = SERVER->GetPlayerManager()->GetOnlinePlayer(item.recver);

    if(pInfo)
    {
        int LineId = pInfo->GetLastLineId();
        int ZoneId = pInfo->GetLastZoneId();

        int svrSocket = SERVER->GetServerManager()->GetGateSocket( LineId, ZoneId );

        if (svrSocket > 0)
        {
            pDBHandle->m_zoneId       = ZoneId;
            pDBHandle->m_zoneSocketId = svrSocket;
        }
    }

    SERVER->GetCommonDBManager()->SendPacket(pDBHandle);
}

//����ZoneServer��ȡ����ȡϵͳ�¼�������
void CLogicEventManager::HandleZoneReceiveEvent(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet )
{
	DO_LOCK( Lock::LogicEvent);

	DB_CheckLogicEvent* pDBHandle = new DB_CheckLogicEvent;
    pDBHandle->m_isOK = Packet->readFlag();
    pDBHandle->m_item.UnpackData(Packet);
    SERVER->GetCommonDBManager()->SendPacket(pDBHandle);
}
*/