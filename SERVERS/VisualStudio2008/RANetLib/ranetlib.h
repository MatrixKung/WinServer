#ifndef _RANETLIB_H_
#define _RANETLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RANETLIB_EXPORTS
#define RANETLIBDECL __declspec( dllexport )
#else
#define RANETLIBDECL
#endif    

#define RALIB_VERSION "1.0.0"

//�������ӽ����ͶϿ�
//ip portΪremote�������Ͽ��Ÿ���ǰ����ĵ�ַ
extern RANETLIBDECL bool RA_Connect(const char* ip,int port);
extern RANETLIBDECL void RA_Disconnect(void);

//���ʺŽ������߲���,�����ص�ǰ�����ľ��
//@svrId �ʺ����ڵ���,���Ϊ0,������������в���,�������ֻ�ܷ���1��

extern RANETLIBDECL int RA_KickAccount(int svrId,const char* name);

//��ȡ�ʺŵ����߽��
enum RAKickRespond
{
    E_KICK_NORESP = 0, //���������ڣ��޷���ѯ
    E_KICK_WAITTING,   //���ڵȴ�����������

    E_KICK_TIMEOUT,    //���߲�����ʱ
    E_KICK_KICKED,     //������
    E_KICK_NOTFOUND,   //�ʺŲ�����Ϸ��
    E_KICK_UNKNOWN,
};

extern RANETLIBDECL int  RA_GetKickRespond(int seq,bool isRemoved = true);

//����������߽��
extern RANETLIBDECL void RA_ClearKickResponds(void);




#ifdef __cplusplus
}
#endif

#endif /*_RANETLIB_H_*/