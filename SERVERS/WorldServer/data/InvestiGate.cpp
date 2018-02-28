//
//  InvestiGate.cpp
//  HelloLua
//
//  Created by th on 14-2-12.
//
//

#include "InvestiGate.h"
#include "Common/OrgBase.h"
#include "CommLib/CommLib.h"

CInvestiGateDataRes* g_InvestiGateDataMgr=new CInvestiGateDataRes();
CInvestiGateDataRes::CInvestiGateDataRes()
{

}
CInvestiGateDataRes::~CInvestiGateDataRes()
{
	clear();
}

bool CInvestiGateDataRes::read()
{
	CDataFile file;
	RData lineData;

	if (!file.readDataFile("data/Data_5_InvestiGate.dat"))
	{
		JUST_ASSERT("read invesitgate error ");
		return false;
	}

	for (int i=0; i<file.RecordNum; ++i)
	{
		CInvestiGateData* pData=new CInvestiGateData();

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate id error");
		pData->m_InvestiGateId=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate layer error");
		pData->m_Layer=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_string,"read investigate name error");
		pData->m_LayerName=lineData.m_string;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate  length error");
		pData->m_length=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate width error");
		pData->m_width=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_enum16,"read investigate gateType error");
		pData->m_GateType=lineData.m_Enum16;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate RoomNum error");
		pData->m_RoomNum=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate  monster group error");
		pData->m_MonsterGroup=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate monster num error");
		pData->m_MonsterNum=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate monster level error");
		pData->m_MonsterLevel=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate npc group error");
		pData->m_NpcGroup=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate npc num error");
		pData->m_NpcNum=lineData.m_S32;

		for(int j = 0; j < CInvestiGateData::MAX_INVESTIGATE_GOODS_NUM; ++j)
		{
			file.GetData(lineData);
			DOASSERT(lineData.m_Type==DType_S32, "read investigate goods group error");
			pData->m_GoodsGroup[j]=lineData.m_S32;

			file.GetData(lineData);
			DOASSERT(lineData.m_Type==DType_S32, "read investigate goods num error");
			pData->m_GoodsNum[j]=lineData.m_S32;
		}

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate track group error");
		pData->m_TrackGroup=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate track num error");
		pData->m_TrackNum=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate track num error");
		pData->m_SceneState=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate earth group error");
		pData->m_EarthGroup=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate wall error");
		pData->m_Wall=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate wall error");
		pData->m_WallBreak=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate water error");
		pData->m_Water=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate null error");
		pData->m_Null=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate way error");
		pData->m_Way=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_S32,"read investigate door error");
		pData->m_Door=lineData.m_S32;

		file.GetData(lineData);
		DOASSERT(lineData.m_Type==DType_string,"read investigate RoomName error");
		pData->m_RoomName=lineData.m_string;

		m_DataMap.insert(INVESTIGATEDATAMAP::value_type(pData->m_InvestiGateId,pData));
	}
	return true;
}

void CInvestiGateDataRes::clear()
{
	for (INVESTIGATEDATA_ITR itr=m_DataMap.begin(); itr!=m_DataMap.end(); ++itr) {
		SAFE_DELETE(itr->second);
	}
	m_DataMap.clear();
}

void CInvestiGateDataRes::close()
{
	clear();
}

CInvestiGateData* CInvestiGateDataRes::getInvestiGateData(S32 InvestiGateId)
{
	INVESTIGATEDATA_ITR itr=m_DataMap.find(InvestiGateId);

	if (itr!=m_DataMap.end()) {
		if (itr->second)
			return itr->second;
		else
			return NULL;
	}
	return NULL;
}

S32 CInvestiGateDataRes::getNextGateId(S32 gateId, S32 layerId)
{
	U32 nGateType = gateId / 1000;
	++layerId;
	nGateType = nGateType * 1000 + layerId;
	CInvestiGateData* pData = getInvestiGateData(nGateType);
	if(pData)
		return pData->m_InvestiGateId;
	else 
		return -1;//�Ĺؿ��Ѿ�����ͷ
}