#include "DataFile.h"
#include <Windows.h>
#include "BASE/UtilString.h"

#define FP_READ( m, s, p ) fread( &m, sizeof( s ), 1, p )

CDataFile::CDataFile()
{
	ReadDataInit();
}
//
CDataFile::~CDataFile()
{
	ReadDataClose();
}
//
void  CDataFile::ReadDataInit()
{
	ColumNum = 0 ;
	RecordNum  = 0 ;
	readstep  = 0 ;
	pfdata = NULL;
}
//
void CDataFile::ReadDataClose()
{
	if(pfdata)
	{
		fclose( pfdata );
		pfdata = NULL;
	}

}
//
bool  CDataFile::readDataFile(const char* name)
{
	m_DataTypes.clear();
	m_CurrentColumnIndex = 0;

	//�ȶ�λ����
	errno_t err = fopen_s( &pfdata, name, "rb" );

	if(!pfdata)
		return FALSE;

	char tchr[1024];

	if(pfdata)
	{
		while(1)
		{
			ZeroMemory(tchr,sizeof(tchr));
			fread(tchr,sizeof(char),1,pfdata);
			//pfdata->read(sizeof(char),tchr);
			//
			if(strcmp(tchr,"@")==0)  //�ҵ������ļ��Ŀ�ͷ
			{
				fread(tchr,sizeof(char),1,pfdata);  //����ǻ����ַ�
				//pfdata->read(sizeof(char),tchr);
				break;
			}
		}
		fread(&RecordNum,sizeof(int),1,pfdata);   //�õ���¼����
		fread(&ColumNum,sizeof(int),1,pfdata);    //�õ��е�����
		/*pfdata->read(sizeof(int), &RecordNum);
		pfdata->read(sizeof(int), &ColumNum);*/
	}
	//
	readstep = RecordNum * ColumNum;
	U8 nDataType = 0;

	for(S32 nColumnIndex = 0; nColumnIndex < ColumNum; nColumnIndex++)
	{
		fread( &nDataType, sizeof(U8), 1, pfdata );
		//pfdata->read(sizeof(U8), &nDataType);
		m_DataTypes.push_back(nDataType);
	}

	return TRUE;
}

//�õ����������
bool  CDataFile::GetData(RData& readsct)
{
	/****************************
	��ʽ:
	ͷ�ļ�:
	1���ܼ�¼��(int)
	2�����ֶ���(int)
	�ֶθ�ʽ:
	1���ֶγ���(int)
	2���ֶ���������(int->2,string->1,enum->3,float->4)
	3���ֶ�����(int,string)
	*************************/
	if( (readstep == 0) || (!pfdata))
		return FALSE;
	int isize = 0 ; //���ݵĴ�С
	ZeroMemory(&readsct, sizeof(readsct));
	//
	switch (m_DataTypes[m_CurrentColumnIndex])
	{
	case DType_string: 
		{
			short strSize;
			FP_READ( strSize, short, pfdata );
			//pfdata->read(sizeof(U16), &isize);

			fread( readsct.m_string, sizeof( char ), strSize, pfdata );
			//char* pBuff = Util::Utf8ToMbcs(readsct.m_string);
			//dStrcpy(readsct.m_string, sizeof(readsct.m_string), pBuff);
			//delete[] pBuff;
			//pfdata->read(sizeof(char)*isize, readsct.m_string);
		};break;
	case DType_U8:
		{
			FP_READ( readsct.m_U8, char, pfdata );
			//pfdata->read(sizeof(U8), &readsct.m_U8);
		};break;
	case DType_S8:
		{
			FP_READ( readsct.m_S8, char, pfdata );
			//pfdata->read(sizeof(S8), &readsct.m_S8);
		};break;
	case DType_U16:
		{
			FP_READ( readsct.m_U16, short, pfdata );
			//pfdata->read(sizeof(U16), &readsct.m_U16);
		};break;
	case DType_S16:
		{
			FP_READ( readsct.m_S16, short, pfdata );
			//pfdata->read(sizeof(S16), &readsct.m_S16);
		};break;
	case DType_U32:
		{
			FP_READ( readsct.m_U32, int, pfdata );
			//pfdata->read(sizeof(U32), &readsct.m_U32);
		};break;
	case DType_S32:
		{
			FP_READ( readsct.m_S32, int, pfdata );
			//pfdata->read(sizeof(S32), &readsct.m_S32);
		};break;
	case DType_enum8:
		{
			FP_READ( readsct.m_Enum8, char, pfdata );
			//pfdata->read(sizeof(U8), &readsct.m_Enum8);
		};break;
	case DType_enum16:
		{
			FP_READ( readsct.m_Enum16, short, pfdata );
//			pfdata->read(sizeof(U16), &readsct.m_Enum16);
		};break;
	case DType_F32:
		{
			FP_READ( readsct.m_F32, float, pfdata );
			//pfdata->read(sizeof(F32), &readsct.m_F32);
		};break;
	case DType_F64:
		{
			FP_READ( readsct.m_F64, double, pfdata );
			//pfdata->read(sizeof(F32), &readsct.m_F64);
		};break;
	}

	readsct.m_Type = m_DataTypes[m_CurrentColumnIndex];
	m_CurrentColumnIndex = (m_CurrentColumnIndex + 1) % ColumNum;
	readstep--;
	return TRUE;
}
