#include "ChatMgr.h"

#include "..\ChatServer\PlayerMgr.h"
#include "..\ChatServer\ServerMgr.h"
#include "..\ChatServer\ChatServer.h"
#include "WINTCP/AsyncSocket.h"
#include <string>

#include <time.h>
#include "Common/MemGuard.h"
#include "Common/FilterOpt.h"
#include "Common/BanInfoData.h"
#include "AtlConv.h"

#define CHAT_PACKET_SIZE (sizeof(stChatMessage) + 32)

enum CharTarget
{
	CHAT_TARGET_PERSON = 0, //����
	CHAT_TARGET_TEAM,       //���
	CHAT_TARGET_GUILD,      //����
	CHAT_TARGET_ZONE,       //��ͼ
	CHAT_TARGET_ALLZONE,    //ȫ��
};

static char* _province_city[40][30];

CChatManager::CChatManager()
{
	m_chatLog.SetLogName("chatrec", false);
}

CChatManager::~CChatManager()
{
}

const char* _sep = "%#!@$^";

void CChatManager::writeLog(int playerId, int type, const char* msg, int playerIdTo)
{
	try
	{
		if (type >= CHAT_MSG_TYPE_UNKNOWN)
		{
			type = CHAT_MSG_TYPE_UNKNOWN;
		}

		if (0 == msg)
		{
			msg = "";
		}

		if (playerIdTo < 0)
		{
			ChatPlayerInfo srcPlayer;
			bool hasSrc = SERVER->getPlayerMgr()->GetPlayerInfo(playerId, srcPlayer);

			m_chatLog.WriteLog("%s%s%d%s%s%s%d%s%s%s%s", hasSrc ? srcPlayer.accountName : "?",
				_sep,
				playerId,
				_sep,
				hasSrc ? srcPlayer.playerName : "?",
				_sep,
				0,
				_sep,
				"ȫ������",
				_sep,
				msg);
		}
		else if (playerIdTo == 0 || playerIdTo == playerId)
		{
			ChatPlayerInfo srcPlayer;
			bool hasSrc = SERVER->getPlayerMgr()->GetPlayerInfo(playerId, srcPlayer);

			m_chatLog.WriteLog("%s%s%d%s%s%s%d%s%s%s%s", hasSrc ? srcPlayer.accountName : "?",
				_sep,
				playerId,
				_sep,
				hasSrc ? srcPlayer.playerName : "?",
				_sep,
				0,
				_sep,
				"?",
				_sep,
				msg);

		}
		else
		{
			ChatPlayerInfo srcPlayer;
			ChatPlayerInfo dstPlayer;
			bool hasSrc = SERVER->getPlayerMgr()->GetPlayerInfo(playerId, srcPlayer);
			bool hasDst = SERVER->getPlayerMgr()->GetPlayerInfo(playerIdTo, dstPlayer);

			m_chatLog.WriteLog("%s%s%d%s%s%s%d%s%s%s%s", hasSrc ? srcPlayer.accountName : "?",
				_sep,
				playerId,
				_sep,
				hasSrc ? srcPlayer.playerName : "?",
				_sep,
				playerIdTo,
				_sep,
				hasDst ? dstPlayer.playerName : "?",
				_sep,
				msg);
		}
	}
	catch (...)
	{
	}
}

void CChatManager::Initialize()
{
	//ע��8������Ƶ��
	for (int i = 0; i < 8; i++)
	{
		m_FamilyChannels[i] = GetChannelManager()->RegisterChannel(CHAT_MSG_TYPE_RACE, "");
	}

	initCityChannels();
	g_BanInfoRepository.read();
}

CChannelManager* CChatManager::GetChannelManager()
{
	return &m_channelManager;
}

void CChatManager::HandleZoneSendMessage(int lineId, stPacketHead* pHead, Base::BitStream* pBitStream)
{
	stChatMessage msg;
	stChatMessage::UnpackMessage(msg, *pBitStream);

	std::string sendName = CChatServer::getInstance()->getPlayerMgr()->getPlayerName(msg.nSender);

	if (sendName != "")
	{
		dStrcpy(msg.szSenderName, sizeof(msg.szSenderName), sendName.c_str());
	}

	switch (msg.btMessageType)
	{
	case CHAT_MSG_TYPE_SYSTEM:
	case CHAT_MSG_TYPE_SYSPLUS:
	{
		if (msg.nRecver == 0)
		{
			SendMessageToLine(msg, lineId);
			g_Log.WriteLog("ȫ����������:%s, ���ID:%d, �������:%s", msg.szMessage, msg.nSender, msg.szSenderName);
		}
		else
		{
			SendMessageToWorld(msg);
			g_Log.WriteLog("ȫ����������:%s, ���ID:%d, �������:%s", msg.szMessage, msg.nSender, msg.szSenderName);
		}
	}
	break;
	case CHAT_MSG_TYPE_LINE:
		SendMessageToLine(msg, lineId);
		break;
	case CHAT_MSG_TYPE_WORLD:
		SendMessageToWorld(msg);
		break;
	case CHAT_MSG_TYPE_WORLDPLUS:
	{
		SendMessageToWorld(msg);
		writeLog(msg.nSender, msg.btMessageType, msg.szMessage, -1);
		break;
	}
	case CHAT_MSG_TYPE_MSGBOX:
		SendMessageToWorld(msg);
		break;
	case CHAT_MSG_TYPE_SUPER_WORLD:
		SendMessageToWorld(msg);
		break;
	case CHAT_MSG_TYPE_SUPER_WORLD2:
		SendMessageToWorld(msg);
		break;
	case CHAT_MSG_TYPE_RACE:
	{
		if (msg.nRecver >= 1 && msg.nRecver <= 8)
		{
			SendMessageToChannel(msg, m_FamilyChannels[msg.nRecver - 1]);
		}
	}
	break;
	case CHAT_MSG_TYPE_TEAM:
	{
		U32 channelId = GetChannelManager()->GetChannelId(msg.nRecver, msg.btMessageType);
		SendMessageToChannel(msg, channelId);
	}
	break;
	case CHAT_MSG_TYPE_SQUAD:
	{
		U32 channelId = GetChannelManager()->GetChannelId(msg.nRecver, msg.btMessageType);
		SendMessageToChannel(msg, channelId);
	}
	break;
	case CHAT_MSG_TYPE_AWARD:
	{
		SendMessageToWorld(msg);
	}
	break;
	default:
		break;
	}
}

