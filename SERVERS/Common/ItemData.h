#ifndef	_ITEMDATA_H
#define _ITEMDATA_H
/*
#include "Common/OrgDepotData.h"

class CItemData
{
public:
	enum Column
	{
		Column_ItemID, //���߱��
		Column_ItemName, //��������
		Column_Category, //�������
		Column_SubCategory, //��������
		Column_DescriptionID, //��������
		Column_HelpID, //����˵��
		Column_PurposeID, //��;
		Column_IconName, //����icon
		Column_PetInsight, //��������
		Column_PetTalent, //�������
		Column_SoundIDA, //������ЧA
		Column_EffectSoundID, //�ػ���Ч
		Column_TimeMode, //ʱЧ���㷽ʽ
		Column_Duration, //ʱЧ
		Column_SaleType, //���۷�ʽ
		Column_SalePrice, //���ۼ۸�
		Column_MaxOverNum, //��������
		Column_Reserve, //�����ֶ�
		Column_MissionID, //������
		Column_MissionNeed, //������ʾ
		Column_ColorLevel, //��ɫ�ȼ�
		Column_QualityLevel, //Ʒ�ʵȼ�
		Column_DropRate, //���伸��
		Column_BaseLimit, //��������
		Column_SexLimit, //�Ա�����
		Column_LevelLimit, //�ȼ�����
		Column_FamilyLimit, //��������
		Column_UseTimes, //ʹ�ô���
		Column_Renascences, //ת������
		Column_CoolTimeType, //��Ʒ��ȴ����
		Column_CoolTime, //��ȴʱ��
		Column_VocalTime, //����ʱ��
		Column_IdentifyType, //����
		Column_RandomNameID, //�����������ID
		Column_SuitID, //��װ���
		Column_BaseAttribute, //װ��������

		Column_AppendAttribute_1,
		Column_AppendAttribute_12 = Column_AppendAttribute_1+11,

		Column_SkillAForEquip, //��������
		Column_SkillBForEquip, //��������
		Column_SkillForUse, //ʹ��ʱ����
		Column_MaxWear, //����;�
		Column_MaxEnhanceWears, //���ǿ������
		Column_BindMode, //��Ʒ��ģʽ
		Column_BindPro, //��������
		Column_AllowEmbedSlot, //������Ƕ��
		Column_OpenedEmbedSlot, //�ѿ�����Ƕ��

		Column_MaleEquipEffectID, //��װ����ЧID
		Column_MaleEquipStrengthenEffectID, //��ǿ����ЧID
		Column_FemaleEquipEffectID,
		Column_FemaleEquipStrengthenEffectID,
		Column_EquipStrengthenEffectLevel, //��Ч�ȼ�����

		Column_EquipStrengthen_1, //1��װ������
		Column_EquipStrengthenSkin_1, //1����ͼ
		Column_EquipStrengthen_15 = Column_EquipStrengthen_1+2*14,
		Column_EquipStrengthenSkin_15 = Column_EquipStrengthenSkin_1+2*14,

		Column_EquipStrengthenLevel_1,
		Column_EquipStrengthenLevel1_1,
		Column_EquipStrengthenLevel_3 = Column_EquipStrengthenLevel_1+2*2,
		Column_EquipStrengthenLevel1_3 = Column_EquipStrengthenLevel1_1+2*2,

		Column_MaleShapesSetId, //ģ������(��)
		Column_FemaleShapesSetId, //ģ������(Ů)
		Column_NewFamilyMaleShapesSetId, //��ģ������(����)
		Column_NewFamilyFemaleShapesSetId,
		Column_StoneLimit,   //��ʯ����
		Column_TrailEffect,  //��β��Ч
		Column_SoundIDB,     //������ЧB
		Column_PetType,      //��������
		Column_ExFlags,      //���⹦�ܱ�־
		Column_JobLimit,     //ְҵ����
		Column_Priority,     //�������ȼ�
		Column_ProductType,  //������ʽ
		Column_ProductWarns, //������������
		Column_SoulStoneLimit, //��װ��ʯ����
		Column_SkillSlotType,  //���ܲ������

		Column_Count
	};

	typedef TDataInstance<CItemData>::DataManager::DataPtr Data;

	static CItemData* GetInstance();
	Data GetData(U32 itemID) const;
	Data Begin() const;
	Data Next() const;
    
    void Reload(void);
private:
	CItemData();

	TDataInstance<CItemData> m_dataInstance;
};

#define ITEMDATA CItemData::GetInstance()
*/
#endif //_ITEMDATA_H