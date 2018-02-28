#include "Define.h"
#include "dblib/dbLib.h"
#include "Base/Log.h"
#include "TBLAccount.h"
#include "DBUtility.h"
#include "Common/PacketType.h"
#include "Common/PlayerStruct.h"

// ----------------------------------------------------------------------------
// �����ʺ����Ʋ�ѯ�ʺ�ID
DBError TBLAccount::QueryID(const char* AccountName, int& AccountID)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		if(!StrSafeCheck(AccountName, ACCOUNT_NAME_LENGTH))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		M_SQL(GetConn(),"SELECT AccountID FROM Tbl_Account WHERE AccountName='%s'", AccountName);
		if(GetConn()->More())
			AccountID = GetConn()->GetInt();
		else
			throw ExceptionResult(DBERR_ACCOUNT_NOFOUND);
		err = DBERR_NONE;
	}
	DBSCATCH(AccountName)
	return err;
}

// ----------------------------------------------------------------------------
// �����ʺ�ID��ѯ�ʺ�
TBLAccount::ALoginType TBLAccount::QueryAccountById(int AccountId)
{
	DBError err = DBERR_UNKNOWERR;
	ALoginType nLogin = Login_Max;
	try
	{
		//��¼��ʽ�����ٵ�¼��������ͨ��¼
		//0���ʺ�ûע�ᣬ1�����ٵ�¼AccountId��¼��2����ͨ�ʺ������¼

		M_SQL(GetConn(),"EXECUTE Sp_CheckAccountExist '%d'", AccountId);
		if(GetConn()->More())
		{
			nLogin = (ALoginType)GetConn()->GetInt();
		}
		else 
			throw ExceptionResult(DBERR_UNKNOWERR);
	}
	DBSCATCH(AccountId)
	return nLogin;
}

// ----------------------------------------------------------------------------
// �����¼��Ϣ
DBError TBLAccount::SaveLoginInfo(int AccountID, const char* LoginIP, int LoginError)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		if(!StrSafeCheck(LoginIP, 20))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		M_SQL(GetConn(),"EXECUTE Sp_UpdateAccountLogin  %d,'%s',%d", AccountID, LoginIP ? LoginIP:"0.0.0.0", LoginError);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(AccountID)
	return err;
}

void TBLAccount::GetAccountInfo(stAccount* Info)
{
	Info->AccountID			= GetConn()->GetInt();
	dStrcpy(Info->AccountName, ACCOUNT_NAME_LENGTH, GetConn()->GetString());
	Info->IsAdult			= GetConn()->GetInt() != 0;
	Info->Mode				= GetConn()->GetInt();
	Info->Status			= GetConn()->GetInt();
	Info->GMFlag			= GetConn()->GetInt();
	Info->LoginStatus		= GetConn()->GetInt();
	Info->LoginTime			= GetConn()->GetTime();
	Info->LogoutTime		= GetConn()->GetTime();
	dStrcpy(Info->LoginIP, 20, GetConn()->GetString());
	Info->LoginError		= GetConn()->GetInt();
	Info->TotalOnlineTime	= GetConn()->GetInt();
	Info->AccountFunction	= GetConn()->GetInt();
	Info->dTotalOnlineTime	= GetConn()->GetInt();
	Info->dTotalOfflineTime	= GetConn()->GetInt();
}

void TBLAccount::GetAccountInfo_GM(stAccount* Info)
{
	Info->AccountID			= GetConn()->GetInt();
	dStrcpy(Info->AccountName, ACCOUNT_NAME_LENGTH, GetConn()->GetString());
	Info->IsAdult			= GetConn()->GetInt() != 0;
	Info->Mode				= GetConn()->GetInt();
	Info->Status			= GetConn()->GetInt();
	Info->GMFlag			= GetConn()->GetInt();
	Info->LoginStatus		= GetConn()->GetInt();
	Info->LoginTime			= GetConn()->GetTime();
	Info->LogoutTime		= GetConn()->GetTime();
	dStrcpy(Info->LoginIP, 20, GetConn()->GetString());
	Info->LoginError		= GetConn()->GetInt();
	Info->TotalOnlineTime	= GetConn()->GetInt();
	Info->AccountFunction	= GetConn()->GetInt();
	Info->dTotalOnlineTime	= GetConn()->GetInt();
	Info->dTotalOfflineTime	= GetConn()->GetInt();
}

