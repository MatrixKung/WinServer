#ifndef CHAT_BASE_H
#define CHAT_BASE_H

//#include "Base/Log.h"
#ifndef _WINDOWS_
#include <WinSock2.h>
#include <windows.h>
#endif
#pragma once
#include <iostream>
#include <sstream>
#include "Base/bitStream.h"
#include "Common/PlayerStruct.h"
#include "Common/ScriptVariable.h"
#include "Common/PacketType.h"
#include "Common/UtilString.h"
#include "BASE/base64.h"

const int CHAT_MESSAGE_MAX_LENGTH = 512;
const int CHAT_MESSAGE_MAX_NAME_LENGTH = 32;

using namespace std;

enum eSendChatResponseType
{
	SEND_CHAT_ACK_UNKNOWN = 0,
	SEND_CHAT_ACK_SUCCESSED,
	SEND_CHAT_ACK_TOOFAST,
	SEND_CHAT_ACK_REFUSED,
	SEND_CHAT_ACK_ERROR,
	SEND_CHAT_ACK_NOPLAYER,
	SEND_CHAT_ACK_ILLEGAL,
};

enum enChatType
{
	CHAT_TYPE_NORMAL        = BIT(0),//��������
	CHAT_TYPE_ITEM          = BIT(1),//����Ʒ������
};

#define CHATBEGIN(name, script) SCRIPTVARTYPES8(name, script, 0); const int CHAT_VALUE_BEING = __LINE__;
#define CHATDEFINE(name, script) SCRIPTVARTYPES8(name, script, __LINE__-CHAT_VALUE_BEING);
enum
{
	CHAT_MSG_TYPE_SYSTEM,
	CHAT_MSG_TYPE_WORLDPLUS,
	CHAT_MSG_TYPE_WORLD,
	CHAT_MSG_TYPE_LINE,
	CHAT_MSG_TYPE_RACE,
	CHAT_MSG_TYPE_ORG,
	CHAT_MSG_TYPE_SQUAD,
	CHAT_MSG_TYPE_TEAM,
	CHAT_MSG_TYPE_GROUP,
	CHAT_MSG_TYPE_REALCITY,
	CHAT_MSG_TYPE_NEARBY,
	CHAT_MSG_TYPE_PRIVATE,
	CHAT_MSG_TYPE_P2P,
	CHAT_MSG_TYPE_GM,
	CHAT_MSG_TYPE_PERSON,
	CHAT_MSG_TYPE_PATH,
	CHAT_MSG_TYPE_BATTLE,
	CHAT_MSG_TYPE_RECORD,
	CHAT_MSG_TYPE_SYSPLUS,
	CHAT_MSG_TYPE_LEAGUE,
	CHAT_MSG_TYPE_WORLD_2,
	CHAT_MSG_TYPE_SUPER_WORLD,
	CHAT_MSG_TYPE_BFCORPS,
	CHAT_MSG_TYPE_MSGBOX,
	CHAT_MSG_TYPE_CHALLENGE,
	CHAT_MSG_TYPE_AWARD,
	CHAT_MSG_TYPE_SUPER_WORLD2,
	CHAT_MSG_TYPE_UNKNOWN,
	CHAT_MSG_TYPE_COUNT,
};

