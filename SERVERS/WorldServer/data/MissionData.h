//
//  NpcData.h
//  HelloLua
//
//  Created by BaoGuo on 14-1-25.
//
//
#ifndef HelloLua_MissionData_h
#define HelloLua_MissionData_h
#include "Common/DataFile.h"
#include <string.h>
#include <hash_map>
#include <set>

class CMissionData;
class CMissionDataRes
{
public:
	typedef stdext::hash_multimap<S32, S32> MISSIONIDMAP;
	typedef MISSIONIDMAP::iterator MISSIONIDITR;

	enum{
		MISSION_BEGIN_ID = 700001,
		MISSION_END_ID   = 799999,
	};

	typedef stdext::hash_map<S32, CMissionData*> MISSIONDATAMAP;
	typedef MISSIONDATAMAP::iterator MISSIONDATAITR;
	CMissionDataRes();
	~CMissionDataRes();
	bool read();
	void close();
	void clear();
	CMissionData* getMissionData(S32 iMissionId);
	static CMissionData* getMissionDataEx(S32 iMissionId);
	//��ȡ�¸�����
	bool  getNextMissionId(S32 iPreMissionId, std::set<S32>& MissionSet);

private:
	MISSIONDATAMAP m_MissionDataMap;
	MISSIONIDMAP   m_MissisonIdMap;
};

class CMissionData
{
public:
	enum enMissionCategory
	{
		MISSION_CATEGORY_PLOT           =  1,//����
		MISSION_CATEGORY_ACTIVITY       =  2,//�
		MISSION_CATEGORY_GROWUP         =  3,//�ɳ�
		MISSION_CATEGORY_GATE           =  4,//�ؿ�
		MISSION_CATEGORY_COMBAT         =  5,//ս��
		MISSION_CATEGORY_ITEM           =  6,//��Ʒ
		MISSION_CATEGORY_ORG            =  7,//����
		MISSION_CATEGORY_DAILY          =  8,//�ճ�
		MISSION_CATEGORY_LOGIN          =  9,//����
		MISSION_CATEGORY_BANG			=  10,//���
	};

	enum enSubMissionCategory
	{
		MISSION_SUBCATEGORY_PRINCIPAL_LINE      = 101,
		MISSION_SUBCATEGORY_BRANCH_LINE         = 102,
		MISSION_SUBCATEGORY_SPECIAL             = 103,
		MISSION_SUBCATEGORY_TIMELIMIT           = 201,
		MISSION_SUBCATEGORY_COUNTLIMIT          = 202,
		MISSION_SUBCATEGORY_LEVELLIMIT          = 203,
		MISSION_SUBCATEGORY_LEVEL               = 301,
		MISSION_SUBCATEGORY_PRO                 = 302,
		MISSION_SUBCATEGORY_NEWPALYERGATE       = 401,
		MISSION_SUBCATEGORY_MISSIONNAME         = 402,
		MISSION_SUBCATEGORY_YUEYOUJING			= 403,//���ľ�
		MISSION_SUBCATEGORY_LUOXIYUAN			= 404,//��ϦԨ
		MISSION_SUBCATEGORY_SHENMULIN			= 405,//��ľ��
		MISSION_SUBCATEGORY_SHUANGSHENGSHAN		= 406,//˫��ɽ
		MISSION_SUBCATEGORY_XITIAN				= 407,//����
		MISSION_SUBCATEGORY_PENGLAI				= 408,//����
		MISSION_SUBCATEGORY_BINGCHI				= 410,//���α���
		MISSION_SUBCATEGORY_KUNLUN				= 411,//���ع���
		MISSION_SUBCATEGORY_WANSHULOU			= 412,//����¥
		MISSION_SUBCATEGORY_TIANSHUTA			= 413,//������
		MISSION_SUBCATEGORY_DOUXIAN				= 414,//��������
		MISSION_SUBCATEGORY_XINMO				= 415,//��ħ����
		MISSION_SUBCATEGORY_TIANGUAN			= 416,//�޼����
		MISSION_SUBCATEGORY_DUOBAO				= 417,//�����ᱦ
		MISSION_SUBCATEGORY_WANLING				= 418,//����֮ս
		MISSION_SUBCATEGORY_BAIHUA				= 419,//�ٻ��ؾ�
		MISSION_SUBCATEGORY_YUNXIANZHEN 		= 420,//������
		MISSION_SUBCATEGORY_LONGWNAG			= 421,//��������
		MISSION_SUBCATEGORY_JIUXIAN				= 422,//�����Ծ�
		MISSION_SUBCATEGORY_YZCHENGBAO			= 423,//���гǱ�
		MISSION_SUBCATEGORY_XHQIJU				= 424,//������
		MISSION_SUBCATEGORY_TMSHILIAN		    = 425,//��������
		MISSION_SUBCATEGORY_DNTIANGONG			= 426,//�����칬
		MISSION_SUBCATEGORY_HAOTIANTA			= 427, //�����
		MISSION_SUBCATEGORY_JGQINGYUAN			= 428,//�Ź���Ե
		MISSION_SUBCATEGORY_KILLNUM             = 501,
		MISSION_SUBCATEGORY_COLLECT             = 601,
		MISSION_SUBCATEGORY_HECHENG				= 602,//�ϳ�
		MISSION_SUBCATEGORY_LEVEL_ORG           = 701,
		MISSION_SUBCATEGORY_TIMELIMIT_DIALY     = 801,
		MISSION_SUBCATEGORY_COUNTLIMIT_DIALY    = 802,
		MISSION_SUBCATEGORY_LEVELLIMIT_DIALY    = 803,
		MISSION_SUBCATEGORY_ZHANDOU				= 804,//ս��
		MISSION_SUBCATEGORY_LOGIN_ONE           = 901,
		MISSION_SUBCATEGORY_LOGIN_TWO           = 902,
		MISSION_SUBCATEGORY_LOGIN_THREE         = 903,
		MISSION_SUBCATEGORY_LOGIN_FOUR          = 904,
		MISSION_SUBCATEGORY_LOGIN_FIVE          = 905,
		MISSION_SUBCATEGORY_LOGIN_SIX           = 906,
		MISSION_SUBCATEGORY_LOGIN_SEVEN         = 907,
		MISSION_SUBCATEGORY_JIUZHU				= 1001, //����
		MISSION_SUBCATEGORY_BEIJIU				= 1002,//����
		MISSION_SUBCATEGORY_ZHUZHAN				= 1003,//��ս
	};

