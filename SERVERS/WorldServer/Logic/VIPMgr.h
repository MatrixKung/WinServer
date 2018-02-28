#pragma once

#include "VIPBase.h"
#include "BaseMgr.h"

class CVIPMgr : public CBaseManager<stVIPBase>
{
public:
	CVIPMgr();
	~CVIPMgr();

	static CVIPMgr* Instance();
	//VIP�ۼƳ�ֵ
	bool HandleRecharge(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);
	//VIP�����ȡ��ÿ��VIP�ȼ����ֻ����ȡһ��
	bool HandleClientVipGiftRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream *Packet);

	//��ȡ�������޼ӳ�
	U32 GetVipVIMTopLimit(U32 nPlayerId) const;
	//��ȡ�������޼ӳ�
	U32 GetVipStrengthTopLimit(U32 nPlayerId) const;
	//��ȡ�������޼ӳ�
	U32 GetVipActivityTopLimit(U32 nPlayerId) const;
	//��ȡ�����������������ظ�ʱ�����
	F32 GetVipVSAReducePercent(U32 nPlayerId) const;

	//��������
	U32 GetVipApplyTopLimit(U32 nPlayerId) const;
	//����������
	U32 GetImposeTopLimit(U32 nPlayerId) const;
	//����ϯλ����
	U32 GetPriceNumTopLimit(U32 nPlayerId) const;
	//����
	U32 GetResIncreasePercent(U32 nPlayerId) const;

	//�Ƿ��������ս������
	bool CanJumpBattleAnim(U32 nPlayerId) const;

	//VIP���ճ�ʼֵ
	void VIPInitAddImpose(U32 nPlayerId);

	static void Export(struct lua_State* L);
private:
	//VIP Buff ���� ���������ޡ����ռӳɡ�����ϯλ�ӳɡ��������ޡ������ʱ����١�����ս������
	bool AddVipBuff(U32 nPlayerId, U32 OldVipLevel, U32 CurrentVipLevel);
	//����û���δ�ﵽ�˵ȼ��򷵻�0
	U32 GetVipLvByGiftId(U32 nPlayerId, U32 VipGiftId);
private:
	const U8 MAXVIPLEVEL;
};

#define VIPMGR CVIPMgr::Instance()