/*CHATBEGIN(CHAT_MSG_TYPE_SYSTEM,		"chatMsg_System")		// ϵͳ
CHATDEFINE(CHAT_MSG_TYPE_WORLDPLUS,	"chatMsg_WordPlus")		// ����
CHATDEFINE(CHAT_MSG_TYPE_WORLD,		"chatMsg_Word")			// ���
CHATDEFINE(CHAT_MSG_TYPE_LINE,		"chatMsg_Line")			// ȫ��
CHATDEFINE(CHAT_MSG_TYPE_RACE,		"chatMsg_Race")			// ����
CHATDEFINE(CHAT_MSG_TYPE_ORG,		"chatMsg_Org")			// ���
CHATDEFINE(CHAT_MSG_TYPE_SQUAD,		"chatMsg_Squad")		// ��
CHATDEFINE(CHAT_MSG_TYPE_TEAM,		"chatMsg_Team")			// ����
CHATDEFINE(CHAT_MSG_TYPE_GROUP,		"chatMsg_Group")		// Ⱥ��Ϣ
CHATDEFINE(CHAT_MSG_TYPE_REALCITY,	"chatMsg_RealCity")		// ͬ��
CHATDEFINE(CHAT_MSG_TYPE_NEARBY,	"chatMsg_Nearby")		// ����
CHATDEFINE(CHAT_MSG_TYPE_PRIVATE,	"chatMsg_Private")		// ˽��
CHATDEFINE(CHAT_MSG_TYPE_P2P,		"chatMsg_P2P")			// ˽�Ĵ���
CHATDEFINE(CHAT_MSG_TYPE_GM,		"chatMsg_GM")			// GM��Ϣ
CHATDEFINE(CHAT_MSG_TYPE_PERSON,	"chatMsg_Person")		// ����
CHATDEFINE(CHAT_MSG_TYPE_PATH,		"chatMsg_Path")			// Ѱ��
CHATDEFINE(CHAT_MSG_TYPE_BATTLE,	"chatMsg_Battle")		// ս��
CHATDEFINE(CHAT_MSG_TYPE_RECORD,	"chatMsg_Record")		// ����
CHATDEFINE(CHAT_MSG_TYPE_SYSPLUS,	"chatMsg_SysPlus")		// ϵͳ����Ƶ��
CHATDEFINE(CHAT_MSG_TYPE_LEAGUE,	"chatMsg_League")		// ����
CHATDEFINE(CHAT_MSG_TYPE_WORLD_2,   "chatMsg_World2")	    // �ͻ������½ǵ�ȫ���㲥
CHATDEFINE(CHAT_MSG_TYPE_SUPER_WORLD,"chatMsg_SuperWorld")  // ��������
CHATDEFINE(CHAT_MSG_TYPE_BFCORPS,   "chatMsg_BFCorps")		// ս���Ŷ�
CHATDEFINE(CHAT_MSG_TYPE_MSGBOX,	"chatMsg_MsgBox")		// ȫ������
CHATDEFINE(CHAT_MSG_TYPE_CHALLENGE,	"chatMsg_Challenge")	// ������ȫ������
CHATDEFINE(CHAT_MSG_TYPE_AWARD,     "chatMsg_MadDialAward")	// ����ת�̻���Ϣ��¼
CHATDEFINE(CHAT_MSG_TYPE_SUPER_WORLD2,"chatMsg_SuperWorld2")// ǧ�ﴫ������ĳ�������
CHATDEFINE(CHAT_MSG_TYPE_UNKNOWN,	"chatMsg_Unknown")		// Ԥ��
CHATDEFINE(CHAT_MSG_TYPE_COUNT,		"chatMsg_Count")	*/


//enum eChatMessageType
//{
//	CHAT_MSG_TYPE_SYSTEM,		// ϵͳ
//	CHAT_MSG_TYPE_WORLDPLUS,	// ����
//	CHAT_MSG_TYPE_WORLD,		// ���
//	CHAT_MSG_TYPE_LINE,			// ȫ��
//	//----------------------------------------
//	CHAT_MSG_TYPE_RACE,			// ����
//	CHAT_MSG_TYPE_ORG,			// ���
//	CHAT_MSG_TYPE_SQUAD,		// ��
//	CHAT_MSG_TYPE_TEAM,			// ����
//	CHAT_MSG_TYPE_GROUP,		// Ⱥ��Ϣ
//	CHAT_MSG_TYPE_REALCITY,		// ͬ��
//	CHAT_MSG_TYPE_NEARBY,		// ����
//	//----------------------------------------
//	CHAT_MSG_TYPE_PRIVATE,		// ˽��
//	CHAT_MSG_TYPE_P2P,			// ˽�Ĵ���
//	CHAT_MSG_TYPE_GM,			// GM��Ϣ
//	CHAT_MSG_TYPE_PERSON,		// ����
//	CHAT_MSG_TYPE_PATH,			// Ѱ��
//	CHAT_MSG_TYPE_BATTLE,		// ս��
//	CHAT_MSG_TYPE_RECORD ,	    // ����
//	CHAT_MSG_TYPE_SYSPLUS,		// ϵͳ����Ƶ��
//	CHAT_MSG_TYPE_LEAGUE,		// ����
//	CHAT_MSG_TYPE_UNKNOWN,		// Ԥ��
//
//    CHAT_MSG_TYPE_COUNT
//};

