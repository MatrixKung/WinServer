#ifndef __PLAYSERSTRUCTEX_H__
#define __PLAYSERSTRUCTEX_H__

class BitStream;

#define PACK_DATA_DEF	packData
#define UNPACK_DATA_DEF	unpackData
#include <WinSock2.h>
#include "common/PacketType.h"
#include "Common/PlayerStruct.h"

struct PlayerStatusType
{
	enum Type
	{
		Offline,		// ����
		Idle,			// ����
		Copymap,		// ����
		Team,			// ���
		Fight,			// ս��
		Business,		// ��̯
		TempOffline,	// ����
		Count
	};
};

struct stPlayerTransferNormal
{
	char szPlayerName[MAX_NAME_LENGTH];
};

//ð���¼�������Ϣ
struct FakePlayerInfo
{
    U32  playerId;
    char playerName[MAX_NAME_LENGTH];
    U8   sex;
    U8   level;
    U8   family;
    U8	 firstClass;
    U8   FaceImage;
    U8   line;
    U32  body;
	U32  face;
	U32  hair;

    U32  startLoginTime;
    U32  nextLoginTime;
    U32  selZoneId;
	bool isSel;

	float posx;
	float posy;
	float posz;

    FakePlayerInfo(void)
    {
        memset(this,0,sizeof(FakePlayerInfo));
    }
};

const unsigned int MAX_PLAYER_STRUCT_SIZE	= (sizeof(SimplePlayerData) + 400);
#endif//__PLAYSERSTRUCTEX_H__