//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef	_COLLECTITEMDATA_H
#define _COLLECTITEMDATA_H
#include <map>

struct  stCollectItem
{
	U32    itemId;      // ��Ʒid
	U32    quantity;    // ����

	stCollectItem() { memset(this,0,sizeof(stCollectItem)); }
};

class CollectItemData
{
public:
	friend class CollectItemRepository;
protected:
	U16                         mEventId;               // �ռ��¼� 
	F32                         mCloseInterval;         // �رռ��ʱ��
	std::vector<stCollectItem>  mCollectItem;           // ���ռ���Ʒ

public:
	CollectItemData();

	inline  U16                    getEventId       ()           { return mEventId; }
	inline  U8                     getSize          ()           { return (U8)mCollectItem.size(); }
	inline  U32                    getCloseInterval ()           { return (U32)(mCloseInterval * 86400.0f); }
	inline  stCollectItem&         getCollectItem   (int index)  { return mCollectItem.at(index); }

};

class CollectItemRepository
{
public:

	enum 
	{
		MAXITEM_NUM = 5,    //�����ռ�����
	};

	typedef std::map<U16,CollectItemData*>  CollectItemMap;

public:
	~CollectItemRepository() { clear(); }

	bool                            read              ();
	void                            clear             ();
	CollectItemData*                getData           (U16 iEventId);


	CollectItemMap&                 getCollectItemMap ()       { return mCollectItemMap; }
	static CollectItemRepository&   Instance          ();

private:
	CollectItemMap    mCollectItemMap;
};

#define g_CollectItemRepository CollectItemRepository::Instance()

#endif