static char* strChatMessageType[CHAT_MSG_TYPE_COUNT] = 
{
	"ϵͳ",
	"����",
	"���",
	"ȫ��",
	//----------------------------------------
	"����",
	"���",
    "��",
	"����",
    "Ⱥ��Ϣ",
	"ͬ��",
	"����",
	//----------------------------------------
	"˽��",
	"˽�Ĵ���",
	"GM��Ϣ",
	"����",
	"Ѱ��",
	"ս��",
	"����",
	"����",
	"����",
	"..."
};

struct stChatMessage
{
	int	nSender;	// ������
	int nRecver;	// ������ 
	int nSenderLineID;		//�����ߵ��߳�id

	BYTE btMessageType;
	char szMessage[CHAT_MESSAGE_MAX_LENGTH];
	char szSenderName[CHAT_MESSAGE_MAX_NAME_LENGTH];
	char szRecverName[CHAT_MESSAGE_MAX_NAME_LENGTH];
	BYTE btPrivilegeFlagType; //��Ȩ���� 

	stChatMessage()
	{
		memset( this, 0, sizeof( stChatMessage ) );
		nSenderLineID = -1;
	}

	static void PackMessage( stChatMessage& chatMessage, Base::BitStream& stream )
	{
		static size_t buffSize = 0;
		stream.writeInt( chatMessage.nSender, Base::Bit32 );
		stream.writeInt( chatMessage.nRecver, Base::Bit32 );
		stream.writeInt( chatMessage.nSenderLineID, Base::Bit32 );

		stream.writeInt( chatMessage.btMessageType, Base::Bit8 );
		stream.writeString(chatMessage.szSenderName, CHAT_MESSAGE_MAX_NAME_LENGTH );
		stream.writeString( chatMessage.szRecverName, CHAT_MESSAGE_MAX_NAME_LENGTH );
		stream.writeString(chatMessage.szMessage, CHAT_MESSAGE_MAX_LENGTH);
		stream.writeInt( chatMessage.btPrivilegeFlagType, Base::Bit8 );
	}

	static void PackMessage1(stChatMessage& chatMessage, Base::BitStream& stream)
	{
		static size_t buffSize = 0;
		stream.writeInt(chatMessage.nSender, Base::Bit32);
		stream.writeInt(chatMessage.nRecver, Base::Bit32);
		stream.writeInt(chatMessage.nSenderLineID, Base::Bit32);

		stream.writeInt(chatMessage.btMessageType, Base::Bit8);
		stream.writeString(Util::MbcsToUtf8(chatMessage.szSenderName), CHAT_MESSAGE_MAX_NAME_LENGTH);
		stream.writeString(Util::MbcsToUtf8(chatMessage.szRecverName), CHAT_MESSAGE_MAX_NAME_LENGTH);
		//stream.writeString(Util::MbcsToUtf8(chatMessage.szMessage), CHAT_MESSAGE_MAX_LENGTH);
		std::string str = Util::MbcsToUtf8(chatMessage.szMessage);
		stream.writeString(base64_encode((unsigned char *)str.c_str(), str.length()), CHAT_MESSAGE_MAX_LENGTH);
		stream.writeInt(chatMessage.btPrivilegeFlagType, Base::Bit8);
	}

