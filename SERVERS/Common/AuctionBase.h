#ifndef __AUCTIONBASE_H__
#define __AUCTIONBASE_H__

#include "Base/BitStream.h"
#include "Common/PacketType.h"
#include "common/PlayerStruct.h"
#include "CommLib/CommLib.h"
#include <map>
#include <hash_map>
class ItemShortcut;

enum enAuction
{
	MAX_AUCTION_QUERY	= 200,			//һ��Pack��Zone������������������
	MAX_AUCTION_TAX		= 20,			//������������Ϊ��������Ԫ����20%
	MAX_AUCTION_SLOTS	= 7,			//��������Ʒ���ÿҳ����ʾ����Ʒ��
	MAX_AUCTION_QUERYINTERVAL = 5000,	//�����в�ѯ���

	AUCTION_BROWSE_0	= 0,			//������Ʒ�����
	AUCTION_BROWSE_1	= 1,			//������Ʒ������ʷ��
};

enum enAuctionState
{
	AUCSTATE_NONE		= 0,			//������
	AUCSTATE_CANCEL		= 10,			//������Ʒ������ȡ��
	AUCSTATE_EXPIRY		= 11,			//������Ʒ��������
	AUCSTATE_BUYOK		= 13,			//������Ʒ������ɹ��¼�
};

enum enAuctionTime
{
	AUCTIONTIME_12,			//12Сʱ
	AUCTIONTIME_24,			//24Сʱ
	AUCTIONTIME_36,			//36Сʱ
	AUCTIONTIME_48,			//48Сʱ
	AUCTIONTIME_MAX,
};

// ----------------------------------------------------------------------------
// ���㱣�ܷѵĹ�ʽ
inline S32 calcDeposit(U32 timeType, S32 fixedPrice)
{
	return mClamp((S32)(fixedPrice/100.00f * (timeType+1)), 1000,1000000);
}

#pragma pack(push,1)
// ----------------------------------------------------------------------------
// �����л�������
struct stAuctionInfo
{
	U8  Status;				//��������Ʒ״̬
	U32 Id;					//������ID
	U32	Owner;				//������(Note: Ҳ���Ǻ������������)
	S32 FixedPrice;			//�����۸�(Note: Ҳ���Ժ���������ۼ۸�)
	U32	ExpiryTime;			//����ʱ��
	U32 ItemId;				//��ƷID
	S32 ItemNum;			//��Ʒ����
	stItemInfo* pItem;		//��Ʒ����

	stAuctionInfo():Id(0),Owner(0),Status(AUCSTATE_NONE),FixedPrice(0),
		ItemId(0),ItemNum(1),ExpiryTime(0),pItem(NULL)
	{
	}

	~stAuctionInfo() {if(pItem) delete pItem;}
	bool canClear() { return (Status >= AUCSTATE_CANCEL);}
};

//��������Ʒ
struct stAuctionGoods
{
	U32 Id;							// ��������ƷID
	U32 ExpiryTime;					// ���ܹ���ʱ��
	S32 FixedPrice;					// Ԫ���۸�
	U32 ItemId;						// ��ƷID
	S32 ItemNum;					// ��Ʒ����
	ItemShortcut* pItem;			// ��Ʒʵ��
	stAuctionGoods():Id(0),ExpiryTime(0),
		FixedPrice(0),ItemId(0),ItemNum(0),pItem(NULL) {}
};

#pragma pack(pop)

enum enAuctionNetOp
{
	AUCTION_NET_OPEN,				// ��������
	AUCTION_NET_CLOSE,				// �ر�������
	AUCTION_NET_SETTING,			// ���ü����ϼ���Ϣ
	AUCTION_NET_BUY,				// ������������Ʒ
	AUCTION_NET_BID,				// ������������Ʒ
	AUCTION_NET_CANCLE,				// ȡ������
	AUCTION_NET_BROWSE,				// �����������Ʒ
	AUCTION_NET_BROWSEHISTORY,		// �����������ʷ
	AUCTION_NET_BROWSEPAGE,			// �����������Ʒ��ҳ
	AUCTION_NET_CLEARTEMP,			// ������ռ�������������Ʒ
};

