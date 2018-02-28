#pragma once

// ������Ϣ����Ҫ������Ӧ��
#define DECLARE_NET_EVENT(p) p##Request, p##Response

// ֪ͨ����Ϣ������ҪӦ��
#define DECLARE_NET_NOTIFY(p) p##Notify


struct NetEventTypes
{
	enum Type
	{
		None,

		CLIENT_SERVER_Login,				// �ͻ��˵�¼
		CLIENT_SERVER_LoginAck,

		CLIENT_SERVER_Logout,				// �ͻ��˵ǳ�
		CLIENT_SERVER_LogoutAck,

		MONITOR_SERVER_RegisterServer,		// ������ע��

		MONITOR_SERVER_Register,			// ������ע��
		MONITOR_SERVER_RegisterAck,

		CLIENT_SERVER_ServerList,			// ����������б�
		CLIENT_SERVER_ServerListAck,

		SERVER_CLIENT_ServerOffline,		// ����������
		SERVER_CLIENT_ServerOfflineAck,

		SERVER_CLIENT_ServerOnline,			// ����������
		SERVER_CLIENT_ServerOnlineAck,


		CLIENT_SERVER_RefreshServer,		// ˢ�·���������
	
		CLIENT_SERVER_StartServer,			// ��������
	
		CLIENT_SERVER_StopServer,			// ֹͣ����
        CLIENT_SERVER_TerminateServer,

		SERVER_MONITOR_StartServer,			// ��������
	
		SERVER_MONITOR_StopServer,			// ֹͣ����

		SERVER_MONITOR_RefreshServer,		// ˢ�·���

		SERVER_MONITOR_UpdateServer,		// ���·���
		
		CLIENT_SERVER_Update,				// ֪ͨ����������

		MONITOR_SERVER_ServerStatusUpdate,	// ������֪ͨ������

		SERVER_CLIENT_ServerStatusUpdate,
        
        //��ط���������ͣ��ָ��
		CLIENT_SERVER_STOPSERVERINTIME,
		SERVER_MONITOR_STOPSERVERINTIME,
        MONITOR_STOP_SERVER_REQUEST,
        MONITOR_STOP_SERVER_RESPOND,
        
        MSG_HEARTBEAT,

		Count,		
	};
} ;

template< typename _Ty >
struct StructBase
{
	StructBase()
	{
		memset( this, 0, sizeof(_Ty) );
	}

	StructBase( const _Ty& src )
	{
		memcpy( this, &src, sizeof( _Ty ) );
	}

	StructBase& operator = ( _Ty& src )
	{
		memcpy( this, &src, sizeof( _Ty ) );
		return *this;
	}
};

const int COMMON_STRING_LENGTH_T = 32;

struct ServerStruct : public StructBase<ServerStruct >
{
    ServerStruct(void): LastStatus(0),IsWaitForKill(0),ProcessHandle(0),lineId(0),isSelected(0) {}

    std::string     groupName;
	std::string		ServerName;			// ������
    std::string     ip;

	std::string		PathName;			// ����·��
	std::string		Argument;			// ��������

	std::string		UpdateUrl;			// ����·��
    std::string     updateFile;         // �����ļ����غ�ı���·��

	int				SessionId;			// �ỰID
    char			LastStatus;		    // ״̬
	char			Status;				// ״̬
	char			CPUPayload;			// CPU����

	bool			IsWaitForKill;		// �ȴ����̽���
    bool            IsCfgAutoRestart;
	HANDLE			ProcessHandle;		// ���̾��
    bool            isProcessSelfCreate;// �����Ƿ��Լ�����
    HANDLE          processExitHandle;

    std::string     svrUpdateFile;      //���صķ��������������ļ�·��

	bool			IsAutoRestart;
    int             lastHeartbeatTime;
    int             lineId;
    bool            isSelected;

    std::string GetId()
    {
        char buf[256] = {0};
        itoa(lineId,buf,10);
        return groupName + "-" + ServerName + "-" + ip + "-" + buf;
    }
};

struct ServerStatus
{
	enum Type
	{
		None,
		Start,
        ReStart,
		Stop,
        PassiveStop,
		Unknown,
		Offline,
		Update,
        Dead,
        UPDATE_STATUS_START,
        UPDATE_STATUS_DOWNLOAD,
        UPDATE_STATUS_PROGRASS,
        UPDATE_STATUS_FINISHED,
        UPDATE_STATUS_ERROR,
		Count
	};

    std::string groupName;
    std::string serverName;

	int type;
	int CPUPayload;

	ServerStatus()
	{
		type = None;
		CPUPayload = 0;
	}
};

static CPacketStream& operator >> ( CPacketStream& packet, ServerStatus& serverStatus )
{
	packet >> serverStatus.groupName >> serverStatus.serverName >> (int&)serverStatus.type >> serverStatus.CPUPayload;
	return packet;
}

static CPacketStream& operator << ( CPacketStream& packet, ServerStatus& serverStatus )
{
	packet << serverStatus.groupName << serverStatus.serverName << (int&)serverStatus.type << serverStatus.CPUPayload;
	return packet;
}