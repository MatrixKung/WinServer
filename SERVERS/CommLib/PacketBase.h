#ifndef _PACKETBASE_H_
#define _PACKETBASE_H_

#include "base/bitstream.h"
#include "Common/PacketType.h"

struct stPacketHead;

enum PacketResult
{
	PACKET_ERROR     = 0,
	PACKET_OK,                      
	PACKET_NOTREMOVE,
};

enum PacketCtx
{
	CTX_MAIN = 0,
	CTX_DB,
	CTX_UNCOMPRESS,
	CTX_COMPRESS,
	CTX_LOAD,
};

//������Ϣ���Ļ���,ͨ��packet factory manager����
class PacketBase
{
public:
    //�������
	U32 m_index;
	
	//ִ��״̬
	U32 m_status;
public :
	PacketBase(void)
	{
	    m_index  = 0;
	    m_status = 0;
	}
	
	virtual ~PacketBase(void) {}
	virtual void Clear(void)  {};

	virtual bool Read(Base::BitStream& stream) = 0;
	virtual int	 Execute(int ctxId,stPacketHead *pHead,int SocketHandle,void* param) = 0;
	
	virtual	int	 GetPacketID(void)  const    {return -1;}
	virtual bool CheckPacket(int packetSize) { return true;}

	U32  GetPacketIndex(void) const { return m_index ;};
	void SetPacketIndex(U32 Index ) { m_index = Index ;};

	U32  GetPacketStatus(void) const { return m_status ; } ;
	void SetPacketStatus(U32 Status){ m_status = Status ; } ;
};

#endif /*_PACKETBASE_H_*/
