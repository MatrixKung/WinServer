#ifndef _SAFEPACKET_H_
#define _SAFEPACKET_H_

#include <memory>
#include <hash_map>
#include <vector>
#include "base/Locker.h"
#include "base/Bitstream.h"

class Player;
typedef std::tr1::shared_ptr<Player> AccountRef;

//����������Ϸ״̬��,�����п��ܴ��ڴ��ͻ����״̬,������Ϣ���޷����ͳ�ȥ,SafePacket
//�����Ϣ�����л���,������ҽ�����Ϸ���ͻ������Ϣ��
class SafePacket
{
    struct DelayPacket
    {
        DelayPacket(void)
        {
            pData = 0;
            size  = 0;
        }

        char* pData;
        int   size;
    };

public:
    static SafePacket* Instance(void)
    {
        static SafePacket local;
        return &local;
    }
    
    bool Send(AccountRef spPlayer,Base::BitStream& packet);

    //ǿ�Ʒ������е���Ϣ��
    void Flush(Player* pPlayer);
    
    //������Ҫ���͵���Ϣ��
    void Update(void);
protected:
    SafePacket(void);

    bool DoSend(Player* pPlayer,Base::BitStream& packet,bool isBuffed);

    CMyCriticalSection m_cs;
    
    typedef stdext::hash_map<U32,std::vector<DelayPacket> > PACKETS_MAP;
    PACKETS_MAP m_packets;
};

#define SAFEPKT SafePacket::Instance()

#endif /*_SAFEPACKET_H_*/