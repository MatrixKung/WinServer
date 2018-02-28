#ifndef HelloLua_BuffData_h
#define HelloLua_BuffData_h
#include "Common/DataFile.h"
#include <string.h>
#include <hash_map>
#include "../Utility/GameObject/Stats.h"

class CNpcData;
//��Ч1
enum enEffect
{
	Effect_None=0,
	Effect_Attack,
	Effect_MaxAttack,
	Effect_Defend,
	Effect_MaxDefend,
	Effect_HP,
	Effect_MaxHP,
	Effect_ResumeHP,
	Effect_Power,
	Effect_MaxPower,
	Effect_ConsumePower,
	Effect_FireHurt,
	Effect_IceHurt,
	Effect_EleHurt,
	Effect_SubFireHurt,
	Effect_SubIceHurt,
	Effect_SubEleHurt,
	Effect_ComAttHideOdds,
	Effect_FarAttHideOdds,
	Effect_HurtFix,
	Effect_FarAttFix,
	Effect_FarHurtFix,
	Effect_Speed,
	Effect_ExpGet,
	Effect_SkillGet,
	Effect_ViewArea,
	Effect_WeaponAttResume,
	Effect_HurtResume,
	Effect_HurtRaceAdd,
	Effect_HurtRaceSub,
	Effect_AttBeatBack,
	Effect_AddImprint,
	Effect_GoodsThrowDis,
	Effect_BagCap,
	Effect_EatFoodFix,
	Effect_MonsterAdd,
	Effect_MagicOdds,
	Effect_Max,
};

const static char* s_EffectName[Effect_Max]=
{
	"��",
	"����",
	"��󹥻�",
	"����",
	"������",
	"HP",
	"���HP",
	"�����ظ�HP",
	"����",
	"�������",
	"�ж���������",
	"���˺�",
	"���˺�",
	"���˺�",
	"������˺�",
	"������˺�",
	"������˺�",
	"��ͨ����������",
	"Զ������������",
	"�ܵ��˺�����",
	"Զ����������������",
	"�ܵ�Զ�������˺�����",
	"�ٶ�",
	"�����ȡ",
	"�����Ȼ�ȡ",
	"��Ұ��Χ",
	"ÿ�����������ظ�HP",
	"ÿ������˺��ظ�HP",
	"�������˺�����%",
	"�ܵ������˺�����%",
	"�ܵ���ͨ�����������˺�",
	"����ӡ��",
	"��ƷͶ������",
	"��������",
	"��ʳ��ʱ���ָ�����������",
	"�ٻ��Ĺ������Բ��������",
	"�ؼ���������"
};

