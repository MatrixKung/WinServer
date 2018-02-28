#pragma once
#include "Common\CommonPacket.h"

class WorldClientPacketProcess : public CommonPacket
{
	typedef CommonPacket Parent;

	int		m_ConnectTimes;
public:
	WorldClientPacketProcess()
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

_inline void WorldClientPacketProcess::OnConnect(int Error)
{
	if(!Error)
		SendClientConnect();
}

_inline void WorldClientPacketProcess::OnDisconnect()
{
	Parent::OnDisconnect();
	if(m_ConnectTimes>=1)
	{
		g_Log.WriteError("������������Ͽ�");
	}
}

_inline void WorldClientPacketProcess::HandleClientLogin()
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

	RegisterServer(SERVICE_CHATSERVER,1,1,m_ConnectTimes);
	Parent::HandleClientLogin();
}

_inline void WorldClientPacketProcess::HandleClientLogout()
{
	Parent::HandleClientLogout();
}

_inline bool WorldClientPacketProcess::HandleGamePacket(stPacketHead *pHead,int iSize)
{
	CChatServer::getInstance()->getWorkQueue()->PostEvent(0,pHead,iSize,true);
	return true;
}