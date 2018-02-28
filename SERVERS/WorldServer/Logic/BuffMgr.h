#ifndef BUFF_MANAGER_H
#define BUFF_MANAGER_H

#include <hash_map>
#include "WINTCP/IPacket.h"
#include "Common/PlayerStruct.h"
#include "BaseMgr.h"
#include "BuffData.h"

/************************************************************************/
/* ս���������
/************************************************************************/
class CBuffManager : public CBaseManager<StatsRef>
{
public:
	CBuffManager();
	~CBuffManager();
	static CBuffManager* Instance();

	//�������ս������
	void ClearStatus(U32 nPlayerID);
	void AddStatus(U32 nPlayerID, Stats& stats);
	void LoadDB(U32 nPlaydID);

	static void Export(struct lua_State* L);
};

#define BUFFMGR CBuffManager::Instance()
#endif//BUFF_MANAGER_H