void CChatManager::SendChatResponse(int nSocket, int nAccountId, char cType, int value, BYTE nChannel)
{
	char buf[64];
	Base::BitStream SendPacket(buf, sizeof(buf));
	stPacketHead* pHead = IPacket::BuildPacketHead(SendPacket, "CLIENT_CHAT_SendMessageResponse_Notify");

	pHead->DestServerType = SERVICE_CLIENT;
	pHead->Id = nAccountId;
	pHead->DestZoneId = value;
	pHead->SrcZoneId = cType;
	pHead->LineServerId = nChannel;

	pHead->PacketSize = SendPacket.getPosition() - sizeof(stPacketHead);

	CChatServer::getInstance()->getServerSocket()->Send(nSocket, SendPacket);
}

void CChatManager::initCityChannels()
{
	_province_city[0][0] = "1";
	_province_city[0][1] = "����";


	_province_city[1][0] = "1";
	_province_city[1][1] = "�Ϻ�";


	_province_city[2][0] = "1";
	_province_city[2][1] = "���";


	_province_city[3][0] = "12";
	_province_city[3][1] = "�ӱ�";
	_province_city[3][2] = "ʯ��ׯ";
	_province_city[3][3] = "��ɽ";
	_province_city[3][4] = "�ػʵ�";
	_province_city[3][5] = "����";
	_province_city[3][6] = "��̨";
	_province_city[3][7] = "����";
	_province_city[3][8] = "�żҿ�";
	_province_city[3][9] = "�е�";
	_province_city[3][10] = "����";
	_province_city[3][11] = "�ȷ�";
	_province_city[3][12] = "��ˮ";

	_province_city[4][0] = "12";
	_province_city[4][1] = "ɽ��";
	_province_city[4][2] = "̫ԭ";
	_province_city[4][3] = "��ͬ";
	_province_city[4][4] = "��Ȫ";
	_province_city[4][5] = "����";
	_province_city[4][6] = "����";
	_province_city[4][7] = "˷��";
	_province_city[4][8] = "����";
	_province_city[4][9] = "�˳�";
	_province_city[4][10] = "����";
	_province_city[4][11] = "�ٷ�";
	_province_city[4][12] = "����";


	_province_city[5][0] = "13";
	_province_city[5][1] = "���ɹ�";
	_province_city[5][2] = "���ͺ���";
	_province_city[5][3] = "��ͷ";
	_province_city[5][4] = "�ں�";
	_province_city[5][5] = "���";
	_province_city[5][6] = "ͨ��";
	_province_city[5][7] = "������˹";
	_province_city[5][8] = "���ױ���";
	_province_city[5][9] = "�����׶�";
	_province_city[5][10] = "�����첼";
	_province_city[5][11] = "�˰�";
	_province_city[5][12] = "���ֹ���";
	_province_city[5][13] = "������";


	_province_city[6][0] = "15";
	_province_city[6][1] = "����";
	_province_city[6][2] = "����";
	_province_city[6][3] = "����";
	_province_city[6][4] = "��ɽ";
	_province_city[6][5] = "��˳";
	_province_city[6][6] = "��Ϫ";
	_province_city[6][7] = "����";
	_province_city[6][8] = "����";
	_province_city[6][9] = "Ӫ��";
	_province_city[6][10] = "����";
	_province_city[6][11] = "����";
	_province_city[6][12] = "�̽�";
	_province_city[6][13] = "����";
	_province_city[6][14] = "����";
	_province_city[6][15] = "��«��";

	_province_city[7][0] = "10";
	_province_city[7][1] = "����";
	_province_city[7][2] = "����";
	_province_city[7][3] = "����";
	_province_city[7][4] = "��ƽ";
	_province_city[7][5] = "��Դ";
	_province_city[7][6] = "ͨ��";
	_province_city[7][7] = "��ɽ";
	_province_city[7][8] = "��ԭ";
	_province_city[7][9] = "�׳�";
	_province_city[7][10] = "�ӱ�";

	_province_city[8][0] = "13";
	_province_city[8][1] = "������";
	_province_city[8][2] = "������";
	_province_city[8][3] = "�������";
	_province_city[8][4] = "ĵ����";
	_province_city[8][5] = "��ľ˹";
	_province_city[8][6] = "����";
	_province_city[8][7] = "����";
	_province_city[8][8] = "˫Ѽɽ";
	_province_city[8][9] = "����";
	_province_city[8][10] = "��̨��";
	_province_city[8][11] = "�׸�";
	_province_city[8][12] = "�ں�";
	_province_city[8][13] = "�绯";


	_province_city[9][0] = "14";
	_province_city[9][1] = "����";
	_province_city[9][2] = "�Ͼ�";
	_province_city[9][3] = "��";
	_province_city[9][4] = "����";
	_province_city[9][5] = "����";
	_province_city[9][6] = "����";
	_province_city[9][7] = "��ͨ";
	_province_city[9][8] = "����";
	_province_city[9][9] = "�γ�";
	_province_city[9][10] = "����";
	_province_city[9][11] = "���Ƹ�";
	_province_city[9][12] = "����";
	_province_city[9][13] = "̩��";
	_province_city[9][14] = "��Ǩ";


	_province_city[10][0] = "12";
	_province_city[10][1] = "�㽭";
	_province_city[10][2] = "����";
	_province_city[10][3] = "����";
	_province_city[10][4] = "����";
	_province_city[10][5] = "����";
	_province_city[10][6] = "����";
	_province_city[10][7] = "����";
	_province_city[10][8] = "��";
	_province_city[10][9] = "����";
	_province_city[10][10] = "��ɽ";
	_province_city[10][11] = "̨��";
	_province_city[10][12] = "��ˮ";

	_province_city[11][0] = "18";
	_province_city[11][1] = "����";
	_province_city[11][2] = "�Ϸ�";
	_province_city[11][3] = "����";
	_province_city[11][4] = "�ߺ�";
	_province_city[11][5] = "����";
	_province_city[11][6] = "��";
	_province_city[11][7] = "����";
	_province_city[11][8] = "ͭ��";
	_province_city[11][9] = "����";
	_province_city[11][10] = "��ɽ";
	_province_city[11][11] = "����";
	_province_city[11][12] = "����";
	_province_city[11][13] = "����";
	_province_city[11][14] = "����";
	_province_city[11][15] = "����";
	_province_city[11][16] = "����";
	_province_city[11][17] = "����";
	_province_city[11][18] = "����";

	_province_city[12][0] = "10";
	_province_city[12][1] = "����";
	_province_city[12][2] = "����";
	_province_city[12][3] = "����";
	_province_city[12][4] = "����";
	_province_city[12][5] = "����";
	_province_city[12][6] = "Ȫ��";
	_province_city[12][7] = "����";
	_province_city[12][8] = "��ƽ";
	_province_city[12][9] = "����";
	_province_city[12][10] = "����";

	_province_city[13][0] = "12";
	_province_city[13][1] = "����";
	_province_city[13][2] = "�ϲ�";
	_province_city[13][3] = "�Ž�";
	_province_city[13][4] = "������";
	_province_city[13][5] = "Ƽ��";
	_province_city[13][6] = "����";
	_province_city[13][7] = "ӥ̶";
	_province_city[13][8] = "����";
	_province_city[13][9] = "�˴�";
	_province_city[13][10] = "����";
	_province_city[13][11] = "����";
	_province_city[13][12] = "����";


	_province_city[14][0] = "18";
	_province_city[14][1] = "ɽ��";
	_province_city[14][2] = "����";
	_province_city[14][3] = "�ൺ";
	_province_city[14][4] = "�Ͳ�";
	_province_city[14][5] = "��ׯ";
	_province_city[14][6] = "��Ӫ";
	_province_city[14][7] = "��̨";
	_province_city[14][8] = "Ϋ��";
	_province_city[14][9] = "����";
	_province_city[14][10] = "̩��";
	_province_city[14][11] = "����";
	_province_city[14][12] = "����";
	_province_city[14][13] = "����";
	_province_city[14][14] = "����";
	_province_city[14][15] = "����";
	_province_city[14][16] = "�ĳ�";
	_province_city[14][17] = "����";
	_province_city[14][18] = "����";

	_province_city[15][0] = "19";
	_province_city[15][1] = "����";
	_province_city[15][2] = "֣��";
	_province_city[15][3] = "����";
	_province_city[15][4] = "����";
	_province_city[15][5] = "ƽ��ɽ";
	_province_city[15][6] = "����";
	_province_city[15][7] = "�ױ�";
	_province_city[15][8] = "����";
	_province_city[15][9] = "����";
	_province_city[15][10] = "���";
	_province_city[15][11] = "���";
	_province_city[15][12] = "���";
	_province_city[15][13] = "����Ͽ";
	_province_city[15][14] = "����";
	_province_city[15][15] = "����";
	_province_city[15][16] = "����";
	_province_city[15][17] = "�ܿ�";
	_province_city[15][18] = "פ���";
	_province_city[15][19] = "��Դ";

	_province_city[16][0] = "17";
	_province_city[16][1] = "����";
	_province_city[16][2] = "�人";
	_province_city[16][3] = "��ʯ";
	_province_city[16][4] = "ʮ��";
	_province_city[16][5] = "�˲�";
	_province_city[16][6] = "�差";
	_province_city[16][7] = "����";
	_province_city[16][8] = "����";
	_province_city[16][9] = "Т��";
	_province_city[16][10] = "����";
	_province_city[16][11] = "�Ƹ�";
	_province_city[16][12] = "����";
	_province_city[16][13] = "����";
	_province_city[16][14] = "��ʩ";
	_province_city[16][15] = "����";
	_province_city[16][16] = "Ǳ��";
	_province_city[16][17] = "����";

	_province_city[17][0] = "15";
	_province_city[17][1] = "����";
	_province_city[17][2] = "��ɳ";
	_province_city[17][3] = "����";
	_province_city[17][4] = "��̶";
	_province_city[17][5] = "����";
	_province_city[17][6] = "����";
	_province_city[17][7] = "����";
	_province_city[17][8] = "����";
	_province_city[17][9] = "�żҽ�";
	_province_city[17][10] = "����";
	_province_city[17][11] = "����";
	_province_city[17][12] = "����";
	_province_city[17][13] = "����";
	_province_city[17][14] = "¦��";
	_province_city[17][15] = "����";

	_province_city[18][0] = "22";
	_province_city[18][1] = "�㶫";
	_province_city[18][2] = "����";
	_province_city[18][3] = "�ع�";
	_province_city[18][4] = "����";
	_province_city[18][5] = "�麣";
	_province_city[18][6] = "��ͷ";
	_province_city[18][7] = "��ɽ";
	_province_city[18][8] = "����";
	_province_city[18][9] = "տ��";
	_province_city[18][10] = "ï��";
	_province_city[18][11] = "����";
	_province_city[18][12] = "����";
	_province_city[18][13] = "÷��";
	_province_city[18][14] = "��β";
	_province_city[18][15] = "��Դ";
	_province_city[18][16] = "����";
	_province_city[18][17] = "��Զ";
	_province_city[18][18] = "��ݸ";
	_province_city[18][19] = "��ɽ";
	_province_city[18][20] = "����";
	_province_city[18][21] = "����";
	_province_city[18][22] = "�Ƹ�";

	_province_city[19][0] = "15";
	_province_city[19][1] = "����";
	_province_city[19][2] = "����";
	_province_city[19][3] = "����";
	_province_city[19][4] = "����";
	_province_city[19][5] = "����";
	_province_city[19][6] = "����";
	_province_city[19][7] = "���Ǹ�";
	_province_city[19][8] = "����";
	_province_city[19][9] = "���";
	_province_city[19][10] = "����";
	_province_city[19][11] = "��ɫ";
	_province_city[19][12] = "����";
	_province_city[19][13] = "�ӳ�";
	_province_city[19][14] = "����";
	_province_city[19][15] = "����";


	_province_city[20][0] = "3";
	_province_city[20][1] = "����";
	_province_city[20][2] = "����";
	_province_city[20][3] = "����";


	_province_city[21][0] = "22";
	_province_city[21][1] = "�Ĵ�";
	_province_city[21][2] = "�ɶ�";
	_province_city[21][3] = "�Թ�";
	_province_city[21][4] = "��֦��";
	_province_city[21][5] = "����";
	_province_city[21][6] = "����";
	_province_city[21][7] = "����";
	_province_city[21][8] = "��Ԫ";
	_province_city[21][9] = "����";
	_province_city[21][10] = "�ڽ�";
	_province_city[21][11] = "��ɽ";
	_province_city[21][12] = "�ϳ�";
	_province_city[21][13] = "üɽ";
	_province_city[21][14] = "�˱�";
	_province_city[21][15] = "�㰲";
	_province_city[21][16] = "����";
	_province_city[21][17] = "�Ű�";
	_province_city[21][18] = "����";
	_province_city[21][19] = "����";
	_province_city[21][20] = "����";
	_province_city[21][21] = "����";
	_province_city[21][22] = "��ɽ";

	_province_city[22][0] = "9";
	_province_city[22][1] = "����";
	_province_city[22][2] = "����";
	_province_city[22][3] = "����ˮ";
	_province_city[22][4] = "����";
	_province_city[22][5] = "��˳";
	_province_city[22][6] = "ͭ��";
	_province_city[22][7] = "ǭ����";
	_province_city[22][8] = "ǭ����";
	_province_city[22][9] = "ǭ��";

	_province_city[23][0] = "17";
	_province_city[23][1] = "����";
	_province_city[23][2] = "����";
	_province_city[23][3] = "����";
	_province_city[23][4] = "��Ϫ";
	_province_city[23][5] = "��ɽ";
	_province_city[23][6] = "��ͨ";
	_province_city[23][7] = "����";
	_province_city[23][8] = "�ն�";
	_province_city[23][9] = "�ٲ�";
	_province_city[23][10] = "����";
	_province_city[23][11] = "���";
	_province_city[23][12] = "��ɽ";
	_province_city[23][13] = "��˫����";
	_province_city[23][14] = "����";
	_province_city[23][15] = "�º�";
	_province_city[23][16] = "ŭ��";
	_province_city[23][17] = "����";


	_province_city[24][0] = "8";
	_province_city[24][1] = "����";
	_province_city[24][2] = "����";
	_province_city[24][3] = "����";
	_province_city[24][4] = "ɽ��";
	_province_city[24][5] = "�տ���";
	_province_city[24][6] = "����";
	_province_city[24][7] = "����";
	_province_city[24][8] = "��֥";


	_province_city[25][0] = "11";
	_province_city[25][1] = "����";
	_province_city[25][2] = "����";
	_province_city[25][3] = "ͭ��";
	_province_city[25][4] = "����";
	_province_city[25][5] = "����";
	_province_city[25][6] = "μ��";
	_province_city[25][7] = "�Ӱ�";
	_province_city[25][8] = "����";
	_province_city[25][9] = "����";
	_province_city[25][10] = "����";
	_province_city[25][11] = "����";

	_province_city[26][0] = "15";
	_province_city[26][1] = "����";
	_province_city[26][2] = "����";
	_province_city[26][3] = "������";
	_province_city[26][4] = "���";
	_province_city[26][5] = "����";
	_province_city[26][6] = "��ˮ";
	_province_city[26][7] = "����";
	_province_city[26][8] = "��Ҵ";
	_province_city[26][9] = "ƽ��";
	_province_city[26][10] = "��Ȫ";
	_province_city[26][11] = "����";
	_province_city[26][12] = "����";
	_province_city[26][13] = "¤��";
	_province_city[26][14] = "����";
	_province_city[26][15] = "����";

	_province_city[27][0] = "9";
	_province_city[27][1] = "�ຣ";
	_province_city[27][2] = "����";
	_province_city[27][3] = "����";
	_province_city[27][4] = "����";
	_province_city[27][5] = "����";
	_province_city[27][6] = "����";
	_province_city[27][7] = "����";
	_province_city[27][8] = "����";
	_province_city[27][9] = "����";


	_province_city[28][0] = "6";
	_province_city[28][1] = "����";
	_province_city[28][2] = "����";
	_province_city[28][3] = "ʯ��ɽ";
	_province_city[28][4] = "����";
	_province_city[28][5] = "��ԭ";
	_province_city[28][6] = "����";


	_province_city[29][0] = "19";
	_province_city[29][1] = "�½�";
	_province_city[29][2] = "��³ľ��";
	_province_city[29][3] = "��������";
	_province_city[29][4] = "��³��";
	_province_city[29][5] = "����";
	_province_city[29][6] = "����";
	_province_city[29][7] = "��������";
	_province_city[29][8] = "��������";
	_province_city[29][9] = "������  ";
	_province_city[29][10] = "��������";
	_province_city[29][11] = "��ʲ";
	_province_city[29][12] = "����";
	_province_city[29][13] = "���������";
	_province_city[29][14] = "����";
	_province_city[29][15] = "����̩";
	_province_city[29][16] = "ʯ����";
	_province_city[29][17] = "������";
	_province_city[29][18] = "ͼľ���";
	_province_city[29][19] = "�����";

	_province_city[30][0] = "1";
	_province_city[30][1] = "���";


	_province_city[31][0] = "1";
	_province_city[31][1] = "����";


	_province_city[32][0] = "1";
	_province_city[32][1] = "̨��";

	_province_city[33][0] = "1";
	_province_city[33][1] = "����";


	for (int i = 0; i <= 33; i++)
	{
		int count = atoi(_province_city[i][0]);

		for (int j = 1; j <= count; j++)
		{
			char* name = _province_city[i][j];
			mChannelMap[i * 100 + j] = GetChannelManager()->RegisterChannel(CHAT_MSG_TYPE_REALCITY, name);
		}
	}
}