	static void UnpackMessage( stChatMessage& chatMessage, Base::BitStream& stream )
	{
		static size_t buffSize = 0;
		chatMessage.nSender = (int)stream.readInt( Base::Bit32 );
		chatMessage.nRecver = (int)stream.readInt( Base::Bit32 );
		chatMessage.nSenderLineID = (int)stream.readInt( Base::Bit32 );

		chatMessage.btMessageType = (BYTE)stream.readInt( Base::Bit8 );
		stream.readString( chatMessage.szSenderName, CHAT_MESSAGE_MAX_NAME_LENGTH );
		stream.readString( chatMessage.szRecverName, CHAT_MESSAGE_MAX_NAME_LENGTH );
		stream.readString(chatMessage.szMessage, CHAT_MESSAGE_MAX_LENGTH);
		chatMessage.btPrivilegeFlagType = (BYTE)stream.readInt( Base::Bit8 );
	}

	static void UnpackMessage1(stChatMessage& chatMessage, Base::BitStream& stream)
	{
		static size_t buffSize = 0;
		chatMessage.nSender = (int)stream.readInt(Base::Bit32);
		chatMessage.nRecver = (int)stream.readInt(Base::Bit32);
		chatMessage.nSenderLineID = (int)stream.readInt(Base::Bit32);

		chatMessage.btMessageType = (BYTE)stream.readInt(Base::Bit8);
		stream.readString(chatMessage.szSenderName, CHAT_MESSAGE_MAX_NAME_LENGTH);
		stream.readString(chatMessage.szRecverName, CHAT_MESSAGE_MAX_NAME_LENGTH);
		//stream.readString(chatMessage.szMessage, CHAT_MESSAGE_MAX_LENGTH);
		std::string str = stream.readString(CHAT_MESSAGE_MAX_LENGTH);
		std::string str1 = base64_decode(str);
		dMemcpy(chatMessage.szMessage, str1.c_str(), str1.length());
		chatMessage.btPrivilegeFlagType = (BYTE)stream.readInt(Base::Bit8);
	}

};

struct AutoMsg
{
	AutoMsg(void) : playerId(0),channel(0),startTime(0),endTime(0),lastSendTime(0),freq(-1),times(-1) {}

	stChatMessage msg;          //��Ϣ
	int           playerId;     //����������ҵ���Ϊ��
	int           channel;      //Ƶ��
	__time64_t    startTime;    //���͵Ŀ�ʼʱ��
	__time64_t    endTime;      //��Ϣ����ʱ��
	__time64_t    lastSendTime; //�ϴη����͵�ʱ��
	int           freq;         //����s����1��
	int           times;        //�ܹ����Ͷ��ٴ�

	template< typename _Ty >
	void ReadData( _Ty* packet )
	{
        stChatMessage::UnpackMessage(msg,*packet);
        playerId = packet->readInt(Base::Bit32);
        channel  = packet->readInt(Base::Bit32);
        packet->readBits(sizeof(startTime) * 8,&startTime);
        packet->readBits(sizeof(endTime) * 8,&endTime);
        packet->readBits(sizeof(lastSendTime) * 8,&lastSendTime);
        freq  = packet->readInt(Base::Bit32);
        times = packet->readInt(Base::Bit32);
	}

	template< typename _Ty >
	void WriteData( _Ty* packet )
	{
        stChatMessage::PackMessage(msg,*packet);
        packet->writeInt(playerId,Base::Bit32);
        packet->writeInt(channel,Base::Bit32);
        packet->writeBits(sizeof(startTime) * 8,&startTime);
        packet->writeBits(sizeof(endTime)  * 8,&endTime);
        packet->writeBits(sizeof(lastSendTime) * 8,&lastSendTime);
        packet->writeInt(freq,Base::Bit32);
        packet->writeInt(times,Base::Bit32);
	}
};

//ע��������Ϣ
struct ChatPlayerInfo
{
    ChatPlayerInfo(void)
    {
        memset(this,0,sizeof(ChatPlayerInfo));
    }
    
    U32  playerId;
    U32  accountId;
    char playerName[32];
    char accountName[ACCOUNT_NAME_LENGTH];
	 char loginIp[32];

    //��ǰ������gate
    U32  lineId;
    U32  gateId;
};

#endif
