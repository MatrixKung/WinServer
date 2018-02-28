/** 
@file  
@brief		����Ĳ�����,�����������װ����������URL����Ĳ�����ʽ��װ��
@version	2010-11-16		

**@exmaple:
format=json&pageflag=0&reqnum=20

CParam oParam;
oParam.AddParam(TXWeibo::Param::strFormat,TXWeibo::Param::strJson);
oParam.AddParam("pageflag","0");
oParam.AddParam("reqnum","20");
*
*/

#pragma once
#include <map>
#include <string>

class CParam
{
public:
	CParam();
	~CParam();

	///���Ӳ���
	/**
	@param prKey   in : ������
	@param pValue  in : ����ֵ
	*/
	void	AddParam(const char* pKey, const char* pValue);

	///��ȡ����
	/**
	@param pKey  in : ������
	*/
	/*����ֵ�����ReleaseData�ͷ�*/
	char*	GetParam(const char* pKey);

	///����ͼƬ������
	/**
	@param pKey    in : ��������
	@param pValue  in : ����ֵ
	*/
	void	AddPicNameParam(const char* pKey, const char* pValue);
	///�õ�ͼƬ��
	/**
	@param pKey  in : ������
	*/
	/*����ֵ�����ReleaseData�ͷ�*/
	char*	GetPicNameParam(const char* pKey);

	/// �õ���װ�õĲ������ַ���
	/*����ֵ�����ReleaseData�ͷ�*/
	char*	GetUrlParamString();

	/// ������в���������CParam��������ظ�ʹ��
	void	Clear();

public:
	typedef	std::map<std::string, std::string>	MapKey2Value;
	
	MapKey2Value& GetParamMap();
	MapKey2Value& GetPicParamMap();

protected:
	MapKey2Value m_mapParam;		///< ���������map���Ѳ�����ӳ�䵽����ֵ
	MapKey2Value m_mapPicParam;		///< ����ͼƬ����map�����ļ�������ӳ�䵽�ļ���
};