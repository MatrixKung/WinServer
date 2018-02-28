#ifndef STOCK_BASE_H
#define STOCK_BASE_H

#include "Base/BitStream.h"
#include "Common/PacketType.h"
#include <xfunctional>
#include <map>


//Ԫ���ҵ�����
enum enOrderType
{
	OT_BUY,						//����Ԫ���ҵ�
	OT_SELL,					//����Ԫ���ҵ�
};

//Ԫ�����׹ҵ�״̬
enum enOrderStatus
{
	OS_NONE,					//�ҵ�����������
	OS_FINISH,					//�ҵ��ɽ����
	OS_CLOSE,					//�ҵ��Ѿ������̲���ȡ��
	OS_CANCEL,					//�ҵ��Ѿ�����ҳ���
	OS_MAX,
};

static char* szOrderStatus[OS_MAX] = 
{
	"�ȴ�����",
	"��ɽ���",
	"���̳���",
	"��ҳ���",
};

//Ԫ����������
enum enGoldOp
{
	GO_DRAWSAVE,				//��ȡ����
	GO_BUYGOODS,				//��Ʒ����
	GO_DRAWREWARD,				//���ڷ�������Ԫ��
	GO_BUYGOLDSAVE = 10,		//Ԫ���������뽻�״���
	GO_SELLGOLDSAVE,			//Ԫ�����������۳�
	GO_CANCELBUYORDER,			//��������Ԫ���ҵ�������Ǯ
	GO_CANCELSELLORDER,			//��������Ԫ���ҵ�����Ԫ��
	GO_CLOSEORDER,				//���̷���Ԫ��
	GO_RELIFE,					//��Ԫ����������
	GO_EXCHANGE_CR,				//��ֵ�����һ�
	GO_EXCHANGE_DR,				//��ȡԪ���һ�
	GO_EXCHANGE_WR,				//ÿ�ܸ����һ�
	GO_EXCHANGE_MR,				//ÿ�¸����һ�
	GO_EXCHANGE_VR0,			//�����ۻ��һ�
	GO_EXCHANGE_VR1,			//�м��ۻ��һ�
	GO_EXCHANGE_VR2,			//�߼��ۻ��һ�
	GO_EXCHANGE_VR3,			//�������ۻ��һ�
	GO_EXCHANGE_VR4,			//�ƽ��ۻ��һ�
	GO_EXCHANGE_VR5,			//��ʯ���ۻ��һ�
    GO_CHALLENGE_INC,           //��������ս��������
	GO_EQUIP_SUOHUN,			//װ����������Ԫ��
	GO_GUANXING_ZHANBU,			//����ռ��
	GO_GUANXING_OPENSLOT,		//���������
    GO_ACTIVE_XXPACK,           //�������󱦴�
    GO_ADD_XXPACK_POWER,        //���󱦴�����
	GO_ACTIVE_PRIVILEGE,        //��ͨ��Ȩ
	GO_PAY_AGAIN_PRIVILEGE,     //��Ȩ����
	GO_UPGRADE_PRIVILEGE,        //��Ȩ����
	GO_GMUPDATE,				//GM����
	GO_TRUMP_SUOHUN,            //������������Ԫ��
	GO_SOULEQ_SUOHOLE,          //��װ������ʯ��
};

//�ҵ���������
enum enGoldReason
{
	GR_PENDING,					//�ҵ��ɹ�(δ���κν���)
	GR_PART,					//�ҵ��ɹ�(�в��ֽ���)
	GR_FINISH,					//�ҵ��ɹ�(ȫ����ɽ���)
	GR_FAILED,					//�ҵ�ʧ��(δ֪ԭ��)
	GR_REDUCEGOLD,				//�ҵ�ʧ��(Ԥ��Ԫ��ʧ��)
};

//Ԫ�����׹ҵ����ݽṹ
struct stGoldTrade
{
	U32  OrderID;				//������ˮ��
	U32  AccountID;				//�ʺ�ID
	U32  PlayerID;				//��ɫID
	char PlayerName[32]; 		//��ɫ����
	U8	 OrderType;				//���׷�ʽ(0-����Ԫ�� 1-����Ԫ��)
	int  Price;					//����(ÿ100Ԫ����ֵ)
	int  TradeNum;				//�ҵ�ԭʼ����
	int  LeftNum;				//ʣ������
	U8	 OrderStatus;			//����״̬
	U32  UpTime;				//�ҵ�ʱ��
	stGoldTrade() { Clear();}
	void Clear() { memset(this, 0, sizeof(stGoldTrade));}
};

//Ԫ�����׽�����ʷ�ṹ
struct stGoldTradeDetail
{
	U32  OrderID;				//������ˮ��
	U32  TradeNum;				//��������
	U32  TradeDate;				//�ɽ�ʱ��
	stGoldTradeDetail() {OrderID = TradeNum = TradeDate = 0;}
};

typedef std::multimap<int, stGoldTrade*> MAPASC;
typedef std::multimap<int, stGoldTrade*, std::greater<int> > MAPDESC;

//Ԫ�����׶�������
struct stQuotation
{
	struct stPrice
	{
		U16 Num;
		int Price;
	};
	struct stLatestPrice
	{
		U16 Num;
		int Price;
		U32 TradeDate;
	};

	enum {MAX_ITEMS = 5};

	stPrice BuyPrice[MAX_ITEMS];		//������ǰ5������Ԫ���ҵ���
	stPrice SellPrice[MAX_ITEMS];		//������ǰ5������Ԫ���ҵ���
	stLatestPrice LatestPrice[MAX_ITEMS];//���5�ʽ��׼�¼
	int CurrentPrice;					//����ɽ���(�������һ�ʳɹ����׵�Ԫ������)
	int ClosePrice;						//������̼�(����6:00����ǰ���һ�ʳɹ����׵�Ԫ������)
	int Pendings;						//δ���Ĺҵ���
	stQuotation() { Clear();}
	void Clear() {memset(this, 0, sizeof(stQuotation));}
};

//�ɽ�Ԫ�����׹ҵ�
struct stGoldTradeEx : stGoldTrade
{
	int BargainNum;		//�ɽ�����
};

//����Ԫ�����׹ҵ�
struct stCloseGoldTrade
{
	int OrderID;	//�ҵ����
	int PlayerID;	//��ɫID
	int LeftValue;	//�˻���ʣ��Ԫ����˰�ս�
};

#endif//STOCK_BASE_H