
#pragma once
#ifndef _DBPOOL_H
#define _DBPOOL_H

#include "dbCore.h"
#include "dbException.h"
#include "Base\Locker.h"

#define DB_ATTACH_TIMEOUT       (30*1000)           //���û�ȡ��ʱΪ30s


//���ݿ����ӳ���
class CDBConnPool;

//���ݿ�����
class CDBConn : public CDBCore
{
public:
	CDBConn(CDBConnPool *);
    CDBConn();
    ~CDBConn();

public:
    void Detach();
	void SetThreadID(int nThreadID){m_nThreadID = nThreadID;};
	int  GetThreadID()const {return m_nThreadID;};

private:
	CDBConnPool	*				m_pPool;            //�󶨵����ӳ�
	int							m_nThreadID;		//�߳�ΨһID ��ΪDBConn�ᱻ���������߳� ˳���ID 
};


//���ݿ����ӳ���
class CDBConnPool
{
public:
	CDBConnPool();
	~CDBConnPool();

public:
	bool Open(int nMaxConn,const char *svr, const char *usr, const char *pwd, const char *db);
	void Close();

public:
	CDBConn *Lock();
	bool Unlock(CDBConn *);

protected:
	std::list<CDBConn *>        m_pFreeList;                //�ն���
	CMyCriticalSection            m_csFreeList;               //�ն���Ψһʹ��

	char                        m_Server[64];               //������
	char						m_UserName[128];            //�û���
	char						m_Password[128];            //����
	char						m_Database[64];             //���ݿ�

	int							m_nConnCount;				//������
};

class CDBOperator
{
	CDBConn *m_pObject;

public:
	CDBOperator(CDBConnPool *);
	~CDBOperator();
	CDBConn* GetConn()		{ return m_pObject;}
	CDBConn* operator->()	{ return m_pObject;}
};

void ConvertHex(char *pBuff,const unsigned char *pData,int iSize);

#endif