void CChatManager::HandleJoinCity(int nSocket, stPacketHead* pHead, Base::BitStream* pBitStream)
{
	int playerId = pHead->Id;

	if (pHead->DestZoneId >= MAX_CITY_CHANNEL)
		return;

	U32 channelId = GetChannelManager()->GetChannelId(playerId, CHAT_MSG_TYPE_REALCITY);

	if (0 != channelId)
	{
		GetChannelManager()->RemovePlayer(playerId, channelId);
	}

	GetChannelManager()->AddPlayer(playerId, mChannelMap[pHead->DestZoneId]);
}

void CChatManager::HandleIncomingMsg(int nSocket, stPacketHead* pHead, Base::BitStream* pBitStream)
{
	int nPlayerId = pHead->Id;

	int lineId = SERVER->getServerMgr()->getLineId(nSocket);
	int accountId = SERVER->getPlayerMgr()->getAccountId(nPlayerId);

	if (0 == accountId)
		return;

	int nameRecver = 0;

	// ����ͨ������ֱ�ӷ�����Ϸ��Ϣ
	if (pHead->DestZoneId == 1)
	{
		char playerName[COMMON_STRING_LENGTH];
		pBitStream->readString(playerName, COMMON_STRING_LENGTH);

		int namePlayerId = SERVER->getPlayerMgr()->getPlayerId(playerName);

		if (0 == namePlayerId)
		{
			SendChatResponse(nSocket, accountId, SEND_CHAT_ACK_NOPLAYER);
			return;
		}

		nameRecver = namePlayerId;
	}

	// ���ڷ��͸�����Ϣ	
	std::vector<int> nearPlayerList;

	if (pHead->DestZoneId == 2)
	{
		int playerCount = pBitStream->readInt(16);

		for (int i = 0; i < playerCount; i++)
		{
			int pid = pBitStream->readInt(32);
			nearPlayerList.push_back(pid);
		}
	}

	stChatMessage msg;
	stChatMessage::UnpackMessage1(msg, *pBitStream);
	msg.nSender = nPlayerId;

	if (nameRecver != 0)
		msg.nRecver = nameRecver;

	// �����Ϣ�Ƿ�Ϸ�
	if (IsBaned(msg))
	{
		SendChatResponse(nSocket, accountId, SEND_CHAT_ACK_REFUSED);
		return;
	}

	stChatMessage sendMsg;
	std::string tempStr = Util::Utf8ToMbcs(msg.szMessage);
	//std::string tempStr = msg.szMessage;
	if (!FilterOpt::replaceBanWord(sendMsg.szMessage, 512, tempStr.c_str())){
		dStrcpy(sendMsg.szMessage, 512, tempStr.c_str());
	}

	std::string str = SERVER->getPlayerMgr()->getPlayerName(msg.nSender);

	// �����������ȡ����������֣���ӷ�����Ϣ��ȡ���������
	if (str == "")
		dStrcpy(sendMsg.szSenderName, 32, msg.szSenderName);
	else
		dStrcpy(sendMsg.szSenderName, 32, str.c_str());

	sendMsg.btMessageType = msg.btMessageType;
	sendMsg.nSender = msg.nSender;
	sendMsg.nSenderLineID = msg.nSenderLineID;
	sendMsg.btPrivilegeFlagType = msg.btPrivilegeFlagType;

	// ��鷢��ʱ����
	U64 nPendingTime = GetPlayerChatPendingTime(nPlayerId, msg.btMessageType);
	U64 nLastTime = GetPlayerChatLastTime(nPlayerId, msg.btMessageType);

	if (::GetTickCount64() - nLastTime < nPendingTime)
	{
		SendChatResponse(nSocket, accountId, SEND_CHAT_ACK_TOOFAST, msg.btMessageType == CHAT_MSG_TYPE_P2P ? msg.nRecver : 0, msg.btMessageType);
		return;
	}

	SetPlayerChatLastTime(nPlayerId, msg.btMessageType, ::GetTickCount64());

	// ��鷢����Ϣ����
	if (msg.btMessageType < CHAT_MSG_TYPE_LINE)
	{
		// ��Ҳ���ͨ��������Ϣ�ķ�ʽ���ʹ��ģ��Ϣ
		SendChatResponse(nSocket, accountId, SEND_CHAT_ACK_ERROR);
		return;
	}

	writeLog(msg.nSender, msg.btMessageType, msg.szMessage, msg.nRecver);

	U32 channelId = GetChannelManager()->GetChannelId(nPlayerId,msg.btMessageType);

	if(0 == channelId)
	{
		// �����Ⱥ��Ϣ
		if( msg.btMessageType == CHAT_MSG_TYPE_GROUP)
		{
			channelId = GetChannelManager()->GetChannelId(msg.nRecver);
		}

		if( msg.btMessageType == CHAT_MSG_TYPE_RACE && msg.nRecver != 0 )
		{
			if(msg.nRecver >= 1 && msg.nRecver <= 8)
			{
				channelId = GetChannelManager()->GetChannelId(m_FamilyChannels[msg.nRecver-1]);
			}
		}
	}

	//�����ַ�
	if (isSusWord(sendMsg.szMessage))
	{
		writeBadWordPlayer(sendMsg.nSender, sendMsg.szMessage, ChatBanInfo::SUSP);
	}

	//�����ַ�
	if (isSensWord(sendMsg.szMessage))
	{
		writeBadWordPlayer(sendMsg.nSender, sendMsg.szMessage, ChatBanInfo::SENS);
		SendChatResponse(nSocket, accountId, SEND_CHAT_ACK_ILLEGAL);
		return;
	}


	if ((msg.btMessageType == CHAT_MSG_TYPE_PRIVATE /*|| msg.btMessageType == CHAT_MSG_TYPE_P2P */) && msg.nRecver != msg.nSender) // ���ܸ��Լ�����Ե���Ϣ
	{
		std::string recvName = SERVER->getPlayerMgr()->getPlayerName(msg.nRecver);

		if (recvName == "")
			dStrcpy(sendMsg.szRecverName, 32, msg.szRecverName);
		else
			dStrcpy(sendMsg.szRecverName, 32, recvName.c_str());

		sendMsg.nRecver = msg.nRecver;
		int accountId = SERVER->getPlayerMgr()->getAccountId(msg.nRecver);

		if (accountId)
		{
			SendMessageTo(sendMsg, msg.nRecver);

			if (msg.btMessageType == CHAT_MSG_TYPE_PRIVATE || msg.btMessageType == CHAT_MSG_TYPE_P2P)
				SendMessageTo(sendMsg, msg.nSender);
		}
		else
			SendChatResponse(nSocket, accountId, SEND_CHAT_ACK_NOPLAYER, msg.nRecver);
	}
	else if (msg.btMessageType == CHAT_MSG_TYPE_LINE)
	{
		SendMessageToWorld(sendMsg);
	}
	else if (msg.btMessageType == CHAT_MSG_TYPE_AWARD)
	{
	SendMessageToWorld( sendMsg );
	}
	else
	{
		// ����һ����Ч����Ϣ
		if(0 == channelId)
		{
			if( msg.btMessageType == CHAT_MSG_TYPE_NEARBY )
			{
				SendMessage( sendMsg, nearPlayerList );
			}
			else
			{
				SendChatResponse( nSocket, accountId, SEND_CHAT_ACK_ERROR );
			}

			return;
		}

		SendMessageToChannel(sendMsg,channelId);
	}
}

