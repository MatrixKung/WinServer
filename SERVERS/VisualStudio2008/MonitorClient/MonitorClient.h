#ifndef _MONITORCLIENT_H_
#define _MONITORCLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MONITORCLIENT_EXPORTS
#define MONITORDECL __declspec( dllexport )
#else
#define MONITORDECL
#endif    

#define MONITOR_DLL_VERSION "1.0.0"

//runStatusֵ����
//0 : �������� 
//1 : ����ر�(���������رպͶ�ʱ�ر�)
//2 : �յ���ʱ�ر�����

extern MONITORDECL bool Monitor_Init(unsigned int& runStatus);
extern MONITORDECL bool Monitor_Close(void);

//�ڶ�ʱ�ر�ģʽ�£���ȡ��رյ�ʣ��ʱ��
extern MONITORDECL int  Monitor_GetCloseSeconds(void);

#ifdef __cplusplus
}
#endif

#endif /*_MONITORCLIENT_H_*/