	enum enCompleteType
	{
		COMPLETE_TYPE_LOGIN                     = 1,
		COMPLETE_TYPE_KILL                      = 2,
		COMPLETE_TYPE_GETITEM                   = 3,
		COMPLETE_TYPE_USE                       = 4,
		COMPLETE_TYPE_STRENGTHEN                = 5,
		COMPLETE_TYPE_COMPOSE                   = 6,
		COMPLETE_TYPE_JIUREN					= 7,//����
		COMPLETE_TYPE_BEIJIU					= 8,//����
		COMPLETE_TYPE_HELPBEAT					= 9,//������ս
		COMPLETE_TYPE_CHUANGGUAN				= 10,//����
		COMPLETE_TYPE_XIOAFEI					= 11,//����
		COMPLETE_TYPE_KILL_MAN                  = 101,
		COMPLETE_TYPE_KILL_SHENG                = 102,
		COMPLETE_TYPE_KILL_FO                   = 103,
		COMPLETE_TYPE_KILL_XIAN                 = 104,
		COMPLETE_TYPE_KILL_JIN                  = 105,
		COMPLETE_TYPE_KILL_GUI                  = 106,
		COMPLETE_TYPE_KILL_GUAI                 = 107,
		COMPLETE_TYPE_KILL_YAO                  = 108,
		COMPLETE_TYPE_KILL_MO                   = 109,
		COMPLETE_TYPE_KILL_SHOU                 = 110,
		COMPLETE_TYPE_KILL_LONG                 = 111,
		COMPLETE_TYPE_KILL_NORMAL               = 201,
		COMPLETE_TYPE_KILL_ELITE                = 202,
		COMPLETE_TYPE_KILL_BOSS                 = 203,
		COMPLETE_TYPE_KILL_SPECIAL_MONSTER		= 301,//ɱ���ƶ�����
		COMPLETE_TYPE_LEVEL_UP					= 401,//�ȼ�����
		COMPLETE_TYPE_ATKVALUE_GOTO				= 402,//����ֵ�ﵽ
		COMPLETE_TYPE_HUJIAVALUE_GOTO			= 403,//����ֵ�ﵽ
	};

	enum enTimeSetInt
	{
		INTERVAL_DAY							= 1,//ÿ��
		INTERVAL_WEEK							= 2,//ÿ��
		INTERVAL_MONTH							= 3,//ÿ��
		INTERVAL_YEAR							= 4,//ÿ��
		INTERVAL_LOOP							= 5,//ѭ��
		INTERVAL_NEVER							= 6,//����
		INTERVAL_LIANXU							= 7 //����
	};


	CMissionData():m_MissionId(0), m_Category(0), m_SubCategory(0), m_PreMissionId(0), m_MissionName(""), m_RecLevel(0), m_SubLevel(0), \
		m_WeekDay(0), m_BeginTime(""), m_LastTime(0), m_RecNpcId(0), m_SubNpcId(0), m_MissionType(0), m_TargetId(0), m_NeedNum(0),\
		m_AwardExp(0), m_AwardGold(0),m_AwardItemId(0), m_AwardMoney(0), m_Interval(0), m_MissionDesc(""), m_Points(0),m_AwardScore(0)
	{
	}
	~CMissionData() {};
	S32 m_MissionId;
	U16 m_Category;//����
	U16 m_SubCategory;//����
	S32 m_PreMissionId; //ǰ������
	std::string m_MissionName; //����
	U32 m_RecLevel;//������ȼ�
	U32 m_SubLevel;//������ȼ�
	U32 m_WeekDay;//����
	std::string m_BeginTime;//����ʱ��
	U32 m_LastTime;//����ʱ��
	U32 m_RecNpcId; //��ʼNPC
	U32 m_SubNpcId; //����NPC
	U16 m_MissionType;//�������
	S32 m_TargetId;//Ŀ��
	S32 m_NeedNum;//����
	S32 m_AwardExp; //����
	S32 m_AwardMoney; //��Ԫ
	S32 m_AwardGold;//Ԫ��
	S32 m_Points;// ����
	
	S32 m_AwardItemId;//����
	S32 m_AwardScore;//�ɾ͵�
	U16 m_Interval;//����
	std::string m_MissionDesc; //����

	void printAllEmlem();
};

extern CMissionDataRes* g_MissionDataMgr;
#endif
