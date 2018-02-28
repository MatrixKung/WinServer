#ifndef __LOGICEVENTBASE_H__
#define __LOGICEVENTBASE_H__

#include "Base/BitStream.h"
#include "Common/PacketType.h"

struct stEventItem
{
	enum
	{
		RESERVE_MAX			= 3,			//�������ݸ���

		EVENTSTATE_NONE		= 0,			//δ����
		EVENTSTATE_SEND		= 1,			//�ѷ���

		EVENTTYPE_NONE		= 0,			//�Ƕ�ʱ�¼�
		EVENTTYPE_TIMER		= 1,			//��ʱ�¼�

		DATATYPE_NONE		= 0,			//��
		DATATYPE_MONEY		= 1,			//��Ԫ
		DATATYPE_BINDMONEY	= 2,			//�󶨽�Ԫ
		DATATYPE_GOLD		= 3,			//Ԫ��
		DATATYPE_BINDGOLD	= 4,			//��Ԫ��
		DATATYPE_FAME		= 5,			//����
		DATATYPE_BANKMONEY	= 6,			//���н�Ԫ
		DATATYPE_EXP		= 7,			//����
		DATATYPE_EXPPOOL	= 8,			//ʦ������ؾ���
		DATATYPE_EXPLOIT	= 9,			//ʦ��
		DATATYPE_REDUCEGOLD	= 10,			//�ۼ�Ԫ��


		REASON_OK			= 0,			//����ɹ�
		REASON_SEND			= 1,			//��ѯ������
		REASON_OVERTIME		= 2,			//��ʱ��ʱʱ��
		REASON_INVALIDTIME	= 3,			//�¼�����
	};

	U64		id;								//ΨһID
	U32		sender;							//������
	U32		recver;							//��ȡ��
	U8		event_state;					//�¼�״̬(��EVENTSTATEö��ֵ)
	U8		event_type;						//�¼�����(��EVENTTYPEö��ֵ)
	U32		timer;							//���Ƕ�ʱ�¼�,���ֵΪ����ʱ��
											//����ʱ�¼�,���ֵΪ��ʱʱ��
	U8		data_type;						//��ȡ��������(��DATATYPE)
	S32		data_value;						//��ȡ����
	S32		reserve[RESERVE_MAX];			//����ֵ
	stEventItem():id(0),sender(0),recver(0),
		event_type(EVENTTYPE_NONE),timer(1602864000),
		data_type(DATATYPE_NONE),data_value(0),
		event_state(EVENTSTATE_NONE)
	{
		for(int i = 0; i < RESERVE_MAX; i++)
			reserve[i] = 0;
	}

	template<typename T>
	void PackData(T* stream)
	{
		stream->writeBits(Base::Bit64, &id);
		stream->writeInt(sender,		Base::Bit32);
		stream->writeInt(recver,		Base::Bit32);
		stream->writeInt(event_state,	Base::Bit8);
		stream->writeInt(event_type,	Base::Bit8);
		stream->writeInt(timer,			Base::Bit32);
		stream->writeInt(data_type,		Base::Bit8);
		stream->writeInt(data_value,	Base::Bit32);
		for(int i = 0; i < RESERVE_MAX; i++)
			stream->writeInt(reserve[i],Base::Bit32);
	}

	template<typename T>
	void UnpackData(T* stream)
	{
		stream->readBits(Base::Bit64, &id);
		sender		= stream->readInt(Base::Bit32);
		recver		= stream->readInt(Base::Bit32);
		event_state	= stream->readInt(Base::Bit8);
		event_type	= stream->readInt(Base::Bit8);
		timer		= stream->readInt(Base::Bit32);
		data_type	= stream->readInt(Base::Bit8);
		data_value	= stream->readInt(Base::Bit32);
		for(int i = 0; i < RESERVE_MAX; i++)
			reserve[i] = stream->readInt(Base::Bit32);
	}
};

#endif//__LOGICEVENTBASE_H__