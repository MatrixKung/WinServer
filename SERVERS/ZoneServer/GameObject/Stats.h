//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Base/types.h"    
#include <iostream>
// ========================================================================================================================================
//  Stats
// ========================================================================================================================================

enum TIP_KIND
{
	TIP_DAMAGE=0,
	TIP_NO_HIT,
	TIP_SKILL,
	TIP_TRIG_STAT,
	TIP_TRAP,
	TIP_DOWN,
	TIP_NO_WALK,
	TIP_ITEM_FULL,
	TIP_ABORT_ITEM,
	TIP_WALK_ON,
	TIP_RECOVER_STATE,
	TIP_WATCH,
	TIP_ITEM_STOLE,
	TIP_ITEM_COMBINE,
	TIP_ITEM_STRENG,
	TIP_ITEM_GET,
	TIP_EXP_GET,
	TIP_LV_UP,
	TIP_MONSTER_UP,
	TIP_IMMUNITY,//����
	TIP_JOUK,//����
	TIP_FORCE_ATK,//����
	TIP_DIZZY,//��
	TIP_DEATH,//����
	TIP_COM_EFFECT,//ͨ����Ч
};


class CNpcData;

struct Stats
{
	enum DerateOrSwellType
	{
		DerateOrSwellType_nullity			= 0,
		DerateOrSwellType_availability,
	};
	S32			HP;					// ����
	F32			HP_gPc;				// �����ٷֱ�

	S32			MaxHP;				// �������
	F32			MaxHP_gPc;			// ��������ٷֱ�

	S32			Str;				// ����
	F32			Str_gPc;			// �����ٷֱ�

	S32			MaxStr;				// �������
	F32			MaxStr_gPc;			// ��������ٷֱ�

	F32			Cri_gPc;            // �������ʰٷֱ�

	F32			Doe_gPc;            // �չ����ܼ���

	F32         DoeR_gPc;           // Զ�����������ܼ���

	F32         Spe;                // �ٶ�

	S32			PhyDamage;			// �����˺�
	F32			PhyDamage_gPc;		// �����˺��ٷֱ�

	S32			PhyDefence;			// �������
	F32			PhyDefence_gPc;		// ��������ٷֱ�

	S32			MuDamage;			// ľ�˺�
	F32			MuDamage_gPc;		// ľ�˺��ٷֱ�

	S32			MuDefence;			// ľ�˺�����
	F32			MuDefence_gPc;		// ľ�˺�����ٷֱ�

	S32			HuoDamage;			// ���˺�
	F32			HuoDamage_gPc;		// ���˺��ٷֱ�

	S32			HuoDefence;			// ���˺�����
	F32			HuoDefence_gPc;		// ���˺�����ٷֱ�

	S32			TuDamage;			// ���˺�
	F32			TuDamage_gPc;		// ���˺��ٷֱ�

	S32			TuDefence;			// ���˺�����
	F32			TuDefence_gPc;		// ���˺�����ٷֱ�

	S32			JinDamage;			// ���˺�
	F32			JinDamage_gPc;		// ���˺��ٷֱ�

	S32			JinDefence;			// ���˺�����
	F32			JinDefence_gPc;		// ���˺�����ٷֱ�

	S32			ShuiDamage;			// ˮ�˺�
	F32			ShuiDamage_gPc;		// ˮ�˺��ٷֱ�

	S32			ShuiDefence;		// ˮ�˺�����
	F32			ShuiDefence_gPc;	// ˮ�˺�����ٷֱ�

	F32         Sheng_Damage_gPc;   // �����˺��ٷֱ�
	F32         Fo_Damage_gPc;
	F32         Xian_Damage_gPc;
	F32         Jing_Damage_gPc;
	F32         Gui_Damage_gPc;
	F32         Guai_Damage_gPc;
	F32         Yao_Damage_gPc;
	F32         Mo_Damage_gPc;
	F32         Shou_Damage_gPc;
	F32         Long_Damage_gPc;
	S32         DeriveHP;//��Ѫ
	S32         RecoverHP;//��Ѫ
	F32         RecoverHp_gPc;//��Ѫ�ٷֱ�add
	S32         See;//��Ұ
	S32         ReduceStr;//û�غ���������add

	S32         PhyImmunit;          // �����˺�����fer
	F32         PhyImmunit_gPc;      //

	U32         RemoteAtk;
	F32         RemoteAtk_gPc;

	F32         Reflect_gPc;





	Stats();
	void Clear();

	Stats& operator+=(const Stats&);
	Stats& operator-=(const Stats&);
	Stats& operator*=(const F32);

