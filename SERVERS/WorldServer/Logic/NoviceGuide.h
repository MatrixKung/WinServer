#pragma once

#include "BASE/types.h"

enum enNoviceGuideType
{
	SUPPORT = 0,	//����
	SKILL,			//�Ƽ�
	CHOOSEPRINCESS,	//����
	COURTAFFAIRFAST,//�������
	RITEFAST,		//���Ǽ���
	PLANTFAST,		//��������
	DRAGONSPIRIT,	//���굥��
	CONQUER,		//��������
	VISIT,			//Ѱ��
	ADDPRINCE,		//��һ��������
};

class CNoviceGuide
{
public:
	static CNoviceGuide* Instance();

	//��һ�γ���
	bool IsFirstSupportRequest(U32 nPlayerId);
	//��һ���о��Ƽ�����
	bool IsFirstSkillFast(U32 nPlayerId);
	//��һ�η���
	bool IsFirstChoosePrincess(U32 nPlayerId);
	//��һ�ι������
	bool IsFirstCourtAffairFast(U32 nPlayerId);
	//��һ�����Ǽ���
	bool IsFirstRiteFast(U32 nPlayerId);
	//��һ����������
	bool IsFirstPlantFast(U32 nPlayerId);
	//��һ�����굥��
	bool IsFirstDragonSpirit(U32 nPlayerId);
	//��һ����������
	bool IsFirstConquer(U32 nPlayerId);
	//��һ��Ѱ��
	bool IsFirstVisit(U32 nPlayerId);
	//��һ��Ѱ�ã����ı�Ѱ��״̬
	bool IsFirstVisitAndHold(U32 nPlayerId);
	//��һ��������
	bool IsFirstAddPrince(U32 nPlayerId);
private:
	bool IsFirstAndChangeStatus(U32 nPlayerId, enNoviceGuideType type);

	bool IsFirstAndHoldStatus(U32 nPlayerId, enNoviceGuideType type);
};

#define NGUIDE CNoviceGuide::Instance()