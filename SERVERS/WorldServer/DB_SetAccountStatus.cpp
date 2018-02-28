#include "stdafx.h"
#include "DB_SetAccountStatus.h"
#include "DBLayer/common/DBUtility.h"
#include "DBLayer/data/TBLAccount.h"

DB_SetAccountStatus::DB_SetAccountStatus(void)
{
    m_accountId = 0;
    m_status    = 0;
}

int DB_SetAccountStatus::Execute(int ctxId,void* param)
{
    CDBConn* pDBConn = (CDBConn*)param;
    assert(0 != pDBConn);

    g_Log.WriteLog("�յ��ʺ�[%d]��״̬��������",m_accountId);

    TBLAccount tbl_account(0);
    tbl_account.AttachConn(pDBConn);

    if (DBERR_NONE != tbl_account.SetAccountStatus(m_accountId,m_status))
    {
        g_Log.WriteError("�޷������ʺ�[%d]��״̬Ϊ[%d]",m_accountId,m_status);
        return true;
    }
    
    return PACKET_OK;
}