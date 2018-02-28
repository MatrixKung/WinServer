#ifndef __BILLINGBASE_H__
#define __BILLINGBASE_H__

#include "Base/BitStream.h"
#include "Common/PacketType.h"
#include <vector>

//�̳�����
enum enStoreType
{
	STORE_IMPORTALSTONE			= 1,	//Ԫ���̳�
	STORE_DEITYSTONE			= 2,	//��Ԫ���̳�
};

const struct {U32 id,start,end;} sgGoldBonus[] = {
	{105103049, 1326297600, 1556969599},
};

#define PRICEID  U64
#define MAINTYPE_NEWGOODS		1		//�Ƽ�����̶�Ϊ��1��
#define SUBTYPE_INTIME			101		//�Ƽ�����̶���ʱ��������Ϊ101
#define SUBTYPE_NEWGOODS		102		//�Ƽ�����̶���Ʒ����Ϊ102

#define MAINTYPE_HOTITEM		7		//�Ƽ����̶�Ϊ��7��
#define SUBTYPE_INVENTORY		701		//�Ƽ�������̶�����������Ϊ701
#define SUBTYPE_ANSWERHOT		706		//�Ƽ�������̶�����������Ϊ706

#define MAYDAY_START			615000000	//�Ͷ�����ʼʱ��
#define MAYDAY_END				731000000	//�Ͷ��ڽ���ʱ��

enum enPROMBonus
{
	PROM40_3=105101195,		//40��3�����ƹ�Ա����
	PROM50_3=105101196,		//50��3�����ƹ�Ա����
	PROM60_3=105101197,		//60��3�����ƹ�Ա����
	PROM50_4=105101198,		//50��4�����ƹ�Ա����
	PROM60_4=105100442,		//60��4�����ƹ�Ա����
	PROM70_4=105100576,		//70��4�����ƹ�Ա����
	PROM70_5=105100580,		//70��5�����ƹ�Ա����
	PROM75_4=105101199,		//75��4�����ƹ�Ա����
	PROM75_5=105100443,		//75��5�����ƹ�Ա����
	PROM80_4=105100581,		//80��4�����ƹ�Ա����
	PROM80_5=105100524,		//80��5�����ƹ�Ա����
};

#pragma pack(push, 1)

//�̳���Ʒ�ṹ
struct stGoods
{
	static const U32 MAX_ITEMS = 20;
	U8	StoreType;			//�̳�����
	U8  GoodsMainType;		//��Ʒ����
	U16	GoodsSubType;		//��ƷС��
	U8	ItemCount;			//���߸���
	U8	ItemNum[MAX_ITEMS];	//��������
	U32 GoodsID;			//��Ʒ���
	U32 ItemID[MAX_ITEMS];	//����ID
	S32 Price;				//��Ʒ�۸�
	U32 OnSale;				//�ϼ�ʱ��(���磺2011��1��1��12ʱ30��,Ϊ1101011230)
	U32 OffSale;			//�¼�ʱ��

	stGoods() { Clear();}
	void Clear() { memset(this, 0, sizeof(stGoods));}
	int Count() { return ItemCount;}
};

// ----------------------------------------------------------------------------
// �̳��ϼ���Ʒ�ṹ
struct stGoodsPrice
{
	static const U32 MAX_ITEMS = 20;
	PRICEID	PriceID;		//�ϼ���Ʒ���
	U32 GoodsID;			//��Ʒ���
	char GoodsName[32];		//��Ʒ����
	U8	StoreType;			//�̳�����
	U8  GoodsMainType;		//��Ʒ����
	U16	GoodsSubType;		//��ƷС��
	U32 ShowItemID;			//����չʾ�ĵ���ID
	U8	PriceType;			//��Ʒ��������
	S32 Price;				//ʵ�ʹ��򵥼�
	U8	ItemCount;			//���߸���
	U32 ItemID[MAX_ITEMS];	//����ID
	U8	ItemNum[MAX_ITEMS];	//��������
	U32 DiscountID;			//�ۿ����ñ��
	U8	DiscountRate;		//�ۿ�ֵ
	U32 OnSale;				//�ϼ�ʱ��
	U32 OffSale;			//�¼�ʱ��
	U32 StartTime;			//�ۿۿ�ʼʱ��
	U32 EndTime;			//�ۿ۽���ʱ��
	stGoodsPrice() { Clear();}
	void Clear() { memset(this, 0, sizeof(stGoodsPrice));}
	int Count() { return ItemCount;}
};

//�̳��ۿ����ýṹ
struct stDiscount
{
	U32 DiscountID;			//�ۿ����ñ��
	U32 GoodsID;			//��Ʒ���
	U8  DiscountRate;		//�ۿ�ֵ(0���ۿ� 1-9����1��-9��)
	U32 StartTime;			//�ۿۿ�ʼʱ��
	U32 EndTime;			//�ۿ۽���ʱ��
};

//�̳��Ƽ����ýṹ
struct stRecommend
{
	static const U32 MAX_GOODS = 5;
	U32 RecommendID;		//�Ƽ����ñ��(С��1000�����ض��Ƽ����)
	PRICEID PriceID[MAX_GOODS];	//�Ƽ���Ʒ����б�
	stRecommend() { Clear();}
	void Clear() { memset(this, 0, sizeof(stRecommend));}
};

//�̳ǽ��׼�¼���ݽṹ
struct stTradeGoods
{
	U32	TradeID;			//������ˮ��
	U32	AccountID;			//�ʺ�ID
	U32	PlayerID;			//��ɫID
	PRICEID	PriceID;		//�ϼ���Ʒ���
	U32	PriceNum;			//������Ʒ����
	S8	PriceType;			//����(0-Ԫ�� 1-��Ԫ��)
	U32	Price;				//����
	U32	AccountGold;		//�ʺ�ʣ��Ԫ����
	U32	AreaID;				//�������ѷ�
	U32	TradeDate;			//���׷���ʱ��
	stTradeGoods() { Clear();}
	void Clear() { memset(this, 0, sizeof(stTradeGoods));}
};

// �̳ǹ㲥��Ϣ���ݽṹ 
struct stMallBoard
{
	char Content[250];		//�㲥��Ϣ�ַ�������
	int  MsgId;				//�㲥��Ϣ��ˮID
	int	 UpdValue;			//���±�־���Ե�����ֵ��ȷ�ϱ仯
	stMallBoard():MsgId(0),UpdValue(0)
	{
		Content[0] = '\0';
	}
};

struct stPROMItemBonus
{
	static const unsigned int MAX_LEVEL = 6;
	static const unsigned int MAX_ITEM = 11;
	S8	DrawFlag[MAX_ITEM];		//��Ʒ�����Ƿ���ȡ(0-������ȡ 1-����ȡ 2-����ȡ)
	int	Levels[MAX_LEVEL];		//�ﵽ�ȼ��ĺ���������80����75��,70����60����50����40��)
	stPROMItemBonus() {	memset(this, 0, sizeof(stPROMItemBonus));}
};

#pragma pack(pop)

typedef std::vector<stGoodsPrice*>	GOODSVEC;
typedef std::vector<stRecommend*>	RECOMMENDVEC;
typedef std::vector<stMallBoard*>	BOARDVEC;

#endif//__BILLINGBASE_H__