	// ���ռ���
	void Calc(const Stats& _add);
	// ��ֵ��ǿ������״̬�����ܵ���ǿ��
	void Plus(const Stats& _orig, const Stats& _plus);
	// ��ǿ���ֵ��+=��-=ʵ�����Ǽ�����ǿ���ֵ
	void PlusDone();
	// ����������Դ�����ԣ���Щֵ���裨���ܣ�����
	void PlusSrc(const Stats& _orig, S32 ratePhy,  S32 rateSpl, S32 ratePnm,S32 rateQua);
	// ���Ե��ӣ���+=��������˵�����
	Stats& Add(const Stats& _add, S32 _count = 1);
	// npc�����и�������
	void percentNpc(CNpcData* pData);

	void percent(const F32 val);

	/*void printS()
	{
		using namespace std;
		cout<<"    HP:"    <<HP<<"    �����ٷֱ�:"    <<HP_gPc<<"    �������:"    <<MaxHP<<"    ��������ٷֱ�:"    <<MaxHP_gPc<<endl;
		cout<<"    ����:"    <<Str<<"    �����ٷֱ�"    <<Str_gPc<<"    �������:"    <<MaxStr<<"    ��������ٷֱ�"    <<MaxStr_gPc<<endl;
		cout<<"    �������ʰٷֱ�:"    <<Cri_gPc<<"    �չ����ܼ���"    <<Doe_gPc<<"    Զ�����������ܼ���"    <<DoeR_gPc<<"    �ٶ�"    <<Spe<<endl;
		cout<<"    �����˺�"    <<PhyDamage<<"    �����˺��ٷֱ�"    <<PhyDamage_gPc<<"    �������"    <<PhyDefence<<"    ��������ٷֱ�"    <<PhyDefence_gPc<<endl;
		cout<<"    ľ�˺�"    <<MuDamage<<"    ľ�˺��ٷֱ�"    <<MuDamage_gPc<<"    ľ����"    <<MuDefence<<"    ľ�����ٷֱ�"    <<MuDefence_gPc<<endl;
		cout<<"    ���˺�"    <<HuoDamage<<"    ���˺��ٷֱ�"    <<HuoDamage_gPc<<"    �����"    <<HuoDefence<<"    ������ٷֱ�"    <<HuoDefence_gPc<<endl;
		cout<<"    ���˺�"    <<TuDamage<<"    ���˺��ٷֱ�"    <<TuDamage_gPc<<"    ������"    <<TuDefence<<"    �������ٷֱ�"    <<TuDefence_gPc<<endl;
		cout<<"    ���˺�"    <<JinDamage<<"    ���˺��ٷֱ�"    <<JinDamage_gPc<<"    �����"    <<JinDefence<<"    ������ٷֱ�"    <<JinDefence_gPc<<endl;
		cout<<"    ˮ�˺�"    <<ShuiDamage<<"    ˮ�˺��ٷֱ�"    <<ShuiDamage_gPc<<"    ˮ����"    <<ShuiDefence_gPc<<"    ˮ�����ٷֱ�"    <<endl;
		cout<<"    �����˺��ٷֱ�"    <<Sheng_Damage_gPc<<"    Fo_Damage_gPc"    <<Fo_Damage_gPc<<"    Xian_Damage_gPc"    <<Xian_Damage_gPc<<endl;

		cout<<"    Jing_Damage_gPc"    <<Jing_Damage_gPc<<"    Gui_Damage_gPc"    <<Gui_Damage_gPc<<"    Yao_Damage_gPc"    <<Yao_Damage_gPc<<endl;
		cout<<"    Mo_Damage_gPc"    <<Mo_Damage_gPc<<"    Shou_Damage_gPc"    <<Shou_Damage_gPc<<"    Long_Damage_gPc"    <<Long_Damage_gPc<<
			endl;
		cout<<"    DeriveHP��Ѫ"    <<DeriveHP<<"    RecoverHP"    <<RecoverHP<<"    RecoverHp_gPc"    <<RecoverHp_gPc<<"    See��Ұ"    <<See<<"    û�غ���������add"    <<ReduceStr<<endl;
		cout<<"    �����˺�����fer"    <<PhyImmunit<<"    PhyImmunit_gPc"    <<PhyImmunit_gPc<<"    RemoteAtk"    <<RemoteAtk<<"    RemoteAtk_gPc"    <<
			RemoteAtk_gPc<<"    Reflect_gPc"    <<Reflect_gPc<<endl;

	}*/
};

extern Stats g_Stats;