void CChatManager::HandleGMMessage(stChatMessage& chatMessage)
{
	// �����ã���ǰ����GM��������Ϣ��Ϊ������Ϣ����
	chatMessage.szMessage[0] = ' ';
	chatMessage.btMessageType = CHAT_MSG_TYPE_SYSTEM;
	SendMessageToWorld(chatMessage);
}

// û�к�ǿ��ʱЧ�ԣ�����ͻ���ͬ������
void CChatManager::HandleAddToFamily(int nSocket, stPacketHead* pHead, Base::BitStream* pBitStream)
{
	int playerId = pHead->Id;
	int family = pHead->DestZoneId;

	if (family == 0 || family > MAX_FAMILY_COUNT)
		return;

	GetChannelManager()->AddPlayer(playerId, m_FamilyChannels[family - 1]);
}

// ��ȫ���緢��ȫ����Ϣ
void CChatManager::SendMessageToWorld(stChatMessage& chatMessage)
{
	char Buffer[CHAT_PACKET_SIZE];
	Base::BitStream SendPacket(Buffer, sizeof(Buffer));
	stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket, "SERVER_CHAT_SendMessageAll", 0, SERVICE_GATESERVER);

	stChatMessage::PackMessage(chatMessage, SendPacket);
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof(stPacketHead);

	int gates[100];
	int count = 100;
	CChatServer::getInstance()->getServerMgr()->getGates(gates, count);

	for (int i = 0; i < count; ++i)
	{
		CChatServer::getInstance()->getServerSocket()->Send(gates[i], SendPacket);
	}
}

