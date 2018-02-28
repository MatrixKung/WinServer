/** 
@file  
@brief	http������
@version	2010-11-19	

*/

#pragma once
#include <map>
#include "curl/curl.h"

enum HTTP_RESULTCODE
{
	HTTPRESULT_OK   = 0,		///<  �ɹ�	
	HTTPRESULT_TIMEOUT,         ///<  ����ʱ
	HTTPRESULT_CANTRESOLVE,     ///<  ���ܽ�����ַ
	HTTPRESULT_CANTCONNECT,     ///<  ��������
	HTTPRESULT_ERROR,           ///<  HTTP��������ݴ��� 
	HTTPRESULT_TOOLAGRE,        ///<  ���ص�������̫��
	HTTPRESULT_FAIL,		    ///<  ʧ��
	HTTPRESULT_OPITON_ERR		///���ʹ����޷�������Чurl
};

class  CHttp
{
public:
	CHttp();
	~CHttp();	

public:
	/// ͬ������
	/** 
	@param strUrl		in:   ���ص�url	
	@param strResult	out:  �������

	@retval  HTTPRESULT_OK:�ɹ�   HTTPRESULT_FAIL:ʧ�� 
	*/	
	HTTP_RESULTCODE	SyncDownload(const std::string& strUrl, std::string& strResult);

	/// �첽����
	/** 
	@param strUrl		in:  ���ص�url
	@param pCallback	in:  ��ȡ�ص���ָ��
	*/	
	bool	AsyncDownload(const std::string& strUrl);

	/// ͬ���ϴ�
	/** 
	@param strUrl		in:   �ϴ���url
	@param strData		in:   �ϴ�������
	@param strResult	out:  �������

	@retval  HTTPRESULT_OK:�ɹ�   HTTPRESULT_FAIL:ʧ�� 
	*/	
	HTTP_RESULTCODE	SyncUpload(const std::string& strUrl, const std::string& strData, std::string& strResult);

	/// �첽�ϴ�
	/** 
	@param strUrl		in:	�ϴ���url
	@param strData		in:	�ϴ�������
	@param pCallback	in:	��ȡ�ص���ָ��
	*/	
	bool	AsyncUpload(const std::string& strUrl, const std::string& strData);

	/// ͬ���ϴ�������enctype="multipart/form-data"
	/** 
	@param strUrl		in:	�ϴ���url
	@param mapData		in:	�ϴ����ֶ�����
	@param mapFile		in:	�ϴ����ļ���
	@param strResult	out: �������

	@retval  HTTPRESULT_OK: �ɹ�   HTTPRESULT_FAIL: ʧ�� 
	*/	
	HTTP_RESULTCODE	SyncUploadFormData(const std::string &strUrl, const std::map<std::string, std::string>& mapData,
										const std::map<std::string, std::string>& mapFile, std::string& strResult);

	/// �첽�ϴ�������enctype="multipart/form-data"
	/** 
	@param strUrl		in:	�ϴ���url
	@param strData		in:	�ϴ�������
	@param pCallback	in:	��ȡ�ص���ָ��
	*/	
	bool	  AsyncUploadFormData(const std::string& strUrl,const std::map<std::string, std::string>& mapData, 
									const std::map<std::string, std::string>& mapFile);


public:
	static    size_t	ProcessResult(void* data, size_t size, size_t nmemb, void* pHttp);


public:
	void			Reset();

	CURLcode		DoHttpGet();
	CURLcode		DoHttpPost();
	CURLcode		DoHttpsPost();
	CURLcode		DoHttpPostFormData();	
	

	bool			WriteResultData(char* pchData, unsigned long uLength);
	bool			GetResultData(std::string& strData);

	//��һ���б������Դ
	void	  recoveryCurlRecourse(CURL* );

public:
	bool			m_bRunning;
	
private:
	char			errorInfo[CURL_ERROR_SIZE];
	std::string		m_strUrl;   
	std::string		m_strData;	 ///<  Post��������

	typedef	std::map<std::string, std::string>    CMapKeyValue;	
	CMapKeyValue	m_mapPostFile;   ///<  ��form-data��ʽ�ϴ����ļ���ͼƬ
	CMapKeyValue	m_mapPostData;   ///<  ��form-data��ʽ�ϴ���post����

	
	char*			m_pchResultData; ///<  �ϴ������صĽ�����
	unsigned long	m_uLength;		 ///<  �ϴ������صĽ����ŵ�BUF�ĳ���
	unsigned long	m_uWritePos;	 ///<  �ϴ������صĽ������buf�ĵ�ǰ��дλ��

};