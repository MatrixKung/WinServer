#ifndef _UTILITY_ITEM_EQUIPMANAGER_H_
#define _UTILITY_ITEM_EQUIPMANAGER_H_
#include "base/types.h"
#include "../../AccountHandler.h"
#include "Common/PlayerStruct.h"

class EquipManager
{
public:
	enum enEquipPos
	{
		EQUIP_WEAPON_POS				 = 0,//����
		EQUIP_DEFENSE_POS				 = 1,//����
		EQUIP_SECONDARY_POS				 = 2,//����װ��
		EQUIP_MAX_POS					 = 3,
	};

	EquipManager()
	{
	}

	~EquipManager()
	{
	}

	static EquipManager* getInstance()
	{
		static EquipManager s_cEquipMgr;
		return &s_cEquipMgr;
	}

	//����װ��
	bool mountEquip(AccountRef pAccount, int SocketHandle, stItemInfo* stItem);

	//ж��װ��
	bool unmoutEquip(AccountRef pAccount, int SocketHandle, stItemInfo* stItem);

	//��ȡװ���Ĳ�λ
	enEquipPos getEquipPose(U32 nItemId);
};

#define  EQUIPMGR EquipManager::getInstance()
#endif