// ----------------------------------------------------------------------------
// �����ʺ����Ʋ�ѯ�ʺ�����
DBError TBLAccount::Load(const char* AccountName, stAccount* pInfo)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{		
		if(!StrSafeCheck(AccountName, ACCOUNT_NAME_LENGTH))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		M_SQL(GetConn(),"SELECT       \
						 [AccountId]   \
						,[AccountName]\
						,[IsAdult]\
						,[Mode]\
						,[Status]\
						,[GMFlag]\
						,[LoginStatus]\
						,[LoginTime]\
						,[LogoutTime]\
						,[LoginIP]\
						,[LoginError]\
						,[TotalOnlineTime]\
						,[AccountFunction]\
						,[dTotalOnlineTime]\
						,[dTotalOfflineTime] FROM Tbl_Account WHERE AccountName='%s'", AccountName);
		if(GetConn()->More())
			GetAccountInfo(pInfo);
		else
			throw ExceptionResult(DBERR_ACCOUNT_NOFOUND);
		err = DBERR_NONE;
	}
	DBSCATCH(AccountName)
	return err;
}

// ----------------------------------------------------------------------------
// �����ʺ����Ʋ�ѯ�ʺ�����
DBError	TBLAccount::Load(int AccountID, stAccount* pInfo)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{		
		M_SQL(GetConn(),"SELECT       \
						[AccountId]   \
						,[AccountName]\
						,[IsAdult]\
						,[Mode]\
						,[Status]\
						,[GMFlag]\
						,[LoginStatus]\
						,[LoginTime]\
						,[LogoutTime]\
						,[LoginIP]\
						,[LoginError]\
						,[TotalOnlineTime]\
						,[AccountFunction]\
						,[dTotalOnlineTime]\
						,[dTotalOfflineTime] FROM Tbl_Account WHERE AccountId=%d", AccountID);
		if(GetConn()->More())
			GetAccountInfo(pInfo);
		else
			throw ExceptionResult(DBERR_ACCOUNT_NOFOUND);
		err = DBERR_NONE;
	}
	DBSCATCH(AccountID);
	return err;
}
// ----------------------------------------------------------------------------
// �����ʺ�����ѯ��¼IP
DBError	TBLAccount::QueryLoginIP(const char* AccountName, int& AccountID, char* LoginIP)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		if(!StrSafeCheck(AccountName, ACCOUNT_NAME_LENGTH))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		M_SQL(GetConn(),"SELECT AccountID,LoginIP FROM Tbl_Account WHERE AccountName='%s'",AccountName);
		if(GetConn()->More())
		{
			AccountID			= GetConn()->GetInt();
			dStrcpy(LoginIP, 20, GetConn()->GetString());
		}
		else
			throw ExceptionResult(DBERR_ACCOUNT_NOFOUND);
		err = DBERR_NONE;
	}
	DBSCATCH(AccountName)
	return err;
}

// ----------------------------------------------------------------------------
// �����ʺŵ�¼ʱ��
DBError TBLAccount::UpdateLoginTime(int AccountID)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{		
		M_SQL(GetConn(),"EXECUTE Sp_UpdateLoginTime  %d", AccountID);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(AccountID)
	return err;
}

DBError TBLAccount::UpdateAccountLogoutTime(int AccountID,const char* time)
{
    DBError err = DBERR_UNKNOWERR;
	try
	{		
		M_SQL(GetConn(),"EXECUTE Sp_UpdateAccountLogoutTime  %d,'%s'", AccountID,time);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(AccountID)
	return err;
}

// ----------------------------------------------------------------------------
// �����ʺŵǳ�ʱ��
DBError TBLAccount::UpdateLogoutTime(int AccountID, int CostTime, int dTotalOfflineTime, int dTotalOnlineTime)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		M_SQL(GetConn(),"EXECUTE Sp_UpdateLogoutTime  %d,%d,%d,%d", 
			AccountID, CostTime, dTotalOfflineTime, dTotalOnlineTime);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(AccountID)
	return err;
}

