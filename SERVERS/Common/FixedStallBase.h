#ifndef __FIXEDSTALLBASE_H__
#define __FIXEDSTALLBASE_H__

#include "Base/BitStream.h"
#include "Common/PacketType.h"
#include "common/PlayerStruct.h"
#include "CommLib/CommLib.h"
  
#define BUYSYSSTALLPRISE(a)    600000 + 24 * a                //����ϵͳ���̼۸�[ע����ʽ��300000 * (1 + m_TotalFlourish / 25000) * 2]

enum enStallStatus
{
	SS_MODIFY	= 0,					//�����޸�״̬
	SS_OPEN		= 1,					//���̿���״̬
};

enum enStallShopType
{
	STALLTYPE_NONE		 = 0,			//��
	STALLTYPE_CHANDLERY	 = 1,			//�ӻ���
	STALLTYPE_PET		 = 2,			//���޵�
	STALLTYPE_GEM		 = 3,			//��ʯ��
	STALLTYPE_EQUIP		 = 4,			//װ����
	STALLTYPE_MATERAIL	 = 5,			//���ϵ�
};

enum enStallConstant
{
	BASEFLOURISH		= 10,			//���̻������ٶ�
	INITSTALLS			= 50,			//��ʼ������
	INITTOTALFLOURISH	= 500,			//��ʼ�ܷ��ٶȣ���ʼ��ҵָ����
	MAXTOTALFLOURISH	= 50000,		//����ܷ��ٶȣ������ҵָ����
	TRADETAX			= 11,			//��ʼ����˰Ϊ0.11
	DAYTAX				= 10200,		//��ʼÿ��˰��
	ATTORNPRICE			= 600000,		//��ʼת�ü۸�
	BUYSTALLIMPREST		= 1050000,		//�������̶���Ԥ����
	INITSELLITEMS		= 24,			//��ʼ������Ʒ������
	INITSELLPETS		= 6,			//��ʼ�������޸���
	SHOUTINTERVAL0		= 60,			//�������ͼ��(1����)
	SHOUTINTERVAL1		= 300,			//�������ͼ��(5����)
	SHOUTINTERVAL2		= 600,			//�������ͼ��(10����)
	BUYSTALLITEM		= 105090012,	//�������̶̹����������ؼ�
	SHOUTITEM0			= 105102503,	//��������ID
	SHOUTITEM1			= 105102506,	//��������ID
	MAXLEAVEWORDS		= 50,			//���������ౣ��50��
	MAXTRADERECORDS		= 50,			//��ҽ�����Ϣ��ౣ��200��
	MAX_MONEY_NUM		= 1000000000,	//��Ǯ�������
};

enum enStallOp
{
	STO_NONE,							//ȫ������
	STO_NAME,							//�޸�̯λ����
	STO_TYPE,							//�޸�̯λ����
	STO_LEAVEWORDS,						//�޸�����
	STO_SHOUT,							//�޸Ĺ�溰��
	STO_FLOWMONEY,						//�޸������ʽ�
	STO_ATTORNPRICE,					//�޸�ת�ü۸�
	STO_BEATTORN,						//�޸��Ƿ�ת��
};

#pragma pack(push,1)

