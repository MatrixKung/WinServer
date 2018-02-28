#ifndef __COLLECTEVENTBASE_H__
#define __COLLECTEVENTBASE_H__

#include "Base/BitStream.h"
struct stCollectEvent
{
	enum
	{
		Max_Items = 5,
	};

	U32	 eventid;				//�ռ���Ʒ�¼�ID
	U32	 itemnum[Max_Items];	//�ռ���Ʒ�ۼ�����
	__time32_t  openTime;       //����ʱ��
	bool bComplete;             //�Ƿ��ռ����

	stCollectEvent() { Clear();}

	void Clear() { memset(this, 0, sizeof(stCollectEvent));}

	stCollectEvent(const stCollectEvent& other)
	{
		*this = other;
	}

	stCollectEvent& operator=(const stCollectEvent& other)
	{
		memcpy(this, &other, sizeof(stCollectEvent));
		return *this;
	}

	stCollectEvent& operator+=(const stCollectEvent& other)
	{
		for(int i = 0; i < Max_Items; i++)
			itemnum[i] += other.itemnum[i];
		return *this;
	}

	template<class T>
	void Pack(T* stream) const
	{
		stream->writeBits(sizeof(stCollectEvent)<<3,this);
	}

	template<class T>
	void Unpack(T* stream)
	{
		stream->readBits(sizeof(stCollectEvent)<<3,this);
	}
};

#endif//__COLLECTEVENTBASE_H__