// ----------------------------------------------------------------------------
// �����ʺ�
DBError TBLAccount::MakeAccount(const char* AccountName, int& accountId, int flag, bool bApple /*= false*/)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		U_INT_CHECK(accountId);
		U_TINYINT_CHECK(flag);

		if (!StrSafeCheck(AccountName, ACCOUNT_NAME_LENGTH))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		M_SQL(GetConn(), "EXECUTE Sp_MakeAccount '%s', %d, %d", AccountName, accountId, (int)flag);
		if (GetConn()->More())
		{
			int ret = GetConn()->GetInt();
			if(ret == -1)
				throw ExceptionResult(DBERR_ACCOUNT_IDEXISTS);	//�ʺ�ID�Ѿ����ڵ��ʺ���������
			else if(ret == -2)
				throw ExceptionResult(DBERR_ACCOUNT_NAMEEXISTS);//�ʺ����Ʋ����ڵ��ʺ�ID������
			else if (ret == -3)
				throw ExceptionResult(DBERR_ACCOUNT_IDNAMENOMATCH);	//�ʺ�ID���ʺ�����ƥ��
			else
			{
				accountId = GetConn()->GetInt();
				err = DBERR_NONE;
			}
		}
		else
			throw ExceptionResult(DBERR_ACCOUNT_NOFOUND);
	}
	DBSCATCH(AccountName)
	return err;
}

// ----------------------------------------------------------------------------
// ����ʺ��Ƿ����
DBError TBLAccount::CheckAccount( const char* AccountName, bool& isExisted )
{
	isExisted = false;
	int AccountID;
	DBError err = QueryID(AccountName, AccountID);
	if(err == DBERR_NONE)
	{
		if(AccountID <= 0)
			err = DBERR_ACCOUNT_NOFOUND;
		else
			isExisted = true;			
	}
	return err;
}

// ----------------------------------------------------------------------------
// ��ѯGM�ʺ�
DBError TBLAccount::QueryGMAccount(int GMFlag, std::vector<std::pair<int, std::string> >& Info)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		U_TINYINT_CHECK(GMFlag);

		M_SQL(GetConn(),"SELECT AccountID,AccountName FROM Tbl_Account WHERE GMFlag=%d", GMFlag);
		while(GetConn()->More())
		{
			int v1         = GetConn()->GetInt();
			std::string v2 = GetConn()->GetString();
			Info.push_back(std::make_pair(v1,v2));
		}
		err = DBERR_NONE;
	}
	DBCATCH(GMFlag)
	return err;
}

// ----------------------------------------------------------------------------
// ����GM�ʺű�־
DBError TBLAccount::UpdateGMFlag(const char* AccountName, int GMFlag)
{
	DBError err = DBERR_UNKNOWERR;	
	try
	{
		U_TINYINT_CHECK(GMFlag);

		if(!StrSafeCheck(AccountName, ACCOUNT_NAME_LENGTH))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		M_SQL(GetConn(),"UPDATE Tbl_Account SET GMFlag=%d WHERE AccountName='%s'", GMFlag, AccountName);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBSCATCH(AccountName)
	return err;
}

// ----------------------------------------------------------------------------
// GM���߲�ѯ�ʺţ�������ѯ��
DBError TBLAccount::QueryForGM(int Mode, const char* Key, const char* Condition, std::vector<stAccount>& AccountInfo)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		U_TINYINT_CHECK(Mode);

		if(!StrSafeCheck(Key, ACCOUNT_NAME_LENGTH))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		//Conditionû����SQL������飬���ô���ȫ���Ժ���
		//if(!StrSafeCheck(Condition, strlen(Condition)))

		std::stringstream s;
		if(Mode == 0|| Mode == 2)
        {
			s << "EXEC Sp_QueryForGM 0,'";
			s << Key << "',";
			s << Mode << ",'";
			s << Condition << "'";
			//M_SQL(GetConn(),"EXECUTE Sp_QueryForGM 0,'%s',%d,'%s'", Key, Mode, Condition);
        }
		else 
        {
			int idKey = atoi(Key);
			s << "EXEC Sp_QueryForGM ";
			s << idKey << ",'',";
			s << Mode << ",'";
			s << Condition << "'";
			//M_SQL(GetConn(),"EXECUTE Sp_QueryForGM %s,'',%d,'%s'", Key, Mode, Condition);
        }

		GetConn()->SQLExt(s.str().c_str());
		while(GetConn()->Fetch())
		{
			stAccount Info;
			GetAccountInfo_GM(&Info);
			AccountInfo.push_back(Info);
		}
		err = DBERR_NONE;
	}
	DBSCATCH(Key)
	return err;
}

