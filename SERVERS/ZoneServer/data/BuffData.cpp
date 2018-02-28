//
//  BuffData.cpp
//  HelloLua
//
//  Created by BaoGuo on 14-2-9.
//
//

#include "BuffData.h"
#include "NpcData.h"
#include "Common/OrgBase.h"
CBuffData g_BuffData;
CBuffDataRes *g_BuffDataMgr=new CBuffDataRes();


void CBuffData::Plus(const CBuffData& _orig, const CBuffData& _plus)
{
	m_Stats.Plus(_orig.m_Stats, _plus.m_Stats);
	m_LimitTimes = _orig.m_LimitTimes + _plus.m_LimitTimes;
	m_LimitCount = _orig.m_LimitCount + _plus.m_LimitCount;
}

void CBuffData::PlusDone()
{
	m_Stats.PlusDone();
}

void CBuffData::percentNpc(CNpcData* pData)
{
	m_Stats.percentNpc(pData);
}

CBuffDataRes::CBuffDataRes()
{

}

CBuffDataRes::~CBuffDataRes()
{
	clear();
}

void CBuffDataRes::readFamily(Stats& stats, U32 family, bool bAdd /*= true*/)
{
	enFamily enFam = (enFamily)family;
	if(Family_Sheng == enFam && bAdd)
	{
		stats.Sheng_Damage_gPc += 0.5;
	}
	else if(Family_Sheng == enFam && !bAdd)
	{
		stats.Sheng_Damage_gPc -= 0.5;
	}

	if(Family_Fo == enFam && bAdd)
	{
		stats.Fo_Damage_gPc += 0.5;
	}
	else if(Family_Fo == enFam && !bAdd)
	{
		stats.Fo_Damage_gPc -= 0.5;
	}

	if(Family_Xian == enFam && bAdd)
	{
		stats.Xian_Damage_gPc += 0.5;
	}
	else if(Family_Xian == enFam && !bAdd)
	{
		stats.Xian_Damage_gPc -= 0.5;
	}

	if(Family_Jing == enFam && bAdd)
	{
		stats.Jing_Damage_gPc += 0.5;
	}
	else if(Family_Jing == enFam && !bAdd)
	{
		stats.Jing_Damage_gPc -= 0.5;
	}

	if(Family_Gui == enFam && bAdd)
	{
		stats.Gui_Damage_gPc += 0.5;
	}
	else if(Family_Gui == enFam && !bAdd)
	{
		stats.Gui_Damage_gPc -= 0.5;
	}

	if(Family_Guai == enFam && bAdd)
	{
		stats.Guai_Damage_gPc += 0.5;
	}
	else if(Family_Guai == enFam && !bAdd)
	{
		stats.Guai_Damage_gPc -= 0.5;
	}

	if(Family_Yao == enFam && bAdd)
	{
		stats.Yao_Damage_gPc += 0.5;
	}
	else if(Family_Yao == enFam && !bAdd)
	{
		stats.Yao_Damage_gPc -= 0.5;
	}

	if(Family_Mo == enFam && bAdd)
	{
		stats.Mo_Damage_gPc += 0.5;
	}
	else if(Family_Mo == enFam && !bAdd)
	{
		stats.Mo_Damage_gPc -= 0.5;
	}

	if(Family_Shou == enFam && bAdd)
	{
		stats.Shou_Damage_gPc += 0.5;
	}
	else if(Family_Shou == enFam && !bAdd)
	{
		stats.Shou_Damage_gPc -= 0.5;
	}

	if(Family_Long == enFam && bAdd)
	{
		stats.Long_Damage_gPc += 0.5;
	}
	else if(Family_Long == enFam && !bAdd)
	{
		stats.Long_Damage_gPc -= 0.5;
	}
}

