#ifndef _ACCOUNTHANDLER_H_
#define _ACCOUNTHANDLER_H_

#include <string>
#include <hash_map>
#include <set>
#include <memory>
#include <time.h>
#include "Common/TimerStateMachine.h"
#include "Common/PlayerStructEx.h"
#include "base/memPool.h"

#include <WinSock2.h>
#include <windows.h>
#include "WorldServer.h"
#include "CommLib/SimpleTimer.h"
#include "Common/AccountSMData.h"
#include "Common/Common.h"

typedef std::tr1::shared_ptr<SimplePlayerData>  PlayerSimpleDataRef;

#define PLAYER_TIMEOUT 400
#define QUERYACCOUNT_TIMEOUT 10*60 //10����

struct stPacketHead;
class  Base::BitStream;
class Player;

typedef std::tr1::shared_ptr<Player> AccountRef;
enum AccountStats
{
	ACCOUNT_BAND = BIT(0),//���
	ACCOUNT_BAND_CHAT = BIT(1),//����
};

/************************************************************************/
/* �ʺź��ʺŶ�Ӧ��ҵķ�װ
/************************************************************************/
class Player : public AccountSMData
{
    typedef AccountSMData Parent;
public:
	typedef std::vector<std::pair<S32, U32>> PAIRVEC;
	typedef PAIRVEC::iterator PAIRVECITR;
	//��Ҫ�첽ִ�е�,�����ó�������Ӧ�ı�־;
	enum PlayerEvent
	{
		EVENT_WAIT_PLAYERDATA	= BIT(0),	//�����������

		//��ǰ���ڵȴ�Zone���߷���
		EVENT_WAIT_KICKPLAYER	= BIT(1),

		//�ȴ���֤��
		EVENT_WAIT_CREATEPLAYER = BIT(2),	//�������
		EVENT_WAIT_ENTERGAME	= BIT(3),	//�ȴ���½��Ϸ
		EVENT_TRANSPORT			= BIT(4),	//���ڴ���

		//�ͻ��˵���
		EVENT_CLIENTLOST		= BIT(5),
        EVENT_LOGOUTED          = BIT(6),
	};

    //�ʺŵ�״̬,����״̬��
    enum Status
    {
        STATUS_IDEL              = 0, //����״̬
		STATUS_LOGIN,				  //�ʺŵ�¼
        STATUS_IN_SELECT,             //��ɫѡ��
        STATUS_IN_GAME,               //����Ϸ������,��������(���ߺ͵�ͼ����)
        STATUS_LOGOUT,				  //�ʺ��˳�,����һ��ʱ����л���offline״̬.��ɾ���ʺ�����
		STATUS_OFFLINE,

        STATUS_COUNT,
    };

public:
    Player();
	~Player(void);

	Player &operator=(const Player& Other)
    {
        LastTime		  = (int)time(NULL);
        isGM			  = Other.isGM;
        isAdult			  = Other.isAdult;
        UID				  = Other.UID;
        AccountId		  = Other.AccountId;
        mSocketId		  = Other.mSocketId;
        CurPlayerPos	  = Other.CurPlayerPos;
        PlayerNum		  = Other.PlayerNum;
        status            = Other.status;
		loginTime		  = Other.loginTime;
		logoutTime		  = Other.logoutTime;
        m_isFirstLoginGame= Other.m_isFirstLoginGame;
		loginType		  = Other.loginType;

        dStrcpy(AccountName,sizeof(AccountName),Other.AccountName);
        memcpy(m_playerIdList, Other.m_playerIdList, sizeof(m_playerIdList));

		dStrcpy(loginIP,sizeof(loginIP),Other.loginIP);
	
       
        return *this;
    }
    
	int  GetState(void)		   {return m_status.GetState();}

	void SetState(int state)    
    {
        AccountSMData::SetState(state);
        m_status.SetState(state);
    }
    
    void    SetGateSocketId(U32 id)     {mSocketId = id;}
    U32     GetGateSocketId(void) const {return mSocketId;}
   
    void    Update(U32 curTime);
    
    bool    IsOffline(void)  const {return m_status.GetState() == STATUS_OFFLINE;}
	bool    IsLogout(void)   const {return m_status.GetState() == STATUS_LOGOUT;}
    bool    IsInGame(void)   const {return m_status.GetState() == STATUS_IN_GAME;}

    bool    Kick(void);

    //���Ի�����Ŀǰ�ݶ�gmflgΪ200
    bool    IsRobot(void)  const {return (unsigned char)isGM == 200;}
    bool    IsGM(void)     const {return isGM != 0 && !IsRobot();}

	const char* GetPlayerName(void);
	const char* GetPlayerName(U32 playerId);

    U32     GetPlayerId(void) const {return m_playerId;}
	

	U32		GetPlayerCount(void) const;

	bool	VerifyUID(U32 id,int SocketHandle);

    bool    SetPlayerId(U32 playerId);

	//���ͽ�ɫ�б�
	bool	SendPlayerList(void);
    int     CheckDeletePlayer(U32 playerId,bool& canDeleted,int& deleteTime);
	void    DeletePlayer(U32 playerId);
	int     CreatePlayer(U32 playerId);

    void    SendPlayerDeleteRespond(U32 playerId,int error,int deleteTime);

	void	ResetLostTimer(void);
	U32		GetLogoutTime(void);

    void    UpdateLogoutTime(void);
    void    UpdateAccountLogoutTime(void);
	void	ClearAllSpPlayerData();

    //����������ҵĽ�ɫ�б�,spThis��ָ���Լ�������ָ��.�������ָ������ü��������Լ������
    int     ReloadPlayerList(Players& playerList, AccountRef spThis);

    void    SendClientErrorCode(Base::BitStream& sendPacket);

    void    SendGateLoginRespond(int error);

    void    SetFirstLoginFlag(bool isFirst) {m_isFirstLoginGame = isFirst;}

    U8      GetLevel(void);

    //�����Ƿ��������ҷ����߼�����,��Ҫ����Ϸ�ﲢ�Ҳ��ڴ��ͻ��������״̬��
    bool    CanSendLogicData(void);
	//----------------------------���������а�����------------------------------------------
	void ComeNewTopRank(int nType = -1);
	static void Export(struct lua_State* L);
	static Player getPlayer(U32 nPlayerId);

	bool isVaild();
	//----------------------------------------
public:
    //��ҵ�½��gate
    int		mSocketId;
    int     m_newPlayerId;//�½���ɫid

	CreatePlayerInfo m_createInfo;
    bool    m_isFirstLoginGame;

	typedef void(Player::*EVENT_CALLBACK)(int,int,stPacketHead*,Base::BitStream*);
	CTimerStateMachine<Player,STATUS_COUNT,EVENT_CALLBACK> m_status;
private:
    //��ʼ��״̬
    void    __InitStatus(void);
    
    void    __OnStateChangedHandler(void);

	void	__OnEnterLoginHandler(void);
    void    __OnEnterLogoutHandler(void);

    void    __OnEnterGameHandler(void);
	void	__OnLeaveGameHandler(void);

	void    __OnEnterSelectHandler(void);
	void	__OnLeaveSelectHandler(void);

	//���˼�ʱ��10(s)���һ��,�����ʱ��û�з���
	//���Զ����������Ϊ����״̬
	SimpleTimer<10> m_kickTimer;

	//��ҵ��ߺ�5�����е�����״̬
	SimpleTimer<30> m_lostTimer;

	//��ҵĶ���ı���ʱ��
	SimpleTimer<60> m_offlineTimer;
};

#endif /*_ACCOUNTHANDLER_H_*/
