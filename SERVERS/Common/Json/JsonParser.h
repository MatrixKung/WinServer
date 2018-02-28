/** 
@file  
@brief		json����ʵ����
@version	2010-11-18		
*/

#pragma once
#include "json.h"
#include "string.h"

class JsonParser
{
public:
	JsonParser();
	JsonParser(Json::Value* pValue);
	~JsonParser();

	/// �жϵ�ǰparser�Ƿ�Ϊreadonly��Ϊreadonlyʱ�����ܵ���Parse�ӿڣ�����ʧ��
	bool	IsReadyOnly();

	/// ����json�ļ�
	/*
	@param	pJsonContent	in��json�ַ���������Ϊutf8
	*/
	bool	Parse(const char* pJsonContent);

	/// �������ֻ�ȡ�ӽڵ�
	/*
	@param	pName	in���ӽڵ�����
	*/
	JsonParser	GetChild(const char* pName); 

	/// ������Ż�ȡ�ӽڵ�
	/*
	@param	nIndex	in���ӽڵ����
	*/
	JsonParser	GetChild(const int nIndex);

	/// �������ֻ�ȡ�ӽ��ַ���
	/*
	@param	pName	in���ӽڵ�����
	*/
	/*����ֵ�����ReleaseData�ͷ�*/
	std::string	GetString(const char* pName);

	/// ������Ż�ȡ�ӽڵ��ַ���
	/*
	@param	nIndex	in���ӽڵ����
	*/
	/*����ֵ�����ReleaseData�ͷ�*/
	std::string	GetString(const int nIndex);

	/// �������ֻ�ȡ�ӽڵ㲼��ֵ
	/*
	@param	pName	in���ӽڵ�����
	*/
	bool	GetBool(const char* pName);

	/// ������Ż�ȡ�ӽڵ㲼��ֵ
	/*
	@param	nIndex	in���ӽڵ����
	*/
	bool	GetBool(const int nIndex);

	/// �������ֻ�ȡ�ӽڵ�����ֵ
	/*
	@param	pName	in���ӽڵ�����
	*/
	int		GetInt(const char* pName);

	/// ������Ż�ȡ�ӽڵ�����ֵ
	/*
	@param	nIndex	in���ӽڵ����
	*/
	int		GetInt(const int nIndex);

	/// �������ֻ�ȡ�ӽڵ�Ƿ�������ֵ
	/*
	@param	pName	in���ӽڵ�����
	*/
	unsigned int	GetUint(const char* pName);

	/// ������Ż�ȡ�ӽڵ�Ƿ�������ֵ
	/*
	@param	nIndex	in���ӽڵ����
	*/
	unsigned int	GetUint(const int nIndex);

	/// �������ֻ�ȡ�ӽڵ�64λ�Ƿ�������ֵ
	/*
	@param	pName	in���ӽڵ�����
	*/
	long long	GetUint64(const char* pName);

	/// ������Ż�ȡ�ӽڵ�64λ�Ƿ�������ֵ
	/*
	@param	nIndex	in���ӽڵ����
	*/
	long long	GetUint64(const int nIndex);

	/****************************************************************/
	// add by natural
	// ��ӽڵ�
	void AddNode(std::string key, std::string value);

	// ��ȡJason�ַ���
	std::string GetJasonString();

	template<typename T>
	Json::Value GetValue(T b)
	{
		Json::Value oValue(Json::Value::null);
		if(m_pRoot && Json::Value::null != (*m_pRoot)[b])
			oValue = (*m_pRoot)[b];
		return oValue;
	}

private:
	static JsonParser	GetReadOnlyReader(Json::Value* pRoot);
	Json::Value*		GetChildByName(std::string& strName);
	Json::Value*		GetChildByIndex(const int nIndex);


private:	
	Json::Value*		m_pRoot;		///<  json�ļ����ڵ�
	bool				m_bReadOnly;	///<  �Ƿ�ֻ��
	bool				m_bDeleteSelf;	///<  �Ƿ���Ҫ�Լ������ڴ��ͷ�
};