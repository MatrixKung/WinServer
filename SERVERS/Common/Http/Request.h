/** 
@file  
@brief	QWBlogAPI�������ȡ���ݵĽӿ�
@version	2010-11-15	

*/

#pragma once
#include <string>
#include "Param.h"
#include "JSON/JsonParser.h"
#include "Http.h"
#include <map>

enum  EHttpMethod
{
	EHttpMethod_Get,
	EHttpMethod_Post,
};

/// �첽����ص��ӿ�
class  VCallback
{
public:
	/// �첽����ص�����
	/** 
		@param  eHttpRetCode    in :    Http������
		@param  pReqUrl			in :	����URL
		@param  oParam			in :	�����������Ĳ��������
		@param  pData			in :    ���ص�����
		@param  nLen            in :    ���ص����ݳ���
	*/	
	virtual void OnRequestComplete(HTTP_RESULTCODE eHttpRetCode, const char* pReqUrl, CParam& oParam, const char* pData, int nLen) = 0;
};

struct tagRequest
{
	std::string	strUrl;
	CParam		oParam;
	VCallback*	pCallback;
};


class  CRequest 
{
public:	
	~CRequest();
	static CRequest* GetInstance(); 

	
	/** ͬ����������
	    @param  strUrl              in :  ����url
		@param	eHttpMethod			in :  ���������
		@param  oQWBlogParam		in :  �����������Ĳ��������
		@param  strData				out ��ͬ�����ص�����

		@retval		HTTPRESULT_OK:�ɹ�	����:ʧ��
	*/
	HTTP_RESULTCODE SyncRequest(std::string strUrl, EHttpMethod eHttpMethod,  
								CParam& oQWBlogParam, std::string& strData);
	

	/** ͬ����������
	    @param  strUrl              in :  ����url
		@param	eHttpMethod			in :  ���������
		@param  oJsonParser			in :  ������jason����
		@param  strData				out ��ͬ�����ص�����

		@retval		HTTPRESULT_OK:�ɹ�	����:ʧ��
	*/
	HTTP_RESULTCODE SyncHttpsRequest(std::string strUrl, EHttpMethod eHttpMethod,  
								JsonParser& oJsonParser, std::string& strData);

	/** ͬ����������
	    @param strUrl           in : ����url
		@param eRequest			in : ���������
		@param oQWBlogParam		in : �����������Ĳ��������
		@param pCallback		in ���ص�ָ��

		@retval		true:�ɹ�	false:ʧ��
	*/
	bool AsyncRequest(std::string strUrl, EHttpMethod eHttpMethod,  
						CParam& oQWBlogParam, VCallback* pCallback);



	/// http��ɻص�����,  �̳������ʵ�ָú���������ϴ����ؽ����֪ͨ
	/** 
		@param pHttp			in�� CHttp����
		@param eErrorCode		in�� ״̬���룬ȡֵHTTP_RESULTCODE
		@param strData			out���������
	*/	
	virtual void OnHttpComplete(CHttp* pHttp, HTTP_RESULTCODE eErrorCode);


private:
	CRequest();
	CRequest(const CRequest& other);
	CRequest& operator=(const CRequest& other);

	std::string GetBaseUrl(std::string& strUrl, CParam& oParam);

private:
	typedef std::map<CHttp*, tagRequest> MapHttp2Request;
	MapHttp2Request m_mapHttp2Request;
};