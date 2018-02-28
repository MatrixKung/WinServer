#ifndef _BONUSBASE_H_
#define _BONUSBASE_H_

#define	MAX_PAGE_BONUS_ITEM     12            //ÿҳ����������
#define MAX_BONUS_ITEM_NUM      10            //ÿ��������10����Ʒ
#define MAX_ITEMBONUS_COUNT     48            //һ�������50��
#define MAX_BONUS_CODESTRING    100           //code��󳤶�

enum enBonusType
{
	BONUSTYPE_NONE = 0,
	BONUSTYPE_VIP,              // VIP
	BONUSTYPE_ACTIVITY,         // �
	BONUSTYPE_SERIAL,           // code��ȡ
	BONUSTYPE_OTHER,            // ����
	BONUSTYPE_MAXCOUNT,
};

enum enBonusDB
{
	BONUS_DB_NONE = 0,        // ������ȡ
	BONUS_DB_ISGET,           // �ѱ���ȡ
	BONUS_DB_ONFIND,          // δ�ҵ���¼
	BONUS_DB_PLAYERERROR,     // ��ɫ��ƥ��
	BONUS_DB_UNKNOWN,         // δ֪����
};

enum enReceiveFlag
{
	RECEIVE_CAN = 0,         //������ȡ
	RECEIVE_CANNOT,          //��������ȡ��������
	RECEIVE_END,             //�Ѿ���ȡ
};

struct stBonusItem
{
	int            uId;            // ������
	int            itemId;         // ��ƷId
	int            receiveFlag;    // �Ƿ���ȡ
	char           szDesc[512];    // ����
	unsigned char  type;           // ����
	unsigned short quantity;       // ��Ʒ����

	void copy(const stBonusItem& other)
	{
		memcpy(this,&other,sizeof(stBonusItem));
	}

	stBonusItem()
	{
		memset(this,0,sizeof(stBonusItem));
	}

	stBonusItem(const stBonusItem& other)
	{
		copy(other);
	}

	stBonusItem& operator =(const stBonusItem& other)
	{
		copy(other);
		return *this;
	}

	template<class T>
	void WriteData(T* stream)
	{
		stream->writeBits(sizeof(stBonusItem)<<3,this);
	}

	template<class T>
	void ReadData(T* stream)
	{
		stream->readBits(sizeof(stBonusItem)<<3,this);
	}
};
#endif //_BONUSBASE_H_