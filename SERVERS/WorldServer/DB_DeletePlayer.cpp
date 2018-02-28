#include "stdafx.h"
#include "DB_DeletePlayer.h"
#include "DBLayer/common/DBUtility.h"
#include "DBLayer/data/TBLPlayer.h"
#include "DBContext.h"
#include "WorldServer.h"
#include "ServerMgr.h"
#include "PlayerMgr.h"
#include "GamePlayLog.h"
#include "wintcp/dtServerSocket.h"
#include "wintcp/asyncSocket.h"

DB_DeletePlayer::DB_DeletePlayer(void)
{
    m_accountId = 0;
    m_playerId  = 0;
    m_error     = 0;
    m_isDeleteRecord = false;
    m_isFromZone     = false;
}

int DB_DeletePlayer::Execute(int ctxId,void* param)
{
    if(DB_CONTEXT_NOMRAL == ctxId)
    {
        CDBConn* pDBConn = (CDBConn*)param;
        assert(0 != pDBConn);

        TBLPlayer tb_player(0);
        tb_player.AttachConn(pDBConn);

        if (m_isDeleteRecord)
        {
            m_error = tb_player.DeleteRecord(m_playerId);
        }   
        else
        {
            //ɾ����Ҽ�¼����������ݱ��ݵ�ɾ������   
            m_error = tb_player.Delete_BLOB(m_playerId);
        }
        
        PostLogicThread(this);
        return PACKET_NOTREMOVE;
    }
    else
    {
        if (0 != m_error)
        {
            g_Log.WriteLog("ɾ���ʺ�[%d] ��ɫ[%d]ʧ��",m_accountId,m_playerId);
        }
        else
        {
            //ȥ����������ݹ���
            SERVER->GetPlayerManager()->OnPlayerDeleted(m_accountId,m_playerId);
            
            if(m_isDeleteRecord)
            {
                g_Log.WriteLog("֪ͨRAɾ����ɫ");
                //�����¼��ȫɾ��,����Ҫ��remote account�ϵ����ɾ��
                char buf[64];
                Base::BitStream remotePacket(buf, sizeof(buf));
                stPacketHead* pSendHead = IPacket::BuildPacketHead(remotePacket,WORLD_REMOTE_DeletePlayer,m_playerId);
                SERVER->GetRemoteAccountSocket()->Send(remotePacket);
            }
        }

        //����ɾ�������Ӧ
        if (!m_isFromZone)
        {
            AccountRef spAccount = SERVER->GetPlayerManager()->GetAccount(m_accountId);

            if(!spAccount)
            {
                return PACKET_OK;
            }
            
            spAccount->SendPlayerDeleteRespond(m_playerId,m_error,0);
        }
    }

    return PACKET_OK;
}