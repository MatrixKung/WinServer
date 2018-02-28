#ifndef _UTILITY_ITEM_ITEMMANAGER_H_
#define _UTILITY_ITEM_ITEMMANAGER_H_
#include "base/types.h"
#include "Common/PlayerStruct.h"
#include "../../AccountHandler.h"
#include "../../ManagerBase.h"
#include <set>
#include <vector>
#include <deque>
#include <string>

class CItemManager:public CManagerBase<CItemManager, MSG_ITEM_BEGIN, MSG_ITEM_END >
{
public:
	enum
	{
		EQUIP_PROMOTE_LEVEL				= 5,
	};

	enum enItemOPError
	{
		ITEM_OP_NONE					 = 0,
		ITEM_OP_UNKNOW					 = 1,
		ITEM_OP_ITEMSTRUCTERROR			 = 2,
		ITEM_OP_STRENGTHENLEVEL_LIMIT	 = 3,
		ITEM_OP_STRENGTHEN_SUCESS		 = 4,
	};

	typedef std::set<U32>			SET_U32;
	typedef SET_U32::iterator		SETITR_U32;
	typedef std::deque<U32>			DEQ_U32;
	typedef DEQ_U32::iterator		DEQITR_U32;
	typedef std::vector<std::string>UIDVEC;
	typedef UIDVEC::iterator        UIDITR;

	CItemManager();
	virtual ~CItemManager();

	//-------------------װ����������---------------------------------
	bool ItemAddExp(stItemInfo* stItem, AccountRef pAccount, int SocketHandle, U32 nExp = 1);
	bool ItemLevelUp(stItemInfo* stItem, AccountRef pAccount, int SocketHandle);//װ������
	U32  getLevelUpExp(stItemInfo* stItem);				//��ȡװ����������
	bool CanLevelUp(stItemInfo* stItem);				//װ���ܷ�����

	//װ������
	bool ItemPromote(stItemInfo* stItem, AccountRef pAccount, int SocketHandle);//װ������
	bool CanPromote(stItemInfo* stItem);				//�ܷ����

	//��ȡװ������Ƕ��ʯ������ǿ���ȼ�
	void getEquipStrenthenAndGemHole(stItemInfo* stItem, U32& nMaxStrengthen, U32& nMaxGemHole);

	//װ��ǿ�� lvǿ���ĵȼ�
	bool ItemStrengthen(stItemInfo* stItem, AccountRef pAccount, int SocketHandle,U8 lv=1, enShortcutType slotType =SHORTCUTTYPE_EQUIP);//װ��ǿ��
	bool CanStrengthen(stItemInfo* stItem);

	//װ���ϳ�
	bool ItemCompose(stItemInfo* stItem, AccountRef pAccount, int SocketHandle, std::string UID[MAX_COMPOSE_SIZE], enShortcutType slotType = SHORTCUTTYPE_EQUIP);
	bool CanCompose(stItemInfo* stItem);
	bool TransferEmbedSlot(stItemInfo* stItem, stItemInfo* stSubItem);

	//������Ʒ
	stItemInfo* CreateItem(U32 nItemId, U32 nQuantity = 1);
	//�����Ʒ
	bool AddItem(U32 nItemId,  AccountRef pAccount, int SocketHandle, U32 nQuantity = 1);
	//Ϊ�ʼ�����һЩ������Ʒ
	bool AddItemEx(stItemInfo* pItem, AccountRef pAccount);
	//ʹ����Ʒ
	bool UseItem(stItemInfo* stItem,AccountRef pAccount, int SocketHandle);
	//---------------------------�ֿ⴦��-----------------------------
	bool StoreItemToBank(UIDVEC& uidVec, AccountRef pAccount);
	bool GetItemFromBank(UIDVEC& uidVec, AccountRef pAccount);
	//---------------------------��Ϣ������--------------------------
	void HandleClientMountEquipRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//����װ��
	void HandleClientUnmountEquipRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//ж��װ��
	void HandleClientEquipAddExpRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//װ����Ӿ���
	void HandleClientStrengthenEquipRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//���ǿ��װ��
	void HandleClientComposeEquipRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet); //��Һϳ�װ��
	void HandleClientUseItemRequset(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet); //ʹ����Ʒ
	void HandleClientAddItemRequset(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet); //�����Ʒ
	void HandlePlayerBankStoreItemRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//��ֿ�����
	void HandlePlayerBankGetItemRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//�Ӳֿ�ȡ��Ʒ����
	void HandleClientDropItemRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet); //������Ʒ
	void HandleClientPickItemRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet); //������Ʒ
};
#endif