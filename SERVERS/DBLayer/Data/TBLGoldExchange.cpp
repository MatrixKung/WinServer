#include "Define.h"
#include "dblib/dbLib.h"
#include "Base/Log.h"
#include "DBUtility.h"
#include "TblGoldExchange.h"

// ----------------------------------------------------------------------------
// ��ѯĳ��ҵ���Ԫ���һ�����
DBError TBLGoldExchange::Query(stGoldExchange* exchange)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		int ExchangeType;

		//��ѯ���ۻ���ȡԪ����
		M_SQL(GetConn(),"EXECUTE Sp_QueryAllGolds %d,%d", exchange->AccountID, exchange->PlayerID);
		if(GetConn()->More() && GetConn()->GetInt() == 0)
		{
			exchange->YearMonth = GetConn()->GetInt();
			exchange->CurrentGold = GetConn()->GetInt();
			exchange->MonthGolds = GetConn()->GetInt();
			err = DBERR_NONE;
		}
		else
		{
			throw ExceptionResult(DBERR_GOLDEXCHAGE_NOFOUND);
		}

		M_SQL(GetConn(),"EXECUTE Sp_QueryGoldExchange %d", exchange->PlayerID);
		while(GetConn()->More())
		{
			ExchangeType = GetConn()->GetInt();
			if(ExchangeType < 0 || ExchangeType >= GEF_MAX)
				continue;
						
			exchange->Exchanges[ExchangeType] = GetConn()->GetInt();
		}
	}
	DBECATCH()
	return err;
}

// ----------------------------------------------------------------------------
// ��ѯĳ��ҵ���Ԫ���һ�����
DBError TBLGoldExchange::QueryMonthGolds(U32 PlayerID, int& MonthGolds)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		M_SQL(GetConn(),"EXECUTE Sp_QueryMonthGolds %d", PlayerID);
		if(GetConn()->More())
		{
			MonthGolds = GetConn()->GetInt();
			err = DBERR_NONE;
		}
		else
			throw ExceptionResult(DBERR_GOLDEXCHAGE_NOFOUND);
	}
	DBECATCH()
	return err;
}

// ----------------------------------------------------------------------------
// ��������³�ֵ����
DBError TBLGoldExchange::SavePays(U32 PlayerID, S32 MonthPays)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		INT_CHECK(MonthPays);

		M_SQL(GetConn(),"EXECUTE Sp_UpdateGoldMonthPays %d,%d", PlayerID, MonthPays);
		GetConn()->Exec();
	}
	DBCATCH(PlayerID)
	return err;
}

// ----------------------------------------------------------------------------
// ��ѯ�¶һ���������
DBError TBLGoldExchange::QueryConfig(stGoldConfig* config)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		int ExchangeType;
		M_SQL(GetConn(),"EXECUTE Sp_QueryGoldConfig");
		while(GetConn()->More())
		{
			ExchangeType = GetConn()->GetInt();
			if(ExchangeType < 0 || ExchangeType >= GEF_MAX)
				continue;
			
			config->Change[ExchangeType]	= GetConn()->GetInt();
			config->ItemID[ExchangeType]	= GetConn()->GetInt();
			config->Times[ExchangeType]		= GetConn()->GetInt();
			config->Condition[ExchangeType] = GetConn()->GetInt();
			config->Person[ExchangeType]	= GetConn()->GetInt();
		}
		err = DBERR_NONE;
	}
	DBECATCH()
	return err;
}

// ----------------------------------------------------------------------------
// ��ȡ����Ԫ���һ�����
DBError TBLGoldExchange::Draw(int AccountID, int PlayerID, const char* PlayerName, int type,
					int& Exchanges, int& Gold, int& TotalDrawGolds)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		if(!StrSafeCheck(PlayerName, 32))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		TINYINT_CHECK(type);

		Exchanges = 9999;
		Gold = 0;

		M_SQL(GetConn(),"EXECUTE Sp_DrawGoldExchange %d,%d,'%s',%d", AccountID, PlayerID, PlayerName, type);
		if(GetConn()->More() && GetConn()->GetInt() == 0)
		{
			Exchanges = GetConn()->GetInt();
			Gold = GetConn()->GetInt();
			TotalDrawGolds = GetConn()->GetInt();
			err = DBERR_NONE;
		}
		else
		{
			throw ExceptionResult(DBERR_GOLDEXCHANGE_DRAWFAILED);
		}
	}
	DBCATCH(PlayerID)
	return err;
}