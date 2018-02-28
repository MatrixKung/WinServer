//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once
#include "Base/types.h"
#include "Stats.h"
#include "../Data/BuffData.h"
#include "Base/tVector.h"
#include <vector>
#include <hash_map>
#include "CGameBase.h"
#include "BASE/mMathFn.h"
class CGameObject;
class Player;

class Buff
{
public:

	enum Origin
	{
		Origin_Base = 0,				// ��������
		Origin_Buff,					// ״̬
		Origin_Equipment,				// װ��
		Origin_Level,                   // �ȼ�����buff��npc�������npc�ӳɣ�Ŀǰ�ӳ�ֻ�еȼ���
	};

	enum
	{
		Mask_Remove_Sys = BIT(0),		// �������Ƴ���־��ͬ����
		Mask_Remove_Common = BIT(1),	// ��ͨ
	};

	CBuffData*	m_BuffData;				// ״̬
	Stats		m_BuffStats;			// ״̬�����ӳɺ����������

	Origin		m_Origin;				// ���״̬����Դ
	bool		m_Self;					// ���״̬���ͷ����Ƿ�Ϊ����
	U32			m_Count;				// ������
	U32			m_RemainTimes;			// ʣ�����
	U32			m_LimitTimes;			// �ܴ���
	bool        m_bRemove;			// �Ƴ���־��ӦBuffData::Trigger_RemoveA~D
	bool        m_bNeedProcess;

	Vector<U32> m_Plus;					// ��¼����츳�Ը�״̬�ļӳ�
	SimObjectPtr<CGameObject> m_pSource;	// �ͷ����״̬�Ķ���

	Buff();
	~Buff();

	//ɾ����Ч
	void Remove(CGameObject* pObj);
	bool Process(CGameObject* pObj, float dt);//�غ��ƽ�
	void ProcessTick(CGameObject* pObj);//�����˺�buff

	// ����(Ԥ��)
	void onTrigger(U32 mask, CGameObject* carrier/*����*/, CGameObject* trigger);
private:
};


// ========================================================================================================================================
//  BuffTable
// ========================================================================================================================================
class BuffTable
{
public:
	typedef Vector<Buff*> BuffTableType;
	typedef stdext::hash_multimap<U32, Buff*> BuffTableMap;

private:
	// �Ƿ���Ҫˢ��
	bool m_Refresh;
	// ������������
	Stats m_Stats;

	// ���������ܺͣ�δ�����ռ��㣩
	Stats m_Summation;

	// ��Щ�����ƽ���ֻ�Ǽ�¼ĳЩ��Ϣ������������CD�ȣ�
	// ����Buff::m_Plus�����ǵ�PlusTableһ��
	// Ϊ��Լ�ڴ棬�ϲ���һ��hash��
	BuffTableMap m_General;

	// ��Щ״̬���ƽ�
	BuffTableType m_Buff;

	// ӵ����Щ״̬�Ķ���
	CGameObject* m_pObj;

	// ����״̬Ч���ļ���
	U64 m_EffectMask1;
	U64 m_EffectMask2;

public:

	BuffTable();
	~BuffTable();
	void Clear();
	inline void SetObj(CGameObject* pObj) {m_pObj = pObj;}
	inline bool GetRefresh() { return m_Refresh; }
	inline void SetRefresh(bool val) { m_Refresh = val; }
	inline const Stats& GetStats() { return m_Stats; }
	inline const Stats& GetSummation() {return m_Summation; }
	inline U64 GetEffect1() const { return m_EffectMask1; }
	inline U64 GetEffect2() const { return m_EffectMask2; }
	F32  getFamilyAddDamage(U8 family);

	bool onTriggerGroup(CBuffData* pBuffData, CGameObject* src);
	bool AddBuff(Buff::Origin _origin, U32 _buffId, CGameObject* _src, S32 _count = 1, U32 _time = 0, Vector<U32> *_list = NULL, U32 rate = 10000);
	// ȥ��m_General�е���ֵ
	bool RemoveBuff(U32 _buffId, Vector<U32> *_list = NULL);

	// ȥ��m_Buff��m_Debuff��m_System�е�״̬
	bool RemoveBuff(BuffTableType& _buffTab, U32 _buffId, U32 index, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys);

	bool RemoveBuffBySrc(U32 _buffId, CGameObject* src, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys);
	void ClearBuff();												// �������buff

	void Process(float dt);
	//npc��ǿ
	bool CalculateNpcStats(CBuffData& pData);
	bool CalculateStats();

	bool checkBuff(U32 id, S32 count, bool seriesId = false) const;	// ���m_Buff��m_Debuff��m_System�е�״̬
	U32 getBuffCount(U32 _buffId) const;	// ���m_Buff��m_Debuff��m_System�е�״̬

	template<class T, class V>
	static bool greateSort(T Itr, V val)
	{
		return Itr.first < val;
	}

	template<class V, class T>
	static bool greateSort1(V val, T Itr)
	{
		return Itr.first < val;
	}

	// ���ú���
	inline void setHP(S32 val) { m_Stats.HP = mClamp(val, 0, m_Stats.MaxHP);}
	inline void setStr(S32 val) { m_Stats.Str = mClamp(val, 0, m_Stats.MaxStr);}
	inline void addHP(S32 val) { m_Stats.HP = mClamp(m_Stats.HP + val, 0, m_Stats.MaxHP);}
	inline void addStr(S32 val) { m_Stats.Str = mClamp(m_Stats.Str + val, 0, m_Stats.MaxStr);}
	inline void setMaxHP(S32 val) {m_Stats.MaxHP=val;}
	inline void addMaxHP(S32 val) {m_Stats.MaxHP+=val;}

	// ȡ��m_Buff��m_Debuff
	const inline BuffTableType& GetBuff()  const { return m_Buff; }

	// buff�ڲ��ӿڣ�������������������ԣ�
	bool CalcAddAbnormity(CGameObject* pSource, CBuffData *pData, U32 rate);
};


