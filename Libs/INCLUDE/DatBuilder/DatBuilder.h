
#pragma once
#include "base/types.h"
//#include "windows.h"
#include "ConfigStruct.h"
//#include "../../game/Engine/source/core/stringTable.h"
//#include "..\..\game\Engine\source\core\stringTable.h"


#define  MAX_FILE_PATH	256
#define  max_strLen  1024*30
struct FCheck 
{
	std::string  path;    //·��
	std::string title;    //����
};

struct RData
{
	int       m_Type;   //��������
	//union
	struct
	{
		char	m_string[max_strLen];
		
		S8		m_S8;
		S16		m_S16;
		S32		m_S32;
		
		U8		m_U8;
		U16		m_U16;
		U32		m_U32;
		
		F32		m_F32;
		F64		m_F64;

		U8		m_Enum8;
		U16		m_Enum16;
	} m_Data;
};

#define		m_string		m_Data.m_string
#define		m_S8			m_Data.m_S8
#define		m_S16			m_Data.m_S16
#define		m_S32			m_Data.m_S32
#define		m_U8			m_Data.m_U8
#define		m_U16			m_Data.m_U16
#define		m_U32			m_Data.m_U32
#define		m_Enum8			m_Data.m_Enum8
#define		m_Enum16		m_Data.m_Enum16
#define		m_F32			m_Data.m_F32
#define		m_F64			m_Data.m_F64


class COperateData
{
public:
	//HWND       m_hWnd ;                          //���ߴ��ڵ�HANDLE���������ʾ��Ϣ
    void       OutData();						//����ata�ļ�
	void       OutExlINi();                     //����EXL��INI
	//���ڶ�ȡ���ݵı���
	int  RecordNum;  //��¼����
	int  ColumNum;   //������

	char* ChangetxtUtf8ToAnsi(char* szu8);
	void ChangeTextEncode(const char *filename);
	void ReadDataInit   ();
	void ReadDataClose  ();
	bool readDataFile   (const char* name);    //��ȡdata�ļ�
	bool GetData        (RData&);              //�õ����������
	//
	COperateData();
	~COperateData();
	//
private:


	std::vector<Cconfig>  m_ConfigList;
	std::vector<FCheck>  m_IexFlieList;    //iex�б�
	std::vector<FCheck>  m_IniFlieList;    //int�б�
	std::vector<FCheck>  m_DataFlieList;   //Data�б�
	//
	char			 m_Path[MAX_FILE_PATH];        //�����ļ���·��
	char             m_FileName[MAX_FILE_PATH];    //Ҫ���ɵ��ļ���
	char             m_FileName2[MAX_FILE_PATH];   //
	char			 ErrorChr[256];
	//���ڶ�ȡ���ݵı���
	FILE* pfdata;
	int   readstep;   //���ƶ����ܵ�����
	//
	std::string     GetFileExtenName(const char* name);  //�õ��ļ�����չ��  
	std::string     GetFileTitleName(const char* name);
	void			RansackFile(const char* lpPath);     //�������е��ļ� isOut 

	//
	void		BuildData(const char* fiex);       //���Ddata�ļ�
	void		BulidConfigVector(const char* fini); //������Ҫ�Ľ�����Ϣ
	void		ParseData(const char* filename);   //����Data�ļ�
	
	//����Ψһ�Լ��(����ֻ������string��int)
	std::vector<int>     iMainKeyVt;
	std::vector<std::string>  sMainKeyVt;

	DataType             MainKeyType;              //����������
	int                  MainKeyIndex;

	bool       CheckmKeyExclusive(S32 itemp, std::string stemp);           //���������Ψһ�� 
	//
	void       ShowErrorMessage(const char* chr);  //��ʾ������Ϣ
};
