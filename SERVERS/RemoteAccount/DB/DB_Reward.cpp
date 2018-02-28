//#include "stdafx.h"
#include "DBContext.h"
#include "DB_Reward.h"

// ----------------------------------------------------------------------------
// �ʺŽ�������ȡ
int DB_Reward_Draw::Execute(int ctxId,void* param)
{
	CDBConn* pDBConn = (CDBConn*)param;
	IF_ASSERT(pDBConn == NULL)
		return PACKET_ERROR; //TODO: ����Ƿ��ͷ���Դ
	try
	{
		M_SQL(pDBConn,"UPDATE TBL_PURCHASE SET [Status] = 1 WHERE UID = '%s'", UID.c_str());
		pDBConn->Exec();
	}
	catch (CDBException &e)
	{
		g_Log.WriteFocus("<<ERROR>>msg=%s, dberr=%s, ecode=%d, func=" __FUNCTION__, e.w_msgtext, e.m_dberrstr, e.m_dberr);
	}
	catch (...)
	{
		g_Log.WriteError("RemoteAccount draw reward failed");
	}
	return PACKET_OK;
}