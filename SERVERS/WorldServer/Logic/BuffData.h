#ifndef __BUFF_DATA__
#define __BUFF_DATA__
#include "Common/DataFile.h"
#include "BaseData.h"
#include <string.h>
#include <hash_map>
#include <memory>

struct CBuffData;
struct Stats;

class CBuffDataRes:public CBaseDataRes<CBuffData>
{

public:
	CBuffDataRes();
	virtual ~CBuffDataRes();

	bool read();
};

struct CBuffData
{
	CBuffData():m_BuffId(0), m_sDes(""), m_BuffType(0), m_BuffVal(0), m_BattleFlag(0)
	{};
	~CBuffData(){};

	void getStats(Stats& stats);

	inline bool IsAddBusinsessPer()			{ return m_BuffType == 1; }
	inline bool IsAddOfficerBusPer()		{ return m_BuffType == 2; }
	inline bool IsRedDisPatchPer()			{ return m_BuffType == 3; }
	inline bool IsAddAchievementPer()		{ return m_BuffType == 5; }
	inline bool IsRedMoneyPer()				{ return m_BuffType == 6; }
	inline bool IsAddOfficerTecPer()		{ return m_BuffType == 10; }
	inline bool IsAddTechnologyPer()		{ return m_BuffType == 11; }
	inline bool IsAddMilitaryPer()			{ return m_BuffType == 13; }
	inline bool IsAddOfficerMilPer()		{ return m_BuffType == 14; }
	inline bool IsAddAttackPer()			{ return m_BuffType == 15; }
	inline bool IsAddCriticalPer()			{ return m_BuffType == 16; }
	inline bool IsAddHpPer()				{ return m_BuffType == 17; }
	inline bool IsAddDodgePer()			    { return m_BuffType == 18; }
	inline bool IsEquipAddAgi()				{ return m_BuffType == 28; }
	inline bool IsEquipAddInt()				{ return m_BuffType == 29; }
	inline bool IsEquipAddStr()				{ return m_BuffType == 30; }
	inline bool IsRedCardLevelPer()			{ return m_BuffType == 34; }
	inline bool IsAddHp()					{ return m_BuffType == 35; }
	inline bool IsAddAttack()				{ return m_BuffType == 36; }


	U32 m_BuffId;
	std::string	m_sDes;
	U8  m_BuffType;
	F32	m_BuffVal;
	U8	m_BattleFlag;
};
extern CBuffDataRes* g_BuffDataMgr;

struct Stats
{
	F32 Bus_Pc;//��ҵ����Ч��1
	F32 OffBus_gPc;//��λ��ҵֵ����2
	F32 DevTime_Pc;//��������ʱ�����3
	F32 PvpHP_gPc;//����ʱѪ������4
	F32 Ach_Pc;//��ѫ���������5
	F32 LvCost_Pc;//������������6
	F32	Queen_gPc;	//���Ч������7
	F32 PvpAtk_gPc;//����ʱ�˺�����8
	F32 TecCost_gPc;//�о��Ƽ����ü���9
	F32 OffTec_gPc;//��λ��ҵֵ����10
	F32 Tec_Pc;//��ҵ����Ч��11
	F32 TecTime_gPc;//�о��Ƽ�ʱ�����12
	F32	Mil_Pc;//���¿���Ч������13
	F32	OffMil_gPc;//��λ����ֵ����14
	F32 Atk_gPc;//ʿ����������15
	F32 Cri_Pc;//ʿ����������16
	F32 HP_gPC;//ʿ��Ѫ������17
	F32 Dog_gPC;//�˺�����18
	F32 Skill[9];//����19
	F32 Agi;//���� 28
	F32 Int;//ѧʶ29
	F32 Str;//ͳ��30
	F32 Bus;//��ҵ31
	F32 Tec;//��ҵ32
	F32 Mil;//����33
	F32 ClvCost_gPc;//��������������34
	F32 SolHp;//ʿ��Ѫ��35
	F32 SolAtk;//ʿ������36
	F32 FlowerSuc_gPc;//������ɹ���37
	F32 Culture_gPc;//��������ֵ38
	F32 Rite_Pc;//ѧϰ����ֵ39
	F32 PBattle_Pc;//�����񹬶�ֵ40
	F32 FlowerSuc_Pc;//������ɹ���41
	F32 Charm;//����ֵ42
	F32	PrinceAbi_gPc;//��������ȫ��������43
	F32 DevCost_gPc;//������������44
	F32 AddMoney_gPc;//����Сʱ����45
	F32 AddOre_gPc;//��ʯСʱ����46
	F32 DevNum;//����������47
	F32 Bus_gPc;//������ҵЧ��48
	F32 Tec_gPc;//������ҵЧ��49
	F32 Agi_gPc;//�������д�����50
	F32 Ach_gPc;//�������д󳼹�ѫ���51
	F32 Land_gPc;//����������������52
	F32 AddRes_gPc;//������������ʯ��ʿ��{0}%Сʱ����53
	F32	StrongHold_gP;//����{ 0 }%���������54
	F32 CardNum;//����{ 0 }����ϯλ55
	F32 Str_gPc;////�������д�{0}%ͳ��56
	F32 OffRank;//���Ź�Ʒ57
	F32 TechNum;//����{0}���Ƽ�����58
	F32 ApplyNum;//����{0}����������59
	F32 Rite_gPc;//����{0}%��������ѧϰЧ��60
	F32 Int_gPc;//�������д�{0}%ѧʶ61
	F32 PrinceNum;//����{0}�����鷿����ϯλ62
	F32 Cri_gPc;//����ʿ��{0}%������	63
	F32 RedAch_gPc;//����ͻ�����Ĺ�ѫ����{0}%	64
	F32 CallTroop_gPc;//����{0}%��������	65
	F32 Mil_gPc;//����{0}%��������Ч��		66
	F32 BattleNum;//����1������λ		67
	F32 PvpAgi_gPc;//��ʱ��������     68
	F32 PvpInt_gPc;//��ʱѧʶ����     69
	F32 PvpStr_gPc;//��ʱ��������	  70
	F32 PvpHp_gPc;//�ظ����Ѫ��	  71
	F32 PvpStats_gPc;//����ȫ���Խ���	  72
	F32 Impose_gPc;//����������			  73
	F32 OrgPlayerNum;//�����������			74
	F32 OrgContribute_gPc;//���˾��׸��˹�������{0}%	75
	F32 OrgContributeExp_gPc;//���˾��׾�������{0}%s	76
	F32 OrgCharm;//���������������		77
	F32 OrgConquest_gPc; //�������������������{ 0 }%		78
	F32 OrgImposeMoney_gPc;//�������������������{0}%		79
	F32 OrgOre_gPc;//������ҿ�ʯ��������{0}%		80
	F32 OrgTroop_gPc;//�������������������{0}%		81
	F32 OrgManagerNum;//���˾�Ӣ��Ա����			82

	Stats();
	void Clear();

	Stats& operator+=(const Stats&);
	Stats& operator-=(const Stats&);
};

typedef std::shared_ptr<Stats> StatsRef;
extern Stats g_Stats;
#endif//__BUFF_DATA__