// ----------------------------------------------------------------------------
// �����ʺ�״̬
DBError TBLAccount::SetAccountStatus( int accountId, unsigned int status )
{
	DBError err = DBERR_UNKNOWERR;	
	try
	{
		U_INT_CHECK(accountId);		
		INT_CHECK(status);

		M_SQL(GetConn(),"UPDATE Tbl_Account SET [Status]=%d WHERE AccountID=%d", status, accountId );
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(accountId)
	return err;
}

// ----------------------------------------------------------------------------
// �����ʺ�״̬(statusΪ0ʱ��1) 
DBError TBLAccount::SetAccountStatus( int accountId)
{
	DBError err = DBERR_UNKNOWERR;	
	try
	{
		U_INT_CHECK(accountId);	

		M_SQL(GetConn(),"UPDATE Tbl_Account SET [Status]=1 WHERE AccountID=%d AND [Status]=0", accountId );
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(accountId)
	return err;
}

DBError TBLAccount::CheckFirstLoginStatus( int accountId, int playerId, const char* playName, int bonusCount )
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		if (bonusCount < 0)
			bonusCount = 0;
		
		U_INT_CHECK(accountId);
		U_INT_CHECK(playerId);

		if(!StrSafeCheck(playName, COMMON_STRING_LENGTH))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		M_SQL(GetConn(),"EXECUTE Sp_FirstLoginBonus %d, %d,'%s', %d", accountId, playerId, playName, bonusCount );
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(accountId)
	return err;
}

DBError TBLAccount::CheckOldPlayerLogin( int accountId, int playerId, const char* playName )
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
 
		U_INT_CHECK(accountId);
		U_INT_CHECK(playerId);

		if(!StrSafeCheck(playName, COMMON_STRING_LENGTH))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		M_SQL(GetConn(),"EXECUTE Sp_OldPlayerLoginBonus %d, %d,'%s'", accountId, playerId, playName );
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(accountId)
		return err;
}

DBError TBLAccount::LoadPlayerList(int AccountID, Players& playerList)
{
    DBError err = DBERR_UNKNOWERR;

	try
	{		
		U_INT_CHECK(AccountID);
		M_SQL(GetConn(), "SELECT top(18) PlayerID FROM Tbl_Player WHERE AccountID=%d order by [Level] DESC", AccountID);

		while (GetConn()->More())
		{
			PlayerInfo info;
			info.playerID = GetConn()->GetInt();

            playerList.push_back(info);
		}

		err = DBERR_NONE;
	}
	DBCATCH(AccountID)
	return err;
}

DBError	TBLAccount::UpdateNetBarDrawTime( int accountId,int index )
{
	/*DBError err = DBERR_UNKNOWERR;	
	try
	{
		U_INT_CHECK(accountId);	

		M_SQL(GetConn(),"EXECUTE Sp_UpdateNetBarDrawTime %d,%d", accountId,index );
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(accountId)
		return err;*/
	return DBERR_NONE;
}

DBError	TBLAccount::QueryNetBarDrawTime( int accountId,int* pTimeList)
{
	DBError err = DBERR_UNKNOWERR;	
	try
	{
		U_INT_CHECK(accountId);	

		M_SQL(GetConn(),"SELECT NetBarDrawTime1,NetBarDrawTime2,NetBarDrawTime3,"\
			"NetBarDrawTime4,NetBarDrawTime5,NetBarDrawTime6,NetBarDrawTime7,"\
			"NetBarDrawTime8,NetBarDrawTime9,NetBarDrawTime10 FROM Tbl_Account WHERE AccountId=%d", accountId);
		if(GetConn()->More())
		{
			for(int i = 0; i < MAX_NETBAR_DRAW; i++)
				pTimeList[i] = GetConn()->GetTime();
		}
		err = DBERR_NONE;
	}
	DBCATCH(accountId)
	return err;
}
