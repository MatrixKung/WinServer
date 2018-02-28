#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include <hash_map>
#include <deque>
#include "WINTCP/IPacket.h"
#include "Common/PlayerStruct.h"
#include "BaseMgr.h"

struct CDropData;
typedef std::pair<S32, S32> DROPITEM_PAIR;
/************************************************************************/
/* ��Ʒ�������
/************************************************************************/
class CItemManager : public CBaseManagerEx<ItemInfoRef, std::string>
{
	enum Item_Event
	{
		ITEM_NO_SHOW = 0,
		ITEM_ADD	 = 1,
		ITEM_LOSE	 = 2,
	};
public:
	CItemManager();
	~CItemManager();

	static CItemManager* Instance();
	//������Ʒ
	ItemInfoRef CreateItem(U32 nItemId, U32 nPayerID, U32 nQuantity = 1);
	//�����Ʒ
	bool AddItem(U32 nPlayerID, U32 nItemId, S32 Quantity = 1);
private:
	bool _AddItem(U32 nPlayerID, U32 nItemId, S32 Quantity = 1);
	bool _ReduceItem(U32 nPlayerID, U32 nItemId, S32 Quantity = 1);
public:
	bool CanReduceItem(U32 nPlayerID, U32 nItemId, S32 Quantity = 1);
	//ʹ����Ʒ
	bool UseItem(U32 nPlayerID, U32 nItemId, S32 Quantity = 1);
	bool UseItem(U32 nPlayerID, std::string UID, S32 Quantity = 1, std::string UID1 = "");
	bool ComposeItem(U32 nPlayerID, U32 nComposeID);
	CDropData*  DropItem(U32 nDropID);
	DROPITEM_PAIR DropItem(U32 nPlayerID, U32 nDropID);
	bool BatchDropItem(U32 nPlayerID, U32 nDropID, S32 Quantity = 1);
	bool AddGift(U32 nPlayerID, U32 nGiftID, S32 Quantity = 1);
	enOpError BuyItem(U32 nPlayerID, U32 nShopId, S32 Quantity = 1);
	//ʹ�ü��ٿ�
	enOpError UseAccCard(U32 nPlayerID, std::string UID, S32 TimerId, S32 Quantity = 1);
	//ʹ�ñ�����
	enOpError UseAddPVPTimesCard(U32 nPlayerID, std::string UID, S32 Quantity = 1);
	//ǿ��
	enOpError EquipStrengthen(U32 nPlayerID, std::string EquipUID[101], std::string CardUID);
	//�һ��̵�
	enOpError BuyRandomShop(U32 nPlayerID, S32 nSlot);
	enOpError OpenRandomShop(U32 nPlayerID);
	enOpError ReflashRandomShop(U32 nPlayerID);
	//�齱(nType:1�����꣬2��10�����꣬ 3��1�������� 4��5������)
	enOpError DrawItem(U32 nPlayerID, S8 nType, std::vector<DROPITEM_PAIR>& ItemVec);
	//---------DB Operator---------//
	DB_Execution* LoadDB(U32 nPlayerID);

	bool UpdateDB(ItemInfoRef pItem);
	bool InsertDB(U32 nPlayerID, ItemInfoRef pItem);
	bool DeleteDB(U32 nPlayerID, std::string UID);

	//----------֪ͨ�ͻ���-----------//
	void UpdateToClient(U32 nPlayerID, ItemInfoRef pItem);
	void SendInitToClient(U32 nPlayerID);

	//----------��ɫ��¼֪ͨ---------//
	bool onPlayerEnter(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool onPlayerLeave(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);

	//----------��Ϣ����----------//
	bool HandleClientUseItemRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleClientComposeItemRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleClientBuyItemRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleClientUseAccCardRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleClientUseAddPVPTimesRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);

	bool HandleClientBuyRandomShopRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleClientOpenRandomShopRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleClientReflashRandomShopRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleClientDrawItemRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleClientEquipStrengthenRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);

	static void Export(struct lua_State* L);
};

#define ITEMMGR CItemManager::Instance()
#endif//ITEM_MANAGER_H
