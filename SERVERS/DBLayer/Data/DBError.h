#ifndef __DBERROR_H__
#define __DBERROR_H__

enum DBError
{
	DBERR_NONE,							//�޴���

	DBERR_DB_INITIALIZE,				//��ʼ�����ݿ�����ʧ��
	DBERR_DB_INTERNALERR,				//���ݲ������ڲ��쳣
	DBERR_DB_BEGINTRAN,					//����������ʧ��
	DBERR_DB_COMMITTRAN,				//�ύ������ʧ��
	DBERR_DB_SQLPARAMETER,				//SQL����������

	DBERR_IO_READFILE,					//��ȡ�����ļ�����ʧ��
	DBERR_IO_WRITEFILE,					//д�뱾���ļ�����ʧ��
	DBERR_IO_CREATEFILE,				//���������ļ�ʧ��
	DBERR_IO_OPENFILE,					//�򿪱����ļ�ʧ��
	DBERR_IO_CHECKCRC,					//��鱾���ļ�����CRC����

	DBERR_ACCOUNT_CREATE,				//�����ʺ�ʧ��
	DBERR_ACCOUNT_NOFOUND,				//δ�ҵ����ʺ��û�
	DBERR_ACCOUNT_IDNAMENOMATCH,		//�ʺ�ID���ʺ����Ʋ�ƥ��
	DBERR_ACCOUNT_IDEXISTS,				//�ʺ�ID����
	DBERR_ACCOUNT_NAMEEXISTS,			//�ʺ����ƴ���
	DBERR_ACCOUNT_UPDATEGOLD,			//�ʺ�Ԫ����ֵʧ��

	DBERR_PLAYER_NOTNEWEST,				//��ɫ���ݲ���ת�������°汾
	DBERR_PLAYER_CONVERTPARAMINVAILD,	//��ɫ����ת���������Ϸ�
	DBERR_PLAYER_CONVERTFAILED,			//��ɫ����ת��ʧ��
	DBERR_PLAYER_DEFINEERROR,			//��ɫ�ṹ�汾���岻��ȷ
	DBERR_PLAYER_NOFOUND,				//δ�ҵ�ָ���Ľ�ɫ����
	DBERR_PLAYER_BLOBUNCOMPRESS,		//����������ݽ�ѹ����
	DBERR_PLAYER_BLOBCOMPRESS,			//�����������ѹ������
	DBERR_PLAYER_BLOBTOOBIG,			//���������ݹ���
	DBERR_PLAYER_BLOBLENGTH,			//���������ݳ����޷�ȷ��
	DBERR_PLAYER_NAMEEXISTS,			//��ɫ���Ѿ���ʹ��
	DBERR_PLAYER_TOOMANY,				//��ɫ��̫��(����5��)
	DBERR_PLAYER_NOASSIGNID,			//�޷������ɫID
	DBERR_PLAYER_CREATE,				//������ɫ����ʧ��
	DBERR_PLAYER_UNDELETE,				//�ָ�ɾ����ɫʧ��

	DBERR_MAIL_UPDATEFAILED,			//�����ʼ�����ʧ��
	DBERR_MAIL_RECVERID,				//�ռ���ID����
	DBERR_MAIL_RECVERNAME,				//�ռ����ƴ���
	DBERR_MAIL_TOOMANY,					//�ʼ������ﵽ����120��
	DBERR_MAIL_NOFOUND,					//δ�ҵ����ʼ�����

	DBERR_INVENTORY_UPDATE,				//���°�������Ʒ����ʧ��
	DBERR_EQUIPLIST_UPDATE,				//����װ������Ʒ����ʧ��
	DBERR_BANKLIST_UPDATE,				//�������вֿ���Ʒ����ʧ��

	DBERR_BUFF_UPDATEBUFF,				//��������BUFF����ʧ��
	DBERR_BUFF_UPDATEDEBUFF,			//���¼���BUFF����ʧ��
	DBERR_BUFF_UPDATESYSBUFF,			//����ϵͳBUFF����ʧ��
	DBERR_BUFF_UPDATESCHEDULEBUFF,		//���µ���BUFF����ʧ��

	DBERR_COOLDOWN_UPDATE,				//������ȴʱ�������ʧ��
	DBERR_FAME_UPDATE,					//������������ʧ��
	DBERR_LIVESKILL_UPDATE,				//�������������ʧ��
	DBERR_PRESCRIPTION_UPDATE,			//�����䷽����ʧ��

	DBERR_MISSION_UPDATE,				//�����������ݴ���
	DBERR_MISSION_UPDATEFLAG,			//��������������ݴ���

	DBERR_PET_UPDATE,					//������������ʧ��
	DBERR_MOUNT_UPDATE,					//�����������ʧ��
	
	DBERR_PANELLIST_UPDATE,				//���¿��������ʧ��
	DBERR_SPLITLIST_UPDATE,				//������Ʒ�ֽ�������ʧ��
	DBERR_COMPOSELIST_UPDATE,			//������Ʒ�ϳ�������ʧ��

	DBERR_SKILLLIST_UPDATE,				//���¼���������ʧ��
	DBERR_SOCIAL_UPDATE,				//��������ϵ����ʧ��
	DBERR_SPIRIT_UPDATE,				//����Ԫ������ʧ��

	DBERR_TIMER_UPDATEITEM,				//����������Ʒ��ʱ��ʧ��
	DBERR_TIMER_UPDATEPETSTUDY,			//���³������м�ʱ��ʧ��

	DBERR_TIMESET_UPDATE,				//���»������ʱЧ��������ʧ��
	DBERR_MPLIST_UPDATE,				//�����ʦ����ʧ��
	DBERR_LOGICEVENT_SAVE,				//����ϵͳ�߼��¼�ʧ��
	DBERR_ZONESETTING_SAVE,				//�����ͼ��������ʧ��

	DBERR_ORG_MASTEREXIST,				//�����峤�Ѿ�����
	DBERR_ORG_NAMEEXIST,				//�������Ѿ�����
	DBERR_ORG_DISBAND,					//��ɢ�������ʧ��

	DBERR_TOPPETBLOB_NOFOUND,			//���а�����blob����δ���ҵ�
	DBERR_TOPITEMBLOB_NOFOUND,			//���а�����blob����δ���ҵ�

	DBERR_LEAGUE_MASTEREXIST,			//���������Ѿ�����
	DBERR_LEAGUE_NAMEEXIST,				//���������Ѿ�����
	DBERR_LEAGUE_DISBAND,				//��ɢ�������ʧ��

	DBERR_GOLDTRADE_REDUCEGOLD,			//Ԫ�����׹ҵ�Ԥ��Ԫ��ʧ��
	DBERR_GOLDTRADE_RETURNGOLD,			//Ԫ���ҵ���������Ԫ��ʧ��
	DBERR_GOLDTRADE_NOFOUND,			//Ԫ���ҵ��ѳ���

	DBERR_GOLDEXCHAGE_NOFOUND,			//Ԫ���һ����ݲ�ѯδ��
	DBERR_GOLDEXCHAGE_CONFIG_NOFOUND,	//Ԫ���һ��������ݲ�ѯδ��
	DBERR_GOLDEXCHANGE_DRAWFAILED,		//��ȡԪ���һ�ʧ��

	DBERR_AUCTION_NOFOUND,				//��������Ʒ��ѯδ��
	DBERR_AUCTION_DONTBUYSELF,			//�������ܶ��Լ���Ʒ����
	DBERR_AUCTION_FIXEDPRICEERR,		//����۸�ƥ��

	DBERR_UNKNOWERR,					//δ֪�Ĵ���
	DBERR_MAX,
};

extern const char* getDBErr(DBError e);

class ExceptionResult
{
public:
	ExceptionResult(DBError e) { code = e;}
	virtual ~ExceptionResult() {}
	const char* str() {return getDBErr(code);}
	DBError code;			//���ش�����
};

#endif//__DBERROR_H__
