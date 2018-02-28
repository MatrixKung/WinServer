#ifndef _GUARANTEEDEVENT_
#define _GUARANTEEDEVENT_

#include <vector>

//Ray: �����������ȷ��������֮��һЩ��Ҫ������ͬ����

const int EVENT_VER = 100;
const int MAX_MAIL_ZIP_SIZE = 4096;

//�����������ɾ����ֻ�����ӣ�����Ӱ�����ݿ����
enum GEventType
{
	invalidEvent = 0,

	deleteMailItem,		//ɾ���ʼ���Ʒ
	deleteLogicEvent,	//ɾ�������¼�
	sendMail,			//�����ʼ�
	storeBuyItem,		//�̳ǹ���
	fixedStallBuyItem,	//���̹���


	//��Ҫת��������
	receiveMailItem,	//��ȡ�ʼ���Ʒ
	receiveLogicEvent,  //��ȡ�¼�

	totalEvent,
};

enum GEventErr
{
	GE_NONE_ERROR,				//û����
	GE_MAX_SIZE_ERROR,			//���������гߴ�
	GE_OVERLAP_ERROR,			//�ظ���һ������
	GE_TIMEOUT_ERROR,			//�������,�����Ѿ�������һ������ִ�е�ɾ�������ַ���һ����������᷵���������
	GE_INVALID_EVENT,			//��Ч���¼�����
	GE_UNKNOW_ERROR,			//δ֪����
};

struct stLogicData
{
	char mTypeA;
	int  mValueA1;
	int  mValueA2;
	char mTypeB;
	int  mValueB1;
	int  mValueB2;
};

struct stBlobData
{
	int mSize;
	char mBuf[MAX_MAIL_ZIP_SIZE];
};

struct GEventData
{
	int mSrcPlayerId;		//�������
	int mDestPlayerId;		//Ŀ�����
	int	mLogicId;			//������Ҫ�õ���ID�������ʼ�ID�������¼�ID��

	GEventData()
	{
		memset(this,0,sizeof(GEventData));
	}

	union 
	{
		stLogicData mData;		//�����߼�����
		stBlobData  mMail;		//�����ķ����ʼ����ݣ������ʼ����⣬���ݣ��ʼ���Ʒ����Ǯ��
	}uData;
};

//����಻�����д����ֹ�߼�������ʶ�ܸ�
class GEvent		
{
	friend class CGuaranteedEventMgr;

protected:
	int			ver;		//�汾�ţ��������ݿ�����������
	int			uid;		//�¼��ĵ�����ţ�����������������
	int			sqid;		//�¼��ط������е�����
	GEventType	mType;

public:
	GEventData  mEventData;

public:
	GEvent();

public:
	//��д���淽��
	virtual bool packData(void *bitStream);
	virtual bool unpackData(void *bitStream,int ver);
	virtual void onSendData(void *obj) = 0;
	virtual bool onReceiveData(void *obj,int err) = 0;
};

#endif