#ifndef PLAYER_STRUCT_H
#define PLAYER_STRUCT_H

#include <memory>
#include "base/bitStream.h"
#include "quicklz/quicklz.h"
#include "sqlGenTypedef.h"
#include "Common/MultiMountBase.h"
#include <sstream>
#include "base/Log.h"
#if !defined(TCP_SERVER)
class BitStream;
#endif

const unsigned int MAX_UUID_LENGTH			= 37;					// ��ƷUID����
const unsigned int MAX_NAME_LENGTH			= 32;
const unsigned int Default_Player_Top_Level = 80;					// ���������ȼ�

enum enFamily
{
	Family_None        = 0,
	Family_Ren,//��
	Family_Sheng,//ʥ
	Family_Fo,
	Family_Xian,//��
	Family_Jing,//��
	Family_Gui,//��
	Family_Guai,//��
	Family_Yao,//��
	Family_Mo,//ħ
	Family_Shou,//��
	Family_Long,//��
	Family_Max,
};

static char* g_strFamily[Family_Max] = 
{
	"��",
	"��",
	"��",
	"��",
	"��",
	"��",
	"��",
	"��",
	"ħ",
	"��",
	"��",
};

static char * g_strFirstClass[] = 
{
	"����",
	"��ʦ",
	"����",
	"���",
	"����",
	"��˾",
	"����",
	"����",
};

// ����ְҵ
enum enClassesMaster
{
	_1st,
	_2nd,
	_3rd,
	_4th,
	_5th,
	MaxClassesMasters,
};

// Ч������(APF)
enum enActivePropertyFlag
{
	EAPF_ATTACH			= 1 << 1,	// װ����־
	EAPF_EQUIPSTRENTH   = 1 << 2,	// װ��ǿ������
};

// ��Ʒ���ݽṹ
struct stItemInfo
{
	char	UID[MAX_UUID_LENGTH];			// ��Ʒ����ΨһID
	U32		ItemID;							// ��Ʒģ��ID
	U32		PlayerID;						// ���ID
	U16		Quantity;						// ��Ʒ����(����װ����ֻ��Ϊ1)
	U8		Level;							// װ���ȼ�
	U32		Exp;							// װ������
	stItemInfo() { InitData();}
	void InitData()
	{
		memset(this, 0, sizeof(stItemInfo));
	}

	void WriteData(Base::BitStream* stream);
	void ReadData(Base::BitStream* stream);
};

struct stPetInfo
{				
	U32			petDataId;						//ģ��ID

	stPetInfo() { InitData();}
	void InitData() { memset(this, 0, sizeof(stPetInfo));}

	void WriteData(Base::BitStream* stream);
	void ReadData(Base::BitStream* stream);
};

//�ʺű����status��ʶ,������Ϊ����ʺ���������ҹ���Ĺ��ܱ�ʶ
enum EAccountStatus
{
	EAS_OLDPLAYERREWARD = BIT(0),		//����ҽ���
	EAS_PRECREATEREWARD = BIT(1),		//Ԥ������ҽ���
	EAS_OLDPLAYER_VIPACCOUNT = BIT(2),  //�����&vip�ʺ�
};

//��չ���������Ϣ,��Щ�ǲ���Ҫ���µ�DB��
struct SimplePlayerData
{
	U32  AccountId;
	U32  PlayerId;
	char PlayerName[32];
	U8  Sex;
	U8  Level;
	S32 Family;
	S32	FirstClass;
	U16 ZoneId;
	U32 Gold;
	U32 DrawGold;
	U32 VipLv;
	U32 LastLogoutTime;
	U32 LastLoginTime;

	SimplePlayerData(void):AccountId(0), PlayerId(0), Sex(0), Level(0), Family(0), FirstClass(0), ZoneId(0), Gold(0),\
		VipLv(0), LastLoginTime(0), LastLogoutTime(0), DrawGold(0)
	{
		memset(PlayerName, 0, sizeof(PlayerName));
	}

	U32 GetId(void) const {return PlayerId;}

	void WriteData(Base::BitStream* stream)
	{
		//stream->writeInt(AccountId, Base::Bit32);
		stream->writeInt(PlayerId, Base::Bit32);
		stream->writeString(PlayerName, 32);
		/*stream->writeInt(Sex, Base::Bit8);
		stream->writeInt(Level, Base::Bit8);
		stream->writeInt(ZoneId, Base::Bit16);*/
		stream->writeInt(Gold, Base::Bit32);
		/*stream->writeInt(Family, Base::Bit8);
		stream->writeInt(FirstClass, Base::Bit8);
		stream->writeInt(VipLv, Base::Bit32);
		stream->writeInt(LastLogoutTime, Base::Bit32);
		stream->writeInt(LastLoginTime, Base::Bit32);
		stream->writeInt(LastLoginTime, Base::Bit32);*/
		//stream->writeBits(sizeof(SimplePlayerData)<<3,this);
	}

	void ReadData(Base::BitStream* stream)
	{
		//AccountId = stream->readInt(Base::Bit32);
		PlayerId = stream->readInt(Base::Bit32);
		stream->readString(PlayerName, 32);
		/*Sex = stream->readInt(Base::Bit8);
		Level = stream->readInt(Base::Bit8);
		ZoneId = stream->readInt(Base::Bit16);*/
		Gold = stream->readInt(Base::Bit32);
		/*Family = stream->readInt(Base::Bit8);
		FirstClass = stream->readInt(Base::Bit8);
		VipLv = stream->readInt(Base::Bit32);
		LastLogoutTime = stream->readInt(Base::Bit32);
		LastLoginTime = stream->readInt(Base::Bit32);
		LastLoginTime = stream->readInt(Base::Bit32);*/
		//stream->readBits(sizeof(SimplePlayerData)<<3,this);
	}
};

typedef std::tr1::shared_ptr<stItemInfo> ItemInfoRef;
#endif


