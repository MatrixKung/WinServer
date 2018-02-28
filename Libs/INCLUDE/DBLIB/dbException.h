#ifndef _DB_EXCEPTION_H
#define _DB_EXCEPTION_H

class CDBException
{
public:
	CDBException();
	CDBException(int severity, int dberr, int oserr, const char * dberrstr,const char * oserrstr);
	CDBException(int severity, int dberr, int oserr, const char * dberrstr,const char * oserrstr,
		long msgno, int msgstate, int _severity, const char * msgtext, const char * srvname
		,const char *procname, unsigned short line);
	CDBException(long msgno, int msgstate, int _severity, const char * msgtext, const char * srvname
		,const char *procname, unsigned short line);
	virtual ~CDBException();

public:
	int         m_severity;			//�����������
	int         m_dberr;			//���ݿ����
	int         m_oserr;			//����ϵͳ����
	char        m_dberrstr[256];	//���ݿ������Ϣ
	char        m_oserrstr[256];	//����ϵͳ������Ϣ

public:	//[��չ]�����쳣�ľ�����Ϣ
	long        w_msgno;			//��ϢID
	int         w_msgstate;			//��Ϣ״̬
	int         w_severity;			//��Ϣ������
	char        w_msgtext[8192];	//��Ϣ�ı�
	char        w_srvname[64];		//��������
	char        w_procname[64];		//������
	unsigned short w_line;			//��Ϣ����
};

#endif//_DB_EXCEPTION_H