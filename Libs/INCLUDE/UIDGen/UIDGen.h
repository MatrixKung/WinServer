#ifndef _UIDGEN_H_
#define _UIDGEN_H_

/************************************************************************/
/* ���㵱ǰ�ĳ�������л����ı�ʶ
/************************************************************************/

#define UID_VERSION   0x10
#define UID_FILE_MARK 0xaf03102f

//�Ƚ�keyֵ�Ƿ��ע������һ��
extern bool IsUIDMatch(void);
extern const char* CalUID(void);

#endif /*_UIDGEN_H_*/