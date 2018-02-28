#include "Define.h"
#include "dblib/dbLib.h"
#include "Base/Log.h"
#include "TBLBadWordPlayer.h"


// ----------------------------------------------------------------------------
// ��¼˵�˹������ֵ����
DBError TBLBadWordPlayer::SaveBadWordPlayer(int playerId, const char* badWord)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		M_SQL(GetConn(),"EXECUTE Sp_UpdateBadWordPlayer  %d,'%s'", playerId, badWord);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBECATCH()
	return err;
}

