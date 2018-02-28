//
//  InvestiGate.h
//  HelloLua
//
//  Created by th on 14-2-12.
//
//

#ifndef __HelloLua__InvestiGate__
#define __HelloLua__InvestiGate__


#include "Common/DataFile.h"
#include <string.h>
#include <hash_map>

class CInvestiGateData;
class CInvestiGateDataRes {

public:
	enum{
		InvestiGate_BEGIN_ID	= 5000001,
		InvestiGate_END_ID		= 5999999,
		InvestiGate_BRITH_ID    = 5901001,
	};

	CInvestiGateDataRes();
	~CInvestiGateDataRes();

	typedef stdext::hash_map<S32,CInvestiGateData*> INVESTIGATEDATAMAP;
	typedef INVESTIGATEDATAMAP::iterator INVESTIGATEDATA_ITR;

	bool read();
	void close();
	void clear();
	CInvestiGateData* getInvestiGateData(S32 InvestiGateId);
	S32 getNextGateId(S32 gateId, S32 layerId);
private:
	INVESTIGATEDATAMAP m_DataMap;
};

class CInvestiGateData
{
public:
	enum
	{
		MAX_INVESTIGATE_GOODS_NUM = 3,
	};

	CInvestiGateData(): m_InvestiGateId(0),m_Layer(0),m_LayerName(""),m_length(0),m_width(0),\
		m_RoomNum(0),m_MonsterGroup(0),m_MonsterNum(0),m_MonsterLevel(0), \
		m_NpcGroup(0),m_NpcNum(0),m_TrackGroup(0),m_TrackNum(0),m_EarthGroup(0),\
		m_Wall(0),m_Water(0),m_Null(0),m_Way(0),m_Door(0), m_RoomName(""),m_SceneState(0)
	{
		memset(m_GoodsGroup, 0, sizeof(m_GoodsGroup));
		memset(m_GoodsNum, 0, sizeof(m_GoodsNum));
	};
	~CInvestiGateData(){};

	S32 m_InvestiGateId;//�ؿ�ID
	S32 m_Layer;//����
	std::string m_LayerName;//�ؿ�����
	S32 m_length;//�ؿ���
	S32 m_width;//�ؿ���
	S32 m_GateType;//�ؿ�����
	S32 m_RoomNum;//��������
	S32 m_MonsterGroup;//������
	S32 m_MonsterNum;//��������
	S32 m_MonsterLevel;//����Ƚ�
	S32 m_NpcGroup;//npc��
	S32 m_NpcNum;//npc����
	S32 m_GoodsGroup[MAX_INVESTIGATE_GOODS_NUM];//��Ʒ��
	S32 m_GoodsNum[MAX_INVESTIGATE_GOODS_NUM];//��Ʒ����
	S32 m_TrackGroup;//������
	S32 m_TrackNum;//����������
	S32 m_SceneState;//����״̬
	S32 m_EarthGroup;//�ر���
	S32 m_Wall;//ǽ
	S32 m_WallBreak;//ǽ���ƻ�
	S32 m_Water;//ˮ
	S32 m_Null;//���
	S32 m_Way;//ͨ��
	S32 m_Door;//��
	std::string m_RoomName;//�̶���������
};

extern CInvestiGateDataRes* g_InvestiGateDataMgr;



#endif /* defined(__HelloLua__InvestiGate__) */
