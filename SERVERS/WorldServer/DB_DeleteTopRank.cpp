#include "stdafx.h"
#include "DB_DeleteTopRank.h"
#include "../Common/TopType.h"

DB_DeleteTopRank::DB_DeleteTopRank(void)
{
	m_playerId = 0;
    m_type     = 0;
	m_isAllClear = false;
}

int DB_DeleteTopRank::Execute(int ctxId,void* param)
{
	CDBConn* pDBConn = (CDBConn*)param;
	assert(0 != pDBConn);

	TBLTopRank tb_toprank(0);
	tb_toprank.AttachConn(pDBConn);
    
    if (!m_isAllClear)
    {
		if (-1 == m_type && m_playerId != 0)
		{
			DBError err = tb_toprank.Delete(m_playerId);

			if(err != DBERR_NONE)
				g_Log.WriteError("ɾ����ɫ%d�������а�����ʧ�ܣ�ԭ��:%s",m_playerId,getDBErr(err));
			else
				g_Log.WriteLog("ɾ����ɫ%d�������а����ݳɹ�!",m_playerId);
		}
		else
		{
			if (m_uid != 0)
			{
				DBError err = tb_toprank.Delete(m_uid,m_type);

				if(err != DBERR_NONE)
					g_Log.WriteError("ɾ��uid%I64d���а�[%d]����ʧ�ܣ�ԭ��:%s",m_uid,m_type,getDBErr(err));
				else
					g_Log.WriteLog("ɾ��uid%I64d���а�[%d]���ݳɹ�!",m_uid,m_type);

			}
			else if(m_playerId != 0)
			{
				DBError err = tb_toprank.Delete(m_playerId,m_type);

				if(err != DBERR_NONE)
					g_Log.WriteError("ɾ����ɫ%d���а�[%d]����ʧ�ܣ�ԭ��:%s",m_playerId,m_type,getDBErr(err));
				else
					g_Log.WriteLog("ɾ����ɫ%d���а�[%d]���ݳɹ�!",m_playerId,m_type);

			}
		}
    }
    else
    {
		//ɾ��ָ�������а�����
        DBError err = tb_toprank.Clear(m_type);

        if(err != DBERR_NONE)
        {
            g_Log.WriteError("ɾ�� TopRank[%d] ����ʧ��,ԭ��:%s",m_type,getDBErr(err));
        }
    }

	return PACKET_OK;
}