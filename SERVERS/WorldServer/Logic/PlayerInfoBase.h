#ifndef PLAYERINFO_BASE_H
#define PLAYERINFO_BASE_H

#include "Base/types.h"
#include "Base/bitStream.h"
#include "CommLib/redis.h"
#include <memory>

struct stBuff
{
	S32 Business;
	S32 Military;
	S32 Technology;
	stBuff() : Business(0), Military(0), Technology(0)
	{
	}

	template<class T>
	stBuff& operator+=(const T other)
	{
		Business += other->Business;
		Military += other->Military;
		Technology += other->Technology;
		return *this;
	}

	template<class T>
	stBuff& operator-=(const T other)
	{
		Business -= other->Business;
		Military -= other->Military;
		Technology -= other->Technology;
		return *this;
	}

	stBuff& operator+=(const stBuff& other)
	{
		Business += other.Business;
		Military += other.Military;
		Technology += other.Technology;
		return *this;
	}

	stBuff& operator-()
	{
		Business = -Business;
		Military = -Military;
		Technology = -Technology;
		return *this;
	}

	void Clear()
	{
		Business = 0;
		Military = 0;
		Technology = 0;
	}
};


struct stPlayerBaseInfo
{
	U32  PlayerID;
	U32	 Business;
	U32  Military;
	U32  Technology;
	S64  Money;
	S64  Ore;
	S64  Troops;
	U32	 Attack;
	U32	 Hp;
	U32	 Land;
	U32  BattleVal;
	S8	 PLearnNum;//̫�������
	S8	 PPlantNum;//����԰����
	S8	 CardDevNum;//�з�����
	S32	 PveScore;//
	S32	 PveMoney;//
	S32	 CashGift;//���
	S32  Conquest;//������
	S32  PlayerFlag;//���ֱ�־
	S32  OrgId;
	//---------------�������ݲ������ݿ�---------------//
	U32 Gold;
	U32 VipLv;
	U32 DrawGold;
	stBuff Buff[4];//��������
	S32	 QueenVal;//����
	U32  AttackEx;
	U32	 HpEx;

	stPlayerBaseInfo() :PlayerID(0), Business(0), Military(0), \
		Technology(0), Money(0), Ore(0), Troops(0), BattleVal(0), \
		Attack(2), Hp(10), Land(100000), Gold(0), VipLv(0), QueenVal(0), \
		PPlantNum(1), PLearnNum(1), CardDevNum(1), PveScore(1000), PveMoney(0), \
		CashGift(0), Conquest(0), PlayerFlag(0), OrgId(0), AttackEx(0), HpEx(0)
	{
		memset(Buff, 0, sizeof(Buff));
	}

	inline stBuff& getBuff(int pos)
	{
		pos = mClamp(pos, 0, 3);
		return Buff[pos];
	}

	void WriteData(Base::BitStream* stream)
	{
		stream->writeInt(Business, Base::Bit32);
		stream->writeInt(Military, Base::Bit32);
		stream->writeInt(Technology, Base::Bit32);
		stream->writeInt(Gold, Base::Bit32);
		stream->writeBits(sizeof(S64) << 3, &Money);
		stream->writeBits(sizeof(S64) << 3, &Ore);
		stream->writeBits(sizeof(S64) << 3, &Troops);
		stream->writeInt(AttackEx, Base::Bit32);
		stream->writeInt(HpEx, Base::Bit32);
		stream->writeInt(Land, Base::Bit32);
		stream->writeInt(BattleVal, Base::Bit32);
		stream->writeInt(DrawGold, Base::Bit32);
		stream->writeInt(PLearnNum, Base::Bit8);
		stream->writeInt(PPlantNum, Base::Bit8);
		stream->writeInt(CardDevNum, Base::Bit8);
		stream->writeInt(PveScore, Base::Bit32);
		stream->writeInt(PveMoney, Base::Bit32);
		stream->writeInt(QueenVal, Base::Bit32);
		stream->writeInt(CashGift, Base::Bit32);
		stream->writeInt(Conquest, Base::Bit32);
		stream->writeInt(OrgId, Base::Bit32);
		stream->writeInt(4, Base::Bit16);
		for (int i = 0; i < 4; ++i) {
			stream->writeFlag(true);
			stream->writeInt(Buff[i].Business, Base::Bit32);
			stream->writeInt(Buff[i].Military, Base::Bit32);
			stream->writeInt(Buff[i].Technology, Base::Bit32);
		}
	}
};

typedef std::tr1::shared_ptr<stPlayerBaseInfo> PlayerBaseInfoRef;
//DECLARE_REDIS_UNIT(stPlayerBaseInfo, m_gRedisMgr, "Tbl_PlayerBaseInfo");
#endif//PLAYERINFO_BASE_H
