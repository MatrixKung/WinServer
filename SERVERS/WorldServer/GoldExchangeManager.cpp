/*
#include "WorldServer.h"
#include "wintcp/AsyncSocket.h"
#include "wintcp/dtServerSocket.h"
#include "ServerMgr.h"
#include "GoldExchangeManager.h"
#include "MailManager.h"
#include "DB_GoldExchange.h"
#include "PlayerMgr.h"
#include "AccountHandler.h"
#include "GamePlayLog.h"
#include "DB_ZoneLogicEvent.h"

GoldExChangeManager::GoldExChangeManager()
{
	registerEvent( CW_QUERYGOLDCONFIG_REQUEST,		&GoldExChangeManager::HandleCWQueryGoldConfig);
	registerEvent( AW_QUERYMONTHPAYS_RESPOND,		&GoldExChangeManager::HandleAWQueryMonthPays);
	registerEvent( CW_QUERYGOLDEXCHANGE_REQUEST,	&GoldExChangeManager::HandleCWQueryGoldExchange);
	registerEvent( CW_DRAWGOLDEXCHANGE_REQUEST,		&GoldExChangeManager::HandleCWDrawGoldExchange);
	registerEvent( CW_QUERYSRCONFIG_REQUEST,		&GoldExChangeManager::HandleCWQuerySRConfig);
	registerEvent( ZW_QUERYSRBONUS_REQUST,			&GoldExChangeManager::HandleZWQuerySRBonus);
	registerEvent( CW_DRAWSRBONUS_REQUEST,			&GoldExChangeManager::HandleCWDrawSRBonus);
	Version = ::GetTickCount();
}

// ----------------------------------------------------------------------------
// ������
void GoldExChangeManager::Update()
{
	DB_GoldConfig_Query* pDBHandle = new DB_GoldConfig_Query;
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// ����Ԫ���һ���������
void GoldExChangeManager::UpdateConfig(stGoldConfig& _config)
{
	DO_LOCK(Lock::GoldExchage);
	memcpy(&config, &_config, sizeof(stGoldConfig));
	//����Ԫ���һ����ð汾ֵ
	Version = ::GetTickCount();
}

// ----------------------------------------------------------------------------
// ��������Client�����ѯԪ���һ��������ݵ���Ϣ
void GoldExChangeManager::HandleCWQueryGoldConfig(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::GoldExchage);
	int PlayerID = pHead->Id;
	U32 ClientVersion = Packet->readInt(Base::Bit32);
	int AccountID = Packet->readInt(Base::Bit32);

	//���ݰ汾��ȷ���Ƿ�Ҫ���¶һ�����
	if(Version !=  ClientVersion)
	{
		AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerID);
		if(NULL == pAccount)
		{
			g_Log.WriteWarn("δ�ܲ�ѯ�����[%d]��������(HandleCWQueryGoldConfig)", PlayerID);
			return;
		}

		char buf[512];
		Base::BitStream sendPacket(buf,sizeof(buf));
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_QUERYGOLDCONFIG_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
		sendPacket.writeInt(Version, Base::Bit32);
		sendPacket.writeBits(sizeof(stGoldConfig)<<3, &config);
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket );
	}

	//��Remote��ѯ��ֵ����
	SendWAQueryMonthPays(AccountID, PlayerID, "", false);
}

// ----------------------------------------------------------------------------
// ��Remote���Ͳ�ѯ��ֵ����
void GoldExChangeManager::SendWAQueryMonthPays(int AccountId, int PlayerId, const char* PlayerName, bool isDraw)
{
	DO_LOCK(Lock::GoldExchage);

    g_Log.WriteLog("RA QueryMonthPays");

	char buf[128];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,WA_QUERYMONTHPAYS_REQUEST,AccountId);
	sendPacket.writeFlag(isDraw);
	sendPacket.writeInt(PlayerId, Base::Bit32);
	sendPacket.writeString(PlayerName, 32);
	SERVER->GetRemoteAccountSocket()->Send(sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Remote��Ӧ��ѯ��ɫ���³�ֵ�ܴ�������Ϣ
void GoldExChangeManager::HandleAWQueryMonthPays(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	bool isDraw = Packet->readFlag();
	int AccountId = pHead->Id;
	int PlayerID = Packet->readInt(Base::Bit32);
	int AccountGold = Packet->readInt(Base::Bit32);
	int MonthPays = Packet->readInt(Base::Bit32);
	char szPlayerName[32];
	Packet->readString(szPlayerName, 32);

	//�ж��Ƿ�ʱ�һ�"��ֵ���"
	if(isDraw)
	{
		//��鱾�³�ֵ�����Ƿ���������
		if(MonthPays >= config.Condition[GEF_CR])
		{
			DB_GoldExchange_Draw* pDBHandle = new DB_GoldExchange_Draw;
			pDBHandle->PlayerID = PlayerID;
			pDBHandle->AccountID = AccountId;
			pDBHandle->type = GEF_CR;
			Packet->readString(szPlayerName, 32);
			SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
		}
		else
		{
			//���ɼ�ʱ�һ�"��ֵ���"
			SendWCDrawGoldExchange(PlayerID, GEF_CR, false, 0);
		}		
	}
	else
	{
		DB_GoldExchange_Query* pDBHandle = new DB_GoldExchange_Query;
		pDBHandle->exchange.AccountID = AccountId;
		pDBHandle->exchange.PlayerID = PlayerID;
		pDBHandle->exchange.AccountGold = AccountGold;
		pDBHandle->exchange.MonthPays = MonthPays;
		SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
	}
}

// ----------------------------------------------------------------------------
// ��Client��Ӧ��ѯ�ۼ���ȡԪ����
void GoldExChangeManager::SendWCQueryMonthGolds(U32 PlayerID, S32 MonthGolds)
{
	DO_LOCK(Lock::GoldExchage);
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerID);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("δ�ܲ�ѯ�����[%d]��������(SendWCQueryMonthGolds)", PlayerID);
		return;
	}

	char buf[128];
	Base::BitStream sendPacket( buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_QUERYMONTHGOLDS_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	sendPacket.writeInt(MonthGolds, Base::Bit32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket );
}

// ----------------------------------------------------------------------------
// ��������Client�����ѯ���¶һ�Ԫ�����ݵ���Ϣ
void GoldExChangeManager::HandleCWQueryGoldExchange(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::GoldExchage);
	int PlayerID = pHead->Id;
	int AccountID = Packet->readInt(Base::Bit32);
	//��Remote��ѯ��ֵ����
	SendWAQueryMonthPays(AccountID, PlayerID, "", false);
}

// ----------------------------------------------------------------------------
// ��Client��Ӧ��ѯ��ҵ��¶һ�Ԫ������
void GoldExChangeManager::SendWCQueryGoldExchange(stGoldExchange* exchange)
{
	DO_LOCK(Lock::GoldExchage);

	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(exchange->PlayerID);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("δ�ܲ�ѯ�����[%d]��������(SendWCQueryGoldExchange)", exchange->PlayerID);
		return;
	}

	char buf[512];
	Base::BitStream sendPacket( buf, sizeof(buf) );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_QUERYGOLDEXCHANGE_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	sendPacket.writeBits(sizeof(stGoldExchange)<<3, exchange);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket );
}

// ----------------------------------------------------------------------------
// ��������Zone������ȡԪ���һ�����Ϣ
void GoldExChangeManager::HandleCWDrawGoldExchange(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::GoldExchage);
	int PlayerID = pHead->Id;
	int AccountID = Packet->readInt(Base::Bit32);
	int type = Packet->readInt(Base::Bit8);
	char szPlayerName[MAX_NAME_LENGTH];
	Packet->readString(szPlayerName, 32); 

	//�ж��Ƿ�"��ֵ�������"�һ�,��Ҫ�ر���
	if(type == GEF_CR)
	{
		//��Remote��ѯ��ֵ����,���غ���ȷ���Ƿ��ܼ�ʱ��ȡ
		SendWAQueryMonthPays(AccountID, PlayerID, szPlayerName, true);
	}
	else if (type > GEF_CR && type < GEF_MAX)
	{
		DB_GoldExchange_Draw* pDBHandle = new DB_GoldExchange_Draw;
		pDBHandle->PlayerID = PlayerID;
		pDBHandle->AccountID = AccountID;
		pDBHandle->type = type;
		dStrcpy(pDBHandle->PlayerName, 32, szPlayerName);
		SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
	}
	else
	{
		g_Log.WriteWarn("�յ�һ�������Ч�����ݰ�(HandleCWDrawGoldExchange)");
	}
}

// ----------------------------------------------------------------------------
// ��Zone��ӦԪ���һ���ȡ���
void GoldExChangeManager::SendWCDrawGoldExchange(int PlayerID, int type, int isOK, int Exchanges)
{
	DO_LOCK(Lock::GoldExchage);
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerID);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("δ�ܲ�ѯ�����[%d]��������(SendWCDrawGoldExchange)", PlayerID);
		return;
	}

	char buf[128];
	Base::BitStream sendPacket( buf, sizeof(buf) );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_DRAWGOLDEXCHANGE_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	sendPacket.writeInt(isOK, 8);
	sendPacket.writeInt(type, 8);
	sendPacket.writeInt(Exchanges, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket );

	if(isOK)
	{
		//��־��¼:���Ԫ���һ�
		GameplayLog::GL_GoldExchange(PlayerID, type, config.Change[type], config.ItemID[type]);

		//��ͻ���֪ͨ�ʼ�����
		SERVER->GetPlayerManager()->SendSimplePacket(PlayerID,WORLD_CLIENT_MailNotify );
	}
}

// ----------------------------------------------------------------------------
// ��������Client�����ѯ���ڷ����������ݵ���Ϣ
void GoldExChangeManager::HandleCWQuerySRConfig(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::GoldExchage);

	DB_StagingRebate_QueryConfig* pDBHandle = new DB_StagingRebate_QueryConfig;
	pDBHandle->PlayerID = pHead->Id;
	pDBHandle->AccountID = Packet->readInt(32);
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// ��������Client�����ѯ���ڷ����������ݵ���Ϣ
void GoldExChangeManager::HandleZWQuerySRBonus(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::GoldExchage);

	DB_StagingRebate_QueryBonus* pDBHandle = new DB_StagingRebate_QueryBonus;
	pDBHandle->PlayerID = pHead->Id;
	pDBHandle->BonusData.SRId = Packet->readInt(32);
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// ��������Client������ȡ���ڷ�����������Ϣ
void GoldExChangeManager::HandleCWDrawSRBonus(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::GoldExchage);

	DB_StagingRebate_DrawBonus* pDBHandle = new DB_StagingRebate_DrawBonus;
	pDBHandle->PlayerID = pHead->Id;
	pDBHandle->AccountID = Packet->readInt(32);
	pDBHandle->SRId = Packet->readInt(32);
	pDBHandle->PerMonth = Packet->readInt(8);
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// ��Client��Ӧ��ѯ���ڷ�����������
void GoldExChangeManager::SendWCQuerySRConfig(U32 PlayerId,
						std::vector<stSR_Period*>& ActivityList,
						std::vector<stSR_Bonus*>& ConfigList,
						std::vector<stSR_History*>& HistoryList)
{
	DO_LOCK(Lock::GoldExchage);
	
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerId);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("δ�ܲ�ѯ�����[%d]��������(SendWCQuerySRConfig)", PlayerId);
		return;
	}

	char* buf = new char[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE);
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_QUERYSRCONFIG_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	//���ڷ������������
	if(sendPacket.writeFlag(!ActivityList.empty()))
	{
		sendPacket.writeInt(ActivityList.size(), 8);
		for(size_t i = 0; i < ActivityList.size(); i++)
		{
			sendPacket.writeBits(sizeof(stSR_Period)<<3, ActivityList[i]);
		}
	}
	//���ڷ������������
	if(sendPacket.writeFlag(!ConfigList.empty()))
	{
		sendPacket.writeInt(ConfigList.size(), 8);
		for(size_t i = 0; i < ConfigList.size(); i++)
		{
			sendPacket.writeBits(sizeof(stSR_Bonus)<<3, ConfigList[i]);
		}
	}
	//���ڷ������ҽ�������
	if(sendPacket.writeFlag(!HistoryList.empty()))
	{
		sendPacket.writeInt(HistoryList.size(), 16);
		for(size_t i = 0; i < HistoryList.size(); i++)
		{
			sendPacket.writeBits(sizeof(stSR_History)<<3, HistoryList[i]);
		}
	}
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket );

	SAFE_DELETE_ARRAY(buf);
}

// ----------------------------------------------------------------------------
// ��Client���ͷ��ڷ����������ò�ѯ���
void GoldExChangeManager::SendWCQuerySRBonus(U32 PlayerId, stSR_History& BonusData)
{
	DO_LOCK(Lock::GoldExchage);

	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerId);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("δ�ܲ�ѯ�����[%d]��������(SendWCQuerySRBonus)", PlayerId);
		return;
	}

	char buf[512];
	Base::BitStream sendPacket( buf, sizeof(buf) );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_QUERYSRBONUS_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	sendPacket.writeBits(sizeof(stSR_History)<<3, &BonusData);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket );
}

// ----------------------------------------------------------------------------
// ��Zone����ҷ�����ȡ���ڷ������
void GoldExChangeManager::SendWZDrawSRBonus(int error, U32 PlayerId, U32 SRId, S32 PerMonth, 
							stSR_Bonus_Data& BonusData)
{
	DO_LOCK(Lock::GoldExchage);

	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int ZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int svrSocket = SERVER->GetServerManager()->GetGateSocket( LineId, ZoneId );
	if(0 == svrSocket)
	{
		if(error != 0)
			return;	//û�гɹ���ȡֱ���˳�������

		//��ȫ���������Ҳ���socket����ֱ����ϵͳ�߼��¼����������
		DB_ZoneLogicEvent* pDBHandle = new DB_ZoneLogicEvent;
		pDBHandle->m_event.data_type = stEventItem::DATATYPE_BINDGOLD;
		pDBHandle->m_event.data_value = BonusData.BindGold;
		pDBHandle->m_event.sender = PlayerId;
		pDBHandle->m_event.recver = PlayerId;
		pDBHandle->m_playerId = PlayerId;
		pDBHandle->m_zoneId = ZoneId;
		pDBHandle->m_zoneSocketId = 0;
		SERVER->GetCommonDBManager()->SendPacket(pDBHandle);

		pDBHandle = new DB_ZoneLogicEvent;
		pDBHandle->m_event.data_type = stEventItem::DATATYPE_MONEY;
		pDBHandle->m_event.data_value = BonusData.Money;
		pDBHandle->m_event.sender = PlayerId;
		pDBHandle->m_event.recver = PlayerId;
		pDBHandle->m_playerId = PlayerId;
		pDBHandle->m_zoneId = ZoneId;
		pDBHandle->m_zoneSocketId = 0;
		SERVER->GetCommonDBManager()->SendPacket(pDBHandle);

		pDBHandle = new DB_ZoneLogicEvent;
		pDBHandle->m_event.data_type = stEventItem::DATATYPE_BINDMONEY;
		pDBHandle->m_event.data_value = BonusData.BindMoney;
		pDBHandle->m_event.sender = PlayerId;
		pDBHandle->m_event.recver = PlayerId;
		pDBHandle->m_playerId = PlayerId;
		pDBHandle->m_zoneId = ZoneId;
		pDBHandle->m_zoneSocketId = 0;
		SERVER->GetCommonDBManager()->SendPacket(pDBHandle);

		pDBHandle = new DB_ZoneLogicEvent;
		pDBHandle->m_event.data_type = stEventItem::DATATYPE_EXP;
		pDBHandle->m_event.data_value = BonusData.Exp;
		pDBHandle->m_event.sender = PlayerId;
		pDBHandle->m_event.recver = PlayerId;
		pDBHandle->m_playerId = PlayerId;
		pDBHandle->m_zoneId = ZoneId;
		pDBHandle->m_zoneSocketId = 0;
		SERVER->GetCommonDBManager()->SendPacket(pDBHandle);

		//��־��¼����ȡ���ڷ��������
		GameplayLog::GL_DrawSRBonus(PlayerId, SRId, PerMonth, BonusData.Gold, BonusData.BindGold,
			BonusData.Money, BonusData.BindMoney, BonusData.Exp);
		
		g_Log.WriteWarn("��ѯ���[%d]���������ڵ�ͼʧ��!(SendWZDrawSRBonus)", PlayerId);
		return;
	}

	char buf[256];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_DRAWSRBONUS_RESPOND, PlayerId, SERVICE_ZONESERVER, ZoneId );
	sendPacket.writeInt(error, 32);
	sendPacket.writeInt(SRId, 32);
	sendPacket.writeBits(sizeof(stSR_Bonus_Data)<<3, &BonusData);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send(svrSocket, sendPacket );

	//���ݵ�ǰ���ѯ��ɫ�ܻ�õķ��ڷ����Ľ�������
	DB_StagingRebate_QueryBonus* pDBHandle = new DB_StagingRebate_QueryBonus;
	pDBHandle->PlayerID = PlayerId;
	pDBHandle->BonusData.SRId = SRId;
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);

	//��־��¼����ȡ���ڷ��������
	GameplayLog::GL_DrawSRBonus(PlayerId, SRId, PerMonth, BonusData.Gold, BonusData.BindGold,
		BonusData.Money, BonusData.BindMoney, BonusData.Exp);
}
*/