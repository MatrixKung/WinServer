#pragma once

#ifndef __DTSERVERSOCKET_H__
#define __DTSERVERSOCKET_H__

#include <vector>
#include <hash_map>

#include "TcpSocket.h"
#include "Base/Locker.h"
#include "Base/Log.h"
#include "Base/memPool.h"
#include "base/bitStream.h"

#define MAX_PACKET_COUNT 2048

struct stServerParam
{
	char Name[64];
	int MinClients;
	int MaxClients;
};

struct stOpParam
{
	int msg;      //the eOpParamType
	U64 val;	  //the value of eOpParamType to be set
};

enum eOpParamType
{
	IDLE_TIMEOUT,			//���г�ʱ
	CONNECT_TIMEOUT,		//���ӳ�ʱ
	CONNECT_TYPE,			
};

class dtServerSocket;
class dtServerSocketClient;
typedef void*(*NET_CALLBACK_FN)(dtServerSocket *pServer,dtServerSocketClient *pClient,int Msg,void *pData);

//the class is listen socket
class dtServerSocket : public ISocket
{
public:
	typedef CTcpSocket Parent;

	enum
	{
		MAX_COMPLETION_THREAD_NUM	= 1,		//���߳�
		ADD_CLIENT_NUM				= 20,
		MAX_PENDING_SEND			= 50,
	};

	enum OP_CODE
	{
		OP_TIMETRACE,			 //����
		OP_NORMAL,
		OP_QUIT,
		OP_RESTART,
		OP_DISCONNECT,
		OP_SEND,
		OP_ADDCLIENT,			
		OP_MAINTENANCE,
		OP_SETPARAM,		    //���ÿͻ�����Ϣ
		
		OP_ON_CONNECT,
		OP_ON_DISCONNECT,
		OP_ON_PACKET,
	};

public:
	virtual bool Initialize(const char* ip, int port,void *param=NULL);
	virtual bool Start();
	virtual bool Stop ();
	virtual bool Restart()										{ return false;}				//����ʵ��
	virtual bool Send(const char *,int,int ctrlType)			{ return false;}				//����ʵ��
	virtual bool Connect()										{ return false;}				//����ʵ��
	virtual bool Disconnect(bool bForce)						{ return false;}				//����ʵ��

protected:
	virtual bool HandleReceive(OVERLAPPED_PLUS *,int ByteReceived)		{ return false;}		//����ʵ��
	virtual bool HandleConnect(OVERLAPPED_PLUS *,int ByteReceived)		{ return false;}		//����ʵ��
	virtual bool HandleSend(OVERLAPPED_PLUS *,int ByteSended)			{ return false;}		//����ʵ��

protected:
	virtual void OnNetFail(int nErrorCode,OVERLAPPED_PLUS *,int ByteSended){}
	virtual void OnDisconnect()									{}
	virtual void OnReceive(const char *pBuf, int nBufferSize)	{ return;		}				//����ʵ��
	virtual void OnConnect(int nErrorCode)						{}
	virtual void OnClear()										{}

private:
	static unsigned int WINAPI EventRoutine(LPVOID Param);
	static unsigned int WINAPI TimerRoutine(LPVOID Param);

public:
	friend class dtServerSocketClient;

	dtServerSocket(void);
	virtual ~dtServerSocket(void);

public:
	//״̬���
	bool IsShuttingDown()								{return m_bShuttingDown;};
	bool CanAccept()									{return m_bCanAccept;};
	int GetEnableClientNum()							{return m_nMaxClients-m_nClientCount;};

	void PostEvent(int Msg,void *pData=NULL);
	void PostEvent(int Msg,int id);
	void Send(int id,const char *,int,int ctrlType=0, bool bWebProtocl = false);
	void Send(int id,Base::BitStream &SendPacket,int ctrlType=0);
    void Send(Base::BitStream &SendPacket,int ctrlType=0);
    
    void DumpPackets(int onlineCount);
public:
	OVERLAPPED_PLUS*	GetBuffer(int SocketHandle,int iSize);
	void SetClientIdleTimeout(int SocketHandle,int idleTimeout);
	void SetClientConnectionTimeout(int SocketHandle,int ConnectionTimeout);
	void SetConnectionType(int SocketHandle,int Type);
	void SetNagle(bool flag) {m_bNagle = flag;}
	bool GetNagle() {return m_bNagle;}
	void CheckTimeEclipse();

protected:
	virtual ISocket *LoadClient	();
	bool AddClient				(int Num);
	int  AssignClientId			();
	dtServerSocketClient *GetClientById(int id);
	virtual int Maintenance		();

protected:
	CMemPool*			m_pMemPool;

	int			m_nClientCount;			//��ǰ�Ѿ����õĿͻ�������
	int			m_nMaxClients;			//���ɲ����Ŀͻ�������(����������)
	int			m_nMinClients;			//��С�ɲ����Ŀͻ�������
	volatile int	m_nIdSeed;

	bool		m_bShuttingDown;		//�Ƿ�׼���رձ�ʶ
	bool		m_bCanAccept;			//�Ƿ������������
	bool		m_bNagle;

	int			m_seq;
protected:
	stdext::hash_map<int,dtServerSocketClient*>		m_ClientList;	//���Ӵ������(inclued all the client info)
	dtServerSocketClient** m_ClientArray;	//���Ӷ���(equal to the m_ClientList)

protected:
	//ά���߳�
	HANDLE		m_hAddClientEvent;
	HANDLE		m_hEndTimerEvent;
	HANDLE		m_hTimerThread;

	//��ɶ˿ں��߳�
	HANDLE		m_CompletionPort;  //IOCP
	HANDLE		m_CompletionThread[MAX_COMPLETION_THREAD_NUM];		//�̳߳�
	int			m_CompletionThreadNum; //the num of IOCP Thread

    //��ز���
    DWORD		m_NetFNListTotalFreq[MAX_PACKET_COUNT];
    DWORD		m_NetFNListTotalSize[MAX_PACKET_COUNT];
public:
	char		m_Name[64];           // teh neame of server
};

#endif