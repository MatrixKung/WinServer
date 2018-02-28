#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H
#include "LogServer.h"
#include <deque>
#include <assert.h>
#include "Common/dbStruct.h"
#include "Base/Locker.h"

#define DISABLE_DB_STAT 1

/* =========================================================================*/
/*	��־д�����߳� LogWriteThread											*/ 
/*																			*/ 
/* =========================================================================*/
class LogWriteThread
{
public:
	LogWriteThread();
	~LogWriteThread();
	void start();
	void run();
	void exit();
	HANDLE getHandle() { return mThread;}
private:
	HANDLE	 mThread ;	// �߳̾��
};

struct stDBStruct
{
	dbStruct* data;		//��־����
	int times;			//���Դ���
};

/* =========================================================================*/
/*	��־�̹߳����� CLogManager												*/ 
/*																			*/ 
/* =========================================================================*/
class CLogManager
{
public:
	CLogManager();
	~CLogManager();
	static CLogManager* GetInstance();

	bool Initialize(const char*, const char*, const char*, const char*, int);
	void Startup();
	bool ShutDown();
	void AddLogData(stDBStruct*);
	stDBStruct* GetLogData();
	size_t GetLogListSize();
	inline U32 GetDataBaseDate() { return mCurrentDBDate;}
	bool IsThreadLoop() { return mIsThreadLoop;}
	CDBConn* GetDBConn();
	bool ChangeDataBase();
	void StopThread();
	void HandleChangeDataBase(U32& DBDate, CDBConn** pDBConn);
private:
	LogWriteThread*				mThreads;				//��־д�����߳���
	int							mMaxThreads;			//����߳���
	volatile bool				mIsThreadLoop;			//�Ƿ������߳�ѭ��

	DataBase*					mCurrentDataBase;		//��ǰ���ݿ�������
	DataBase*					mFreeDataBase;			//��Ҫ�ͷŵ����ݿ�������

	CMyCriticalSection			mCrit;					//�ٽ���
	std::deque<stDBStruct*>		mLogList;				//��־���ݶ���

	char						mServerName[32];		//��������
	char						mDBNamePrefix[32];		//��ǰ���ݿ�ǰ׺��
	char						mUserName[32];			//�û���
	char						mPassword[32];			//����
	U32							mMaxConns;				//������ݿ�������	

	U32							mCurrentDBDate;			//��������
	U32							mThreadChangeDBCount;	//�߳�ȷ���������ݿ�������
}; 

#define LOGMANAGER CLogManager::GetInstance()

#endif