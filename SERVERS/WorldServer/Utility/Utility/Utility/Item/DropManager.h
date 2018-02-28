#ifndef _UTILITY_ITEM_DROPMANAGER_H_
#define _UTILITY_ITEM_DROPMANAGER_H_
#include "base/types.h"
#include "Common/PlayerStruct.h"
#include "CommLib/CommLib.h"
#include <set>
#include <vector>
#include <string>
#include <hash_map>

enum en_DropType
{
	DROP_TYPE_NPC           = 1,//�������
	DROP_TYPE_AUTODROP      = 2,//�����Զ�����
	DROP_TYPE_MANDROP       = 3,//�̶������ֶ�����
	DROP_TYPE_MAX,
};

//��Ʒ�������
struct stDropInfo
{
	std::string UID;
	U32 nItemId;

	stDropInfo(): nItemId(0), UID("")
	{
	}
};

class CDropData;

class DropManager
{
public:
	typedef stdext::hash_map<std::string, stDropInfo*> DROPMAP;
	typedef DROPMAP::iterator DROPITR;
	typedef stdext::hash_map<U32, DROPMAP>	PLAYERDROPMAP;
	typedef PLAYERDROPMAP::iterator PLAYERDROPITR;

	DropManager()
	{

	}

	~DropManager()
	{
		for(PLAYERDROPITR itr = m_PlayerDropMap.begin(); itr != m_PlayerDropMap.end(); ++itr)
		{
			for(DROPITR iter = itr->second.begin(); iter != itr->second.end();  ++iter)
			{
				SAFE_DELETE(iter->second);
			}
			itr->second.clear();
		}
		m_PlayerDropMap.clear();
	}
	//--------------------���ݿ����-------------------------
	bool        saveToDB(U32 nPlayerId, stDropInfo* pDrop);
	bool        delToDB(std::string UID);
	bool		delToDB(U32 nPlayerID);
	bool		loadDB();

	void clearDropMap(U32 nPlayerId);
	void insertDropMap(U32 nPlayerId, stDropInfo* dropInfo, bool bSaveToDB = true);

	stDropInfo* getDropInfo(U32 nPlayerId, std::string UID);
	void		delDropInfo(U32 nPlayerId, std::string UID);
	bool		doDropItem(U32 dropId, U32 nPlayerId, U32& nItemId, std::string& UID);//������Ʒ
	CDropData*	dropItemEx(U32 dropId);//��������������
	U32			dropItem(U32 nNpcId);//���ص����id
	bool		dropMapManCreate(U32 nItemId, U32 nPlayerId, std::string& UID);

	static DropManager* getInstance()
	{
		static DropManager s_cDropMgr;
		return &s_cDropMgr;
	}
private:
	PLAYERDROPMAP m_PlayerDropMap;
};

#define  DROPMGR DropManager::getInstance()

#endif