#ifndef EVENT_FN_H
#define EVENT_FN_H

#include "wintcp/IPacket.h"
#include "Common/PacketType.h"
#include "Common/Common.h"

namespace EventFn
{
	void Initialize();
	//account m_pWorldClient��ansysocket������worldע��ɹ��Ժ�����m_pUserService��dtserversocket��������
	bool HandleRegisterResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleRemoteLoginAck(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleRemoteRegisterResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleClientLoginRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleWorldClientLoginResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleWorldPlayerLogout(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleRemotePlayerCount(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	ERROR_CODE handleAccountLogin(char* AccountName, int& AccountId, ERROR_CODE Error, int SocketHandle, 
		int flag, bool isFirst, int oldPlayerFlag);
}


#endif
