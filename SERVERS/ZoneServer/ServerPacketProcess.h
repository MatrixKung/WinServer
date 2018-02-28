#ifndef WORLD_CLIENT_PACKET_PROCESS_H
#define WORLD_CLIENT_PACKET_PROCESS_H

#include "Common/CommonPacket.h"

using namespace Base;

class ServerPacketProcess : public CommonPacket
{
	typedef CommonPacket Parent;

	int		m_ConnectTimes;
public:
	ServerPacketProcess()
	{
		m_ConnectTimes = 0;
		g_Log.WriteWarn("�ȴ��������������");
	}

	void OnConnect(int Error=0);
	void OnDisconnect();
protected:
	bool HandleGamePacket(stPacketHead *pHead,int iSize);
	void HandleClientLogin();
	void HandleClientLogout();

//��Ϣ����

//��Ϣ����

//���ݿ����
};

_inline void ServerPacketProcess::OnConnect(int Error)
{
	if(!Error)
		SendClientConnect();
}

_inline void ServerPacketProcess::OnDisconnect()
{
	Parent::OnDisconnect();
	if(m_ConnectTimes>=1)
	{
		g_Log.WriteError("������������Ͽ�");
	}
}

_inline void ServerPacketProcess::HandleClientLogin()
{
	m_ConnectTimes++;

	if(m_ConnectTimes>1)
	{
		g_Log.WriteWarn("�������������������");
	}
	else
	{
		g_Log.WriteLog("��������������ɹ�!");
	}

	RegisterServer(SERVICE_ACCOUNTSERVER,1,1,m_ConnectTimes);

	Parent::HandleClientLogin();
}

_inline void ServerPacketProcess::HandleClientLogout()
{
	Parent::HandleClientLogout();
}

_inline bool ServerPacketProcess::HandleGamePacket(stPacketHead *pHead,int iSize)
{
	//SERVER->GetWorkQueue()->PostEvent(0,pHead,iSize,true);
	return true;
}

#endif

