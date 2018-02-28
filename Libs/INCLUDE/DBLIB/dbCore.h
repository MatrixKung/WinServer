#pragma once

#ifndef _DB_CORE_H
#define _DB_CORE_H

#include <hash_map>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#define DBNTWIN32
#include "sqlfront.h"
#include "sqldb.h"

#include "base/Locker.h"

#define DBE_DATA_BUFFER		8192
#define DBE_SQL_BUFFER		32767
#define DBE_CHUNK_SIZE		1024
#define DBE_LASTSQL_BUFFER	2048
#define PTAIL(x)			x+strlen(x)

//ʱ���
typedef struct tagTimeStamp
{
	unsigned short	year;
	unsigned char	month;
	unsigned char	day;
	unsigned char	hour;
	unsigned char	minute;
	unsigned char	second;
	unsigned short	millisecond;
	tagTimeStamp()
	{
		year = 1900;
		month = 1;
		day = 1;
		hour = 0;
		minute =0;
		second = 0;
		millisecond = 0;
	};
}TimeStamp;

enum eStatus
{
	E_NONE,
	E_SET_SQL,
	E_EXE_SQL,
};

// ============================================================================
//  CDBCore
//		ʹ��DB-Library����SQLServer���ݵĻ��࣬��������������Ӷ���
// ============================================================================
class CDBCore
{
	int m_InnerIndex;
public:
	CDBCore(void);
	virtual ~CDBCore(void);

public:
	bool Open(char *svr,char *usr,char *pwd,char *db);		//�����ݿ�����
	void Close();								//�ر����ݿ�����

	void SQL(const char * cmd,...);				//׼��ִ��SQL(�ɱ������ʽ)
	void SQLExt(const char* cmd);				//׼��ִ��SQL(�ǿɱ������ʽ)
	long Exec();								//ִ��SQL������䣬���������
	bool More();								//ִ��SQL��䲢ȡ�������
	bool Eval();								//ִ��SQL���,���ؽ�����ȴ����� ��Ҫ���GetBLOBDataʹ��
	bool Cancel();								//ȡ����ǰ�������ִ�м����д��ڵȴ�״̬�Ľ������
	bool Clone(CDBCore * pDB);					//��¡��ǰ����

	bool Fetch();								//ִ��SQL��䣬����GetRecordSetIndex�ɴ��������

	void*	GetHandle(){return m_pdbHandle;}
public:
	//���ؽ���е�����
	//@nIndex ���������,��0��ʼ
	//@strType ����е�����
	int		GetColType(int nIndex);
	void	GetColType(int nIndex, char * strType);
	//���ؽ���е�����
	//@nIndex ����е�����,��0��ʼ
	const char * GetColName(int nIndex);
	//���ؽ���еĳ���
	//@nIndex ����е�����,��0��ʼ
	int		GetColLen(int nIndex);
	//���ؽ�������ݳ���
	//@nIndex ����е�����,��0��ʼ
	int		GetColDataLen(int nIndex);
	//���ؽ���е�����
	int		GetNumCols();

	// ����δ���Ľ����
	void	DumpResult();

public:
	//��ȡĳһ�е�����
	//@nIndex ָ������У���0��ʼ
	const char * Get(int nIndex);
	const char * Get(const char * pszFieldName);

	inline const char * GetString(int nIndex) 
	{
		const char* ptr = Get(nIndex);
		return ptr ? ptr : "";
	}

	inline const char * GetString(const char * pszFieldName) 
	{
		const char* ptr = Get(pszFieldName);
		return ptr ? ptr : "";
	}

	//��ȡĳһ�е���������
	long	GetLong(int nIndex);
	long	GetLong(const char * pszFieldName);
	inline int GetInt(int nIndex) {return (int)GetLong(nIndex);};
	inline int GetInt(const char * pszFieldName) {return (int)GetLong(pszFieldName);};
	__int64 GetBigInt(const char * pszFieldName);
	//��ȡĳһ�еĸ���������
	double	GetDouble(int nindex);
	double  GetDouble(const char * pszfieldname);

	PBYTE   GetBinary(int nIndex, int &len,PBYTE pBuf=NULL);
	PBYTE   GetBinary(const char * pszFieldName, int &len);

	//Image�ֶεĲ���
	bool	GetBlob(int& len, PBYTE pData, int actlen = 0);
	bool	SetBlob(const char* pszTableName, const char* pszFieldName, PBYTE pData,int len);
	int    SetBlobOpt();

	//��ȡĳһ�е�ʱ��ֵ
	//@nIndex ������,��0Ϊ���
	//@pszFieldName ����
	//@dtfmt ���ص�ʱ���ʽ������Ϊ���¸�ʽ���
	//	%Y	��
	//	%m	��
	//	%D	��
	//	%H	Сʱ
	//	%M	����
	//	%S	��
	//	%MS	����
	TimeStamp	GetDateTime(int nIndex);
	int         GetDateTimeEx(int nIndex);
	TimeStamp	GetDateTime(const char * pszFieldName);
	const char * GetDateTime(int nIndex, const char * dtfmt);
	const char * GetDateTime(const char * pszFieldName, const char * dtfmt);
	//��ѯĳһ��������Ӧ������
	//@pszFieldName ����
	//@@return -1Ϊδ�ҵ���Ӧ��,����ֵΪ������,��0��ʼ
	int			FindIndex(const char * pszFieldName);
	//��ѯ��ǰ�������������ţ���һ��������Ǵ�1��ʼ
	int			GetRecordSetIndex() { return m_nSetCount;}

