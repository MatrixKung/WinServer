#ifndef sqlGenTypedef_H__
#define sqlGenTypedef_H__
#include <map>

enum DBDataType
{
	DBDataType_char_array,
	DBDataType_S8	,
	DBDataType_U8	,
	DBDataType_S16	,
	DBDataType_U16	,
	DBDataType_S32	,
	DBDataType_U32	,
	//DBDataType_S64	,
	DBDataType_U64	,
	DBDataType_bool	,
	DBDataType_F32 ,
	DBDataType_SecondPassword,
	DBDataType_PlayerDataFlag1,
	DBDataType_PlayerDataFlag2
};

typedef std::map<int,std::string> tMapOffset2ColumnName;
typedef std::map<int,DBDataType> tMapOffset2DataType;

#define _DECALRE_INITDB static void InitDBStruct(tMapOffset2ColumnName& m_mapOffset2ColumnName, tMapOffset2DataType& m_mapOffset2DataType, int nBaseOffset =0)

#define REGEDIT_DB_COLUMN(StructType/*��������*/, MemberaType/*��Ա������*/, MemberName/*��Ա��*/, ColumnNameStr/*���ݿ�����*/)\
{\
	MemberaType StructType::* p = &StructType::MemberName;\
	m_mapOffset2ColumnName.insert(std::make_pair ((int&)p+nBaseOffset,#ColumnNameStr));\
	m_mapOffset2DataType.insert(std::make_pair   ((int&)p+nBaseOffset, DBDataType_##MemberaType));\
}

//��char a[5]�����Ľṹӳ�䵽һ�����ݿ�column��
#define REGEDIT_DB_COLUMN_ARRAY(StructType/*��������*/, MemberaType/*��Ա������*/, MemberName/*��Ա��*/, nArrayLength/*���鳤��*/ ,ColumnNameStr/*���ݿ�����*/){\
	MemberaType (StructType::* p )[nArrayLength]= &StructType::MemberName;\
	m_mapOffset2ColumnName.insert(std::make_pair ((int&)p+nBaseOffset,#ColumnNameStr));\
	m_mapOffset2DataType.insert(std::make_pair   ((int&)p+nBaseOffset, DBDataType_##MemberaType##_array));}



//ӳ��һ��������char�ַ���
#define REGEDIT_DB_COLUMN_SecondPassword(StructType/*��������*/, MemberaType/*��Ա������*/, MemberName/*��Ա��*/, nArrayLength/*���鳤��*/ ,ColumnNameStr/*���ݿ�����*/){\
	MemberaType (StructType::* p )[nArrayLength]= &StructType::MemberName;\
	m_mapOffset2ColumnName.insert(std::make_pair ((int&)p+nBaseOffset,#ColumnNameStr));\
	m_mapOffset2DataType.insert(std::make_pair   ((int&)p+nBaseOffset, DBDataType_SecondPassword));}






//�� a[5]�����Ľṹ��Ϊ aname0, aname1, aname2,aname3�����Ľṹ�洢
#define REGEDIT_DB_COLUMN_ARRAY_SLICE(StructType/*��������*/, MemberaType/*��Ա������*/, MemberName/*��Ա��*/, nArrayLength/*���鳤��*/ ,ColumnNameStr/*���ݿ�����*/){\
	MemberaType (StructType::* p )[nArrayLength]= &StructType::MemberName;\
	for(int i=0; i<nArrayLength; i++){\
	std::stringstream ss ; ss<<#ColumnNameStr<<i;\
	m_mapOffset2ColumnName.insert(std::make_pair ((int&)p+i*sizeof(MemberaType)+nBaseOffset,ss.str()));\
	m_mapOffset2DataType.insert(std::make_pair   ((int&)p+i*sizeof(MemberaType)+nBaseOffset, DBDataType_##MemberaType));\
	}}

enum End_Condition //��������
{
	keynum_is_zero, //����Ԫ��keyֵΪ0
	array_endl		//�������
};

enum Index_Sensitive_Status //�±��Ƿ�����
{
	index_sensitive, //�±�����
	index_non_sensitive //�±겻����
};

#endif