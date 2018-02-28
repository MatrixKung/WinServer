#ifndef SOCIAL_MANAGER_H
#define SOCIAL_MANAGER_H

#include <map>
#include <deque>
#include "base/Locker.h"
#include "LockCommon.h"
#include "ManagerBase.h"
#include "AccountHandler.h"
#include "Common/SocialBase.h"

class CDBConnPool;
class CDBConn;

class CSocialManager : public CManagerBase< CSocialManager>
{
public:
    CSocialManager(CDBConnPool* pConnPool);
    ~CSocialManager(void);

    void ProcessEvents(int event,Base::BitStream& recv);
    void OnTimer();
    void OnQuit();
#ifdef _NTJ_UNITTEST
public:
#else
protected:
#endif
	typedef std::deque< int >       PlayerList;
	typedef stdext::hash_set<int>   QuitList;
	typedef std::map< int, U8 >     PlayerStatusMap;
	typedef std::map< int, SocialItemMap > SocialMap;

	enum enSex
	{
		SEX_BOTH	= 0,			//��Ҫ��
		SEX_MALE,					//����
		SEX_FEMALE,					//Ů��
	};

// ��������
protected:
	
	void             updateInfo                ();
	void             onPlayerEnter             (int playerId);                                        // ��������ϵ
	void             onPlayerQuit              (int playerId);                                        // ��������ϵ
	void			 sendSocial				   (int playerId,bool bFlag = true);                      // ���͵�ǰ�������ϵ
	void             destoryAllLink            (int playerId);                                        // ɾ�����й�����ϵ(�������ɾ��)
	void             cleanupSocialCache        (int playerId,int destPlayerId);                       // ���������һ�������
	void             changePlayerStatus        (int playerId,PlayerStatusType::Type status);          // ������״̬	
    bool             updateDb();
protected:
	enSocialError    makeLink                  (int playerId1,int playerId2,SocialType::Type type);   // ����
	enSocialError    destoryLink               (int playerId1,int playerId2);                         // ɾ��

	int              addFriendValue            (int playerId1,int playerId2,int value);           // ���Ӻ��Ѷ�
	int              addFriendValueV1          (int playerId1,int playerId2,int value);           // ��Ӵ�ּӺ��Ѷ�
	int              addFriendValueV2          (int playerId1,int playerId2,int value);           // ���������Ӻ��Ѷ�
	int              queryFriendValue          (int playerId1,int playerId2);                     // ��ѯ���Ѷ�(�øж�)
	bool             setFriendValue            (int playerId1,int playerId2,int value);           // ���ú��Ѷȣ��øжȣ�

	bool             buddyFriend               (int playerId1,int playerId2);                     // ��Ϊ����
	bool             hasFriend                 (int playerId1,int playerId2);                     // ���ں���
	bool             isPlayerBlackListed       (int playerId1,int playerId2);                     // playerId2�Ƿ���playerId1��������

	void			 TimeProcess			   (U32 bExit);
	bool             readDb                    (int playerId);
	bool             saveDb                    (int playerId);
	bool             saveDb                    (int playerId,const stSocialItem& info);
	bool             updateDb                  (int playerId,const stSocialItem& info);
	bool             updateDb                  (int playerId,const stSocialItem& info,int destType);
	bool             deleteSocial              (int playerId,int destPlayer);

	bool             isDataLoading             (int playerId);                // �Ƿ����������
	void             RemoveLotList             (int playerId,int sex = 0);    // �Ƴ���Ե��

	stSocialInfo     buildInfo                 (const stSocialItem& item,SimplePlayerData* data);      // ����������Ϣ
	void             notifyPlayerStatusChange  (int playerId,PlayerStatusType::Type status);                 // ������֪ͨ
	
	enSocialError    onNewAddPlayer            (int playerId1,int playerId2,SocialType::Type type);    // ����һ��
	bool             hasFriend                 (const SocialItemMap& itemMap,int destPlayerId);        // ���ں���ϵ
	bool             buddyFriend               (const SocialItemMap& itemMap,int destPlayerId);        // ��Ϊ����

	void             sendFriendInfo            (int playerId,int destPlayerId);                        // ����player��Ϣ��zoneServer
    
    void NotifyPlayerOffline (int playerId, stSocialItem& socialItem);
	void NotifyPlayerOnline  (int playerId, stSocialItem& socialItem);
// ��Ϣ���ִ���
protected:
	DECLARE_EVENT_FUNCTION(HandleMakeRequest);                            // ������ϵ
	DECLARE_EVENT_FUNCTION(HandleDestoryRequest);                         // �����ϵ
	DECLARE_EVENT_FUNCTION(HandleWorldSocialInfoRequest);             // ��������ϵ��
	/*
	DECLARE_EVENT_FUNCTION(HandleZoneWorldFindFriendRequest);             // ��ѯ������Ϣ
	DECLARE_EVENT_FUNCTION(HandleZoneWorldPlayerStatusChangeRequest);     // ����״̬
	DECLARE_EVENT_FUNCTION(HandleZoneWorldAddFriendValue);                // �Ӻ��Ѷ�
	DECLARE_EVENT_FUNCTION(HandleZoneWorldChangeLinkReuquest);            // �޸Ĺ�ϵ
	DECLARE_EVENT_FUNCTION(HandleZoneWorldFindPlayerRequest);             // �������
	DECLARE_EVENT_FUNCTION(HandleZoneAddFriendRequest);                   // ������Ҽ��Լ�
	DECLARE_EVENT_FUNCTION(HandleZoneUseItemAddFriendValueRequest);       // ʹ�õ��߼Ӻ��Ѷ�
	DECLARE_EVENT_FUNCTION(HandleZonePlayerListAddFriendRequest);         // ������ҼӺ��Ѷ�
	DECLARE_EVENT_FUNCTION(HandleZoneNotFriendClearRequest);              // �Ǻ�������ɾ��

	DECLARE_EVENT_FUNCTION(HandleZoneWorldMasterSocialRequest);           // ��ѯ���ʦͽ����
	DECLARE_EVENT_FUNCTION(HandleZoneWorldPlayerMasterSaveRequest);       // �����ʦ���
	DECLARE_EVENT_FUNCTION(HandleZoneQueryPrenticeListRequest);           // ��ѯ�ѳ�ʦͽ������


	//������Ե��
	//Ҫ��ȫ�����߽�ɫ����ѡһ���ֺ��ʵ����,Ȼ�����ѡ�����е�һλ���ض�������
	//�ص�:	(1)���ȷ���������
	//		(2)���ȷ�����ʵ���(�ߵȼ���Ұ������Ⱥ�)
	DECLARE_EVENT_FUNCTION( HandleZoneWorldLotRequest );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldAddLotRequest );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldRemoveLotRequest );*/

	void FindFriendRequest(Base::BitStream* Packet);

#ifdef _NTJ_UNITTEST
	public:
#else
protected:
#endif

	SocialMap            mSocialMap;          // ����ϵ
	PlayerStatusMap      mPlayerStatusMap;    // ���״̬

	LimitMap             mFriendValueMap;     // ���Ѷȸ���
	LimitMap	         mLimitTeam;          // ��ӼӺ��Ѷ�����
	LimitMap	         mLimitMission;       // ����Ӻ��Ѷ�����

	QuitList             mQuitList;           // �������

	PlayerList	         mMaleList;           // ������Ե��
	PlayerList           mFemaleList;         // Ů����Ե��

	int                  mCurrentDay;         // ��ǰ��

	SimpleTimer<60>      m_60Timer;
	CDBConn* m_pDBConn;
} ;

#endif /* SOCIAL_MANAGER_H */