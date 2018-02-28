#include "Define.h"
#include "dblib/dbLib.h"
#include "Base/Log.h"
#include <time.h>
#include "TBLGold.h"
#include "Common/BillingBase.h"

TBLGold::TBLGold(DataBase* db):TBLBase(db)
{
}

TBLGold::~TBLGold()
{
}

// ----------------------------------------------------------------------------
// ��ѯԪ������
DBError TBLGold::QueryGold(int AccountID, int PlayerID, int& Gold, int& TotalDrawGolds)
{
	DBError err = DBERR_UNKNOWERR;
	Gold = 0;
	try
	{
		M_SQL(GetConn(),"EXECUTE Sp_QueryGolds %d, %d", AccountID, PlayerID);
		if(GetConn()->More())
		{
			Gold = GetConn()->GetInt();
			TotalDrawGolds = GetConn()->GetInt();
		}
		err = DBERR_NONE;
	}
	DBCATCH(AccountID)
	return err;
}

// ----------------------------------------------------------------------------
// ����Ԫ������
DBError TBLGold::UpdateGold(int AccountID, int Gold, int GoldOpType, int PlayerID,
							int& LeftGold, int& TotalDrawGold)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		M_SQL(GetConn(),"EXECUTE Sp_UpdateGold %d,%d,%d,%d", AccountID, Gold, GoldOpType, PlayerID);
		if(GetConn()->More() && GetConn()->GetInt() == 0)
		{
			LeftGold = GetConn()->GetInt();
			TotalDrawGold = GetConn()->GetInt();
			err = DBERR_NONE;
		}
		else
			throw ExceptionResult(DBERR_ACCOUNT_UPDATEGOLD);
	}
	DBCATCH(AccountID)

	//����ȡԪ���ɹ������ж�����Ƿ��з��ڷ�����ɲμӲ����¼��㽱��
	if(err == DBERR_NONE && GoldOpType == 0)
	{
		try
		{
			M_SQL(GetConn(),"EXECUTE Sp_UpdateStagingRebateHistory %d", PlayerID);
			GetConn()->Exec();
		}
		DBCATCH(PlayerID)
	}
	return err;
}

// ----------------------------------------------------------------------------
// ��ѯ�ۼ���ȡԪ������
DBError TBLGold::QueryTotalDrawGolds(int PlayerID, int& Golds)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		M_SQL(GetConn(),"EXECUTE Sp_QueryTotalDrawGolds %d", PlayerID);
		if(GetConn()->More())
		{
			Golds = GetConn()->GetInt();
			err = DBERR_NONE;
		}
		else
			throw ExceptionResult(DBERR_GOLDEXCHAGE_NOFOUND);
	}
	DBECATCH()
	return err;
}