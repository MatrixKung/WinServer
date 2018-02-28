 #pragma once

#include <sstream>
#include <memory>

#include "build_no.h"
#include "DBLayer/Data/DBError.h"
#include "zlib/zlib.h"

#ifndef USE_QUICKLZ
#define SXZ_COMPRESS(a,b,c,d)	compress((Bytef*)a,(u_long*)b, (const Bytef*)c, (u_long)d)
#define SXZ_UNCOMPRESS(a,b,c,d)	uncompress((Bytef*)a,(u_long*)b, (const Bytef*)c, (u_long)d)
#else
#define SXZ_COMPRESS(a,b,c,d)	qz_compress((char*)a,(unsigned long*)b, (const char*)c, (unsigned long)d)
#define SXZ_UNCOMPRESS(a,b,c,d)	qz_uncompress((char*)a,(unsigned long*)b, (const char*)c, (unsigned long)d)
#endif//USE_QUICKLZ

     
const unsigned int MIN_PACKET_SIZE			= 128 * 10;
const unsigned int MAX_PACKET_SIZE			= 256*1024;
const unsigned int COMMON_STRING_LENGTH		= 32;
const unsigned int NETWORK_PROTOCOL_VERSION	= 1;
const unsigned int MD5_STRING_LENGTH		= 16;
const unsigned int MAX_ACTOR_COUNT			= 1;
const unsigned int MAX_DROP_TIME			= 30000;
const unsigned int MAX_CREATE_ACTOR_COUNT	= 1;
const unsigned int URL_STRING_LENGTH		= 1024;
const unsigned int MAX_PASS9_ERRMSG_LEN		= 128;
const unsigned int MAX_YY_REQUEST_CODE_LEN	= 1024;
const unsigned int MAX_PASS9_USERNAME_LEN	= 50;
const unsigned int MAX_PASS9_TICKET_LEN		= 32;
const unsigned int MAX_OPENID_LEN			= 128;
const unsigned int ACCOUNT_NAME_LENGTH		= 100;
const unsigned int PASSWORD_LENGTH			= 50;
const unsigned int MAX_NETBAR_DRAW			= 10;
const unsigned int MAX_BADWORDS_LENGTH		= 250;

/*--------------------------------------------------------------------------------------------*/

#define ITOA(NUMBER)  #NUMBER
#define __TEXT_LINE__(LINE) ITOA(LINE)
#ifdef _DEBUG
#define HACK(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ��HACK�� �� "#MSG
#else
#define HACK(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : HACK!! Error: "#MSG
#endif

#define DB_CATCH_LOG(TAG) 		catch (CDBException &e){	\
	std::stringstream LogStream; \
	LogStream << "Error Message("<<#TAG << "):"<< e.w_msgtext <<  std::endl << e.m_dberrstr << std::ends; \
	g_Log.WriteError(LogStream); \
}catch (...){ \
	std::stringstream LogStream; \
	LogStream << "Error Message("<<#TAG << "):���Զ����쳣" << std::ends; \
	g_Log.WriteError(LogStream); \
}


#define T_UID	 unsigned int
#define UID_BITS 32

typedef unsigned int t_queueToken;

// ���º��ض��� [10/11/2010 cwm]
//#define __ENTER_FUNCTION {try{	
//
//#define __LEAVE_FUNCTION }catch(...){}}

enum LOGIN_STATUS
{
	SF_FROM_ACCOUNT,
	SF_FROM_WORLD,
	SF_FROM_GATE,
	SF_FROM_ZONE
};

enum SERVER_TYPE
{
	SERVICE_NONE,
	SERVICE_CLIENT,
	SERVICE_REMOTESERVER,
	SERVICE_ZONESERVER,
	SERVICE_ACCOUNTSERVER,
	SERVICE_WORLDSERVER,
	SERVICE_GATESERVER,
	SERVICE_GAMESERVER,
	SERVICE_REGAMESERVER,			// ��Ϸ������GATE
	SERVICE_DATAAGENT,
	SERVICE_LOGSERVER,
	SERVICE_REGISTER,
	SERVICE_QUEUE,					// �Ŷ�ר��firera
	SERVICE_CHATSERVER,				// �������
	SERVICE_WORLDDB,
	SERVICE_GETSTATUS,				// ȡ������״̬
	SERVICE_TWEIBO,					// ��Ѷ΢����½
	SERVICE_YYLOGIN,				// YY��½
};

enum GAME_LOGIC_MSG
{
	BEGIN_LOGIC_MSG,
	ON_ZONE_RESTART,
	END_LOGIC_MSG,
};

enum EM_MSG
{
	MSG_INVALID,
	MSG_CONNECT,
	MSG_CONNECTED,
	MSG_AUTH,
	MSG_AUTHED,
	MSG_INFO,//����account��ȡ�˺Ž�ɫ��Ϣ
	MSG_INFO_RET,//account�����˺Ž�ɫ��Ϣ���ͻ���

	MSG_END,
};

//_inline bool IsValidMessage(int Msg) {return Msg>BEGIN_NET_MESSAGE && Msg<END_NET_MESSAGE;}

#define ERROR_CODE int

enum LogicErr
{
	NONE_ERROR = 0,

	VERSION_ERROR = DBERR_MAX,			//�汾�Ŵ���
	LANG_VERSION_ERROR,					//���԰汾����

	GAME_SERVER_INVALID,				//ѡ��ķ�����������
	GAME_UID_ERROR,						//�Ự����(UID������)
	GAME_VCODE_ERROR,					//��֤�벻��ȷ
	GAME_LINE_FULL,						//��·����
	GAME_ZONE_NOFOUND,					//��ͼδ�ҵ�(��ͼΪ�����߷�����Ϊ���ô˵�ͼ)
	GAME_WORLD_FULL,					//�������ﵽ����������

