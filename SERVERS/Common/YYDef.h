#pragma once
enum EM_YYERROR
{
	EM_YYERROR_START = -1,
	EM_YYERROR_NONE,
	EM_YYERROR_NOTARGETONLINE,//Ŀ����Ҳ�����
	EM_YYERROR_END,
};
#define YYPROCESSNAME "yyhzh.exe"
#define MAXWAITTIME 30000    //������ʱ�����ޣ����ж�ΪDLL�ڲ�����ֹͣ��ʼ�����֣���ʼ������
enum EM_YYINITTYPE
{
	EM_YYINIT_NONE = 0,
	EM_YYINIT_PRIVATE,  //��ʼ�����Ժ����˽�˻Ự����
	EM_YYINIT_TEAM,			//������������
	EM_YYINIT_MAIN,		//YY���洰�ڵ������洰��
	EM_YYINIT_FAMILY,   //������������
	EM_YYINIT_END,
};