//��һ����Ⱥ�ڷ�����Ϣ
void CChatManager::SendMessage(stChatMessage& chatMessage, const std::vector<int>& playerList)
{
	if (0 == playerList.size())
		return;

	std::vector<SendInfo> sends;
	sends.reserve(playerList.size());

	U32 gates[MAX_LINE_COUNT] = { 0 };

	for (size_t i = 0; i < playerList.size(); ++i)
	{
		SendInfo sendInfo = SERVER->getPlayerMgr()->GetSendInfo(playerList[i]);

		if (0 == sendInfo.accountId || sendInfo.lineId >= MAX_LINE_COUNT)
			continue;

		gates[sendInfo.lineId]++;
		sends.push_back(sendInfo);
	}

	char Buffer[MAX_PACKET_SIZE];

	for (U32 i = 1; i < MAX_LINE_COUNT; ++i)
	{
		if (0 == gates[i] || gates[i] >= 60000)
			continue;

		int socketId = 0;

		Base::BitStream SendPacket(Buffer, sizeof(Buffer));
		stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket, "SERVER_CHAT_SendMessage", 0, SERVICE_GATESERVER);

		SendPacket.writeInt(gates[i], 16);

		for (size_t iSend = 0; iSend < sends.size(); ++iSend)
		{
			if (sends[iSend].lineId == i)
			{
				SendPacket.writeInt(sends[iSend].accountId, 32);

				if (0 == socketId)
				{
					socketId = sends[iSend].socketId;
				}
			}
		}

		stChatMessage::PackMessage(chatMessage, SendPacket);
		pSendHead->PacketSize = SendPacket.getPosition() - sizeof(stPacketHead);

		SERVER->getServerSocket()->Send(socketId, SendPacket);
	}
}