const static U64 Effect1_PhyDefMax      = BIT64(0);//�޵�
const static U64 Effect1_ImmunitPhy     = BIT64(1);//���˺�
const static U64 Effect1_ImmunitSpl     = BIT64(2);//��ħ
const static U64 Effect1_Floating       = BIT64(3);//����
const static U64 Effect1_TrueSight      = BIT64(4);//����
const static U64 Effect1_Apperceive     = BIT64(5);//��֪
const static U64 Effect1_CrossAny       = BIT64(6);//����
const static U64 Effect1_NotDeath       = BIT64(7);//����
const static U64 Effect1_CantAffortAtk  = BIT64(8);//����
const static U64 Effect1_Deadly         = BIT64(9);//����
//BIT64(10) NO USE
const static U64 Effect1_BeStealed      = BIT64(11);//����
const static U64 Effect1_Dreaming       = BIT64(12);//�þ�
const static U64 Effect1_Forgeting      = BIT64(13);//����
const static U64 Effect1_Poisoning      = BIT64(14);//�ж�
const static U64 Effect1_WetBody        = BIT64(15);//ʪ��
const static U64 Effect1_FireBody       = BIT64(16);//����
const static U64 Effect1_FeelThunder    = BIT64(17);//�е�
const static U64 Effect1_Blind          = BIT64(18);//äĿ
const static U64 Effect1_Invisibility   = BIT64(19);//����
const static U64 Effect1_BeatBack       = BIT64(20);//����
const static U64 Effect1_Stun           = BIT64(21);//����
const static U64 Effect1_Space          = BIT64(22);//�ռ�
const static U64 Effect1_KnockDown      = BIT64(23);//����
const static U64 Effect1_Slumber        = BIT64(24);//˯��
const static U64 Effect1_Fear           = BIT64(25);//�־�
const static U64 Effect1_RunAway        = BIT64(26);//����
const static U64 Effect1_Confusion      = BIT64(27);//����
const static U64 Effect1_Sneer          = BIT64(28);//����
const static U64 Effect1_NoFeet         = BIT64(29);//����
const static U64 Effect1_NoHand         = BIT64(30);//����
const static U64 Effect1_NoEat          = BIT64(31);//����
const static U64 Effect1_NoSeeing       = BIT64(32);//����
const static U64 Effect1_NoEquip        = BIT64(33);//��е
const static U64 Effect1_Deformation    = BIT64(34);//����
const static U64 Effect1_Silence        = BIT64(35);//��Ĭ
const static U64 Effect1_Blurred        = BIT64(36);//�Ժ�


const static U64 Effect2_BreakTrap      = BIT64(0);//����
const static U64 Effect2_CutTree        = BIT64(1);//����
const static U64 Effect2_CutGrassland   = BIT64(2);//���
const static U64 Effect2_BreakWall      = BIT64(3);//��ǽ
const static U64 Effect2_Greedy         = BIT64(4);//����
const static U64 Effect2_CutMeat        = BIT64(5);//����
const static U64 Effect2_Curse          = BIT64(6);//����
const static U64 Effect2_Corrosion      = BIT64(7);//��ʴ
const static U64 Effect2_BreakEquip     = BIT64(8);//��
const static U64 Effect2_Camouflage     = BIT64(9);//αװ
const static U64 Effect2_Marked         = BIT64(10);//���
const static U64 Effect2_Bless          = BIT64(11);//ף��
const static U64 Effect2_DesAtk         = BIT64(12);//ħ��
const static U64 Effect2_NoStealEquip   = BIT64(13);//����
const static U64 Effect2_NoStealFood    = BIT64(14);//ħ��
const static U64 Effect2_NoStealGold    = BIT64(15);//����
const static U64 Effect2_NoBeatBackFly  = BIT64(16);//�˻�
const static U64 Effect2_NoConfusion    = BIT64(17);//���
const static U64 Effect2_NoWeak         = BIT64(18);//����
const static U64 Effect2_NoIgnoreDef    = BIT64(19);//����
const static U64 Effect2_NoSlumber      = BIT64(20);//��˯
const static U64 Effect2_NoFear         = BIT64(21);//���
const static U64 Effect2_NoStun         = BIT64(22);//���
const static U64 Effect2_NoKnockDown    = BIT64(23);//�⵹
const static U64 Effect2_NoDeformation  = BIT64(24);//���
const static U64 Effect2_Puncture       = BIT64(25);//�̻�
const static U64 Effect2_AtkRow         = BIT64(26);//��ն
const static U64 Effect2_AtkRound       = BIT64(27);//��ɨ
const static U64 Effect2_CrossByHp      = BIT64(28);//��Խ
const static U64 Effect2_Summon         = BIT64(29);//�ٻ�
const static U64 Effect2_NoDropEquip    = BIT64(30);//����
const static U64 Effect2_NoBreakEquip   = BIT64(31);//����
const static U64 Effect2_AutoIdentify   = BIT64(32);//����
const static U64 Effect2_SeeAnything    = BIT64(33);//����
const static U64 Effect2_ShopVip        = BIT64(34);//�߹�
const static U64 Effect2_BeAtked        = BIT64(35);//����
const static U64 Effect2_KillEnemy      = BIT64(36);//ɱ��
const static U64 Effect2_DoubleRound    = BIT64(37);//����
const static U64 Effect2_StopRound      = BIT64(38);//����

class CBuffData;

class CBuffDataRes {

public:
	enum {
		BUFF_BEGIN_ID   = 4000001,
		BUFF_END_ID     = 4999999,
	};
	typedef stdext::hash_map<S32,CBuffData*> BUFFDATAMAP;
	typedef BUFFDATAMAP::iterator BUFFDATA_ITR;
	CBuffDataRes();
	~CBuffDataRes();


	bool read();
	void close();
	void clear();
	void readFamily(Stats& stats, U32 family, bool bAdd = true);
	CBuffData* GetBuff(S32 BuffId);
private:
	BUFFDATAMAP m_DataMap;
};

class CBuffData
{
public:
	//Flags
	enum Flags
	{
		Flags_CalcStats			= BIT(0),		// �������ԣ���֮������˺�
		Flags_PhyDamage			= BIT(1),		// �����˺�
		Flags_HuoDamage         = BIT(2),		// ���˺�
		Flags_ShuiDamage		= BIT(3),		// ˮ�˺�
		Flags_JinDamage         = BIT(4),		// �繥��
		Flags_OtherDamage		= BIT(5),		// ��������
		Flags_Heal              = BIT(6),		// HP
		Flags_PlusStats         = BIT(7),       // �����ʶ
	};

	enum DamageType
	{
		Damage_Phy				= BIT(1),
		Damage_Huo				= BIT(2),
		Damage_Shui				= BIT(3),
		Damage_Jin              = BIT(4),
		Damage_Other			= BIT(5),
		Damage_Heal				= BIT(6),
		Damage_Spl				= Damage_Huo | Damage_Shui | Damage_Jin,
		Damage_All              = Damage_Phy | Damage_Spl  | Damage_Other | Damage_Heal,
	};
	//��ʼ�غ�
	CBuffData():m_BuffId(0),m_sName(""),m_sStateInfo(""), m_BuffEffect1(0),m_BuffEffect2(0),
		m_Stats(), m_LimitCount(1), m_LimitTimes(2), m_Flags(0), m_Group(0), \
		m_RemoveGroup(0), m_ImmunitGroup(0), m_LvUpExp(0)
	{
	}

	~CBuffData()
	{
	}

	void Plus(const CBuffData& _orig, const CBuffData& _plus);
	void PlusDone();
	void percentNpc(CNpcData* pData);

	inline U32 GetBuffId()          { return m_BuffId; };
	inline void Clear()             { memset(this, 0, sizeof(CBuffData)); };
	inline Stats& GetStats()        { return m_Stats; }
	inline bool	 IsFlags(U32 flags) { return m_Flags & flags; }
	// dinline bool
	inline U64	GetEffect1()         { return m_BuffEffect1; }
	inline U64	GetEffect2()         { return m_BuffEffect2; }
	inline bool isGroup(U32 group)   { return m_Group & group; }

	inline U32  GetDamageType()      {  return m_Flags & Damage_All; }
	const char* getName(){return m_sName.c_str();}
	Stats m_Stats;
	U32 m_BuffId;
	std::string m_sName;
	std::string m_sStateInfo; //״̬����
	U64 m_BuffEffect1;//״̬Ч��
	U64 m_BuffEffect2;//״̬Ч��
	U32 m_Flags; //״̬��ʶ(���⴦��״̬λ)
	U64 m_Group;//״̬��
	U64 m_RemoveGroup;//�Ƴ���
	U64 m_ImmunitGroup;//������
	U32 m_LimitCount;
	U32 m_LimitTimes;
	U64 m_LvUpExp;
};

extern CBuffData g_BuffData;

extern CBuffDataRes* g_BuffDataMgr;
#endif
