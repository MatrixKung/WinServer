#ifndef _PACKETPROCESS_H_
#define _PACKETPROCESS_H_

#include "wintcp/IPacket.h"
#include "PacketBase.h"
#include "CommLib.h"

//���ݰ�����,������ָ���������������ݰ���ִ�к���
class PacketProcess
{
    enum PACKET_FLAG
    {
        PF_NONE   =0,
        PF_REMOVE,
    };

    struct ASYNC_PACKET
    {
        PacketBase*	 m_pPacket;
        int	         m_sktHandle;
        stPacketHead m_head;
        
        //Ԥ����Ϊ��������ɾ����־
        U32		     m_Flag;

        ASYNC_PACKET(void)
        {
            memset(this,0,sizeof(ASYNC_PACKET));
        };
        
        ~ASYNC_PACKET(void)
        {
            SAFE_DELETE(m_pPacket);
        };
    };
    
public:
	PacketProcess(void);
	virtual ~PacketProcess(void);
	
	void SetCtxId(int ctxId) { m_ctxId = ctxId;}
	
	//Ͷ�����ݰ�,���ݰ����Ŵ��ڻ�����,����runʱ����
	virtual bool    SendPacket(PacketBase* pPacket,int SocketHandle,stPacketHead *pHead);

	virtual int	    run(void* param);
	
	virtual void	stop(void) { m_isActive = false;}
	
	bool			IsActive(void) const {return m_isActive;}

	bool			IsFinished(void);
private:
    bool __ResizeCache(void);
    bool __RecvPacket(PacketBase*& pPacket,int& SocketHandle,stPacketHead*& pHead,U32& Flag);
    
    int  m_ctxId;
	bool m_isActive;
	bool m_isFinished;

	//��ǰģ�����Ϣ����
	ASYNC_PACKET* m_PacketQue;
	U32			  m_QueSize;
	U32			  m_Head;
	U32			  m_Tail;
	
	CMyCriticalSection m_cs;
};

#endif /*_PACKETPROCESS_H_*/