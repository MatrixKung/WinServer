#ifndef TOP_TYPE_H
#define TOP_TYPE_H

#include <string>
#include "time.h"
#include "PlayerStruct.h"

//������಻����һ��
#define RANK(a,b) ((10-(a))*100000+(b))
#define GETFIRST(a) (10-(a)/100000)
#define GETSECOND(a) (a)%100000
#define SPC_TYPE	999999999999999
//���а�����,��Ų��ɸ�
enum TopRankType
{
	eTopType_Start				= 0,
	//ͳ��
	eTopType_Level				= eTopType_Start,					// �ȼ�
	eTopType_Rich				= 1,								// ��Ǯ
	eTopType_Score				= 2,								// ����

	//����
	eTopType_End
};

enum TopRankBlobType
{
	ePetBlob = 0,
	eItemBlob
};

//��������
static const char* strTopRankType[eTopType_End] =
{
	"Level",					// 0-�ȼ�
	"Rich",						// 1-��Ǯ
	"Score",					// 2-����
};

#define TR_SORTTIME (60)        //��
#define TR_SAVETIME (60 * 60)
#define TR_TOPCOUNT (99)
#define TR_ARENACOUNT (200)     //����������ĸ���
#define TR_CLEARTIME (21*24*60*60)  //21�첻���������а�

//���а������
struct TopRankConfig
{     
    TopRankConfig(void)
    {
		sortInterval	= TR_SORTTIME;
		saveInterval	= TR_SAVETIME;
		playerCount		= TR_TOPCOUNT;
		lastSortTime	= (U32)time(0);
		lastSaveTime	= (U32)time(0);
    }
    
	//����ˢ��ʱ��
	U32 sortInterval;

	//���ݿⱣ��ʱ��
	U32 saveInterval;

	//�Ŷӵ������
	U32 playerCount;   
    
    //���ݵ�����
	time_t lastSortTime;
	time_t lastSaveTime;
};

extern TopRankConfig gs_topRankConfigs[eTopType_End];

//struct PlayerEquip
//{
//	stItemInfo	EquipInfo[12];
//	U8			Sex;
//	U8			Level;
//	U8			Family;
//	char		PlayerName[32];
//
//	PlayerEquip()
//	{
//		memset(this,0,sizeof(PlayerEquip));
//	}
//
//	void packData(Base::BitStream& packet,stPlayerStruct pPlayer)
//	{
//		for (int i = 1; i < 13; ++i)   //��ͨװ��1-12��װ��
//		{
//			if(pPlayer.MainData.EquipInfo[i].ItemID != 0)
//			{
//				packet.writeFlag(true);
//				pPlayer.MainData.EquipInfo[i].WriteData(&packet);
//			}
//			else
//				packet.writeFlag(false);
//		}
//		packet.writeInt(pPlayer.BaseData.Sex,8);
//		packet.writeInt(pPlayer.BaseData.Level,8);
//		packet.writeInt(pPlayer.BaseData.Family,8);
//		packet.writeString(pPlayer.BaseData.PlayerName,32);
//	}
//
//	void unpackData(Base::BitStream& packet)
//	{
//		for (int i = 1; i < 13; ++i)   //��ͨװ��1-12��װ��
//		{
//			if (packet.readFlag())
//				EquipInfo[i].ReadData(&packet);		
//		}
//		Sex			= packet.readInt(8);
//		Level		= packet.readInt(8);
//		Family		= packet.readInt(8);
//		packet.readString(PlayerName,32);
//
//	}
//};


//���а��������
struct TopRankItem
{
	U64 uid;				//Ψһ��ʶ,��������һ���Ʒ�����ID
	int playerId;			//���Id
	int idxValue;			//��������
	int lastRank;			//�ϴε�����
	int orgLV;				//����ȼ�����ҵȼ�(������)
	int nValue;				//����ֵ
	int honour;				//���ְҵ(���������)�����޵ȼ�(����������)����������(����װ��)
	U64 exp;				//����  
	time_t lastTime;        //�ϴθ��µ�ʱ��
	std::string strName;	//����
	std::string itemName;	//���������������

	stPetInfo		petInfo;
	stItemInfo		ItemInfo; 
    
    TopRankItem(void)
    {
        memset(this,0,sizeof(TopRankItem));
        lastRank = -1;
    }

	bool operator < (const TopRankItem& other) const
	{	
		if(nValue == other.nValue)
		{
// 			if (exp == other.exp)
// 			{
			
				if(SPC_TYPE == exp && SPC_TYPE == other.exp) // ΪValue��ͬȴ��Ҫ�����ʱ��-��ʼ��ʹ�õĳ�ʼֵexp=999999999999999�������µ��������
					return uid < other.uid;
				else if (-1 == lastRank)  //-1Ϊ����δ�ϰ����ݣ�-2Ϊ����δ�ϰ�����,
					return true;
				else if(-2 == lastRank)  
					return false;
				else
					return true;
// 			}
// 			return exp < other.exp;
		}
			

		return nValue < other.nValue;
	}
    
    void packData(Base::BitStream& packet)
    {
       packet.writeInt(playerId, 32);
		packet.writeInt(nValue, 32);
		packet.writeInt(honour, 32);
		packet.writeInt(orgLV,32);
		packet.writeSignedInt(idxValue,16);
		packet.writeSignedInt(lastRank, 16);
		packet.writeInt((S32)lastTime,32);
		packet.writeString(strName.c_str());
		packet.writeString(itemName.c_str());
		char buf[64] = "";
		_i64toa_s(uid,buf,64,10);
		packet.writeString(buf);
    }
    
	void unpackData(Base::BitStream& packet)
	{
		playerId = packet.readInt(32);
		nValue   = packet.readInt(32);
		honour   = packet.readInt(32);
		orgLV	 = packet.readInt(32);
		idxValue = packet.readSignedInt(16);
		lastRank = packet.readSignedInt(16);
		lastTime = packet.readInt(32);

		char nameBuf[256] = {0};
		packet.readString(nameBuf,sizeof(nameBuf));
		strName  = nameBuf;
		packet.readString(nameBuf,sizeof(nameBuf));
		itemName = nameBuf;
		packet.readString(nameBuf,sizeof(nameBuf));
		uid = _atoi64(nameBuf);

	}
};

#endif