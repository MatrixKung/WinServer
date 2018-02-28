#include "stdafx.h"
#include "DB_TopRankLoadGold.h"
#include "../Common/TopType.h"
#include "DBContext.h"


DB_TopRankLoadGold::DB_TopRankLoadGold(void)
{
    
}

DB_TopRankLoadGold::~DB_TopRankLoadGold(void)
{

}



int DB_TopRankLoadGold::Execute(int ctxId,void* param)
{
	if(DB_CONTEXT_NOMRAL == ctxId)
	{
		CDBConn* pDBConn = (CDBConn*)param;
		assert(0 != pDBConn);
	    
		TBLGoldHistory tb_goldHistory(0);
		tb_goldHistory.AttachConn(pDBConn);
		DBError err = tb_goldHistory.QueryGoldCostMonth(m_topGoldCostMonth);	

		/*
		if(err != DBERR_NONE)
		{
			g_Log.WriteError("���а� %s ��ȡgoldHistoryԪ������ʧ�ܣ�ԭ��:%s",strTopRankType[eTopType_GoldCostMonth],getDBErr(err));
			return PACKET_ERROR;
		}
		else
			g_Log.WriteLog("���а� %s ��ȡgoldHistoryԪ�����ݳɹ�!",strTopRankType[eTopType_GoldCostMonth]);

		err = tb_goldHistory.QueryGoldCostTotal(m_topGoldCostTotal);
		if(err != DBERR_NONE)
		{
			g_Log.WriteError("���а� %s ��ȡgoldHistoryԪ������ʧ�ܣ�ԭ��:%s",strTopRankType[eTopType_GoldCostTotal],getDBErr(err));
			return PACKET_ERROR;
		}
		else
			g_Log.WriteLog("���а� %s ��ȡgoldHistoryԪ�����ݳɹ�!",strTopRankType[eTopType_GoldCostTotal]);

		*/
		PostLogicThread(this);
		return PACKET_NOTREMOVE;

	}
	else
	{
		//SERVER->GetTopManager()->SetGoldMonthCost(m_topGoldCostMonth);
		//SERVER->GetTopManager()->SetGoldTotalCost(m_topGoldCostTotal);
	}
    
    return PACKET_OK;
}


