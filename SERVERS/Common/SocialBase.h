#ifndef SOCIAL_BASE_H
#define SOCIAL_BASE_H
#include "PlayerStructEx.h"
#include <map>

namespace SocialType
{
	enum Type
	{
		Temp,			// ��ʱ
		Friend,			// ����
		Consort,		// ��ż
		Master,			// ʦ��
		Prentice,		// ͽ��	
		Buddie,			// �ֵ�
		Enemy,			// ����
		Mute,			// ����(������)
		Engage,			// ����
		Divorce,		// ���

		Count,
	} ;

    static const char* strType[Count] = 
    {
        "��ʱ",
        "����",
        "����",
        "ʦ��",
        "ͽ��",
        "���",
        "����",
        "����",
		"����",
		"����",
    };
} ;

static int SocialTypeMaxCount[ SocialType::Count ] = 
{
	50,				// ��ʱ
	50,			    // ����
	1,				// ����
	1,				// ʦ��
	5,				// ͽ��
	5,				// ���
	20,				// ����
	100,			// ����
	1,              // ���� 
	1,				// ���
} ;

enum enSocialError
{
	SocialError_None,
	SocialError_Unknown,		// δ֪
	SocialError_Self,			// ���ܺ������Ϊ����ϵ
	SocialError_MaxCount,		// ������ϵ�����ﵽ�������
	SocialError_NotFound,		// Ŀ����Ҳ�����
	SocialError_Existed,		// Ŀ��������Ǵ�������ϵ�б���
	SocialError_Unallowed,      // �ò���������
	SocialError_DbError,        // ���ݿ��������
	
	SocialError_Count,
};

enum enFriendValueType
{
	FriendValue_Non,          // �����Ѻö�
	FriendValue_Kill,         // ɱ���Ѻö�
	FriendValue_Mission,      // �����Ѻö�
};

enum enQueryPlayerInfo
{
	QueryTypePlayer,            // ��ѯ�������
	QueryTypeFriend,            // ��ѯ��������
};

enum enLimintFriendValue
{
	LimintFriendValue_Kill = 300,    //ɱ������
	LimintFriendValue_Mission = 200, //��������
};

enum enSocialConstants
{
	AddFriendnotify,            // �Ӻ���֪ͨ
	AddFriendRequest,           // ����Ӻ���

	SOCAIL_ITEM_MAX = 300,		// ����ϵ�������ֵ
};


// �������ϵ�ṹ
struct stSocialItem
{
	U32					playerId;			// �Է����ID
	U8					type;				// SocialType
	U32					friendValue;		// ���Ѷ�
	stSocialItem() : playerId(0), type(0), friendValue(0)	{}

	template<class T>
	void WriteData(T* stream)
	{
		stream->writeBits(sizeof(stSocialItem)<<3,this);
	}

	template<class T>
	void ReadData(T* stream)
	{
		stream->readBits(sizeof(stSocialItem)<<3,this);
	}
};

// �������ϵ�ṹ
struct stSocialInfo
{
	U32		id;					// ���Id
	char	name[COMMON_STRING_LENGTH];			// ����
	U8		sex;				// �Ա�
	U8		level;				// �ȼ�
	U8		family;				// ����
	U8      job;                // ְҵ
	U8		faceImage;			// ͷ��
	U8		status;				// ״̬
	S32     teamId;             // ����Id
	U8		type;				// ��ϵ����	
	U32		friendValue;		// ���Ѷ�

	stSocialInfo()
	{
		memset( this, 0, sizeof( stSocialInfo ) );
	}

	stSocialInfo( const stSocialInfo& other )
	{
		copy( other );
	}

	stSocialInfo& operator = ( const stSocialInfo& other )
	{
		copy( other );
		return *this;
	}

	void copy( const stSocialInfo& other )
	{
		memcpy(this, &other, sizeof(stSocialInfo));
	}

	template<class T>
	void WriteData(T* stream)
	{
		stream->writeBits(sizeof(stSocialInfo)<<3,this);
	}

	template<class T>
	void ReadData(T* stream)
	{
		stream->readBits(sizeof(stSocialInfo)<<3,this);
	}
};

//��ʦͽ�ܽṹ
struct stLearnPrentice
{
	U32  playerId;                        // ���Id
	char playerName[MAX_NAME_LENGTH];     // �������
	U8   level;                           // �ȼ�
	U8   family;                          // ����
	U8   job;                             // ְҵ
	U8   status;                          // ״̬

	stLearnPrentice() {memset(this,0,sizeof(stLearnPrentice));}

	void copy(const stLearnPrentice &other)
	{
		memcpy(this, &other, sizeof(stLearnPrentice));
	}

	stLearnPrentice(const stLearnPrentice &other)
	{
		if(this != &other)
			copy(other);
	}

	stLearnPrentice& operator=(const stLearnPrentice &other)
	{
		if(this != &other)
			copy(other);

		return *this;
	}

	template<class T>
	void WriteData(T* stream)
	{
		stream->writeBits(sizeof(stLearnPrentice)<<3,this);
	}

	template<class T>
	void ReadData(T* stream)
	{
		stream->readBits(sizeof(stLearnPrentice)<<3,this);
	}

};

typedef std::pair<int,int> PlayerIdPair;
typedef std::map<PlayerIdPair, int> LimitMap;
typedef std::map<int,std::string>  SocialNameMap;
typedef std::map<int,stSocialItem> SocialItemMap;

inline bool isFriendType(U8 iType)
{
	if(iType != SocialType::Temp && iType != SocialType::Mute && iType != SocialType::Enemy)
		return true;
	else
		return false;
}

inline bool isBestFriendType(U8 iType)
{
	if(iType != SocialType::Friend && iType != SocialType::Temp && iType != SocialType::Mute && iType != SocialType::Enemy)
		return true;
	else
		return false;
}

inline bool hasMakeLink(U8 oldType,U8 newType)
{
	if(oldType == newType)
		return false;

	if (oldType != SocialType::Temp && newType == SocialType::Temp)
		return false;

	if (isBestFriendType(oldType) || isBestFriendType(newType)) 
		return false;

	if ((oldType == SocialType::Friend || oldType == SocialType::Mute) && newType == SocialType::Enemy)
		return false;

	return true;
}

struct ZoneClientSocial
{
	U32		masterID;				
	char	masterName[COMMON_STRING_LENGTH];
	U8		selfMasterLevel;

	ZoneClientSocial()
	{
		memset(this, 0, sizeof(ZoneClientSocial));
	}

	ZoneClientSocial(const ZoneClientSocial& other)
	{
		*this = other;
	}

	ZoneClientSocial& operator= (const ZoneClientSocial& other)
	{
		memcpy(this, &other, sizeof(ZoneClientSocial));
		return *this;
	}

	template<class T>
	void WriteData(T* stream)
	{
		stream->writeInt(masterID, Base::Bit32);
		stream->writeString(masterName, sizeof(masterName));
		stream->writeInt(selfMasterLevel, Base::Bit8);
	}

	template<class T>
	void ReadData(T* stream)
	{
		masterID = stream->readInt(Base::Bit32);
		stream->readString(masterName, sizeof(masterName));
		selfMasterLevel = stream->readInt(Base::Bit8);
	}
};

#endif