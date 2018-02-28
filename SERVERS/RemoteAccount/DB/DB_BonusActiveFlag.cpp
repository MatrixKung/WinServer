#include "DB_BonusActiveFlag.h"
#include "DBContext.h"

DB_BonusActiveFlag::DB_BonusActiveFlag()
:PlayerId(0), AccountId(0), ActiveFlag(0)
{
}

int DB_BonusActiveFlag::Execute(int ctxId, void* param)
{
	CDBConn* pDBConn = (CDBConn*)param;
	IF_ASSERT(pDBConn == NULL)
		return PACKET_ERROR; //TODO: ����Ƿ��ͷ���Դ

	try
	{
		M_SQL(pDBConn, "EXECUTE USP_BONUSACTIVEFLAG %d,%d,%d", AccountId, PlayerId, ActiveFlag);
		pDBConn->Exec();		
	}
	catch (CDBException &e)
	{
		g_Log.WriteFocus("<<ERROR>>msg=%s, dberr=%s, ecode=%d, func="__FUNCTION__,
			e.w_msgtext, e.m_dberrstr, e.m_dberr);
	}
	catch (...)
	{
		g_Log.WriteError("RemoteAccount handle BonusActiveFlag error account=%d,playerId=%d,flag=%d",
			AccountId,PlayerId,ActiveFlag);
	}
	return PACKET_OK;
}