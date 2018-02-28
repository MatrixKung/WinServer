#pragma once

class NETLIB_CLASS CNetSession
{
public:
	typedef unsigned long ID_HANDLE;
	static const int SERVER_ID_HANDLE = 0;

	enum ProtocolType
	{
		CONNECT,
		TICK,
		LOGIC,
		DISCONNECT,
	};

	SOCKADDR_IN* getAddress();

	CNetSession();
	virtual ~CNetSession();

	// ����
	virtual void onConnected()	{;}
	
	// ��ʱ
	virtual void onTimeout()	{;}

	// �Ͽ�����
	virtual void onDisconnect()	{;}

	// ����
	virtual void onTick()		{;}

public:
	ID_HANDLE getId();
	DWORD LastTickTime;

private:
	ID_HANDLE m_dwId;
	SOCKADDR_IN mSockaddr;

    static int ms_id;
};