	//˳���ȡ������
	int         GetTime()   {   return GetDateTimeEx(m_InnerIndex++);}
	int			GetInt()	{	return GetInt(m_InnerIndex++);		 }
	__int64		GetBigInt(int nIndex);
	__int64		GetBigInt() {	return GetBigInt(m_InnerIndex++);}
	const char* GetString()	{	return GetString(m_InnerIndex++);	 }
	long		GetLong()	{	return GetLong(m_InnerIndex++);		 }
	double		GetDouble()	{	return GetDouble(m_InnerIndex++);	 }
	PBYTE		GetBin( int &len,PBYTE pBuf = NULL) { return GetBinary(m_InnerIndex++, len, pBuf);}	
	int			GetColLen() {	return GetColDataLen(m_InnerIndex);	}
public:
	/*
	//���������
	//@pszTransName �������,����ȱʡ����ȱʡ����ֻ��һ����
	//���������������Commit��Rollback��������������
	//����������Ψһ�ԣ��������Ƕ�ס�
	*/
	//��������
	bool BeginTrans(const char * pszTransName = NULL);
	//�ύ����
	bool Commit(const char * pszTransName = NULL);
	//����ع�
	bool Rollback(const char * pszTransName = NULL);

public:
	//������������ֻ��Ϊ�������ݿ⾯��ʹ�����Ϣ����
	void AssignSQLError(int severity, int dberr, int oserr, char *dberrstr, char *oserrstr);
	void AssignSQLMessage(long msgno, int msgstate, int severity, char * msgtext, char * srvname,char *procname, unsigned short line);

protected:
	void			ValidateIndex(int nIndex);		//����к��Ƿ���Ч
	bool			Execute();						//�ڲ�����ִ�к���
	bool			IsValidate();					//�ж����ݿ������Ƿ�Ͽ�
	bool			Relink(bool isOpen = true);						//��������

protected:
	int				e_severity;						//�����������
	int				e_dberr;						//���ݿ����
	int				e_oserr;						//����ϵͳ����
	char			e_dberrstr[256];				//���ݿ������Ϣ
	char			e_oserrstr[256];				//����ϵͳ������Ϣ

	long			w_msgno;						//��ϢID
	int				w_msgstate;						//��Ϣ״̬
	int				w_severity;						//��Ϣ������
	char			w_msgtext[8192];				//��Ϣ�ı�
	char			w_srvname[64];					//��������
	char			w_procname[64];					//������
	unsigned short	w_line;							//��Ϣ����

protected:
	void*			m_pdbHandle;					//���ݿ���

	int				m_nStatus;						//��������
	int				m_nCode;						//SQLִ��״̬
protected:
	char*			m_strSvr;						//��������
	char*			m_strUser;						//�û���
	char*			m_strPwd;						//����
	char*			m_strDB;						//���ݿ�

protected:
	int     	    m_nSetCount;					//��¼������
	int				m_nCount;						//�������

	char 		    m_pSQL[DBE_SQL_BUFFER];			//SQL���
	char 		    m_pData[DBE_DATA_BUFFER];		//����
	char			m_pLastSQL[DBE_LASTSQL_BUFFER];	//��һ��SQL���

protected:
	bool			m_bAutoRelink;					//�Զ�������
};

// ============================================================================
//  CDBManager
//		�������ݿ����ӹ�������ݿ��������
// ============================================================================
class CDBConnPool;
class CDBManager
{
public:
	CDBManager();
	~CDBManager();

	//��ʼ����Ӧ��
	void InitDBLib();	
	//�ͷſ�Ӧ��
	void FreeDBLib();
	//�������ӳ�ʱ
	void SetOpenTimeout(int openTimeout);
	//���ò�ѯ��ʱ
	void SetExecTimeout(int execTimeout);
	//�Ǽ����Ӷ���
	bool Regist(CDBCore *);
	//ע�����Ӷ���
	bool Unregist(CDBCore *);
	//��ȡ���Ӷ���
	CDBCore *GetObject(int);
	//��̬Ψһʵ��
	static CDBManager*	GetInstance()	{return &m_Instance;}

private:
	static CDBManager					m_Instance;		//��̬ʵ��
	bool								m_bLibInited;	//�Ƿ��ʼ��DB-LIBRARY��Ӧ��
	CMyCriticalSection					m_cs;			//�ٽ���

	stdext::hash_map<int,CDBCore *>		m_List;			//�����б�����ȫ��errhandle
	//���������»�ȡDBConn����
};


#endif