enum enAuctionType
{
	AUCTIONTYPE_SYSTEM,		//�ٷ�����
	AUCTIONTYPE_WEAPON,		//����
	AUCTIONTYPE_EQUIP,		//װ��
	AUCTIONTYPE_PET,		//��������
	AUCTIONTYPE_BAOSHI,		//�ٱ���ʯ
	AUCTIONTYPE_XIAOHAO,	//�ճ�����
	AUCTIONTYPE_OTHER,		//����

	ASUB_SYSTEM_ITEM			= 1,			//�ٷ�����-��Ʒ

	ASUB_WEAPON_FAQI			= 10,			//����-����
	ASUB_WEAPON_DAOJIAN			= 11,			//����-����
	ASUB_WEAPON_HUANREN			= 12,			//����-����
	ASUB_WEAPON_QIN				= 13,			//����-��
	ASUB_WEAPON_BOW				= 14,			//����-��
	ASUB_WEAPON_FLAGSTAFF		= 15,			//����-����
	ASUB_WEAPON_ARMS			= 16,			//����-����

	ASUB_EQUIP_HEAD				= 20,			//װ��-ͷ��
	ASUB_EQUIP_BODY				= 21,			//װ��-����
	ASUB_EQUIP_BACK				= 22,			//װ��-����
	ASUB_EQUIP_SHOULDER			= 23,			//װ��-�粿
	ASUB_EQUIP_HAND				= 24,			//װ��-�ֲ�
	ASUB_EQUIP_WAIST			= 25,			//װ��-����
	ASUB_EQUIP_FOOT				= 26,			//װ��-�Ų�
	ASUB_EQUIP_AMULET			= 27,			//װ��-�����
	ASUB_EQUIP_NECK				= 28,			//װ��-����
	ASUB_EQUIP_RING				= 29,			//װ��-��ָ

	ASUB_PET_PET				= 30,			//��������-����
	ASUB_PET_MOUNT				= 31,			//��������-���
	ASUB_PET_EQUIP				= 32,			//��������-����װ��
	ASUB_PET_SKILLBOOK			= 33,			//��������-���޼�����

	ASUB_BAOSHI_CHIP			= 40,			//�ٱ���ʯ-��ʯ��Ƭ
	ASUB_BAOSHI_RED				= 41,			//�ٱ���ʯ-�챦ʯ
	ASUB_BAOSHI_GREEN			= 42,			//�ٱ���ʯ-�̱�ʯ
	ASUB_BAOSHI_BLUE			= 43,			//�ٱ���ʯ-����ʯ
	ASUB_BAOSHI_CRYSTAL			= 44,			//�ٱ���ʯ-ˮ��ʯ
	ASUB_BAOSHI_BOX				= 45,			//�ٱ���ʯ-��ʯ��

	ASUB_XIAOHAO_MEDIA			= 50,			//�ճ�����-��ҩʳ��
	ASUB_XIAOHAO_SCROLL			= 51,			//�ճ�����-�����ֽ
	ASUB_XIAOHAO_BAG			= 52,			//�ճ�����-�ռ���չ
	ASUB_XIAOHAO_EQUIP			= 53,			//�ճ�����-װ��ǿ������
	ASUB_XIAOHAO_PET			= 54,			//�ճ�����-����ǿ������
	ASUB_XIAOHAO_SPIRIT			= 55,			//�ճ�����-Ԫ��ǿ������
	ASUB_XIAOHAO_SPIRITADD		= 56,			//�ճ�����-��Ԫ�ϳɲ���

	ASUB_OTHER_OTHER			= 60,			//����-����
};

//˳���б���AuctionManageά��������ֵ��
typedef std::map<U32, U32> AUCTIONSIDMAP;
//�������б�
typedef stdext::hash_map<U32, stAuctionInfo*> AUCTIONINFOMAP;
typedef std::list<stAuctionGoods*> LISTAUCTION;

typedef std::vector<stAuctionInfo*> AUCTIONLIST;

#endif//__AUCTIONBASE_H__