#ifndef _DBTHREADMANAGER_H_
#define _DBTHREADMANAGER_H_

#include "base/types.h"
#ifndef _NTJ_UNITTEST
#include "DBLib/dbLib.h"
#else
class CDBConnPool;
#endif
#include "Thread.h"

#define MAX_DB_CONN 255

class  PacketBase;
struct stPacketHead;
class  DBThreadProcedure;
class  CThread;

//db�߳�Ҳ����Ҫ��ִ����ɺ�ͳһ���д���
typedef void (*DB_PACKET_HANDLE)(PacketBase*,void* param);

//��DB�̺߳�DBConn����,ʹÿ���̶߳������̶���DBConn
class DBThreadManager
{
public:
    DBThreadManager(void);
    ~DBThreadManager(void);
    
    //��ʼ�ͽ���ֻ�ܵ���1��
    bool Start(CDBConnPool* pConnPool,int connCount,int ctxId);
    bool Stop(void);
    
    //����������������Ӧ��db�߳�
    bool SendPacket(PacketBase* pPacket,int SocketHandle,stPacketHead *pHead);
    bool SendPacket(PacketBase* pPacket);
	
	//�Ƿ��������е����ݰ�
	bool IsFinished(void);
	
	//���������db process��,������������ͳһ�����
	//void DoEachPacket(DB_PACKET_HANDLE,void* param);
private:
    bool m_isStopped;
    
    CDBConnPool* m_pConnPool;
       
    DBThreadProcedure* m_dbThreadProcedures[MAX_DB_CONN];
    CThread*           m_dbThreads[MAX_DB_CONN];
    
    U32 m_dbThreadCount;
};

#endif /*_DBTHREADMANAGER_H_*/