void CChatManager::SendMessageTo(stChatMessage& chatMessage, int nPlayerId)
{
	SendInfo sendInfo = SERVER->getPlayerMgr()->GetSendInfo(nPlayerId);

	if (0 == sendInfo.accountId || 0 == sendInfo.socketId)
		return;

	char Buffer[CHAT_PACKET_SIZE];
	Base::BitStream SendPacket(Buffer, sizeof(Buffer));
	stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket, "SERVER_CHAT_SendMessage", sendInfo.accountId, SERVICE_CLIENT);

	stChatMessage::PackMessage(chatMessage, SendPacket);
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof(stPacketHead);

	SERVER->getServerSocket()->Send(sendInfo.socketId, SendPacket);
}

void CChatManager::SendMessageToLine(stChatMessage& chatMessage, int nLineId)
{
	char Buffer[CHAT_PACKET_SIZE];
	Base::BitStream SendPacket(Buffer, sizeof(Buffer));
	stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket, "SERVER_CHAT_SendMessageAll", 0, SERVICE_GATESERVER);

	stChatMessage::PackMessage(chatMessage, SendPacket);
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof(stPacketHead);

	std::vector<int> gateList;
	SERVER->getServerMgr()->getAllGates(gateList, nLineId);

	for (size_t i = 0; i < gateList.size(); ++i)
	{
		CChatServer::getInstance()->getServerSocket()->Send(gateList[i], SendPacket);
	}
}

void CChatManager::SendMessageToChannel(stChatMessage& chatMessage, int nChannelId)
{
	if (0 == nChannelId)
		return;

	std::vector<int> playerList;
	GetChannelManager()->GetPlayerList(nChannelId, playerList);

	SendMessage(chatMessage, playerList);
}

// ������֤��Ϣ�Ϸ���
bool CChatManager::IsBaned(stChatMessage& chatMessage)
{

	for (size_t i = 0; i < m_banList.size(); i++)
	{
		switch (m_banList[i].type)
		{
		case ChatBanInfo::TARGET_TYPE_ACC:
		{
			if (0 == strcmp(m_banList[i].name, SERVER->getPlayerMgr()->getAccountName(chatMessage.nSender).c_str()))
			{
				int curTime = (int)time(0);
				return m_banList[i].IsChatBaned(curTime);
			}
		}
		break;
		case ChatBanInfo::TARGET_TYPE_ACTOR:
		{
			if (0 == strcmp(m_banList[i].name, SERVER->getPlayerMgr()->getPlayerName(chatMessage.nSender).c_str()))
			{
				int curTime = (int)time(0);
				return m_banList[i].IsChatBaned(curTime);
			}
		}
		break;
		case ChatBanInfo::TARGET_TYPE_IP:
		{
			if (0 == strcmp(m_banList[i].name, SERVER->getPlayerMgr()->getPlayerIp(chatMessage.nSender).c_str()))
			{
				int curTime = (int)time(0);
				return m_banList[i].IsChatBaned(curTime);
			}
		}
		break;
		}
	}

	return false;
}

U64 CChatManager::GetPlayerChatPendingTime(int nPlayerId, char cMessageType)
{
	__int64 v;
	v = nPlayerId;
	v = (v << 8) | cMessageType;

	OLD_DO_LOCK(m_cs);

	stPlayerChatRecord& rec = m_playerChatMap[v];

	if (rec.nPendingTime == 0)
	{
		switch (cMessageType)
		{
		case CHAT_MSG_TYPE_PRIVATE:
			rec.nPendingTime = CHAT_PENDING_TIME_PRIVATE;
			break;
		case CHAT_MSG_TYPE_WORLDPLUS:
			rec.nPendingTime = CHAT_PENDING_TIME_WORLDPLUS;
			break;
		case CHAT_MSG_TYPE_REALCITY:
			rec.nPendingTime = CHAT_PENDING_TIME_REALCITY;
			break;
		case CHAT_MSG_TYPE_LINE:
			rec.nPendingTime = CHAT_PENDING_TIME_LINE;
			break;
		default:
			rec.nPendingTime = CHAT_PENDING_TIME_NORAML;
			break;
		}
	}

	return rec.nPendingTime;
}

