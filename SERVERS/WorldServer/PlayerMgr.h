#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <string>
#include <hash_map>
#include <hash_set>
#include <set>
#include <Time.h>

#include "Base/bitStream.h"
#include "Base/Locker.h"
#include "Common/PacketType.h"
#include "Common/PlayerStruct.h"
#include "Common/Tracable.h"
#include "Common/dbStruct.h"

#include "AccountHandler.h"
#include "LockCommon.h"
#include "PlayerCache.h"

#include "CommLib/DataCache.h"
#include "CommLib/SimpleTimer.h"
#include "CommLib/SimpleLocker.h"

class CDBOperator;

#define MAX_LINE_COUNT  3000

extern U32 g_maxLinePlayers;

//���������첽�߳�,����ȡ��ҵĻ�����Ϣ
class AnsyPlayerInfo
{
public:
    AnsyPlayerInfo(void)
    {
        memset(this,0,sizeof(AnsyPlayerInfo));
    }

    U32 playerId;
    U32 accountId;

    //��½gateʱsocket�����ڵ���
    U32 socket;
    U32 gateLineId;

    bool isInGame;
    char gmFlag;
};

#define MAX_ACCOUNT_WRITE 100000

//������������ʺ���Ϣ,�����߳�����������ļ�
struct AccoutWriteInfo
{
    char accountName[ACCOUNT_NAME_LENGTH];
    char playerName[MAX_NAME_LENGTH];
    U32  playerId;
    U32  accountId;
    int  state;
};

struct LoginUnit
{
	LoginUnit(U32 id,const char* n) : playerId(id),name(n ? n : "") {}

	std::string name;
	U32 playerId;
};

#define MAX_FAKE_PLAYER_COUNT 80000

#define MAX_LINE 100

/************************************************************************/
/* ����ǰ���е��ʺ�
/************************************************************************/
class CPlayerManager
{
public:
	CPlayerManager();
	~CPlayerManager();

    //��������ļ򵥽ṹ����Ԥ������һ����.�������ʱ�������㣬�������
    //ʱ��ֵ��ֹͣ����
    void LoadSimpePlayerDatas(int timeVal);

    //-----------------------------------------------------------------------------------------
    //�ṩ��һЩ�첽���߳�ȥ��ȡ��ҵ���Ϣ,��ʱ����ͨ��ֵ������,�����Ƕ������õ���ʽ.
    bool AnsyGetPlayerData(int PlayerId,SimplePlayerData& data);
    bool AnsyGetPlayerData(const char* name,SimplePlayerData& data);

    bool AnsyGetOnlinePlayer(int playerId,AnsyPlayerInfo& data);
    bool AnsyGetOnlinePlayer(const char* name,AnsyPlayerInfo& data);
    
    /************************************************************************/
    /* ����ʺŹ���
    /************************************************************************/
    ERROR_CODE AddAccount(Player& accountInfo, Players& playerList);

	void TimeProcess(U32 bExit);
    void Update(void);

	//��ѯ�����Ϣ,���Ƿ��ؼ򵥽ṹ
	PlayerSimpleDataRef GetPlayerData(int PlayerId);
	PlayerSimpleDataRef GetPlayerData(const char* name);
	std::string			GetPlayerName(int PlayerId);
	U32					GetFirstClass(U32 nPlayerID);
	U32					GetFamily(U32 nPlayerID);
	bool AddPlayerData(PlayerSimpleDataRef spData);

	bool IsPlayerOnline(int PlayerId);

	ERROR_CODE VerifyUID(T_UID UID,int AccountId);

	void SendSimplePacket( int nPlayerId, const char* cMessageType, int nDestZoneId = 0, int nSrcZoneId = 0 );

	int AddPlayerMap(AccountRef pPlayer,U32 playerId,const char* name);

	ERROR_CODE DeletePlayer(T_UID UID,int AccountId,int Playerid,bool isFromZone);

    //DB�������ɾ���ɹ�
    void OnPlayerDeleted(U32 accountId,U32 playerId);

	ERROR_CODE CreatePlayer(T_UID UID,int AccountId,const char *PlayerName,int Sex,int Body,int Face,int Hair,int HairCol,U32 ItemId,int FaceImage,int Family,int ZoneId,int Playerid);
    
	void RemoveAccount( int nAccountId );
	void RemovePlayer(U32 playerId,const char* name);

	AccountRef GetOnlinePlayer(int nPlayerId);
	AccountRef GetOnlinePlayer(const char* playerName);
	AccountRef GetAccount( int AccountId );
    AccountRef GetAccount( const char* accountName);

	void writeAccountInfo();

	void SendSimpleInfoPacket( int nPlayerId, const char * cMessageType, int nDestZoneId, int nSrcZoneId = 0 );

	void RegisterChat();

    bool HasOnlineAccount(U32 accountId);
    void AddOnlineAccount(U32 accountId);
    void DelOnlineAccount(U32 accountId);
    U32  GetOnlineAccountCount(void) const;
    
    void SetCreatedPlayerCount(U32 count);
    U32  GetCreatedPlayerCount(void);
   
    //�����Ҷ������Ѽ��ص�player����
    bool ClearPlayerData(U32 accountId);
	//-----------��Ҳ���---------------
	bool CanAddGold(int PlayerId, S32 Gold);
	bool AddGold(int PlayerId, S32 Gold, enOpType logType = LOG_GOLD_BUY);
	bool SetFace(int PlayerId, S32 Val);
	bool AddDrawGold(int PlayerId, S32 Gold);
	bool SetVipLv(int PlayerId, U32 VipLv);
	//----------------------------------
public:
    void OnNewDay();
    void UpdateLastDayTime();

	//��ҵĻ�����Ϣ
	typedef DataCache<PlayerSimpleDataRef,100000,1800> SPLAYER_CACHE;
	SPLAYER_CACHE m_simplePlayerDatas;
	
	//��ʱ(s)���ʺŷ�����������������
	SimpleTimer<30>   m_30Timer;
	SimpleTimer<60>   m_60Timer;

    //�Ѵ����������Ŀ
    U32 m_createdPlayerCount;

    typedef stdext::hash_map<int,AccountRef>			 HashAccountMap;
	typedef stdext::hash_map<std::string,AccountRef>	 HashPlayerNameMap;
    typedef stdext::hash_set<U32>                        OnlineAccountMap;
    typedef stdext::hash_map<std::string,int>            HashAccountNameMap;

	HashPlayerNameMap       m_PlayerNameMap;			//��¼��PlayerNameΪ��ֵ
	HashAccountMap          m_PlayerMap;				//��¼��PlayerIdΪ��ֵ
	HashAccountMap          m_AccountMap;			    //��¼���ʺ�Ϊ��ֵ
    HashAccountNameMap      m_accountNameMap;			//���ֶ�ӦaccountId
    
    //��¼�����ڽ�ɫѡ�����Ϸ�ڵ��ʺ�
    //����ͳ�Ƶ�ǰ�����������
    OnlineAccountMap m_onlineAccounts;

    volatile time_t m_lastUpdateTime;

    //ͳ�ƽ������ɴ��Ľ�ɫ����
    U32 m_playerEnterInterval;
    
    //����Ҫд�ļ����ʺ���Ϣȫ����������������ֵΪ10W���ʺ�
    //Ȼ������ṹ���ŵ��߳�����������ļ�
    bool m_canWriteAccout;
    AccoutWriteInfo m_accountWriteInfos[MAX_ACCOUNT_WRITE];
};

#endif
