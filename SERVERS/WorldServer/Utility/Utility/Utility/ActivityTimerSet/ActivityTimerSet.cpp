#include "ActivityTimerSet.h"
#include "../../AccountHandler.h"
#include "../../WorldServer.h"
#include "../../PlayerMgr.h"
#include "../../data/MissionData.h"
#include "../../DB_SavePlayerBuff.h"
#include "../platform/platform.h"
#include "../Script/lua_tinker.h"

bool TimerSetMgr::AcceptMission(U32 nMissionId, Player* pPlayer, int SocketHandle)
{
	CMissionData* pMissionData = g_MissionDataMgr->getMissionData(nMissionId);
	if(!pMissionData)
		return false;

	if(!pPlayer)
		return false;

	if(!CanAcceptMisssion(nMissionId, pPlayer))
		return false;

	//��������
	S32 iSlot = pPlayer->findEmptyShortcut(SHORTCUTTYPE_TIMESET);
	if(iSlot == -1 || iSlot >= MAX_TIMESETS)
		return false;

	PlayerDataRef pPlayerData = pPlayer->GetPlayerData();
	if(!pPlayerData)
		return false;

	stTimeSet* pTimer = &pPlayerData->DispData.TimeSet[iSlot];

	pTimer->id		= nMissionId;
	pTimer->flag1	= 0;
	pTimer->flag2	= 0;
	pTimer->finishFlag	|= MISSION_ACCEPT_FLAG;//���ܱ�־

	if(pMissionData->m_Interval == CMissionData::INTERVAL_LOOP)
		pTimer->expiretime = 0xFFFFFFFF;
	else if(pMissionData->m_Interval == CMissionData::INTERVAL_NEVER)
		pTimer->expiretime = 0;
	else if(pMissionData->m_Interval == CMissionData::INTERVAL_LIANXU)
		pTimer->expiretime = 0xFFFFFFFE;
	else
		pTimer->expiretime = Platform::getNextTime(pMissionData->m_Interval);

	//������������
	pPlayer->TimeSaveToDB(pTimer, DB_UPDATE_ALL_MASK, true, false);
	pPlayer->SendClientTime(pTimer, SocketHandle, DB_UPDATE_ALL_MASK, NOTIFY_OPERATER_ADD);
	return true;
}

bool TimerSetMgr::CanAcceptMisssion(U32 nMissionId, Player* pPlayer)
{
	CMissionData* pMissionData = g_MissionDataMgr->getMissionData(nMissionId);
	if(!pMissionData)
		return false;
	
	if(!pPlayer)
		return false;

	PlayerDataRef pPlayerData = pPlayer->GetPlayerData();
	if(!pPlayerData)
		return false;

	//С�ڽ��յȼ�
	if(pPlayerData->BaseData.Level + 1 < pMissionData->m_RecLevel)
		return false;

	//����������ǰ������
	if((pMissionData->m_Category == CMissionData::MISSION_CATEGORY_PLOT) && (pMissionData->m_PreMissionId > 0))
	{
		S32 iPreMissionId = pMissionData->m_PreMissionId;
		//ǰ������û�����
		stTimeSet* pTimer = pPlayer->getTimerSet((U32)iPreMissionId);
		if(!pTimer)
		{
			return false;
		}
		else
		{
			//����û�����
			if(!(pTimer->finishFlag & MISSION_FINISH_FLAG))
				return false;
		}
	}

	{
		//��ǰ�����Ѿ�����
		stTimeSet* pTimer = pPlayer->getTimerSet(nMissionId);
		if(pTimer)
		{
			if(pTimer->finishFlag & MISSION_ACCEPT_FLAG || pTimer->finishFlag & MISSION_FINISH_FLAG)
				return false;
		}
	}

	return true;
}

