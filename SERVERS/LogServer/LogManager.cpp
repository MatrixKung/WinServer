#include "LogManager.h"
#include "dblib/dblib.h"
#include "base/Log.h"
#include "Common/dbStruct.h"
#include "DBLayer/Data/DataBase.h"
#include <time.h>

DWORD WINAPI ThreadProcess( void* WriteThread )
{
	LogWriteThread* pThread = (LogWriteThread *)WriteThread;
	if(pThread)
	{
		pThread->run();
		pThread->exit();
	}
	return 0;
}

U32 GetCurrentDate()
{
	struct tm today;
	time_t curtime;
	time(&curtime);
	localtime_s(&today, &curtime);
	return ((today.tm_year-100)*10000 + (today.tm_mon+1)*100 + today.tm_mday);
}

/* =========================================================================*/
/*	��־д�����߳� LogWriteThread											*/ 
/*																			*/ 
/* =========================================================================*/
LogWriteThread::LogWriteThread () 
{
	mThread	= NULL;	
}

LogWriteThread::~LogWriteThread () 
{
}

void LogWriteThread::run( )
{
	CDBConn* mDBConn = LOGMANAGER->GetDBConn();
	if(NULL == mDBConn )
		return;

	U32	mDBDate = LOGMANAGER->GetDataBaseDate();
	stDBStruct* pData;

	while(LOGMANAGER->IsThreadLoop())
	{
		//�߳��жϴ����Ƿ�ɱ任���ݿ����
		//LOGMANAGER->HandleChangeDataBase(mDBDate, &mDBConn);

		pData = LOGMANAGER->GetLogData();
		if(pData)
		{
			if(pData->times > 2)
			{
				//���Զ��α�������������־
				if(pData->data)
				{
					g_Log.WriteError("One gamelog be droped(type=%d, playerid=%d).", 
							pData->data->optype, pData->data->sourceid);
					delete pData->data;
				}
				delete pData;
			}
			else
			{
				if(NULL == pData->data)
				{
					delete pData;
				}
				else
				{
					if(pData->data->save(mDBConn))
					{
						//�ɹ�,����ɾ��
						delete pData->data;
						delete pData;
					}
					else
					{
						//ʧ�ܣ�����Ȼ���·Żض�����󣬵ȴ��ٳ���һ��
						pData->times ++;				
						LOGMANAGER->AddLogData(pData);
					}	
				}
			}
		}
		else
		{
			::Sleep(10);
		}
	}

	if(mDBConn)
	{
		mDBConn->Detach();
	}
}

void LogWriteThread::start()
{
	mThread	=::CreateThread( NULL, 0, ThreadProcess , this, 0, NULL) ;
}

void LogWriteThread::exit()
{
	::CloseHandle(mThread);
	mThread = NULL;
}

/* =========================================================================*/
/*	��־�̹߳����� CLogManager												*/ 
/*																			*/ 
/* =========================================================================*/
CLogManager::CLogManager()
{
	mIsThreadLoop		= true;
	mCurrentDataBase	= NULL;
	mThreads			= NULL;
	mMaxThreads			= 0;
	mCurrentDBDate		= 0;
	mServerName[0]		= 0;
	mDBNamePrefix[0]	= 0;
	mUserName[0]		= 0;
	mPassword[0]		= 0;
	mMaxConns			= 0;
}

CLogManager::~CLogManager()
{
}

// ----------------------------------------------------------------------------
// Ψһʵ��
CLogManager* CLogManager::GetInstance()
{
	static CLogManager mgr;
	return &mgr;
}

CDBConn* CLogManager::GetDBConn()
{
	CLocker _lock(mCrit);
	CDBConnPool* mPool = NULL;
	CDBConn* mDBConn = NULL;
	if(mCurrentDataBase)
		mPool = mCurrentDataBase->GetPool();

	if(mPool)
	{
		mDBConn = mPool->Lock();
	}
	else
	{
		g_Log.WriteFocus("get db connect error from database pool!");
		return NULL;
	}

	if(mDBConn == NULL)
	{
		g_Log.WriteFocus("No one connect on database pool!");
		return NULL;
	}
	return mDBConn;
}

// ----------------------------------------------------------------------------
// ��ʼ����־�̹߳�����
bool CLogManager::Initialize(const char *svr, const char *db, const char *usr, const char *pwd, int maxconn /* = 5 */)
{
	dStrcpy(mServerName, 32, svr);
	dStrcpy(mDBNamePrefix, 32, db);
	dStrcpy(mUserName, 32, usr);
	dStrcpy(mPassword, 32, pwd);
	mMaxConns = maxconn;
	mMaxThreads = maxconn - 1;	

	mCurrentDBDate = GetCurrentDate();
	mCurrentDataBase = new DataBase;

	char szDBName[32];
	sprintf_s(szDBName, 32, "%s", mDBNamePrefix);
	//sprintf_s(szDBName, 32, "%s%d", mDBNamePrefix, mCurrentDBDate);
	
	DBError err = mCurrentDataBase->Initialize(mServerName, szDBName, mUserName, mPassword, mMaxConns);
	if(err != DBERR_NONE)
	{
		g_Log.WriteFocus("Error::%s���ݿ��ʼ��ʧ��!", db);
		return false;
	}

	g_Log.WriteLog("%s���ݿ��ʼ���ɹ�!",db);
	return true;
}

