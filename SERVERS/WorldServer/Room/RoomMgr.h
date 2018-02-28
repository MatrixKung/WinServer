#ifndef ROOM_MGR_H
#define ROOM_MGR_H
#include "base/bitStream.h"
#include "common/PacketType.h"
#include "../LockCommon.h"
#include "../ManagerBase.h"
#include <hash_map>
#include "ChessTable.h"

typedef stdext::hash_map<U64, ITable*> TABLE_MAP;
typedef TABLE_MAP::iterator			   TABLE_ITR;
typedef stdext::hash_map<U32, U64>	   PLAYER_MAP;
typedef PLAYER_MAP::iterator		   PLAYER_ITR;
typedef stdext::hash_map<U64, bool>	   TABLE_MAP_EX;
typedef TABLE_MAP_EX::iterator		   TABLE_MAP_ITR;

class CRoomMgr
{
public:
	CRoomMgr();
	~CRoomMgr();

public:
	//������������
	enRoomError CreateChessTable(U32 nPlayerId, U64 &nTableId, U32 nGameType, U32 nCreateType, S64 nScore, char* HeadImgUrl, char* nickname);

	//������������
	//enRoomError JoinChessTable(U32 nPlayerId, U64 nTableId);

	//����׼��
	enRoomError ReadyChessTable(U32 nPlayerId, U64 nTableId, U32& nSlot, char* HeadImgUrl, char* nickname);

	//����׼��
	enRoomError RestartChessTable(U32 nPlayerId, U64 nTableId);

	//�뿪��������
	enRoomError LeaveChessTable(U32 nPlayerId, U64 nTableId);

	//��ɢ��������
	enRoomError DisbandChessTable(U32 nPlayerId, U64 nTableId, U8 nAgreeFlag);

	//��������
	CChessTable* FindChessTable(U64 nTableId);

	U64  AssignId();
	static CRoomMgr* Instance();

	bool HandleCreateChessTable(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);
	bool HandleJoinChessTable(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);
	bool HandleReadyChessTable(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);
	bool HandleRestartChessTable(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);
	bool HandleLeaveChessTable(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);
	bool HandleDisbandChessTable(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);
	bool HandleUserOutCard(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);
	bool HandleUserOperateCard(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);

	//��ʱ���¼�
	virtual void TimeProcess(U32 value);
	//��ȡtable ��slot
	CChessTable* FindChessTableByPlayerId(U32 nPlayerId);
private:
	TABLE_MAP m_TableMap;//��Ϸ������Ϣ
	PLAYER_MAP m_PlayerMap;//�����Ϣ
	TABLE_MAP_EX m_TableMapEx;//ϵ�к���Ϣ 
	U64 m_ID;
};
#define ROOMMGR CRoomMgr::Instance()
#endif//ROOM_MGR_H