void CChatManager::SetPlayerChatLastTime(int nPlayerId, char cMessageType, U64 nTime)
{
	__int64 v;
	v = nPlayerId;
	v = (v << 8) | cMessageType;

	OLD_DO_LOCK(m_cs);

	m_playerChatMap[v].nLastTime = nTime;
}

U64 CChatManager::GetPlayerChatLastTime(int nPlayerId, char cMessageType)
{
	__int64 v;
	v = nPlayerId;
	v = (v << 8) | cMessageType;

	OLD_DO_LOCK(m_cs);

	CHATREC_MAP::iterator iter = m_playerChatMap.find(v);

	if (iter == m_playerChatMap.end())
		return 0;

	return iter->second.nLastTime;
}

void CChatManager::Update(void)
{
	OLD_DO_LOCK(m_cs);

	__time64_t curTime = time(0);

	for (std::list<AutoMsg>::iterator iter = m_autoMsgs.begin(); iter != m_autoMsgs.end();)
	{
		AutoMsg& autoMsg = *iter;

		if (curTime < autoMsg.startTime)
		{
			++iter;
			continue;
		}

		bool isEnded = false;

		if (autoMsg.freq <= 0 && autoMsg.times <= 0)
		{
			isEnded = true;
		}

		//�ж�ʱ���Ƿ����
		if (autoMsg.startTime != autoMsg.endTime)
		{
			if (curTime >= autoMsg.endTime)
			{
				isEnded = true;
			}
		}
		else
		{
			if (autoMsg.freq <= 0 || autoMsg.times <= 0)
				isEnded = true;
		}

		//�жϷ��ʹ����Ƿ����
		if (0 == autoMsg.times)
		{
			isEnded = true;
		}

		if (isEnded)
		{
			iter = m_autoMsgs.erase(iter);
			continue;
		}

		//�ƽ�ʱ��
		bool isSend = false;

		if (curTime - autoMsg.lastSendTime >= autoMsg.freq)
		{
			isSend = true;
		}

		if (isSend)
		{
			if (autoMsg.times > 0)
				autoMsg.times--;

			autoMsg.lastSendTime = curTime;

			//������Ϣ
			if (0 == autoMsg.playerId)
			{
				SendMessageToWorld(autoMsg.msg);
			}
			else
			{
				switch (autoMsg.channel)
				{
				case CHAT_TARGET_PERSON:
				{
					autoMsg.msg.btMessageType = CHAT_MSG_TYPE_PRIVATE;
					SendMessageTo(autoMsg.msg, autoMsg.playerId);
				}
				break;
				// ���ڲ����Զ�������Ϣ��TEAM
				//case CHAT_TARGET_TEAM:
				//	{
				//		//��ȡ������ڵ�team

				//		TeamHandle pTeam = SERVER->GetTeamManager()->GetTeamByPlayer(autoMsg.playerId);

				//		if (0 == pTeam)
				//			return;

				//		autoMsg.msg.btMessageType = CHAT_MSG_TYPE_TEAM;
				//		SendMessageToChannel(autoMsg.msg,pTeam->GetChatChannelId());
				//	}   
				//	break;
				}
			}
		}

		++iter;
	}
}

void CChatManager::AddAutoMsg(AutoMsg& msg)
{
	OLD_DO_LOCK(m_cs);
	m_autoMsgs.push_back(msg);
}

void CChatManager::GetAutoMsg(int sid)
{
	DO_LOCK(Lock::Chat);
	for (std::list<AutoMsg>::iterator iter = m_autoMsgs.begin(); iter != m_autoMsgs.end(); ++iter)
	{
		MAKE_PACKET(sendPacket, "WORLD_CHAT_AutoMsgResponse", sid, SERVICE_CHATSERVER);
		iter->WriteData(&sendPacket);
		SEND_PACKET_NSOCK(sendPacket, SERVER->getWorldSocket());
	}
}

void CChatManager::DelAutoMsg(AutoMsg& msg)
{
	DO_LOCK(Lock::Chat);
	for (std::list<AutoMsg>::iterator iter = m_autoMsgs.begin(); iter != m_autoMsgs.end(); ++iter)
	{
		if (strcmp(iter->msg.szMessage, msg.msg.szMessage) == 0 && iter->channel == msg.channel && iter->endTime == msg.endTime)
		{
			m_autoMsgs.erase(iter);
			break;
		}
	}
}


void CChatManager::CancelAutoMsg()
{
	DO_LOCK(Lock::Chat);
	m_autoMsgs.clear();
}

void CChatManager::writeBadWordPlayer(int playerId, char* pWord, int type)
{
	OLD_DO_LOCK(m_cs);

	char inBuf[256];
	GetCurrentDirectoryA(256, inBuf);
	char path[255];
	sprintf_s(path, "%s\\BadWordPlayer.ini", inBuf);

	char cplayerId[10];
	sprintf_s(cplayerId, "%d", playerId);

	if (ChatBanInfo::SENS == type)
	{
		WritePrivateProfileStringA(cplayerId, pWord, "�����ַ�", path);
	}
	else if (ChatBanInfo::SUSP == type)
	{
		WritePrivateProfileStringA(cplayerId, pWord, "�����ַ�", path);
	}

}

void CChatManager::readFile(const char* path, int type)
{

	FILE* pf;
	char buff[MAX_BADWORDS_LENGTH];

	fopen_s(&pf, path, "rb");

	if (pf == NULL)
	{
		fopen_s(&pf, path, "wb");

		if (pf == NULL)
		{
			g_Log.WriteError("�޷�����%s", path);
			assert(0);
			return;
		}
	}
	else
	{
		g_Log.WriteLog("��%s�ɹ�!", path);

		if (ChatBanInfo::SENS == type)
		{
			ZeroMemory(buff, sizeof(buff));
			while (!feof(pf))
			{
				fgets(buff, sizeof(buff), pf);
				for (int i = 0; i < MAX_BADWORDS_LENGTH; ++i)
				{
					if (13 == buff[i])
						buff[i] = 0;
				}

				std::wstring str;
				A2W_s(buff, str);
				m_sensWords.push_back(str);
			}
		}
		else if (ChatBanInfo::SUSP == type)
		{
			ZeroMemory(buff, sizeof(buff));
			while (!feof(pf))
			{
				fgets(buff, sizeof(buff), pf);
				for (int i = 0; i < MAX_BADWORDS_LENGTH; ++i)
				{
					if (13 == buff[i])
						buff[i] = 0;
				}


				std::wstring str;
				A2W_s(buff, str);
				m_susWords.push_back(str);
			}
		}

	}

	fclose(pf);

}

