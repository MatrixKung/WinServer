#ifndef ROOM_BASE_H
#define ROOM_BASE_H
#include "BASE/types.h"
#include <map>

class ITable
{
public:
	explicit ITable() : m_Id(0), m_Type(0)
	{
	}

	virtual ~ITable()
	{
	}

	virtual bool InitTable()  = 0;
	virtual void ResetTable() = 0;
	virtual U32	 GetMaxPlayers() = 0;
	virtual bool HasDisband() = 0;

	void SetId(U64 Id)	{m_Id = Id;}
	U64	 GetId()		{return m_Id;}

	bool HasPlayer(U16 sId) {return m_PlayerMap[sId];}
	U32  GetPlayer(U16 sId) {return m_PlayerMap[sId];}
	U16  GetPlayerSlot(U32 nPlayerID)
	{
		for(int i = 0; i < GetMaxPlayers(); ++i)
		{
			if(m_PlayerMap[i] == nPlayerID)
			{
				return i;
			}
		}
		return GetMaxPlayers();
	}

	U32  GetPlayerNum()	
	{
		U32 nNum = 0;
		for(int i = 0; i < GetMaxPlayers(); ++i)
		{
			if(m_PlayerMap[i])
			{
				++nNum;
			}
		}
		return nNum;
	}
	bool InsertPlayer(U32 nPlayerId, U16 sId)
	{
		if(sId >= GetMaxPlayers())
			return false;

		m_PlayerMap[sId] = nPlayerId;
		return true;
	}
	bool DelPlayer(U32 nPlayerId)
	{
		for(int i = 0; i < GetMaxPlayers(); ++i)
		{
			if(m_PlayerMap[i] == nPlayerId)
			{
				m_PlayerMap[i] = 0; 
				return true;
			}
		}
		return false;
	}
	void ClearPlayer()
	{
		for(int i = 0; i <= GetMaxPlayers(); ++i)
		{
			m_PlayerMap[i] = 0;
		}
	}

	bool HasJoinPlayer(U16 sId) {return m_PlayerJoinMap[sId];}
	U32  GetJoinPlayer(U16 sId) {return m_PlayerJoinMap[sId];}
	U16  GetJoinPlayerSlot(U32 nPlayerID)
	{
		for(int i = 0; i < 10; ++i)
		{
			if(m_PlayerJoinMap[i] == nPlayerID)
			{
				return i;
			}
		}
		return 10;
	}

	U32  GetJoinPlayerNum()	
	{
		U32 nNum = 0;
		for(int i = 0; i < GetMaxPlayers(); ++i)
		{
			if(m_PlayerJoinMap[i])
			{
				++nNum;
			}
		}
		return nNum;
	}
	bool InsertJoinPlayer(U32 nPlayerId, U16 sId)
	{
		if(sId >= 10)
			return false;

		m_PlayerJoinMap[sId] = nPlayerId;
		return true;
	}
	bool DelJoinPlayer(U32 nPlayerId)
	{
		for(int i = 0; i < GetMaxPlayers(); ++i)
		{
			if(m_PlayerJoinMap[i] == nPlayerId)
			{
				m_PlayerJoinMap[i] = 0; 
				return true;
			}
		}
		return false;
	}
	void ClearJoinPlayer()
	{
		for(int i = 0; i <= GetMaxPlayers(); ++i)
		{
			m_PlayerJoinMap[i] = 0;
		}
	}
private:
	U64 m_Id;
	U32 m_Type;
	std::map<U16, U32> m_PlayerMap;		//����slot��Ӧ���	
	std::map<U16, U32> m_PlayerJoinMap;		//����slot��Ӧ���
};

enum enRoomError
{
	RoomError_None,
	RoomError_Unknown,			// δ֪
	RoomError_OutRoom,			// ���ڷ���
	RoomError_InRoom,			// ���ڷ���
	RoomError_NoExist,			// ���䲻����
	RoomError_MaxPlayer,		// ������������������
	RoomError_HasPlayer,		// �÷����Ѿ��������׼����
	RoomError_Playing,			// ��������Ϸ��
	RoomError_Count,
};
#endif