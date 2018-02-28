#pragma once 
#include <vector>
#include <string>

#define  MAXStrLen 128
#define  IntLen     32

enum  DataType
{
	DType_none    = 0,
	
	DType_string,
	
	DType_enum8,
	DType_enum16,
	
	DType_S8,
	DType_S16,
	DType_S32,
	
	DType_U8,
	DType_U16,
	DType_U32,
	
	DType_F32,
	DType_F64,
};
//
struct elementData
{
	std::string elem_name;      //ö��ֵ���ַ�ֵ
	int         elem_value;     //ö��ֵ���߼�ֵ
};
//
class Cconfig
{
public:
	char       m_ColName[MAXStrLen];	//����
	DataType   m_DType;                 //��������
	int        elem_num;                //һ���м���Ԫ��
	std::vector<elementData> m_enumVT;  //ö�����͵�����

	Cconfig();
	~Cconfig();
	void  Init();	
};