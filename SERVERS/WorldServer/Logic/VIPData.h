#pragma once

#include "BaseData.h"

struct VIPData;

class CVIPDataRes : public CBaseDataRes<VIPData>
{
public:
	CVIPDataRes() = default;
	~CVIPDataRes();

	bool read();

};

struct VIPData 
{
	VIPData() : m_VipLevel(0), m_DrawGold(0), m_ImposeTopLimit(0), m_VIMTopLimit(0), m_StrengthTopLimit(0), m_ActivityTopLimit(0), 
		m_ImposePlus(0), m_PrinceNumberTopLimit(0), m_ApplyTopLimit(0), m_VSAReducePercent(0), m_CanJumpBattleAnim(0), m_VIPGiftId(0){}

	U8 m_VipLevel;	//VIP�ȼ�
	U32 m_DrawGold;	//�ۼƳ�ֵ
	U8 m_ImposeTopLimit;	//����������
	U8 m_VIMTopLimit;		//��������
	U8 m_StrengthTopLimit;	//��������
	U8 m_ActivityTopLimit;	//��������
	U8 m_ImposePlus;		//���ռӳ�
	U8 m_PrinceNumberTopLimit;	//����ϯλ��������
	U8 m_ApplyTopLimit;		//������������
	F32 m_VSAReducePercent;	//���������ʱ��
	U8 m_CanJumpBattleAnim;		//�Ƿ��������ս������
	U32 m_VIPGiftId;			//vip���id
};

