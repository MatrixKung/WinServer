/*#include "WorldServer.h"
#include "wintcp/AsyncSocket.h"
#include "wintcp/dtServerSocket.h"
#include "ServerMgr.h"
#include "BillingManager.h"
#include "MailManager.h"
#include "DB_Billing.h"
#include "DB_GoldTrade.h"
#include "PlayerMgr.h"
#include "AccountHandler.h"
#include "common/ItemData.h"
#include "DB_ZoneLogicEvent.h"
#include "GM/GMRespondDef.h"
#include "GM/GMRecver.h"
#include "GamePlayLog.h"

class CMyLog;
extern CMyLog g_gmLog;

BillingManager::BillingManager()
{
	registerEvent( ZW_QUERYGOODS_REQUEST,		&BillingManager::HandleZWQueryGoods);
	registerEvent( ZW_DRAWGOLD_REQUEST,			&BillingManager::HandleZWDrawGold);
	registerEvent( AW_DRAWGOLD_RESPOND,			&BillingManager::HandleAWDrawGold);
	registerEvent( ZW_QUERYGOLD_REQUEST,		&BillingManager::HandleZWQueryGold);
	registerEvent( ZW_BUYGOODS_REQUEST,			&BillingManager::HandleZWBuyGoods);
	registerEvent( CW_QUERYACCOUNTGOLD_REQUEST,	&BillingManager::HandleCWQueryAccountGold);
	registerEvent( AW_QUERYACCOUNTGOLD_RESPOND,	&BillingManager::HandleAWQueryAccountGold);
	registerEvent( ZW_QUERYTOTALDRAWGOLD_REQUEST,	&BillingManager::HandleZWQueryTotalDrawGold);
	registerEvent( CW_MALL_QUERYBOARD_REQUEST,	&BillingManager::HandleCWQueryBoard);
	registerEvent( ZW_REDUCEGOLD_REQUEST,		&BillingManager::HandleZWReduceGold);
	registerEvent( AW_GMQUERYACCOUNTGOLD_RESPOND,	&BillingManager::HandleAWGMQueryAccountGold);
	registerEvent( AW_GMUPDATEACCOUNTGOLD_RESPOND,	&BillingManager::HandleAWGMUpdateAccountGold);
	mVersion = 0;
}

BillingManager::~BillingManager()
{
	ClearGoods();
	ClearBoardList();
}

// ----------------------------------------------------------------------------
// ������
bool BillingManager::Update()
{
	DB_Billing_CheckUpdate* pDBHandle = new DB_Billing_CheckUpdate;
	pDBHandle->mVersion = mVersion;
	SERVER->GetBillingDBManager()->SendPacket(pDBHandle);

	static U32 lastTick = 0;
	U32 curTick = ::GetTickCount();
	if(lastTick != 0 && lastTick + 300000 > curTick)//ÿ3���Ӷ�ʱ��ѯһ�����̹㲥��Ϣ
		return true;

	DB_Billing_QueryMsg* pDBHandle1 = new DB_Billing_QueryMsg;
	SERVER->GetBillingDBManager()->SendPacket(pDBHandle1);

	lastTick = curTick;
	return true;
}

// ----------------------------------------------------------------------------
// �ı��̳����ݰ汾
void BillingManager::ChangeVersion(int ver, GOODSVEC& goodsList)
{
	DO_LOCK(Lock::Billing);
	ClearGoods();
	LoadGoods(goodsList);
	mVersion = ver;
}

// ----------------------------------------------------------------------------
// �����̳���Ʒ����
void BillingManager::LoadGoods(GOODSVEC& goodsList)
{
	stGoodsPrice* pGoods = NULL;
	for(size_t i = 0; i < goodsList.size(); i++)
	{
		if(goodsList[i])
		{
			pGoods = new stGoodsPrice;
			memcpy(pGoods, goodsList[i], sizeof(stGoodsPrice));
			mGoodsList.push_back(pGoods);
		}
	}
}

// ----------------------------------------------------------------------------
// �����Ʒ����
bool BillingManager::ClearGoods()
{
	for(size_t i = 0; i < mGoodsList.size(); ++i)
	{
		if(mGoodsList[i])
			delete mGoodsList[i];
	}
	mGoodsList.clear();
	return true;
}

// ----------------------------------------------------------------------------
// ����̳ǹ㲥��Ϣ�ַ����б�
void BillingManager::ClearBoardList()
{
	for(size_t i = 0; i < mBoardList.size(); i++)
	{
		if(mBoardList[i])
			delete mBoardList[i];
	}
	mBoardList.clear();
}

// ----------------------------------------------------------------------------
// �����̳ǹ㲥��Ϣ�ַ�������
void BillingManager::UpdateBoardList(BOARDVEC& vec)
{
	DO_LOCK(Lock::Billing);
	ClearBoardList();
	stMallBoard* pMsg = NULL;
	for(size_t i = 0; i < vec.size(); i++)
	{
		pMsg = new stMallBoard;
		pMsg->MsgId = vec[i]->MsgId;
		strcpy_s(pMsg->Content, 250, vec[i]->Content);
		pMsg->UpdValue = vec[i]->UpdValue;
		mBoardList.push_back(pMsg);
	}
}

// ----------------------------------------------------------------------------
// ��������Zone�������̳����ݵ���Ϣ
void BillingManager::HandleZWQueryGoods(int SocketHandle,stPacketHead* pHead, Base::BitStream* Packet)
{
	DO_LOCK(Lock::Billing);
	int Version = pHead->Id;
	if(GetVersion() != Version)
	{
		SendWZQueryGoods(SocketHandle, pHead->SrcZoneId);
	}
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ��ѯ�̳���Ʒ����
void BillingManager::SendWZQueryGoods(int SocketHandle, int ZoneID)
{
	const U32 MAX_BLOCK = 180;
	size_t counts = mGoodsList.size() / MAX_BLOCK;
	if(mGoodsList.size() % MAX_BLOCK != 0)
		counts ++;

	char* buf = new char[MAX_PACKET_SIZE];
	Base::BitStream sendPacket(buf, MAX_PACKET_SIZE);

	for(size_t i = 0; i < counts; i++)
	{
		sendPacket.setPosition(0);
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_QUERYGOODS_RESPOND, GetVersion(), SERVICE_ZONESERVER, ZoneID);
		size_t count = MAX_BLOCK;
		if( i == counts - 1)
			count = mGoodsList.size() - i * MAX_BLOCK;

		sendPacket.writeInt(count, Base::Bit16);
		for(size_t j = i * MAX_BLOCK; j < (i * MAX_BLOCK + count); j++)
		{
			sendPacket.writeBits(sizeof(stGoodsPrice)<<3, mGoodsList[j]);
		}
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
	}
	delete[] buf;
}

// ----------------------------------------------------------------------------
// ��������Zone������ȡԪ������Ϣ
void BillingManager::HandleZWDrawGold(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::Billing);
	int PlayerID	= pHead->Id;
	int AccountID	= Packet->readInt(32);
	int TakeGold	= Packet->readInt(32);

    g_Log.WriteLog("RA ZWDrawGold");

	char buf[128];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,WA_DRAWGOLD_REQUEST,AccountID);
	sendPacket.writeInt(PlayerID, Base::Bit32);
	sendPacket.writeInt(TakeGold, Base::Bit32);
	SERVER->GetRemoteAccountSocket()->Send(sendPacket);
}

// ----------------------------------------------------------------------------
// ����Account��Ӧ��ȡԪ������Ϣ
void BillingManager::HandleAWDrawGold(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{	
	DO_LOCK(Lock::Billing);
	DB_Billing_UpdateGold* pDBHandle = new DB_Billing_UpdateGold;
	pDBHandle->type			= GO_DRAWSAVE;
	pDBHandle->AccountID	= pHead->Id;
	pDBHandle->PlayerID		= Packet->readInt(Base::Bit32);
	pDBHandle->Gold			= Packet->readInt(Base::Bit32);
	pDBHandle->AccountGold	= Packet->readInt(Base::Bit32);
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

//��������Zone��������Ʒ����Ϣ
void BillingManager::HandleZWBuyGoods( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
{
	DO_LOCK(Lock::Billing);
	stTradeGoods tradegoods;
	int PlayerID = pHead->Id;
	Packet->readBits(sizeof(stTradeGoods) << 3, &tradegoods);
	int FriendID = Packet->readInt(Base::Bit32);
	tradegoods.AreaID = SERVER->currentAreaId;

	//�����ѵ���Ԫ��
	if(tradegoods.PriceType == 0)
	{
		DB_Billing_UpdateGold* pDBHandle = new DB_Billing_UpdateGold;
		pDBHandle->type			= GO_BUYGOODS;
		pDBHandle->AccountID	= tradegoods.AccountID;
		pDBHandle->PlayerID		= PlayerID;
		pDBHandle->Gold			= 0-tradegoods.Price * tradegoods.PriceNum;
		pDBHandle->FriendID		= FriendID;
		pDBHandle->tradeGoods	= tradegoods;
		SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
	}
	else
	{
		DB_Billing_RecordTrade* pDBHandle = new DB_Billing_RecordTrade;
		pDBHandle->tradeGoods	= tradegoods;
		pDBHandle->LeftGold		= 0;
		pDBHandle->FriendID		= FriendID;
		pDBHandle->Op			= 1;
		SERVER->GetBillingDBManager()->SendPacket(pDBHandle);
	}
}

// ----------------------------------------------------------------------------
// ��World�ϴ���ʧ�ܣ���Ʒ���ʼ����������
void BillingManager::SendMailForPrice(int PlayerID, int FriendID, PRICEID priceId, U32 priceNum)
{
	DO_LOCK(Lock::Billing);
	stGoodsPrice* goods = NULL;
	for(size_t i = 0; i < mGoodsList.size(); i++)
	{
		if(mGoodsList[i] && mGoodsList[i]->PriceID == priceId)
		{
			goods = mGoodsList[i];
			break;
		}
	}

	if(NULL == goods)
	{
		g_Log.WriteFocus("�̳ǹ����Ʒ[PriceID=%I64d]û���ҵ�!", priceId);
		return;
	}

	if(FriendID == 0)
	{
		for(int i = 0; i < goods->ItemCount; i++)
		{
			if(goods->ItemID[i] != 0)
			{
				SERVER->GetMailManager()->sendMailEx(0, PlayerID, 0, goods->ItemID[i], goods->ItemNum[i] * priceNum, "���̳ǹ��",
					"<t>�����������������������������⣬�̳�������Ʒ�ʼĸ���!</t>", 1);
			}
		}
	}
	else
	{
		//��Ʒ���ʼ�����Ѽĳ�
		for(int i = 0; i < goods->ItemCount; i++)
		{
			if(goods->ItemID[i] != 0)
			{				
				SERVER->GetMailManager()->sendMailEx(0, FriendID, 0, goods->ItemID[i], goods->ItemNum[i] * priceNum, "���̳ǹ��",
					"<t>���к������̳�Ϊ����������Ʒ,����ȡ.</t>", 1);
			}
		}
	}
}

// ----------------------------------------------------------------------------
// �������ݿ�����̳ǹ���ص���Ʒ���µ���Ϣ
void BillingManager::BuyGoodsUpdatePriceCallBack(int LeftGold, int FriendID, int Op, stTradeGoods& tradegoods)
{
	DO_LOCK(Lock::Billing);
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(tradegoods.PlayerID);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("����̳ǹ���ɹ����ڻ�ӦZone֮ǰ�޷���ȡ������Ϣ.(PlayerID=%d)", tradegoods.PlayerID);
		if(Op == 1)
		{
			//�̳ǹ������ʼ�����
			SendMailForPrice(tradegoods.PlayerID, FriendID, tradegoods.PriceID,tradegoods.PriceNum);
		}
		return;
	}

	U32 zoneId = pAccount->GetLastZoneId();
	int zoneSocketId = SERVER->GetServerManager()->GetGateSocket(pAccount->GetLastLineId(),zoneId);
	if(zoneSocketId == 0)
	{
		g_Log.WriteWarn("����̳ǹ���ɹ����ڻ�ӦZone֮ǰ�޷���ȡĿ��Zone������.(PlayerID=%d)", tradegoods.PlayerID);
		if(Op == 1)
		{
			//�̳ǹ������ʼ�����
			SendMailForPrice(tradegoods.PlayerID, FriendID, tradegoods.PriceID,tradegoods.PriceNum);
		}
		return;
	}

	char buf[256];
	Base::BitStream sendPacket( buf, sizeof(buf) );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_BUYGOODS_RESPOND, tradegoods.PlayerID, SERVICE_ZONESERVER, zoneId);
	sendPacket.writeBits(sizeof(stTradeGoods) << 3, &tradegoods);
	sendPacket.writeInt(LeftGold, Base::Bit32);
	sendPacket.writeInt(Op, Base::Bit8);
	sendPacket.writeInt(FriendID, Base::Bit32);
	if(FriendID != 0)
	{
		char szFirendName[32] = {0};
		PlayerSimpleDataRef friendplayer = SERVER->GetPlayerManager()->GetPlayerData(FriendID);
		if(friendplayer)
			dStrcpy(szFirendName, 32, friendplayer->PlayerName);
		sendPacket.writeString(szFirendName, 32);
	}
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( zoneSocketId, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�����ѯԪ������Ϣ
void BillingManager::HandleZWQueryGold(int SocketHandle,stPacketHead* pHead, Base::BitStream* Packet)
{
	DO_LOCK(Lock::Billing);

	DB_Billing_QueryGold* pDBHandle = new DB_Billing_QueryGold;
	pDBHandle->AccountID	= Packet->readInt(Base::Bit32);
	pDBHandle->PlayerID		= pHead->Id;
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// ��������Zone�����ѯ�ۼ���ȡԪ������Ϣ
void BillingManager::HandleZWQueryTotalDrawGold(int SocketHandle,stPacketHead* pHead, Base::BitStream* Packet)
{
	DO_LOCK(Lock::Billing);

	DB_Billing_QueryTotalDrawGold* pDBHandle = new DB_Billing_QueryTotalDrawGold;
	pDBHandle->PlayerID		= pHead->Id;
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// ��������Zone����۳�Ԫ������Ϣ
void BillingManager::HandleZWReduceGold(int SocketHandle,stPacketHead* pHead, Base::BitStream* Packet)
{
	DO_LOCK(Lock::Billing);
	DB_Billing_UpdateGold* pDBHandle = new DB_Billing_UpdateGold;
	pDBHandle->AccountID	= Packet->readInt(Base::Bit32);
	pDBHandle->PlayerID		= pHead->Id;
	pDBHandle->type			= Packet->readInt(Base::Bit32);
	pDBHandle->Gold			= 0-Packet->readInt(Base::Bit32);
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// �������ݿ�����̳ǹ���ص���Ʒ���µ���Ϣ
void BillingManager::ReduceGoldCallBack(int PlayerId, int type, int Op)
{
	DO_LOCK(Lock::Billing);
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerId);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("��ҿۼ�Ԫ���ɹ������ڻ�ӦZone֮ǰ�޷���ȡ������Ϣ.(PlayerID=%d)", PlayerId);
		
		DB_ZoneLogicEvent* pDBHandle = new DB_ZoneLogicEvent;
		pDBHandle->m_event.data_type = stEventItem::DATATYPE_REDUCEGOLD;
		pDBHandle->m_event.data_value = type;	//������ſۼ�Ԫ���Ĳ�������
		pDBHandle->m_event.reserve[0] = Op;		//����ֵ������Ƿ�ɹ�
		pDBHandle->m_event.sender = 0;
		pDBHandle->m_event.recver = PlayerId;
		pDBHandle->m_playerId = PlayerId;
		pDBHandle->m_zoneId = 0;
		pDBHandle->m_zoneSocketId = 0;
		SERVER->GetCommonDBManager()->SendPacket(pDBHandle);
		return;
	}

	U32 zoneId = pAccount->GetLastZoneId();
	int zoneSocketId = SERVER->GetServerManager()->GetGateSocket(pAccount->GetLastLineId(),zoneId);
	if(zoneSocketId == 0)
	{
		g_Log.WriteWarn("��ҿۼ�Ԫ���ɹ����ڻ�ӦZone֮ǰ�޷���ȡĿ��Zone������.(PlayerID=%d)", PlayerId);

		DB_ZoneLogicEvent* pDBHandle = new DB_ZoneLogicEvent;
		pDBHandle->m_event.data_type = stEventItem::DATATYPE_REDUCEGOLD;
		pDBHandle->m_event.data_value = type;	//������ſۼ�Ԫ���Ĳ�������
		pDBHandle->m_event.reserve[0] = Op;		//����ֵ������Ƿ�ɹ�
		pDBHandle->m_event.sender = 0;
		pDBHandle->m_event.recver = PlayerId;
		pDBHandle->m_playerId = PlayerId;
		pDBHandle->m_zoneId = zoneId;
		pDBHandle->m_zoneSocketId = 0;
		SERVER->GetCommonDBManager()->SendPacket(pDBHandle);
		return;
	}

	char buf[128];
	Base::BitStream sendPacket( buf, sizeof(buf) );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_REDUCEGOLD_RESPOND, PlayerId, SERVICE_ZONESERVER, zoneId);
	sendPacket.writeInt(type, Base::Bit32);
	sendPacket.writeInt(Op, Base::Bit8);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( zoneSocketId, sendPacket);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ��ѯ���Ԫ��
void BillingManager::SendWZQueryGold(int PlayerID, int LeftGold, int TotalDrawGold, bool IsDraw/*=false*//*)
{
	DO_LOCK(Lock::Billing);
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerID);
	int ZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerID);
	int svrSocket = SERVER->GetServerManager()->GetGateSocket( LineId, ZoneId );

	if(0 == svrSocket)
	{
		g_Log.WriteWarn("��ѯ������������ڵ�ͼʧ��!(��Ҳ�ѯԪ����)");
		return;
	}

	char buf[128];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_QUERYGOLD_RESPOND, PlayerID, SERVICE_ZONESERVER, ZoneId );
	sendPacket.writeInt(LeftGold, Base::Bit32);
	//Note:���������ۼ���ȡԪ����
	sendPacket.writeInt(TotalDrawGold, Base::Bit32);
	sendPacket.writeFlag(IsDraw);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send(svrSocket, sendPacket );
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ��ѯ�ۼ���ȡԪ��
void BillingManager::SendWZQueryTotalDrawGold(int PlayerID, int TotalDrawGolds)
{
	DO_LOCK(Lock::Billing);
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerID);
	int ZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerID);
	int svrSocket = SERVER->GetServerManager()->GetGateSocket( LineId, ZoneId );

	if(0 == svrSocket)
	{
		g_Log.WriteWarn("��ѯ������������ڵ�ͼʧ��!(�ۼ���ȡԪ��)");
		return;
	}

	char buf[128];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_QUERYTOTALDRAWGOLD_RESPOND, PlayerID, SERVICE_ZONESERVER, ZoneId );
	sendPacket.writeInt(TotalDrawGolds, Base::Bit32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send(svrSocket, sendPacket );
}

// ----------------------------------------------------------------------------
// ��������Client�����ѯ�ʺ�Ԫ������Ϣ
void BillingManager::HandleCWQueryAccountGold(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::Billing);
	int PlayerID	= pHead->Id;
	int AccountID	= pHead->DestZoneId;
    
    g_Log.WriteLog("RA QueryAccountGold");

	char buf[128];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,WA_QUERYACCOUNTGOLD_REQUEST,AccountID);
	sendPacket.writeInt(PlayerID, Base::Bit32);
	SERVER->GetRemoteAccountSocket()->Send(sendPacket);
}

// ----------------------------------------------------------------------------
// ����Account��Ӧ��ѯ�ʺ�Ԫ������Ϣ
void BillingManager::HandleAWQueryAccountGold(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{	
	DO_LOCK(Lock::Billing);
	int AccountID	= pHead->Id;
	int PlayerID	= Packet->readInt(Base::Bit32);
	int AccountGold	= Packet->readInt(Base::Bit32);
	SendWCQueryAccountGold(PlayerID, AccountGold);
}	
// ----------------------------------------------------------------------------
// ��������GM���ߵ�����Account��ѯ�ʺ�Ԫ������Ϣ
void BillingManager::HandleWAGMQueryAccountGold(int sid,int AccountID,int GMAccountID)
{
	DO_LOCK(Lock::Billing);

	g_Log.WriteLog("WR GMQueryAccountGold");

	char buf[128];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,WA_GMQUERYACCOUNTGOLD_REQUEST,AccountID,SERVICE_REMOTESERVER);
	sendPacket.writeInt(sid, Base::Bit32);
	sendPacket.writeInt(GMAccountID, Base::Bit32);
	SERVER->GetRemoteAccountSocket()->Send(sendPacket);
}

// ----------------------------------------------------------------------------
// ����Account��ӦGM���ߵĲ�ѯ�ʺ�Ԫ������Ϣ
void BillingManager::HandleAWGMQueryAccountGold(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{	
	DO_LOCK(Lock::Billing);
	int AccountID	= pHead->Id;
	int sid	= Packet->readInt(Base::Bit32);
	int GMAccountID	= Packet->readInt(Base::Bit32);
	int AccountGold	= Packet->readInt(Base::Bit32);

	CGMAccountGoldOpResp resp;
	resp.Gold = AccountGold;

	//��־��¼���ʺ�Ԫ����ѯ
	g_gmLog.WriteLog("Gm�ʺ�:%d ��������:GM_AccountGoldOp ��������ID:%d  �������ݣ��˺�Ԫ����ѯ ",AccountID,GMAccountID);

	CGMRecver::Instance()->PushResp(resp);
	CGMRecver::Instance()->SendResps("GM_AccountGoldOp",0,sid);//sid
}	

// ----------------------------------------------------------------------------
// ��������GM���ߵ�����Account�޸��ʺ�Ԫ������Ϣ
// void BillingManager::HandleWAGMUpdateAccountGold(int sid,int AccountID,int GMAccountID,int gold)
// {
// 	DO_LOCK(Lock::Billing);
// 
// 	g_Log.WriteLog("WR GMQueryAccountGold");
// 
// 	char buf[128];
// 	Base::BitStream sendPacket(buf,sizeof(buf));
// 	stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket,WA_GMUPDATEACCOUNTGOLD_REQUEST,AccountID,SERVICE_REMOTESERVER);
// 	sendPacket.writeInt(sid, Base::Bit32);
// 	sendPacket.writeInt(gold, Base::Bit32);
// 	sendPacket.writeInt(GMAccountID, Base::Bit32);
// 	SERVER->GetRemoteAccountSocket()->Send(sendPacket);
// }

// ----------------------------------------------------------------------------
// ����Account��ӦGM���ߵ��޸��ʺ�Ԫ������Ϣ
void BillingManager::HandleAWGMUpdateAccountGold(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{	
	DO_LOCK(Lock::Billing);
	int AccountID	= pHead->Id;
	int sid	= Packet->readInt(Base::Bit32);
	int GMAccountID	= Packet->readInt(Base::Bit32);
	int AccountGold	= Packet->readInt(Base::Bit32);

	CGMAccountGoldOpResp resp;
	resp.Gold = AccountGold;

	//��־��¼���ʺ�Ԫ����ѯ
	GameplayLog::GL_GMUpdateAccountGold(GMAccountID, GMAccountID);
	g_gmLog.WriteLog("Gm�ʺ�:%d ��������:GM_AccountGoldOp ��������ID:%d  �������ݣ��˺�Ԫ������ ",AccountID,GMAccountID);

	CGMRecver::Instance()->PushResp(resp);
	CGMRecver::Instance()->SendResps("GM_AccountGoldOp",0,sid);//sid
}	

// ----------------------------------------------------------------------------
// ��Client���Ͳ�ѯ�ʺ�Ԫ������Ϣ
void BillingManager::SendWCQueryAccountGold(int PlayerID, int AccountGold)
{
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerID);
	if(NULL == pAccount)
		return;

	char buf[128];
	Base::BitStream sendPacket(buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_QUERYACCOUNTGOLD_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	sendPacket.writeInt(AccountGold, Base::Bit32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket );
}

// ----------------------------------------------------------------------------
// ��������Client�����ѯ�̳ǹ㲥��Ϣ
void BillingManager::HandleCWQueryBoard(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK(Lock::Billing);
	int PlayerId	= pHead->Id;
	size_t counts	= Packet->readInt(32);
	std::vector<std::pair<int,int> > SrcVec;
	BOARDVEC TagVec;
	int MsgId, UpdValue;
	if(counts >= 64)
	{
		g_Log.WriteWarn("�ͻ����ϴ�һ���Ƿ����̳ǹ㲥��Ϣ������ֵ!(Player=%d)", PlayerId);
		return;
	}

	for(size_t i = 0; i < counts; i++)
	{
		MsgId = Packet->readInt(32);
		UpdValue = Packet->readInt(32);
		SrcVec.push_back(std::make_pair(MsgId, UpdValue));
	}

	bool bFind;
	for(size_t i = 0; i < mBoardList.size(); i++)
	{
		bFind = false;
		for(size_t j = 0; j < SrcVec.size(); j++)
		{
			if(SrcVec[j].first == mBoardList[i]->MsgId)
			{
				if(SrcVec[j].second == mBoardList[i]->UpdValue)
					bFind = true;
				break;
			}
		}

		if(bFind == false)
		{
			stMallBoard* pMsg = new stMallBoard;
			memcpy(pMsg, mBoardList[i], sizeof(stMallBoard));
			TagVec.push_back(pMsg);
		}
	}

	if(TagVec.empty())
		return;

	counts = TagVec.size();
	if(counts >= 24)
		counts = 24;//���Ƕ�Ƶ�24����Ŀǰ���ֻ��2������

	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerId);
	if(pAccount)
	{
		char buf[8192];
		Base::BitStream sendPacket(buf,sizeof(buf));
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_MALL_QUERYBOARD_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
		sendPacket.writeInt(counts, 32);
		for(size_t i = 0; i < counts; i++)
		{
			sendPacket.writeBits(sizeof(stMallBoard)<<3, TagVec[i]);
		}
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket);
	}

	for(size_t i = 0; i < TagVec.size(); i++)
	{
		if(TagVec[i])
			delete TagVec[i];
	}
}*/
