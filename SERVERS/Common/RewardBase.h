#ifndef __REWARDBASE_H__
#define __REWARDBASE_H__

#include "Base/BitStream.h"
#include "Common/PacketType.h"
#include <vector>

//��������
enum enRewardType
{
	REWARD_BINDMONEY			= 1,	//�󶨽�Ԫ
	REWARD_BINDGOLD				= 2,	//��Ԫ��
	REWARD_ITEM					= 3,	//��Ʒ
};

#pragma pack(push, 1)

//�����ṹ
struct stAccountReward
{
	U32 AccountID;			//�ʺű��
	U32 RewardID;			//�������
	U8  RewardType;			//��������
	S32 RewardValue;		//��������
	S32 RewardNum;			//��������
	bool IsAuto;			//�Ƿ�ϵͳ�Զ����
	bool IsDraw;			//�Ƿ���ȡ��
	U32 PlayerID;			//��ȡ�Ľ�ɫ���
	stAccountReward() { Clear();}
	void Clear() { memset(this, 0, sizeof(stAccountReward));}
};

#pragma pack(pop)

#endif//__REWARDBASE_H__