// ----------------------------------------------------------------------------
// �������ݿ�����
bool CLogManager::ChangeDataBase()
{	
	static U32 lasttime = ::GetTickCount();
	U32 curtime = ::GetTickCount();
	if(lasttime + 1800000 > curtime)	//ÿ30���Ӽ��һ��
		return false;

	lasttime = curtime;
	U32 currentDate = GetCurrentDate();	
	if(currentDate != mCurrentDBDate)
	{
		char szDBName[32];
		sprintf_s(szDBName, 32, "%s%d", mDBNamePrefix, currentDate);
		DataBase* tmpDatabase = new DataBase;
		DBError err = tmpDatabase->Initialize(mServerName, szDBName, mUserName, mPassword, mMaxConns);
		if(err != DBERR_NONE)
		{
			g_Log.WriteFocus("Error::%s���ݿ��ʼ��ʧ��!", szDBName);
			delete tmpDatabase;
			return false;
		}

		CLocker _lock(mCrit);
		mCurrentDBDate = currentDate;
		mFreeDataBase = mCurrentDataBase;
		mCurrentDataBase = tmpDatabase;
		mThreadChangeDBCount = 0;
	}
	else
	{
		CLocker _lock(mCrit);
		if(mThreadChangeDBCount == mMaxThreads)
		{
			SAFE_DELETE(mFreeDataBase);
			mThreadChangeDBCount = 0;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// �߳��жϴ����Ƿ�ɱ任���ݿ����
void CLogManager::HandleChangeDataBase(U32& DBDate, CDBConn** pDBConn)
{
	CLocker _lock(mCrit);
	if(mCurrentDBDate == DBDate)
		return;

	if(NULL == mCurrentDataBase)
		return;

	if(NULL == pDBConn || NULL == (*pDBConn))
		return;

	CDBConnPool* mPool = mCurrentDataBase->GetPool();
	CDBConn* mDBConn = NULL;
	if(NULL == mPool || (mDBConn = mPool->Lock()) == NULL)
		return;

	(*pDBConn)->Detach();
	DBDate = mCurrentDBDate;
	*pDBConn = mDBConn;
	mThreadChangeDBCount++;
}

// ----------------------------------------------------------------------------
// ����������������־д�������߳�
void CLogManager::Startup()
{
	mThreads = new LogWriteThread[mMaxThreads];
	for(int i = 0; i < mMaxThreads; i++)
		mThreads[i].start();
}

// ----------------------------------------------------------------------------
// �ر���־�̹߳�����
bool CLogManager::ShutDown()
{
	size_t notSaveCount = GetLogListSize();
	while ( 0 != notSaveCount)
	{
		g_Log.WriteLog("���ڵȴ���־ȫ��д�����ݿ�,ʣ��[%d]", notSaveCount);
		Sleep(2000);
		notSaveCount = GetLogListSize();
	}

	//֪ͨ��־�߳��˳�
	StopThread();

	SAFE_DELETE(mCurrentDataBase);
	SAFE_DELETE(mFreeDataBase);
	return true;
}

// ----------------------------------------------------------------------------
// ֪ͨ������־�߳���ֹѭ������׼���˳�
void CLogManager::StopThread()
{
	mIsThreadLoop = false;

	HANDLE* ExitThread = new HANDLE[mMaxThreads];
	for(int i = 0; i < mMaxThreads; i++)
	{
		ExitThread[i] = mThreads[i].getHandle();
	}
	::WaitForMultipleObjects(mMaxThreads, ExitThread, TRUE, INFINITE);
	delete[] ExitThread;

	SAFE_DELETE_ARRAY(mThreads);
}

// ----------------------------------------------------------------------------
// ���һ����־���ݵ���־�����б�
void CLogManager::AddLogData(stDBStruct* data)
{
	CLocker _lock(mCrit);
	mLogList.push_back(data);
}

// ----------------------------------------------------------------------------
// ����־�����б����ȡһ����־����
stDBStruct* CLogManager::GetLogData()
{
	CLocker _lock(mCrit);
	if(mLogList.empty())
		return NULL;
	stDBStruct* pData = mLogList.front();
	mLogList.pop_front();
	return pData;
}

size_t CLogManager::GetLogListSize()
{
	CLocker _lock(mCrit);
	return mLogList.size();
}

// ----------------------------------------------------------------------------
// ��������Զ�̵��͵���־��Ϣ
ServerEventFunction( CLogServer, HandleLogRecordRequest, "ANY_LOGSERVER_Record")
{
	int structType = STNULLLOG;
	try
	{
		structType = pPack->readInt(8);
		dbStruct* pStruct = dbStruct::creator(structType);
		if(!pStruct)
			return false;
		pStruct->unpack(pPack);
		stDBStruct* pData = new stDBStruct;
		pData->data = pStruct;
		pData->times = 1;
		CLogManager::GetInstance()->AddLogData(pData);
	}
	catch (...)
	{
		g_Log.WriteFocus("Found a error log struct.(HandleLogRecordRequest type=%d)",structType);
	}	
	return true;
}