bool CChatManager::isSusWord(const char* pName)
{
	std::wstring wName;
	A2W_s(pName, wName);

	VEC_WSTR_CONITER iterBegin = m_susWords.begin();
	VEC_WSTR_CONITER iterEnd = m_susWords.end();

	for (; iterBegin != iterEnd; ++iterBegin)
	{
		if (iterBegin->size() > 0 && wcsstr(wName.c_str(), iterBegin->c_str()) != NULL)
			return true;
	}

	return false;
}


bool CChatManager::isSensWord(const char* pName)
{
	std::wstring wName;
	A2W_s(pName, wName);

	VEC_WSTR_CONITER iterBegin = m_sensWords.begin();
	VEC_WSTR_CONITER iterEnd = m_sensWords.end();

	for (; iterBegin != iterEnd; ++iterBegin)
	{
		if (iterBegin->size() > 0 && wcsstr(wName.c_str(), iterBegin->c_str()) != NULL)
			return true;
	}

	return false;
}


ServerEventFunction(CChatServer, HandleAddAutoMsg, "WORLD_CHAT_AddAutoMsg")
{
	AutoMsg msg;
	msg.ReadData(pPack);

	SERVER->getChatMgr()->AddAutoMsg(msg);
	return true;
}

ServerEventFunction(CChatServer, HandleGetAutoMsg, "WORLD_CHAT_GetAutoMsg")
{
	int sid = phead->Id;
	SERVER->getChatMgr()->GetAutoMsg(sid);
	return true;
}

ServerEventFunction(CChatServer, HandleDelAutoMsg, "WORLD_CHAT_DelAutoMsg")
{
	AutoMsg msg;
	msg.ReadData(pPack);

	SERVER->getChatMgr()->DelAutoMsg(msg);
	return true;
}

ServerEventFunction(CChatServer, HandleCancelAutoMsg, "WORLD_CHAT_CancelAutoMsg")
{

	SERVER->getChatMgr()->CancelAutoMsg();
	return true;
}

/*ServerEventFunction( CChatServer, HandleSendMsgToLine, WORLD_CHAT_SendMessageToLine )
{
int lineId = phead->Id;

stChatMessage msg;
stChatMessage::UnpackMessage( msg, *pPack );

SERVER->getChatMgr()->SendMessageToLine(msg, lineId);
return true;
}*/

ServerEventFunction(CChatServer, HandleSendMsgToWorld, "WORLD_CHAT_SendMessageToWorld")
{
	stChatMessage msg;
	stChatMessage::UnpackMessage(msg, *pPack);

	SERVER->getChatMgr()->SendMessageToWorld(msg);
	return true;
}

ServerEventFunction(CChatServer, HandleSendMsgToPlayer, "WORLD_CHAT_SendMessageToPlayer")
{
	stChatMessage msg;
	stChatMessage::UnpackMessage(msg, *pPack);

	SERVER->getChatMgr()->SendMessageTo(msg, phead->Id);
	return true;
}

/*ServerEventFunction( CChatServer, HandleSendMsgToChannel, WORLD_CHAT_SendMessageToChannel )
{
int channelId = phead->Id;

stChatMessage msg;
stChatMessage::UnpackMessage( msg, *pPack );

SERVER->getChatMgr()->SendMessageToChannel( msg, channelId );

return true;
}

ServerEventFunction( CChatServer, HandleJoinCityRequest, CLIENT_CHAT_JoinCityRequest )
{
SERVER->getChatMgr()->HandleJoinCity( socketId, phead, pPack );
return true;
}*/

ServerEventFunction(CChatServer, HandleSendMsg, "CLIENT_CHAT_SendMessageRequest")
{
	SERVER->getChatMgr()->HandleIncomingMsg(socketId, phead, pPack);
	return true;
}

ServerEventFunction(CChatServer, HandleRegisterChannel, "WORLD_CHAT_RegisterChannelRequest")
{
	int type = phead->Id;
	int channelId = SERVER->getChatMgr()->GetChannelManager()->RegisterChannel(type, "");

	if (0 == channelId)
		return true;

	MAKE_PACKET(sendPacket, "WORLD_CHAT_RegisterChannelResponse", phead->SrcZoneId, SERVICE_WORLDSERVER, channelId, phead->DestZoneId, phead->Id);
	SEND_PACKET_NSOCK(sendPacket, SERVER->getWorldSocket());
	return true;
}

ServerEventFunction(CChatServer, HandleUnregisterChannel, "WORLD_CHAT_UnregisterChannelRequest")
{
	SERVER->getChatMgr()->GetChannelManager()->UnregisterChannel(phead->Id);
	return true;
}

ServerEventFunction(CChatServer, HandleAddPlayerToChannel, "WORLD_CHAT_ChannelAddPlayer")
{
	int playerId = phead->Id;
	int channelId = phead->DestZoneId;

	SERVER->getChatMgr()->GetChannelManager()->AddPlayer(playerId, channelId);
	return true;
}

ServerEventFunction(CChatServer, HandleRemovePlayerFromChannel, "WORLD_CHAT_ChannelRemovePlayer")
{
	int playerId = phead->Id;
	int channelId = phead->DestZoneId;

	SERVER->getChatMgr()->GetChannelManager()->RemovePlayer(playerId, channelId);
	return true;
}

/*
ServerEventFunction( CChatServer, HandleZoneAddFamily, ZONE_CHAT_AddToFamily )
{
SERVER->getChatMgr()->HandleAddToFamily(socketId,phead,pPack);
return true;
}

ServerEventFunction( CChatServer, HandleZoneSendMsg, ZONE_CHAT_SendMessageRequest )
{
int lineId = pPack->readInt(Base::Bit32);
SERVER->getChatMgr()->HandleZoneSendMessage( lineId, phead, pPack );
return true;
}


ServerEventFunction( CChatServer, HandleMdfChatBanList, WORLD_CHAT_ChatBanList )
{
int size = pPack->readInt(Base::Bit32);
ChatBanInfo chatBanInfo;
std::vector<ChatBanInfo>& vBanList = SERVER->getChatMgr()->m_banList;

vBanList.clear();
for (int i=0; i<size; i++)
{
chatBanInfo.unpack(pPack);
vBanList.push_back(chatBanInfo);
}

return true;
}*/