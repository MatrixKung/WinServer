#ifndef TOPRANK_MANAGER_H
#define TOPRANK_MANAGER_H

#include "TopBase.h"
#include <set>
#include <unordered_map>

// ���а������
class CTopManager 
{
public:
	CTopManager();
	virtual ~CTopManager();

	static CTopManager* Instance();

	// db
	void LoadDB();
	bool UpdateDB(int type, TopRankRef pData);
	bool InsertDB(std::string UID, int type, TopRankRef pData);
	bool DeleteDB(std::string UID, int type);
	void DeleteDB(int type);
	void DeletePlayerFromDB(int playerId, int type);

	// �������а�,�����ĸ���Ҫ��ˢ�»򱣴浽���ݿ�
	void Update(U32 );
	int getRankForPlayer(int type, int playerId);      //���ڲ�ѯָ�����ָ���񵥵�����ֵ
	TopRankRef GetData(int type, std::string UID);
	TopRankRef GetData(int type, int playerId);
	
	void UpTop(int type, int playerId);

	void NewInData(int type, std::string UID, S32 Value0, S32 Value1 = 0, S32 Value2 = 0);
	void NewInData(int type, S32 nPlayerID, S32 Value0, S32 Value1 = 0, S32 Value2 = 0);
	void NewInDataEx(int type, S32 nPlayerID, S32 OldValue0, S32 Value0, S32 Value1 = 0, S32 Value2 = 0);
	std::vector<std::string> GetRank(int type, std::string UID, S32 nValue);
	std::vector<std::string> GetRank(int type, U32 start, U32 end);

	bool HandleOpenPveRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleOpenPveWinRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleOpenTopRankRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool HandleUpTopRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	bool __isDesc(int type);//��������
	void Clear(U32 type);
	void DelTop(std::string UID, int type = -1);       //ƥ�����idɾ����Ӧ�����а��ڴ�����,type==-1Ϊɾ��ȫ����
	static void Export(struct lua_State* L);
protected:
	void NewInData(int type, TopRankRef pData);//����Ԫ��
	//������а�����
	void InsertData(TopRankRef pData);
	bool isSpecial(int type);        //�Ƿ��������(������������)

	void DeleteOverdue(int iType, time_t curTime);;
	bool __isNeedOverDue(int type);//�Ƿ����

	void UpdateTop(int iType);
private:
	//������,���浱ǰ�����а�����
	typedef std::unordered_map<std::string, TopRankRef> TOPRANKITEMS;
	TOPRANKITEMS m_topRanks[eTopType_End];
};

#define TOPMGR CTopManager::Instance()
#endif//TOPRANK_MANAGER_H
