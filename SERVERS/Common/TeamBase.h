#ifndef TEAM_BASE_H
#define TEAM_BASE_H

#define MAX_TEAM_MAP_MARK_COUNT	6
#define MAX_TEAM_TARGET_COUNT	9
#define MAX_TEAMMATE_NUM		6
#define MAX_TEAM_PENDING_COUNT	10
#define MAX_TEAM_NAME_LENGTH	40

#if defined(NTJ_SERVER) || defined(NTJ_CLIENT) || defined(NTJ_EDITOR)
#include "platform/types.h"
#else
#include "base/bitStream.h"
#endif

//#include "base/bitStream.h"
//
//#ifndef TCP_SERVER
//	#include "core/bitStream.h"
//#endif

//#ifdef NTJ_SERVER
//	#ifndef BASE_BITSTREAM
//		#include "core/bitStream.h"
//		#undef BITSTREAM
//		#define BITSTREAM BitStream
//	#else
//		#define BITSTREAM Base::BitStream
//	#endif
//#else
//	#ifndef BITSTREAM
//		#define BITSTREAM Base::BitStream
//	#endif
//#endif
//
//#ifdef NTJ_CLIENT
//#ifndef BASE_BITSTREAM
//#include "core/bitStream.h"
//#undef BITSTREAM
//#define BITSTREAM BitStream
//#else
//#define BITSTREAM Base::BitStream
//#endif
//#else
//#ifndef BITSTREAM
//#define BITSTREAM Base::BitStream
//#endif
//#endif
//
//#ifdef NTJ_EDITOR
//#ifndef BASE_BITSTREAM
//#include "core/bitStream.h"
//#undef  BITSTRAM
//#define BITSTREAM BitStream
//#else
//#define BITSTREAM Base::BitStream
//#endif
//#else
//#ifndef BITSTREAM
//#define BITSTREAM Base::BitStream
//#endif
//#endif

enum
{
	TEAM_ACTION_BUILD,		// ���
	TEAM_ACTION_JOIN,		// �������������
	TEAM_ACTION_ADD,		// �ӳ���������
	TEAM_ACTION_TURNOFF,    // ��ӿ��عر�

	TEAM_OVERTIME = 90,  // �����������볬ʱ���1��30��
};

enum TEAM_ERROR
{
	TEAM_NONE_ERROR,
	TEAM_NOT_TEAMMATE,					//���Ƕ���
	TEAM_ALREADY_TEAMMATE,				//�Ѿ��Ƕ���
	TEAM_NOT_CAPTION,					//���Ƕӳ�
	TEAM_NO_TEAM,						//û�ж���
	TEAM_SWITCH_DISABLE,				//�ر���ӿ���
	TEAM_SELF_ERROR,					//���ܶ��Լ����
	TEAM_HAVE_TEAM,						//�Ѿ��ж�����
	TEAM_NOTIFY_LIST_FULL,				//������Ϣ�б���
	TEAM_TEAMMATE_FULL,					//��������
	TEAM_JOIN_REQUEST_EXIST,            //�����Ѵ���
	TEAM_ADD_REQUEST_EXIST,             //�����Ѵ���
	TEAM_PLAYER_OFFLINE,                //��Ҳ����߻򲻴���
	TEAM_ADD_INVALID,                   //�����ѹ���
	TEAM_JOIN_INVALID,                  //�����ѹ���
	TEAM_RAND_ERROR,					//�����߻��߱��������Ѿ��������������
	TEAM_UNKNOW_ERROR,					//δ֪����
	TEAM_ERROR_COUNT
};

struct stTeamMapMark
{
	short sMapId;
	short sX;
	short sY;

	stTeamMapMark()
	{
		memset( this, 0, sizeof( stTeamMapMark ) );
	}

	template<class T>
	void WritePacket( T* pPacket )
	{
		pPacket->writeInt( sMapId, 16 );
		pPacket->writeInt( sX, 16 );
		pPacket->writeInt( sY, 16 );
	}

	template<class T>
	void ReadPacket( T* pPacket )
	{
		sMapId = pPacket->readInt( 16 );
		sX = pPacket->readInt( 16 );
		sY = pPacket->readInt( 16 );
	}
};

struct stTeamTargetMark
{
	short sMapId;
	short sTargetId;

	stTeamTargetMark()
	{
		memset( this, 0, sizeof( stTeamTargetMark ) );
	}

	template<class T>
	void WritePacket( T* pPacket )
	{
		pPacket->writeInt( sMapId, 16 );
		pPacket->writeInt( sTargetId, 16 );
	}

