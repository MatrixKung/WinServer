//#include "stdafx.h"
#include "DB_ChangeAccountFlag.h"
#include "DBContext.h"
#include "DBLayer/Common/DBUtility.h"

DB_ChangeAccountFlag::DB_ChangeAccountFlag()
:accountID(0)
{

}

int DB_ChangeAccountFlag::Execute(int ctxId, void* param)
{
	if (DB_CONTEXT_NOMRAL == ctxId)
	{
		CDBConn* pDBConn = (CDBConn*)param;
		IF_ASSERT(pDBConn == NULL)
			return PACKET_ERROR; //TODO: ����Ƿ��ͷ���Դ
		
		//������ôд���Ǻܹ淶�������ھ���ôд��֮���޸ĺܼ򵥣�����ô�Ű�
#ifdef _DEBUG
		g_Log.WriteLog("�յ�World�����޸����ݿ��ʶ...");
#endif
		try
		{
			M_SQL(pDBConn,"EXEC USP_Change_AccountFlag %d", accountID);
			pDBConn->Exec();
		}
		catch (...)
		{
			g_Log.WriteError("RemoteAccount Change Account Flag fail...[AccountId=%d]", accountID);
		}
	}
	return PACKET_OK;
}