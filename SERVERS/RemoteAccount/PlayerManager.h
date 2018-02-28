#pragma once

#include "RemoteAccount.h"
#include "Common\ServerFramwork.h"
#include "base/Locker.h"
#include "Common/RegisterCommon.h"
#include "Common/Common.h"

extern WorldServerInfo g_worldServerInfo;
typedef unsigned long long uint64_t;
class CRemoteAccount;

class CPlayerManager
{
public:
	CPlayerManager();
	virtual ~CPlayerManager();

	typedef stdext::hash_map< std::string, uint64_t > PendingMap;

	typedef stdext::hash_map< std::string, int > AccountSockets;

	enum e_AccountStatus
	{
		STATUS_NONE,					// ��Ч
		STATUS_CHECK_DB,				// ��鱾��AccountDB
		STATUS_CHECK_PASS9,				// ���Զ��Pass9
		STATUS_WAIT_INVATION_CODE,

		STATUS_COUNT,
	};
    
private:

	AccountSockets mAccountSocket;
	AccountSockets mAccountSocketId;

	volatile LONG mIdSeed;
};