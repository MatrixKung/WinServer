#ifndef _UTILITY_SUCCOUR_MANAGER_H_
#define _UTILITY_SUCCOUR_MANAGER_H_
#include "base/types.h"
#include "Common/PlayerStruct.h"
#include "../../ManagerBase.h"
#include "../../AccountHandler.h"
#include "Common/TimerMgr.h"
#include <hash_map>
#include <string>

#define MAX_SUCCOUR_ITEM_LENGTH 512

struct SuccourData
{
	U32 PlayerId;
	U32 GateId;
	U8  Flag;
	U32   createTime;
	char  PlayerName[COMMON_STRING_LENGTH];
	char  item[MAX_SUCCOUR_ITEM_LENGTH];
	int itemLength;
	U32   saveTime;//���ֻ�ڷ����ʹ�ã���¼������ʱ��

	SuccourData()
	{
		memset(this, 0, sizeof(SuccourData));
		itemLength = sizeof(stItemInfo);
	}

	void WriteData(Base::BitStream* stream)
	{
		stream->writeInt(PlayerId,  Base::Bit32);
		stream->writeInt(GateId,	Base::Bit32);
		stream->writeInt(Flag,		Base::Bit8);
		stream->writeInt(createTime, Base::Bit32);
		stream->writeString(PlayerName, COMMON_STRING_LENGTH);
		stream->writeBits(sizeof(stItemInfo)<<3, item);
	}

	void ReadData(Base::BitStream* stream)
	{
		PlayerId = stream->readInt(Base::Bit32);
		GateId   = stream->readInt(Base::Bit32);
		Flag	 = stream->readInt(Base::Bit8);
		createTime = stream->readInt(Base::Bit32);
		stream->readString(PlayerName, COMMON_STRING_LENGTH);
		stream->readBits(sizeof(stItemInfo)<<3, item);
	}
};


class CSuccourManager: public CManagerBase< CSuccourManager, MSG_SUCCOUR_BEGIN, MSG_SUCCOUR_END >, public TimerObject
{
public:
	enum enSuccourFlag
	{
		SUCCOUR_FLAG_CANSAVE		= 0,//���Ծ���
		SUCCOUR_FLAG_SAVEING		= 1,//���ھ�����
		SUCCOUR_FLAG_SAVED			= 2,//�Ѿ�������
	};

	enum
	{
		MAX_SUCCOUR_SIZE			= 4,
		SUCCOUR_TIMEOUT				= 10 * 60,//������ȴʱ�� 
		SUCCOUR_REALIVE_TIME		= 24 * 60 * 60,//����ʱ��
	};

	typedef stdext::hash_map<U32, SuccourData*> SUCCOURMAP;
	typedef SUCCOURMAP::iterator SUCCOURITR;

	CSuccourManager();
	virtual ~CSuccourManager();

	virtual void TimeProcess(U32 bExit);
	void Update();

	//-----------���ݿ����------------
	bool loadDB();
	void saveToDB(SuccourData* pData);
	void delToDB(U32 nPlayerId);
	void updateFlagToDB(U32 nPlayerId, U8 nFlag);

	void insertToMap(SuccourData* pData, bool bSaveToDB = true);
	void delMap(U32 nPlayerId);
	void updatePlayerFlag(U32 nPlayerId, U8 nFlag);
	U32  getCanSuccourSize(U32 nPlayerId);

	//------------------------------��Ϣ����------------------------------------
	void HandlePlayerSuccourPublish(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//��������
	void HandlePlayerSuccourGetPlayerList(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//��ȡ�����б�
	void HandlePlayerSavePlayer(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//�������
	void HandlePlayerSaveResult(int SocketHandle,stPacketHead* pHead,Base::BitStream *Packet);//�������
protected:
	SimpleTimer<60>      m_60Timer;//��ʱ��
	SUCCOURMAP			 m_SurrourMap;
};

#endif