bool CBuffDataRes::read()
{
	CDataFile file;
	RData LineData;


	if (!file.readDataFile("Data_4_Buff.dat")) {
		JUST_ASSERT("read Buffdata error");
		return false;
	}
	for (int i=0; i<file.RecordNum; i++) {
		CBuffData* pdata=new CBuffData();

		file.GetData(LineData);
		ASSERT(LineData.m_Type==DType_S32, "read Buffdata id error");
		pdata->m_BuffId=LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type==DType_string, "read buffdata name error");
		pdata->m_sName=LineData.m_string;

		file.GetData(LineData);
		ASSERT(LineData.m_Type==DType_string, "read buffdata state des error");
		pdata->m_sStateInfo=LineData.m_string;

		file.GetData(LineData);
		ASSERT(LineData.m_Type==DType_F64,"read buffdata esp magic1 error");
		pdata->m_BuffEffect1=LineData.m_F64;

		file.GetData(LineData);
		ASSERT(LineData.m_Type==DType_F64, "read buffdata esp magic2 error");
		pdata->m_BuffEffect2=LineData.m_F64;

		file.GetData(LineData);
		ASSERT(LineData.m_Type==DType_S32, "read buffdata ״̬��ʶ error");
		pdata->m_Flags=LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type==DType_F64, "read buffdata �� error");
		pdata->m_Group=LineData.m_F64;

		file.GetData(LineData);
		ASSERT(LineData.m_Type==DType_F64, "read buffdata �Ƴ��� error");
		pdata->m_RemoveGroup=LineData.m_F64;

		file.GetData(LineData);
		ASSERT(LineData.m_Type==DType_F64, "read buffdata ������error");
		pdata->m_ImmunitGroup=LineData.m_F64;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ��Ұ error");
		pdata->m_Stats.See = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ���� error");
		pdata->m_Stats.HP = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �����ٷֱ� error");
		pdata->m_Stats.HP_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ������� error");
		pdata->m_Stats.MaxHP = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata ��������ٷֱ� error");
		pdata->m_Stats.MaxHP_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ÿ�غϻ�Ѫ error");
		pdata->m_Stats.RecoverHP = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �������ÿ�غϻ�Ѫ�ٷֱ� error");
		pdata->m_Stats.RecoverHp_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ������Ѫ error");
		pdata->m_Stats.DeriveHP = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ���� error");
		pdata->m_Stats.Str = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �����ٷֱ� error");
		pdata->m_Stats.Str_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ������� error");
		pdata->m_Stats.MaxStr = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata ��������ٷֱ� error");
		pdata->m_Stats.MaxStr_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ÿ�غ��������� error");
		pdata->m_Stats.ReduceStr = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �������� error");
		pdata->m_Stats.Cri_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �չ����ܼ��� error");
		pdata->m_Stats.Doe_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata Զ�����������ܼ��� error");
		pdata->m_Stats.DoeR_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �ٶ� error");
		pdata->m_Stats.Spe = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ������ error");
		pdata->m_Stats.PhyDamage = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �������ٷֱ� error");
		pdata->m_Stats.PhyDamage_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ˮ���� error");
		pdata->m_Stats.ShuiDamage = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata ˮ�����ٷֱ� error");
		pdata->m_Stats.ShuiDamage_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata �𹥻�error");
		pdata->m_Stats.HuoDamage = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �𹥻��ٷֱ� error");
		pdata->m_Stats.HuoDamage_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata �繥��error");
		pdata->m_Stats.JinDamage = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �繥���ٷֱ� error");
		pdata->m_Stats.JinDamage_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ������� error");
		pdata->m_Stats.PhyDefence = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata ��������ٷֱ� error");
		pdata->m_Stats.PhyDefence_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata �����˺�����error");
		pdata->m_Stats.PhyImmunit = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �����˺�����ٷֱ� error");
		pdata->m_Stats.PhyImmunit_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ˮ�˺�����error");
		pdata->m_Stats.ShuiDefence = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata ˮ�˺�����ٷֱ� error");
		pdata->m_Stats.ShuiDefence_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ���˺�����error");
		pdata->m_Stats.HuoDefence = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata ���˺�����ٷֱ� error");
		pdata->m_Stats.HuoDefence_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata ���˺�����error");
		pdata->m_Stats.JinDefence = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata ���˺�����ٷֱ� error");
		pdata->m_Stats.JinDefence_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �Թ����˺�����50% error");
		readFamily(pdata->m_Stats, LineData.m_F32, true);

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata ���ٹ����˺�50% error");
		readFamily(pdata->m_Stats, LineData.m_F32, false);

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_S32, "read buffdata Զ����������error");
		pdata->m_Stats.RemoteAtk = LineData.m_S32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata Զ�������˺������ٷֱ� error");
		pdata->m_Stats.RemoteAtk_gPc = LineData.m_F32;

		file.GetData(LineData);
		ASSERT(LineData.m_Type == DType_F32, "read buffdata �����˺��ٷֱ� error");
		pdata->m_Stats.RemoteAtk_gPc = LineData.m_F32;

		m_DataMap.insert(BUFFDATAMAP::value_type(pdata->m_BuffId,pdata));
	}
}


CBuffData* CBuffDataRes::GetBuff(S32 BuffId)
{
	if(BuffId<BUFF_BEGIN_ID || BuffId> BUFF_END_ID)
		return NULL;
	BUFFDATA_ITR itr=m_DataMap.find(BuffId);
	if(itr!=m_DataMap.end())
	{
		if (itr->second) 
			return itr->second;
		else
			return NULL;

	}
	return NULL;
}

void CBuffDataRes::clear()
{
	for (BUFFDATA_ITR itr=m_DataMap.begin(); itr!=m_DataMap.end(); ++itr) {
		SAFE_DELETE(itr->second);
	}
	m_DataMap.clear();
}

void CBuffDataRes::close()
{
	clear();
}