	template<class T>
	void ReadPacket( T* pPacket )
	{
		sMapId = pPacket->readInt( 16 );
		sTargetId = pPacket->readInt( 16 );
	}
};

// ���˵Ķ�����Ϣ, �ݶ�,�����...
// 
struct stTeamInfo
{
	bool	m_bInTeam;
	int		m_nId;
	char	m_szName[MAX_TEAM_NAME_LENGTH];
	bool	m_bIsCaption;
	int		m_nCopymap;
	bool    m_bRandTeam;
	bool    m_bTeamFollow;
	F32     m_fTeamSpeed;

	stTeamInfo()
	{
		memset(this, 0, sizeof(stTeamInfo));
	}

	template<class T>
	void WritePacket(T* pPacket)
	{
		if (pPacket->writeFlag(m_bInTeam))
		{
			pPacket->writeInt(m_nId, Base::Bit32);
			pPacket->writeString(m_szName ,MAX_TEAM_NAME_LENGTH);
			pPacket->writeFlag(m_bIsCaption);
			pPacket->writeInt(m_nCopymap, Base::Bit32);
			pPacket->writeFlag(m_bRandTeam);
			pPacket->writeFlag(m_bTeamFollow);
			pPacket->write(m_fTeamSpeed);
		}
	}

	template<class T>
	void ReadPacket(T* pPacket)
	{
		m_bInTeam = pPacket->readFlag();
		if (m_bInTeam)
		{
			m_nId = pPacket->readInt(Base::Bit32);
			pPacket->readString(m_szName, MAX_TEAM_NAME_LENGTH);
			m_bIsCaption = pPacket->readFlag();
			m_nCopymap = pPacket->readInt(Base::Bit32);
			m_bRandTeam = pPacket->readFlag();
			m_bTeamFollow = pPacket->readFlag();
			pPacket->read(&m_fTeamSpeed);
		}	
		else
		{
			m_nId = 0;
			*m_szName = 0;
			m_bIsCaption = 0;
			m_nCopymap = 0;
			m_bRandTeam = 0;
			m_bTeamFollow = false;
			m_fTeamSpeed = 0;
		}
	}
} ;

#include <map>
struct PlayerAbilityVal
{
	U32 nDefenceVal;
	U32 nDoctorVal;
	U32 nAttackerVal;
};
typedef std::map<U32,PlayerAbilityVal> MAP_ABILITYVAL;

struct stZoneLineInfo
{
	int nZoneId;
	int nLineId;
	int nTriggerId;
	stZoneLineInfo()
	{
		nZoneId = 0;
		nLineId = 0;
		nTriggerId = 0;
	}
	stZoneLineInfo(int zoneId,int triggerId,int lineId)
	{
		nZoneId = zoneId;
		nTriggerId = triggerId;
		nLineId = lineId;
	}
};
typedef std::map<U32,stZoneLineInfo> MAP_PLAYERLINEINFO;

//��Ҫͬ���ģ�����ֻ���ڶ���֪������Ϣ��
struct OtherTeamInfo
{
	float speed;

	OtherTeamInfo()
	{
		Clear();
	}

	OtherTeamInfo(const OtherTeamInfo& other)
	{
		*this = other;
	}

	OtherTeamInfo& operator=(const OtherTeamInfo& other)
	{
		memcpy(this, &other, sizeof(OtherTeamInfo));
		return *this;
	}

	void Clear()
	{
		memset(this, 0, sizeof(OtherTeamInfo));
		speed = 4.5f;
	}

	template<class T>
	void WritePacket(T* pPacket)
	{
		pPacket->writeBits(sizeof(OtherTeamInfo)<<3, this);
	}

	template<class T>
	void ReadPacket(T* pPacket)
	{
		pPacket->readBits(sizeof(OtherTeamInfo)<<3, this);
	}
};

struct TeamMemberInfo
{
	int playerID;
	bool follow;
	bool attack;

	TeamMemberInfo()
	{
		Clear();
	}

	TeamMemberInfo(const TeamMemberInfo& other)
	{
		*this = other;
	}

	TeamMemberInfo& operator=(const TeamMemberInfo& other)
	{
		memcpy(this, &other, sizeof(TeamMemberInfo));
		return *this;
	}

	void Clear()
	{
		memset(this, 0, sizeof(TeamMemberInfo));
	}

	template<class T>
	void WritePacket(T* pPacket)
	{
		pPacket->writeBits(sizeof(TeamMemberInfo)<<3, this);
	}

	template<class T>
	void ReadPacket(T* pPacket)
	{
		pPacket->readBits(sizeof(TeamMemberInfo)<<3, this);
	}
};

#endif