#ifndef CHAT_BASE_H
#define CHAT_BASE_H

#include "Base/Log.h"

const int CHAT_MESSAGE_MAX_LENGTH = 128;

using namespace std;

using namespace stdext;

enum eChatMessageType
{
	CHAT_MSG_TYPE_CHANNEL,		// Ƶ��
	CHAT_MSG_TYPE_PRIVATE,		// ˽��
	CHAT_MSG_TYPE_GM,			// GM��Ϣ
	CHAT_MSG_TYPE_UNKNOWN,		// Ԥ��
};

struct stChatMessage
{
	BYTE btMessageType;
	UINT btChannelID;
	char szMessage[CHAT_MESSAGE_MAX_LENGTH];
};

enum eChatChannelType
{
	CHAT_CHANNEL_TYPE_SYSTEM,		// ϵͳ
	CHAT_CHANNEL_TYPE_AREA,			// ����
	CHAT_CHANNEL_TYPE_GROUP,		// ��Ⱥ
};

#endif