	ACCOUNT_ID_ERROR,					//��¼�ʺŴ���(��Ϸ���ʺŴ���)
	ACCOUNT_IN_USEING,                  //�ʺ��������ط��ѵ�½,������ҹ�һ���½��
	ACCOUNT_FREEZE,                     //�ʺű�����

	PLAYER_CREATE_FULL,					//��ɫ����
	PLAYER_NOFOUND,						//�ʺ�δ��¼���߽�ɫ������
	PLAYER_NAME_ERROR,					//���ִ���
	PLAYER_OLDPLAYER_BONUS_ERROR,		//����ҵ�¼��������
	PLAYER_FIRSTLOGIN_BONUS_ERROR,		//�»����״ε�½��������
	PLAYER_PUNISH,						//��½��֤���������3�Σ�5�����ڲ��ܵ�½
	PLAYER_STRUCT_ERROR,				//��ҽ�ɫ�쳣����ת��ʧ��
	UNKNOW_ERROR,						//δ֪����

	ILLEGAL_GM,							//���ݿ���δƥ�䵽��½��gm�ʺŵ�ip��mac��ַ

    SERVER_NOT_OPENED,                  //������δ����
    

	ERROR_COUNT							
};

enum ERROR_SOURCE
{
	LOGIN_ACCOUNT_SOURCE,				//��½�ʺŷ���������
	LOGIN_GATE_SOURCE,					//��½���س���
	LOGIN_GAME_SOURCE,					//��½��Ϸ����
	SWITCH_MAP_SOURCE,					//�л���ͼ����
	CREATE_PLAYER_SOURCE,				//������ɫ����
	DELETE_PLAYER_SOURCE,				//ɾ����ɫ����
};

enum ePlayerDataTransferType
{
	PLAYER_TRANS_TYPE_BASE = 0,
	PLAYER_TRANS_TYPE_NORMAL,
	PLAYER_TRANS_TYPE_ADV,

	PLAYER_TRANS_TYPE_END,
};

#define MAKE_PACKET( packetName, ... ) \
	CMemGuard buf##packetName( MAX_PACKET_SIZE MEM_GUARD_PARAM ); \
	Base::BitStream packetName( buf##packetName.get(), MAX_PACKET_SIZE ); \
	stPacketHead* packetName##Head =  IPacket::BuildPacketHead( packetName, __VA_ARGS__ );

#define SEND_PACKET( packetName, sockId ) \
	packetName##Head->PacketSize = packetName.getPosition() - sizeof( stPacketHead ); \
	SERVER->GetServerSocket()->Send( sockId, packetName );

#define SEND_PACKET_SOCK( packetName, sockId, _sock ) \
	packetName##Head->PacketSize = packetName.getPosition() - sizeof( stPacketHead ); \
	(_sock)->Send( sockId, packetName );

#define SEND_PACKET_NSOCK( packetName, _sock ) \
	packetName##Head->PacketSize = packetName.getPosition() - sizeof( stPacketHead ); \
	(_sock)->Send( packetName );

#define PASS9 "pass9"

struct WorldServerInfo
{
	static const int MAX_WORLD_COUNT = 256;

	int AreaId[MAX_WORLD_COUNT];
	int PlayerCount[MAX_WORLD_COUNT];
	int maxPlayerCount[MAX_WORLD_COUNT];
	int maxCreatedPlayers[MAX_WORLD_COUNT];

	int Count;
	char operation[COMMON_STRING_LENGTH];

	WorldServerInfo()
	{
		memset(this, 0, sizeof(WorldServerInfo));
		strcpy_s(operation, sizeof(operation), PASS9);
	}

	void convertPlayer(float status1, float status2)
	{
		for( int i = 0; i < Count; i++ )
		{
			float percent = 0;

			if (0 != maxPlayerCount[i])
			{
				percent = (float)PlayerCount[i] / (float)maxPlayerCount[i];
			}
            
			if(percent < status1)
				PlayerCount[i] = 1;
			else if(percent < status2)
				PlayerCount[i] = 2;
			else 
				PlayerCount[i] = 3;
		}
	}

	template< typename _Ty >
	void ReadData( _Ty* packet )
	{
		Count = packet->readInt( 32 );
		packet->readString(operation, sizeof(operation));

		for( int i = 0; i < Count; i++ )
		{
			AreaId[i]        = packet->readInt( 8 );
			PlayerCount[i]    = packet->readInt( 32 );
			maxPlayerCount[i] = packet->readInt( 32 );
			maxCreatedPlayers[i] = packet->readInt( 32 );
		}
	}

	template< typename _Ty >
	void WriteData( _Ty* packet )
	{
		packet->writeInt( Count, 32 );
		packet->writeString(operation, sizeof(operation));

		for( int i = 0; i < Count; i++ )
		{
			packet->writeInt( AreaId[i], 8 );
			packet->writeInt( PlayerCount[i], 32 );
			packet->writeInt( maxPlayerCount[i], 32 );
			packet->writeInt( maxCreatedPlayers[i], 32 );
		}
	}
};

struct stSwitchInfo
{
	int IP;
	int Port;
	int Id;
};

enum
{
  QUERYACCOUNTFORPLAYER_3RDFLAG  = 1<<0,		//��ѯ������ƽ̨��Ա�ʺ�ע���־
  QUERYACCOUNTFORPLAYER_PROMTION = 1<<1,		//��ѯ��ɫ���ƹ�Ա�ʺ�ID
};
