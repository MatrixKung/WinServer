
#ifndef _AUTH_LIB_ERROR_CODE_H_
#define _AUTH_LIB_ERROR_CODE_H_

// ����������
#define ERROR_OK                             0   /* �ɹ� */
#define ERROR_UNSUPPORTED_PROTOCOL           101 /* ��֧�ֵ�Э�� */
#define ERROR_UNSUPPORTED_PROTOCOL_VERSION   102 /* �����ݵ�Э��汾 */
#define ERROR_CHECKSUM                       103 /* У��ʹ��� */
#define ERROR_ENCRYPTED                      104 /* ���ܴ��� */
#define ERROR_DECRYPTED                      105 /* ���ܴ��� */
#define ERROR_DISCONNECTED                   106 /* ���������ж� */
#define ERROR_DISPATCH                       107 /* ��Ϣת������ */

#define ERROR_INVALID_SERVICE                151 /* ���Ϸ��ķ��� */
#define ERROR_INVOKE_SERVICE                 152 /* ���÷������ */
#define ERROR_INVALID_SITE                   153 /* δ֪��վ�� */

#define ERROR_OVERLOAD                       9998 /* ϵͳ��æ */
#define ERROR_UNKNOWN                        9999 /* δ֪���� */

// ���ڴ�����
#define ERROR_PENDING                        901  /* �������ڴ����� */
#define ERROR_NO_RESULT   ERROR_PENDING           /* ��ʱû�з��ؽ�� */


// ҵ�������
#define ERROR_MEM                            1002 /* �ڴ������� */
#define ERROR_OUT_SERVICE                    1003 /* ���ò����ڷ��� */
#define ERROR_UNKOWN_TXN                     1005 /* �����벻���� */
#define ERROR_PARA                           1006 /* �������� */
#define ERROR_FILE                           1007 /* �����ļ�ʧ�� */
#define ERROR_IN_OVERFLOW                    1008 /* ������ϢԽ�� */
#define ERROR_MEMCACHED                      1011 /*����memcached����*/

#define ERROR_NO_PROTECT_METHOD              9000 /*�û�û�а󶨰�ȫ����*/
#define ERROR_TIMESTAMP_EXPIRED              9001 /*ʱ����ѹ���*/
#define ERROR_BAN_FOR_PASSWD_ERROR           9002 /*�û����������������������*/
#define ERROR_GET_BASIC_FROM_DB              9003 /*�����ݿ��ȡ�û�������Ϣʧ��*/
#define ERROR_ERROR_PASSWD                   9004 /*�������*/
#define ERROR_GEN_TICKET                     9005 /*����ticket����*/
#define ERROR_GET_PROTECT_STATUS             9006 /*��ȡ�û���ȫ����״̬����*/
#define ERROR_PASSWD_DECRYPT                 9007 /*�������ʧ��*/
#define ERROR_MD5                            9008 /*MD5ʧ��*/
#define ERROR_LOGINNAME_NOT_EXIST            9009 /*�û���������*/
#define ERROR_ERROR_TICKET                   9010 /*ticket����*/
#define ERROR_GAME_NOT_ACTIVE                9011 /*�û�û�м�����Ϸ��*/
#define ERROR_GAME_PARTITION_BAN             9012 /*��Ϸ������ͣ*/
#define ERROR_TICKET_EXPIRED                 9013 /*ticket����*/
#define ERROR_INVALID_CERT_ID                9014 /*��������֤��ʽ*/
#define ERROR_CACHE_SET                      9015 /*дcacheʧ��*/
#define ERROR_FCM_CANT_ACTIVE_MORE_PARTITION 9016 /*�������û�ֻ�ܼ���һ����Ϸ��һ������*/
#define ERROR_NO_MATRIX_SERVICE              9017 /*�û�û�п�ͨ�ܱ�������*/
#define ERROR_GET_GAME_CONFIG                9018 /*�û���Ϸ����ʧ��*/
#define ERROR_INVALID_SITECD                 9019 /*�Ƿ�����Ϸ*/
#define ERROR_INVALID_GAME_PARTIITON_SERVER  9020 /*�Ƿ�����Ϸ����*/
#define ERROR_ERROR_MATRIX_CODE              9021 /*�ܱ�����̬λ����֤ʧ��*/
#define ERROR_NO_TOKEN_SERVICE               9022 /*�û�û�п�ͨ���Ʒ���*/
#define ERROR_MATRIX_NOT_EXIST               9023 /*�ܱ���������*/
#define ERROR_TOKEN_NOT_EXIST                9024 /*���Ʋ�����*/
#define ERROR_TOKEN_CODE                     9025 /*���ƶ�̬������֤ʧ��*/
#define ERROR_MATRIX_GEN_CODE                9026 /*�����ܱ�����̬λ�ô���*/
#define ERROR_YAUTH_INIT                     9027 /*YAuth_init��ʼ��ʧ��*/
#define ERROR_MUTEX_LOCK                     9028 /*�߳�lock����*/
#define ERROR_MUTEX_UNLOCK                   9029 /*�߳�unlock����*/
#define ERROR_VERIFY_YY_TOKEN                9030 /*��֤YYtokenʧ��*/
#define ERROR_INSERT_YYUSER                  9031 /*�����µ�yy�û�����*/
#define ERROR_PASSWD_ENCRYPT                 9032 /*�������ʧ��*/

#endif //_AUTH_LIB_ERROR_CODE_H_