struct stFixedStall
{	
	U16		StallID;					//����ID
	char	StallName[MAX_NAME_LENGTH];	//��������
	U32		StallerID;					//��������ID
	char	StallerName[MAX_NAME_LENGTH];//������������
	U32		PreStallerID;				//��λ����ID
	U8		StallType;					//��������
	U8		StallStatus;				//����״̬
	S32		TradeTax;					//���̽���˰
	S32		StallTax;					//���̶̹�˰
	U32		PayTaxTime;					//�ϴν�˰ʱ��
	bool	IsAttorn;					//�Ƿ�ת��
	S32		AttornPrice;				//ת�ü۸�
	U32		AttornStartTime;			//ת��ʱ��
	S32		FixedMoney;					//�̶��ʽ�
	S32		FlowMoney;					//�����ʽ�
	U8		Flourish;					//���ٶ�
	char	Notice[128];				//���̸�ʾ
	bool	IsShoutAll;					//�Ƿ�ȫ������
	char	ShoutContent[128];			//��������(�����)
	U16		ShoutInterval;				//�������ʱ��(��)
	U32		ShoutItemID;				//�������ĵ��߱��
	U16		ShoutItemNum;				//�������ĵ�������
	U32		ShoutLastTime;				//�ϴκ���ʱ��
	U16		BuySellItems;				//���������Ʒ����
	U8		BuySellPets;				//���������������
	U32		BuyTime;					//��������ʱ��
	U64		DataVerion;					//���ݰ汾��
	//��ʼ������(ϵͳ��������Ҳ���ô˷���)
	void Init(U16 id)
	{
		StallID = id;
		sprintf_s(StallName, MAX_NAME_LENGTH, "%d������", id+1);
		StallerID = PreStallerID = 0;
		strcpy_s(StallerName, MAX_NAME_LENGTH, "ϵͳ����");
		StallStatus = SS_MODIFY;
		TradeTax = TRADETAX;
		StallType = STALLTYPE_NONE;
		StallTax = DAYTAX;

		U32 curtime;
		_time32((__time32_t*)&curtime);

		PayTaxTime = getZeroTime(curtime);
		IsAttorn = 0;
		AttornPrice = ATTORNPRICE;
		AttornStartTime = 0;
		FixedMoney = FlowMoney = 0;
		Flourish = BASEFLOURISH;
		Notice[0] = 0;
		IsShoutAll = 0;
		ShoutContent[0] = 0;
		ShoutInterval = SHOUTINTERVAL0;
		ShoutItemID = 0;
		ShoutItemNum = 0;
		ShoutLastTime = 0;
		BuySellItems = INITSELLITEMS;
		BuySellPets = INITSELLPETS;
		BuyTime = 0;
		DataVerion = ::GetTickCount64();
	}
};

struct stFixedStallItem : stItemInfo
{
	U16 StallID;
	U16 Pos;
	S32 Price;
	S16 BuyNum;		//�������չ����趨��ʼ�չ�����
	S16 LeftNum;	//�������չ�����ǰʣ������
	U64 DataVersion;//���ݰ汾��
	void InitData(U16 stallid, U16 pos)
	{
		stItemInfo::InitData();
		StallID = stallid;
		Pos = pos;
		Price = 0;
		BuyNum = 0;
		LeftNum = 0;
		DataVersion = ::GetTickCount64();
	}
};

struct stFixedStallPet : stPetInfo
{
	U16 StallID;
	U16 Pos;
	S32 Price;
	U64 DataVersion;
	void InitData(U16 stallid, U16 pos)
	{
		stPetInfo::InitData();
		StallID = stallid;
		Pos = pos;
		Price = 0;
		DataVersion = ::GetTickCount64();
	}
};

struct stStallVersion
{
	U32 SellItemVer[MAX_FIXEDSTALL_SELLITEM];
	U32 BuyItemVer[MAX_FIXEDSTALL_BUYITEM];
	U32 SellPetVer[MAX_FIXEDSTALL_SELLPET];
};

//�����б�������ṹ
struct stStallListEntry
{
	U8		StallID;					//̯λID
	U8		Flourish;					//���ٶ�
	U8		StallType;					//̯λ����
	U8		StallStatus;				//̯λ״̬
	bool	IsAttorn;					//�Ƿ�ת��
	char	StallName[MAX_NAME_LENGTH];	//̯λ����
	char	Notice[128];				//̯������
	char	StallerName[MAX_NAME_LENGTH];//̯������
	U32		StallerID;					//̯��ID
	S32		AttornPrice;				//ת�ü۸�
	U32		BuyTime;					//����̯λʱ��
};

//�������Խṹ
struct stStallChat
{
	U32 time;							//��ǰʱ��
	char sender[MAX_NAME_LENGTH];		//������
	char content[128];					//��������
};

//�������̽��׼�¼�ṹ
struct stStallRecord
{
	enum
	{
		TRADE_SELLITEM,					//������Ʒ
		TRADE_SELLPET,					//��������
		TRADE_BUYITEM,					//������Ʒ
	};
	U8 type;							// ���׷�ʽ
	U32 time;							// ��¼����ʱ��
	S32 money;							// ������Ǯ
	S32 tax;							// ��������˰��
	char targetName[MAX_NAME_LENGTH];	// �Է��������
	union
	{
		stItemInfo* item;				//����Ʒ����
		stPetInfo* pet;					//  ��������
	} trade;
	
	stStallRecord() { memset(this, 0, sizeof(stStallRecord));}
	~stStallRecord()
	{
		if((type == 0 || type == 2) && trade.item) delete trade.item; 
		else if(type == 1 && trade.pet) delete trade.pet;
	}
};

#pragma pack(pop)

#endif//__FIXEDSTALLBASE_H__