bool TimerSetMgr::SubmitMission(U32 nMissionId, Player* pPlayer, int SocketHandle)
{
	CMissionData* pMissionData = g_MissionDataMgr->getMissionData(nMissionId);
	if(!pMissionData)
		return false;

	if(!pPlayer)
		return false;

	if(!CanSubmitMission(nMissionId, pPlayer))
		return false;

	PlayerDataRef pPlayerData = pPlayer->GetPlayerData();
	if(!pPlayerData)
		return false;

	//��ȡ��ǰ����
	stTimeSet* pTimer = pPlayer->getTimerSet(nMissionId);
	if(!pTimer)
	{
		return false;
	}

	pTimer->finishFlag |= MISSION_FINISH_FLAG;
	//������������
	pPlayer->TimeSaveToDB(pTimer, TIMESET_FINISHFLAG, false, false);
	pPlayer->SendClientTime(pTimer, SocketHandle, TIMESET_FINISHFLAG, NOTIFY_OPERATER_UPDATE);

	//������ɽ���
	if(pMissionData->m_AwardExp > 0)
		pPlayer->addExp(pMissionData->m_AwardExp);
	if(pMissionData->m_AwardGold > 0)
		pPlayer->addGold(pMissionData->m_AwardGold);
	if(pMissionData->m_AwardMoney > 0)
		pPlayer->addMoney(pMissionData->m_AwardMoney);
	//�����Ʒ����
	//if(pMissionData->m_AwardItemId > 0)


	if((pMissionData->m_Category == CMissionData::MISSION_CATEGORY_PLOT))
	{
		std::set<S32> MissionSet;
		g_MissionDataMgr->getNextMissionId(pMissionData->m_MissionId, MissionSet);
		if(!MissionSet.empty())
		{
			for(std::set<S32>::iterator itr = MissionSet.begin(); itr != MissionSet.end(); ++itr)
			{
				AcceptMission(*itr, pPlayer, SocketHandle);
			}
		}
	}
	return true;
}

bool TimerSetMgr::CanSubmitMission(U32 nMissionId, Player* pPlayer)
{
	CMissionData* pMissionData = g_MissionDataMgr->getMissionData(nMissionId);
	if(!pMissionData)
		return false;

	if(!pPlayer)
		return false;

	PlayerDataRef pPlayerData = pPlayer->GetPlayerData();
	if(!pPlayerData)
		return false;

	//С���ύ�ȼ�
	if(pPlayerData->BaseData.Level  + 1 < pMissionData->m_SubLevel)
		return false;

	{
		//��ǰ�����Ƿ��Ѿ�����
		stTimeSet* pTimer = pPlayer->getTimerSet(nMissionId);
		if(!pTimer)
		{
			return false;
		}
		else
		{
			if(!(pTimer->finishFlag & MISSION_ACCEPT_FLAG) || pTimer->finishFlag & MISSION_FINISH_FLAG)
				return false;
		}
	}

	return true;
}

bool TimerSetMgr::ChangeMission(U32 nMissionId, Player* pPlayer, S32 flag1, S32 flag2, int SocketHandle)
{
	if(!pPlayer)
		return false;

	CMissionData* pMissionData = g_MissionDataMgr->getMissionData(nMissionId);
	if(!pMissionData)
		return false;

	PlayerDataRef pPlayerData = pPlayer->GetPlayerData();
	if(!pPlayerData)
		return false;
	
	stTimeSet* pTimer = pPlayer->getTimerSet((U32)nMissionId);
	if(!pTimer)
		return false;

	if(pTimer->finishFlag & MISSION_FINISH_FLAG)
		return false;

	if( flag1 > 0)
		pTimer->flag1 = flag1;
	else 
		pTimer->flag1 = 0;

	if(flag2 > 0)
		pTimer->flag2 = flag2;
	else
		pTimer->flag2 = 0;

	pPlayer->TimeSaveToDB(pTimer, TIMESET_FLAG_FLAG1|TIMESET_FLAG_FLAG2, false, false);
	pPlayer->SendClientTime(pTimer, SocketHandle, TIMESET_FLAG_FLAG1|TIMESET_FLAG_FLAG2, NOTIFY_OPERATER_UPDATE);

	return true;
}

bool TimerSetMgr::ResetMission(U32 nMissionId, Player* pPlayer, int SocketHandle)
{
	if(!pPlayer)
		return false;

	CMissionData* pMissionData = g_MissionDataMgr->getMissionData(nMissionId);
	if(!pMissionData)
		return false;

	//ֻ��������½��Ч
	if(pMissionData->m_Interval != CMissionData::INTERVAL_LIANXU)
		return false;

	PlayerDataRef pPlayerData = pPlayer->GetPlayerData();
	if(!pPlayerData)
		return false;

	stTimeSet* pTimer = pPlayer->getTimerSet((U32)nMissionId);
	if(!pTimer)
		return false;

	//δ���,���߲�����������
	if(!(pTimer->finishFlag & MISSION_FINISH_FLAG))
		return false;

	pTimer->finishFlag = MISSION_ACCEPT_FLAG;
	pTimer->flag1  = 0;
	pTimer->flag2  = 0;

	pPlayer->TimeSaveToDB(pTimer, TIMESET_FLAG_FLAG1|TIMESET_FLAG_FLAG2|TIMESET_FINISHFLAG, false, false);
	pPlayer->SendClientTime(pTimer, SocketHandle, TIMESET_FLAG_FLAG1|TIMESET_FLAG_FLAG2|TIMESET_FINISHFLAG, NOTIFY_OPERATER_UPDATE);

	return true;
}
