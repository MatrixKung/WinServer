#ifndef _RALIBDEF_H_
#define _RALIBDEF_H_

enum RANetMessage
{
    RA_MSG_KICK_ACCOUNT = 100,
    RA_MSG_KICK_ACCOUNT_RESP,
};

enum KickRespond
{
    E_KICKRESP_KICKED = 3,  //������
    E_KICKRESP_NOTFOUND,   //�ʺŲ�����Ϸ��
};

#endif /*_RALIBDEF_H_*/