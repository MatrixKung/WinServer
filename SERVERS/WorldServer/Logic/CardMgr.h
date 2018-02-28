#ifndef CARD_MANAGER_H
#define CARD_MANAGER_H

#include <unordered_map>
#include "WINTCP/IPacket.h"
#include "CardBase.h"
#include "BaseMgr.h"
#include <vector>

struct Stats;
struct stBuff;
struct stFate 
{
	S32 PlayerID;
	S32 FateID;//����Ե��ID
	S32 Level;//����Ե�ֵȼ�
	S32 Val;//���Ƹ�������
	std::string UID[10];

	stFate():PlayerID(0), FateID(0), Level(0), Val(0)
	{
	}
};

typedef std::tr1::shared_ptr<stFate> FateRef;
/************************************************************************/
/* ���ƹ������
/************************************************************************/
class CCardManager : public CBaseManagerEx<CardRef, std::string>
{
	static const int s_AddAbilityVal[MAX_ADD_SLOT];
	static const int s_CostGrowItemID[3];
	static const int s_AddAbilityConfig[12][3];
	static const int s_OfficalRankConfig[10][2];
	typedef		std::unordered_map<S32, FateRef>		FATEMAP;
	typedef		std::unordered_map<S32, FATEMAP>		FATEPLAYERMAP;
public:
	typedef		DATA_MAP	DATA_MAP;
	CCardManager();
	~CCardManager();

	static CCardManager* Instance();

	CardRef CreateCard(U32 nPlayerID, U32 nCardID);
	bool CanAddCard(U32 nPlayerID, U32 nCardID);
	bool AddCard(U32 nPlayerID, U32 nCardID, bool bNotify = true);
	bool RemoveCard(U32 nPlayerID, std::string UID);
	void RemovePlayer(U32 nPlayerID);
	enOpError DispatchCard(U32 nPlayerID, std::string UID, U32 nType);//��ǲ����
	enOpError DispatchCardAward(U32 nPlayerID, S32 nTimeID, S32& nAddVal, bool& bDouble);
	enOpError AddCardLevel(U32 nPlayerID, std::string UID, S32 level);//�����ȼ�
	enOpError AddCardLevelLimit(U32 nPlayerID, std::string UID);//�ȼ�ͻ��
	enOpError AddCardOfficalRank(U32 nPlayerID, std::string UID);//������Ʒ
	enOpError AddCardSkill(U32 nPlayerID, std::string UID, S32 SkillID, bool& bScuess);//��������
	enOpError AddCardEquip(U32 nPlayerID, std::string UID, std::string ItemUID);//װ����Ʒ
	enOpError RemoveCardEquip(U32 nPlayerID, std::string UID, S32 nPos);//װ����Ʒ
	enOpError AddCardAbility(U32 nPlayerID, std::string UID, U32 nType, S32 nTimes);//��������
	enOpError AddCardGrow(U32 nPlayerID, std::string UID, U32 nType, S32& nAddVal);//�����ɳ�
	enOpError HireCard(U32 nPlayerID, U32 nCardID);//�ٻ�����
	enOpError UpdateApply(U32 nPlayerID, S32 iCreateNum);//������
	enOpError ReadApply(U32 nPlayerID, S32 TimeID, bool bApply);//��������
	bool BanishCard(U32 nPlayerID, std::string UID);
	//------------Ե��-----------//
	void RemoveFatePlayer(U32 nPlayerID);
	bool CaculteCardFate(U32 nPlayerID, bool bNotify = true);
	bool LoadCardFateDB(U32 nPlayerID);
	void CaculteFateState(U32 nPlayerID, std::string UID, S32 nBase_gPc[3]);
	//___________________________//
	enOpError _LearnSkill(U32 nPlayerID, std::string UID, S32 SkillID, bool& bScuessful);
	enOpError _LeaveUpSkill(U32 nPlayerID, std::string UID, S32 SkillID, bool& bScuessful);

	DATA_MAP GetDataMap(U32 nPlayerID);
	S32		 GetCardNum(U32 nPlayerID, S32 nLevel = 0);
	S32		 GetCardLevel(U32 nPlayerID, S32 CardID);
	bool CaculateCardStats(U32 nPlayerID, std::string UID, Stats& stats);//���㵥������
	bool CaculateStats(U32 nPlayerID, std::string UID, bool bNotify = true);//��������
	bool CaculateStats(U32 nPlayerID);//�������д�����
	bool CaculateEquipStats(U32 nPlayerID, std::string UID, U32 nEquipAddPro[MAX_EQUIP_SLOT]);//����װ������
	bool AddCardAchievement(U32 nPlayerID, std::string UID, S32 val, bool bAddItem = false, bool bNotify = true);//���ӹ�ѫ
	bool CanAddCardAchievement(U32 nPlayerID, std::string UID, S32 val);//���ӹ�ѫ
	//---------DB Operator---------//
	DB_Execution* LoadDB(U32 nPlayerID);
	bool DeleteDB(U32 nPlayerID, std::string UID);
	bool InsertDB(U32 nPlayerID, CardRef pCard);
	bool UpdateDB(U32 nPlayerID, CardRef pCard);
	//----------ͬ����ȡ-----------//
	DATA_MAP SyncLoadDB(U32 nPlayerID);

	//---------֪ͨ�ͻ���----------//
	void UpdateToClient(U32 nPlayerID, CardRef pCard);
	void SendInitToClient(U32 nPlayerID);
	static void Export(struct lua_State* L);

	//----------��ɫ��¼֪ͨ------//
	bool onPlayerEnter(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);
	bool onPlayerLeave(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);

	//----------��Ϣ����----------//
	bool HandleAddCardLevelRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleAddCardLevelLimitRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleAddCardSkillRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleAddCardEquipRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleRemoveCardEquipRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleAddCardAbilityRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleAddCardGrowRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleHireCardRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleDispatchCardRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleReadApplyRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleAddCardOfficalRankRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleDispatchCardAwardRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);

private:
	//�󳼼���
	S32 GetSkillFailedNumBySkillId(U64 SkillsFailedNum, S32 SkillId);
	void AddSkillFailedNumBySkillId(U64& SkillsFailedNum, S32 SkillId);
	void ClearSkillFailedNumBySkillId(U64& SkillsFailedNum, S32 SkillId);
	//������
	S32 GetGrowFailedNumByType(U32 GrowFialedNum, S32 Type);
	void AddGrowFailedNumByType(U32& GrowFialedNum, S32 Type);
	void ClearGrowFailedNumByType(U32& GrowFialedNum, S32 Type);

private:
	FATEPLAYERMAP m_FatePlayerMap;
};

#define CARDMGR CCardManager::Instance()
#endif//CARD_MANAGER_H
