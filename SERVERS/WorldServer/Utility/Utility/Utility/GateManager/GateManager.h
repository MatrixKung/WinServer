#ifndef _UTILITY_GATE_MANAGER_H_
#define _UTILITY_GATE_MANAGER_H_
#include "base/types.h"
#include "Common/PlayerStruct.h"
#include "../../ManagerBase.h"
#include "../../AccountHandler.h"
#include <set>
#include <vector>
#include <string>

class CGateManager: public CManagerBase<CGateManager, MSG_SCENE_BEGIN, MSG_SCENCE_END>
{
public:
	enum enGateType
	{
		GATE_TYPE_BEGIN =  1,
		GATE_TYPE_END	= 27,
	};

	CGateManager();

	virtual ~CGateManager();

	bool verifyPlayerFakt(AccountRef pAccount, U32 LocalLoginTime);
	bool gotoNextGate(AccountRef pAccount);
	bool gotoNewTypeGate(AccountRef pAccount, S32 nGateType);
	bool sceneInitFinsh(AccountRef pAccount, Base::BitStream *Packet);

	//-------------------------------������Ϣ-------------------------------
	void HandlePlayerVerifyFakeRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//��֤������
	void HandlePlayerGoToNextGateRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//����������
	void HandlePlayerGoToNewTypeGateRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//����������
	void HandlePlayerSceneInitedRequest(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//���������������
};

#endif