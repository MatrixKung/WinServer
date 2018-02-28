#include "stdafx.h"
#include "DB_SaveTopRank.h"
#include "../Common/TopType.h"

DB_SaveTopRank::DB_SaveTopRank(void)
{
    m_iType = 0;
    m_isEnded = false;
	m_iSaveBlobType = -1;
}

int DB_SaveTopRank::Execute(int ctxId,void* param)
{
    CDBConn* pDBConn = (CDBConn*)param;
	assert(0 != pDBConn);
    
    TBLTopRank tb_toprank(0);
    tb_toprank.AttachConn(pDBConn);
	DBError err = tb_toprank.Save(m_iType,m_toprankList,m_iSaveBlobType);	

	if(err != DBERR_NONE)
	{
		g_Log.WriteError("�洢TopRank %s ����ʧ�ܣ�ԭ��:%s",strTopRankType[m_iType],getDBErr(err));
	}
	else
	{
		g_Log.WriteLog("�洢TopRank %s ���ݳɹ�!",strTopRankType[m_iType]);

		if (eTopType_End-1 == m_iType)
		{
			g_Log.WriteLog("�洢TopRank���,�����ɹ�!");
		}
	}
    

    if (!m_isEnded)
    {
        //����ֹͣ10����,��ֹ�����ݿ����ѹ��
        Sleep(10000);
    }

    return PACKET_OK;
}