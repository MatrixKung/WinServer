#ifndef __GOLDEXCHANGEBASE_H__
#define __GOLDEXCHANGEBASE_H__

#include "Base/BitStream.h"
#include "Common/PacketType.h"
#include <vector>


//����Ԫ���һ����ݱ�־
enum enExchangeType
{
	GEF_CR,					//��ֵ�����һ�����
	GEF_DR,					//��ȡ���Ͷһ�����
	GEF_WR,					//�´��һ�����
	GEF_MR,					//��С��һ�����
	GEF_VR0,				//һ���һ�����
	GEF_VR1,				//�����һ�����
	GEF_VR2,				//�����һ�����
	GEF_VR3,				//�ļ��һ�����
	GEF_VR4,				//�弶�һ�����
	GEF_VR5,				//�����һ�����
	GEF_MAX,				//�һ���������
};

//���ڻ״̬
enum enPeriodStatus
{
	APS_EXPIRE,				//�ѽ���
	APS_DOING,				//������
	APS_NOOPEN,				//δ��ʼ
};

#pragma pack(push, 1)

// ----------------------------------------------------------------------------
// �¶һ��������ݽṹ
struct stGoldConfig
{
	S32 Change[GEF_MAX];
	U32 ItemID[GEF_MAX];
	U32 Times[GEF_MAX];
	S32 Condition[GEF_MAX];
	U32 Person[GEF_MAX];
	stGoldConfig() { Clear();}
	void Clear() {memset(this, 0, sizeof(stGoldConfig));}
};

// ----------------------------------------------------------------------------
// �¶һ����ݽṹ
struct stGoldExchange
{
	U32 AccountID;			//�ʺ�ID
	U32 PlayerID;			//��ɫID
	U32 YearMonth;			//�һ�����
	U32 AccountGold;		//����ȡ�ʺ�Ԫ����
	U32 CurrentGold;		//��ǰ����ȡԪ����
	U32 MonthPays;			//�³�ֵ����
	U32 MonthGolds;			//���ۻ���ȡԪ����
	U32 Exchanges[GEF_MAX]; //�¶һ�����
	stGoldExchange() { Clear();}
	void Clear() { memset(this, 0, sizeof(stGoldExchange));}
};

// ----------------------------------------------------------------------------
// ���ڷ�����Ч�ڼ��������ݽṹ
struct stSR_Period
{
	U8  Status;				//�״̬(0-�ѽ��� 1-������ 2-δ��ʼ)
	U8	IsDefault;			//�Ƿ�ȱʡ��ʾ�Ļ
	U8  IsRewardGold;		//�Ƿ�����Ԫ��Ԫ��
	U32 SRId;				//���ڷ����ID
	U32 StartDate;			//���ڷ������ʼ����
	U32 EndDate;			//���ڷ������������
	U32 DrawDate1st;		//���ڷ������һ����ȡ����
	U32 DrawDate2nd;		//���ڷ�����ڶ�����ȡ����
	U32 DrawDate3rd;		//���ڷ������������ȡ����
	stSR_Period() { memset(this, 0, sizeof(stSR_Period));}
};

// ----------------------------------------------------------------------------
// ���ڷ��������������ݽṹ
struct stSR_Bonus
{
	U8  PerStag;			//���ڷ����ĵڼ��׶�
	U8	PerMonth;			//���ڷ����ĵڼ�����
	U32 SRId;				//���ڷ����ID
	S32 Gold;				//����Ԫ������
	S32 BindGold;			//������Ԫ������
	S32 Money;				//������Ԫ����
	S32 BindMoney;			//�����󶨽�Ԫ����
	S32 Exp;				//����������Ϊ����
	stSR_Bonus()
	{ 
		SRId = Gold = BindGold = Money = BindMoney = Exp = 0;
		PerMonth = PerStag = 0;
	}
};

struct stSR_Bonus_Data
{
	U8	IsDraw;				//�Ƿ���ȡ��(0-δ��ȡ 1-����ȡ 2-������ȡ)
	S32 Gold;				//����Ԫ��
	S32 BindGold;			//������Ԫ��
	S32 Money;				//������Ԫ
	S32 BindMoney;			//�����󶨽�Ԫ
	S32 Exp;				//����������Ϊ
	stSR_Bonus_Data()
	{
		IsDraw = 2;
		Gold = BindGold = Money = BindMoney = Exp = 0;
	}
};

// ----------------------------------------------------------------------------
// ��ɫ�ķ��ڷ����������ݽṹ
struct stSR_History
{
	U32 SRId;				//���ڷ����ID
	S32 TotalDrawGolds;		//��ڼ���ȡ��Ԫ����
	stSR_Bonus_Data Data[3];//���������µĽ�������
	stSR_History()
	{
		SRId = TotalDrawGolds = 0;
	}

	void Clear() { memset(this, 0, sizeof(stSR_History));}
};

#pragma pack(pop)

#endif//__GOLDEXCHANGEBASE_H__