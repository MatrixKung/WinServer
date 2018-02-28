#ifndef ACCOUNT_CLIENT_PACKET_PROCESS_H
#define ACCOUNT_CLIENT_PACKET_PROCESS_H

#include "Common/CommonPacket.h"
#ifdef _NTJ_UNITTEST_PRESS
#include "Common/PlayerStructEx.h"
#endif
#include "PlayerMgr.h"

using namespace Base;

class AccountClientPacketProcess : public CommonPacket
{
	typedef CommonPacket Parent;

	int		m_ConnectTimes;
public:
	AccountClientPacketProcess()
	{
		m_ConnectTimes = 0;
#ifdef _NTJ_UNITTEST_PRESS
		m_recvnum = 0;
#endif
		g_Log.WriteWarn("�ȴ��˺ŷ�����");
	}
#ifdef _NTJ_UNITTEST_PRESS
	int m_recvnum;
#endif
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

_inline void AccountClientPacketProcess::OnConnect(int Error)
{
	if(!Error)
    {
		SendClientConnect();
    }
}

_inline void AccountClientPacketProcess::OnDisconnect()
{
	Parent::OnDisconnect();

	if(m_ConnectTimes>=1)
	{
		g_Log.WriteError("������������Ͽ�");
	}
}

_inline void AccountClientPacketProcess::HandleClientLogin()
{
	m_ConnectTimes++;

	int nNetLineId = atoi( NetLineId.c_str() );
	int nNetGateId = atoi( NetGateId.c_str() );
	const char* szIp = UserNetIP.c_str();
	int port = atoi( UserNetPort.c_str() );

	RegisterServer(SERVICE_GATESERVER,nNetLineId,nNetGateId,m_ConnectTimes, szIp, port,(UserNetIP2 != "") ? UserNetIP2.c_str() : 0);

	if(m_ConnectTimes>1)
	{
		g_Log.WriteWarn("�����������˺ŷ�����");
	}
	else
	{
		g_Log.WriteLog("�����˺ŷ������ɹ�!");
	}
    //���͵�ǰ�����ʺ���Ϣ��world,�������°�world�ϵ��ʺŶ�Ӧgate������
    SERVER->GetPlayerManager()->SendAccounts();

	Parent::HandleClientLogin();
}

_inline void AccountClientPacketProcess::HandleClientLogout()
{
	Parent::HandleClientLogout();
}

_inline bool AccountClientPacketProcess::HandleGamePacket(stPacketHead *pHead,int iSize)
{
	SERVER->GetWorkQueue()->PostEvent(-3,pHead,iSize,true);
	return true;
}

#endif //ACCOUNT_CLIENT_PACKET_PROCESS_H

