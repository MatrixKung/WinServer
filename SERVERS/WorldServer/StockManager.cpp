/*
#include "StockManager.h"
#include "WorldServer.h"
#include "PlayerMgr.h"
#include "wintcp/dtServerSocket.h"
#include "DBLayer/Data//TBLGoldTrade.h"
#include "BillingManager.h"
#include "MailManager.h"
#include "DB_GoldTrade.h"
#include "GamePlayLog.h"

#ifndef _NTJ_UNITTEST
#include "Common/UnitTest.h"
#endif

GoldTradeManage::GoldTradeManage()
{
	registerEvent( CW_QUERYQUOTATION_REQUEST,	&GoldTradeManage::HandleCWQueryQuotation );
	registerEvent( ZW_COMMISSION_REQUEST,		&GoldTradeManage::HandleZWCommissionRequest );
	registerEvent( CW_CANCELORDER_REQUEST,		&GoldTradeManage::HandleCWCancelOrderRequest );
	registerEvent( CW_QUERYORDER_REQUEST,		&GoldTradeManage::HandleCWQueryOrderRequest );
	
	mNextCloseTime = 0;
	mClosePrice = mCurrentPrice = 0;
}

GoldTradeManage::~GoldTradeManage()
{
	ClearTable();
}

// ----------------------------------------------------------------------------
// World��������ʼ��Ԫ�����׹���
bool GoldTradeManage::Initialize()
{
	TBLGoldTrade tbl_goldtrade(SERVER->GetActorDB());
	return tbl_goldtrade.LoadOrder(mSellTable, mBuyTable) == DBERR_NONE;
}

// ----------------------------------------------------------------------------
// ��ʱȡ������18Сʱ�Ĺҵ�
void GoldTradeManage::Update()
{
	DO_LOCK(Lock::Player);
	DB_GoldTrade_BreakOrder* pDBHandle = new DB_GoldTrade_BreakOrder;
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// ����(�ر����йҵ�)
void GoldTradeManage::CloseOrder(U32 currentTime)
{
	DO_LOCK(Lock::Stock);
	DB_GoldTrade_CloseOrder* pDBHandle = new DB_GoldTrade_CloseOrder;
	pDBHandle->currentTime = currentTime;
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// �ӹҵ��б����޳���ʱ�Ĺҵ�
void GoldTradeManage::SetBreakOrder(std::vector<int>& buyList, std::vector<int>& sellList)
{
	DO_LOCK(Lock::Player);
	stGoldTrade* pOrder;

	MAPDESC::iterator itBuy;
	for(size_t i = 0; i < buyList.size(); i++)
	{
		for(itBuy = mBuyTable.begin(); itBuy != mBuyTable.end(); ++itBuy)
		{
			pOrder = itBuy->second;
			if(pOrder && pOrder->OrderID == buyList[i])
			{
				delete pOrder;
				mBuyTable.erase(itBuy++);
				break;
			}
		}
	}

	MAPASC::iterator itSell;
	for(size_t i = 0; i < sellList.size(); i++)
	{
		for(itSell = mSellTable.begin(); itSell != mSellTable.end(); ++itSell)
		{
			pOrder = itSell->second;
			if(pOrder && pOrder->OrderID == sellList[i])
			{
				delete pOrder;
				mSellTable.erase(itSell++);
				break;
			}
		}
	}
}

// ----------------------------------------------------------------------------
// �������̺�ʱ�����һ�����̼�
void GoldTradeManage::SetClosePrice(int closePrice, U32 nextCloseTime, VECCLOSE& taxList, VECCLOSE& goldList)
{
	DO_LOCK(Lock::Stock);
	mCurrentPrice = 0;
	mClosePrice = closePrice;
	mNextCloseTime = nextCloseTime;

	//��Ҫ��ʱ�������߹ҵ��б�������̵Ĺҵ���
	for(size_t i = 0; i < taxList.size(); i++)
	{
		MAPDESC::iterator it = mBuyTable.begin();
		for(; it != mBuyTable.end();++it)
		{
			stGoldTrade* pOrder = it->second;
			if(pOrder && pOrder->OrderID == taxList[i]->OrderID)
			{
				delete pOrder;
				mBuyTable.erase(it++);
				break;
			}
		}
	}

	for(size_t i = 0; i < goldList.size(); i++)
	{
		MAPASC::iterator it = mSellTable.begin();
		for(; it != mSellTable.end();++it)
		{
			stGoldTrade* pOrder = it->second;
			if(pOrder && pOrder->OrderID == goldList[i]->OrderID)
			{
				delete pOrder;
				mSellTable.erase(it++);
				break;
			}
		}
	}
	
	for each(stQuotation::stLatestPrice* it in mLatestPrice)
	{
		if(it) delete it;
	}
	mLatestPrice.clear();
}

// ----------------------------------------------------------------------------
// ��ӦZone�����ѯ�����
void GoldTradeManage::HandleCWQueryQuotation(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
{
	DO_LOCK(Lock::Stock);
	SendWCQueryQuotation(pHead->Id);
}

// ----------------------------------------------------------------------------
// ��ͻ��˻�Ӧ��ѯ���������
void GoldTradeManage::SendWCQueryQuotation(int PlayerID)
{
	stQuotation Quote;
	Quote.ClosePrice = mClosePrice;
	Quote.CurrentPrice = mCurrentPrice;
	Quote.Pendings = 0;

	int count = 0;
	for each(const std::pair<int, stGoldTrade*>& it in mBuyTable)
	{
		stGoldTrade* pOrder = it.second;
		if(!pOrder)
			continue;

		//ͳ�Ƶ�ǰ���δ���Ĺҵ���
		if(pOrder->PlayerID == PlayerID)
			Quote.Pendings++;

		if(Quote.BuyPrice[0].Price == 0)
		{
			Quote.BuyPrice[0].Price = pOrder->Price;
		}

		if(pOrder->Price == Quote.BuyPrice[count].Price)
		{
			Quote.BuyPrice[count].Num += pOrder->LeftNum;
		}
		else
		{
			if(count < stQuotation::MAX_ITEMS - 1)
			{
				count ++;
				Quote.BuyPrice[count].Price = pOrder->Price;
				Quote.BuyPrice[count].Num = pOrder->LeftNum;
			}
		}
	}

	count = 0;
	for each(const std::pair<int, stGoldTrade*>& it in mSellTable)
	{
		stGoldTrade* pOrder = it.second;
		if(!pOrder)
			continue;

		//ͳ�Ƶ�ǰ���δ���Ĺҵ���
		if(pOrder->PlayerID == PlayerID)
			Quote.Pendings++;

		if(Quote.SellPrice[0].Price == 0)
			Quote.SellPrice[0].Price = pOrder->Price;

		if(pOrder->Price == Quote.SellPrice[count].Price)
		{
			Quote.SellPrice[count].Num += pOrder->LeftNum;
		}
		else
		{
			if(count < stQuotation::MAX_ITEMS - 1)
			{
				count ++;
				Quote.SellPrice[count].Price = pOrder->Price;
				Quote.SellPrice[count].Num = pOrder->LeftNum;
			}			
		}
	}

	count = 0;
	for each(stQuotation::stLatestPrice* it in mLatestPrice)
	{
		if(!it)
			continue;
		Quote.LatestPrice[count] = *it;
		if(++count >= stQuotation::MAX_ITEMS)
			break;
	}

	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerID);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("Ϊ��ѯ��������ۻ�ȡ���������������!(PlayerID=%d)", PlayerID);
		return;
	}

	char buf[512];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_QUERYQUOTATION_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	sendPacket.writeBits(sizeof(stQuotation)<<3, &Quote);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket);
}

// ----------------------------------------------------------------------------
// ��������ZoneServerί�������������Ϣ
void GoldTradeManage::HandleZWCommissionRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet)
{
	DO_LOCK(Lock::Stock);
	DB_GoldTrade_CreateOrder* pDBHandle = new DB_GoldTrade_CreateOrder;
	Packet->readBits(sizeof(stGoldTrade)<<3, &pDBHandle->trade);
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

// ----------------------------------------------------------------------------
// ί�йҵ��ɹ���ʧ�ܻ�ӦZone
void GoldTradeManage::SendZoneCommission(int error, int PlayerID)
{
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerID);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("�ҵ�������Zone��Ӧ������޷���������������!(PlayerID=%d)", PlayerID);
		return;
	}

	char buf[128];
	Base::BitStream sendPacket( buf, sizeof(buf) );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_COMMISSION_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	sendPacket.writeInt(error, Base::Bit32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket);
}

// ----------------------------------------------------------------------------
// ���д���ί����Ϣ
void GoldTradeManage::CommissionOrder(bool IsOk, stGoldTrade* pTrade)
{
	DO_LOCK(Lock::Stock);

	if(IsOk == false)
	{
		//�����ҵ�ʧ�ܣ�����ί������Ԫ���ҵ��������˻���Ԫ
		if(pTrade->OrderType == OT_BUY)
		{
			S64 money = pTrade->Price * pTrade->TradeNum * 101/100;
			char szMsg[1024];
			sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ��ί������Ԫ���ҵ�����ʧ�ܣ�ϵͳ�˻����Ľ�Ǯ���뼰ʱ��ȡ.</t>",
				pTrade->PlayerName);
			SERVER->GetMailManager()->sendMail(0, pTrade->PlayerID, (S32)money, 0, 0, "��Ԫ���ҵ���", szMsg, 1);
			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(pTrade->PlayerID,WORLD_CLIENT_MailNotify );

			//��Client֪ͨ�ҵ����
			SendZoneCommission(GR_REDUCEGOLD, pTrade->PlayerID);
		}
		else
		{
			//��Client֪ͨ�ҵ����
			SendZoneCommission(GR_FAILED, pTrade->PlayerID);
		}		
		return;
	}

	int NeedNum		= pTrade->TradeNum;			//���ֹҵ��Ĺҵ�����
	int ReduceNum	= 0;						//���ֹҵ������ֹҵ��ɽ�������
	U32 curtime;
	_time32((__time32_t*)&curtime);

	DB_GoldTrade_CommissionOrder* pDBHandle = new DB_GoldTrade_CommissionOrder;
	pDBHandle->trade = *pTrade;					//���ֹҵ�����
	stGoldTradeEx* pSource = NULL;				//���ֹҵ�����

	if(pTrade->OrderType == OT_BUY)		//���ֹҵ�����Ϊ����Ԫ��
	{
		//������Ԫ���ҵ��б���ɸѡ�����з��Ϻ��ֹҵ�����۵����ֹҵ�
		MAPASC::iterator it = mSellTable.begin();
		for(; it != mSellTable.end();)
		{
			if(it->first > pTrade->Price)
				break;

			stGoldTrade* pOrder = it->second;
			if(!pOrder)
			{
				mSellTable.erase(it++);
				continue;
			}

			pSource = new stGoldTradeEx;
			memcpy(pSource, pOrder, sizeof(stGoldTrade));

			if(pOrder->LeftNum > NeedNum)
			{
				ReduceNum = NeedNum;
				pOrder->LeftNum -= NeedNum;
				NeedNum = 0;
				++it;
			}
			else
			{
				ReduceNum = pOrder->LeftNum;
				pOrder->LeftNum = 0;
				NeedNum -= ReduceNum;
				delete pOrder;
				mSellTable.erase(it++);
			}

			pSource->BargainNum	= ReduceNum;
			pDBHandle->source.push_back(pSource);

			//���������׼�¼
			AddLatestPrice(curtime, pSource->Price, ReduceNum);

			if(NeedNum == 0)
				break;
		}
	}
	else					 //���ֹҵ�����Ϊ����Ԫ��
	{
		//������Ԫ���ҵ��б���ɸѡ�����з��Ϻ��ֹҵ������۵����ֹҵ�
		MAPDESC::iterator it = mBuyTable.begin();
		for(; it != mBuyTable.end();)
		{
			if(it->first < pTrade->Price)
				break;

			stGoldTrade* pOrder = it->second;
			if(!pOrder)
			{
				mBuyTable.erase(it++);
				continue;
			}

			pSource = new stGoldTradeEx;
			memcpy(pSource, pOrder, sizeof(stGoldTrade));

			if(pOrder->LeftNum > NeedNum)
			{
				ReduceNum = NeedNum;
				pOrder->LeftNum -= NeedNum;
				NeedNum = 0;
				++it;
			}
			else
			{
				ReduceNum = pOrder->LeftNum;
				pOrder->LeftNum = 0;
				NeedNum -= ReduceNum;
				delete pOrder;
				mBuyTable.erase(it++);
			}

			pSource->BargainNum	= ReduceNum;
			pDBHandle->source.push_back(pSource);

			//���������׼�¼
			AddLatestPrice(curtime, pSource->Price, ReduceNum);

			if(NeedNum == 0)
				break;
		}
	}	
	
	//�жϺ��ֹҵ����б��е��������ֹҵ��Ƿ��н���
	if(pDBHandle->source.empty())
	{
		//��Ҫɾ�����ݿ��̶߳���
		delete pDBHandle;
		//���ֹҵ���Ҫֱ�ӽ��б�
		stGoldTrade* pTemp = new stGoldTrade;
		memcpy(pTemp, pTrade, sizeof(stGoldTrade));
		if(pTemp->OrderType == OT_BUY)
		{			
			mBuyTable.insert(MAPDESC::value_type(pTemp->Price, pTemp));
		}
		else
		{
			mSellTable.insert(MAPASC::value_type(pTemp->Price, pTemp));
		}

		//��Client֪ͨ�ҵ����
		SendZoneCommission(GR_PENDING, pTrade->PlayerID);
		if(pTrade->OrderType == OT_SELL)
		{
			//��Zone����ҷ���ʣ��Ԫ��
			SendWZLeftGold(pTrade->PlayerID, pTrade->AccountID);
		}
		//��ͻ�����ҷ������������
		SendWCQueryQuotation(pTrade->PlayerID);
	}
	else
	{
		//ֻ�������5�ʽ��׼�¼,����Ķ��޳���
		if(mLatestPrice.size() > stQuotation::MAX_ITEMS)
		{
			size_t count = mLatestPrice.size() - stQuotation::MAX_ITEMS;
			for(size_t i = 0; i < count; i++)
			{
				stQuotation::stLatestPrice* pPrice = mLatestPrice.front();
				if(pPrice)
					delete pPrice;
				mLatestPrice.pop_front();
			}
		}

		SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
	}
}

// ----------------------------------------------------------------------------
// Ԫ���ҵ����ݿ�������׽��
void GoldTradeManage::CommissionOrderResult(bool IsOk, stGoldTrade* pTrade, std::vector<stGoldTradeEx*> sourceList)
{
	DO_LOCK(Lock::Stock);
	if(IsOk == false)
	{
		//���ݿ�������׹ҵ�ʧ�ܣ�����Ҫ�����ֹҵ��ٷŻض���
		for(size_t i = 0; i < sourceList.size(); i++)
		{
			stGoldTradeEx* pSource = sourceList[i];
			if(!pSource)
				continue;
			stGoldTrade* pOrder = NULL;
			if(pSource->OrderType == OT_BUY)	//���ֹҵ�����Ϊ����Ԫ��
			{
				MAPDESC::iterator itB = mBuyTable.lower_bound(pSource->Price);
				MAPDESC::iterator itE = mBuyTable.upper_bound(pSource->Price);
				for(; itB != itE; ++itB)
				{
					pOrder = itB->second;
					if(pOrder && pOrder->OrderID == pSource->OrderID)
					{
						break;
					}
					pOrder = NULL;
				}

				if(pOrder == NULL)
				{
					pOrder = new stGoldTrade;
					memcpy(pOrder, pSource, sizeof(stGoldTrade));
					pOrder->LeftNum = pSource->BargainNum;
					mBuyTable.insert(MAPDESC::value_type(pOrder->Price, pOrder));
				}
				else
				{
					pOrder->LeftNum += pSource->BargainNum;
				}
			}
			else						//���ֹҵ�����Ϊ����Ԫ��
			{
				MAPASC::iterator itB = mSellTable.lower_bound(pSource->Price);
				MAPASC::iterator itE = mSellTable.upper_bound(pSource->Price);
				for(; itB != itE; ++itB)
				{
					pOrder = itB->second;
					if(pOrder && pOrder->OrderID == pSource->OrderID)
					{
						break;
					}
					pOrder = NULL;
				}

				if(pOrder == NULL)
				{
					pOrder = new stGoldTrade;
					memcpy(pOrder, pSource, sizeof(stGoldTrade));
					pOrder->LeftNum = pSource->BargainNum;
					mSellTable.insert(MAPASC::value_type(pOrder->Price, pOrder));
				}
				else
				{
					pOrder->LeftNum += pSource->BargainNum;
				}
			}
		}		
	}

	//�������ݿ���������ǳɹ�����ʧ�ܣ������ֹҵ�����ʣ������������Ҫ�������
	if(pTrade->TradeNum > 0 && pTrade->LeftNum > 0)
	{
		stGoldTrade* pTemp = new stGoldTrade;
		memcpy(pTemp, pTrade, sizeof(stGoldTrade));
		if(pTrade->OrderType == OT_BUY)
		{			
			mBuyTable.insert(MAPDESC::value_type(pTrade->Price, pTemp));
		}
		else
		{
			mSellTable.insert(MAPASC::value_type(pTrade->Price, pTemp));
		}

		if(pTrade->TradeNum == pTrade->LeftNum)
		{
			//��Client֪ͨ�ҵ����
			SendZoneCommission(GR_PENDING, pTrade->PlayerID);
		}
		else
		{
			//��Client֪ͨ�ҵ����
			SendZoneCommission(GR_PART, pTrade->PlayerID);
		}
	}
	else
	{
		if(IsOk)
		{
			//��Client֪ͨ�ҵ����
			SendZoneCommission(GR_FINISH, pTrade->PlayerID);
		}
	}

	if(IsOk)
	{
		//��Zone����ҷ���ʣ��Ԫ��
		SendWZLeftGold(pTrade->PlayerID, pTrade->AccountID);
		//��ͻ���֪ͨ�ʼ�����
		SERVER->GetPlayerManager()->SendSimplePacket(pTrade->PlayerID,WORLD_CLIENT_MailNotify );		
	}
	//��ͻ�����ҷ������������
	SendWCQueryQuotation(pTrade->PlayerID);
}

// ----------------------------------------------------------------------------
// ��������ZoneServer��ҳ����ҵ�����
void GoldTradeManage::HandleCWCancelOrderRequest(int SocketHandle, stPacketHead *pHead, Base::BitStream *Packet)
{
	DO_LOCK(Lock::Stock);

	int PlayerID = pHead->Id;
	int OrderID = Packet->readInt(32);
	int Price = Packet->readInt(32);	

	stGoldTrade* pOrder = NULL;
	MAPASC::iterator it = mSellTable.lower_bound(Price);
	for(; it != mSellTable.upper_bound(Price); ++it)
	{
		pOrder = it->second;
		if(pOrder && pOrder->PlayerID == PlayerID && pOrder->OrderID == OrderID)
		{
			mSellTable.erase(it);
			break;
		}
		pOrder = NULL;
	}

	if(pOrder == NULL)
	{
		MAPDESC::iterator it1 = mBuyTable.lower_bound(Price);
		for(; it1 != mBuyTable.upper_bound(Price); ++it1)
		{
			pOrder = it1->second;
			if(pOrder && pOrder->PlayerID == PlayerID && pOrder->OrderID == OrderID)
			{
				mBuyTable.erase(it1);
				break;
			}
			pOrder = NULL;
		}
	}

	if(pOrder)
	{
		DB_GoldTrade_CancelOrder* pDBHandle = new DB_GoldTrade_CancelOrder;
		pDBHandle->trade = *pOrder;
		SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
		delete pOrder;
	}
	else
	{
		SendWCCancelOrder(pHead->Id, OrderID, false);
	}
}

// ----------------------------------------------------------------------------
// �������ݿ���������ҵ����
void GoldTradeManage::CancelOrderResult(bool IsOk, stGoldTrade* pOrder)
{
	DO_LOCK(Lock::Stock);
	//��Ӧ�ͻ��˳����ɹ����
	SendWCCancelOrder(pOrder->PlayerID, pOrder->OrderID, IsOk);
	if(IsOk)
	{
		//��ͻ���֪ͨ�ʼ�����
		SERVER->GetPlayerManager()->SendSimplePacket(pOrder->PlayerID,WORLD_CLIENT_MailNotify );
		//��ͻ�����ҷ������������
		SendWCQueryQuotation(pOrder->PlayerID);
	}
	else
	{
		//��ʧ�ܣ�����Ҫ�Ż���ҵ����б���
		stGoldTrade* pTemp = new stGoldTrade;
		memcpy(pTemp, pOrder, sizeof(stGoldTrade));
		if(pTemp->OrderType == OT_BUY)
		{			
			mBuyTable.insert(MAPDESC::value_type(pTemp->Price, pTemp));
		}
		else
		{
			mSellTable.insert(MAPASC::value_type(pTemp->Price, pTemp));
		}
	}
}

// ----------------------------------------------------------------------------
// ��ͻ��˻�Ӧ����ί�йҵ�
void GoldTradeManage::SendWCCancelOrder(int PlayerID, int OrderID, bool IsOK)
{
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerID);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("Ϊ��Ӧ����ί�йҵ���ȡ���������������!(PlayerID=%d)", PlayerID);
		return;
	}
	char buf[128];
	Base::BitStream sendPacket( buf, sizeof(buf) );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_CANCELORDER_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	sendPacket.writeInt(OrderID, Base::Bit32);
	sendPacket.writeFlag(IsOK);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket);
}

// ----------------------------------------------------------------------------
// �������Բ�ѯָ����ҹҵ����ݵ�����
void GoldTradeManage::HandleCWQueryOrderRequest(int SocketHandle,stPacketHead* pHead, Base::BitStream* Packet)
{
	DO_LOCK(Lock::Stock);
	DB_GoldTrade_QueryOrder* pDBHandle = new DB_GoldTrade_QueryOrder;
	pDBHandle->PlayerID = pHead->Id;
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}

void GoldTradeManage::SendWCQueryOrder(int PlayerID, bool IsOk, std::vector<stGoldTrade*>& order,
									   std::vector<stGoldTradeDetail*>& detail)
{
	DO_LOCK(Lock::Stock);
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerID);
	if(NULL == pAccount)
	{
		g_Log.WriteWarn("Ϊ��Ӧ��ѯί�йҵ���ȡ���������������!(PlayerID=%d)", PlayerID);
		return;
	}

	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket(buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_QUERYORDER_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
	if(IsOk)
	{
		sendPacket.writeInt(order.size(), 8);
		for(size_t i = 0; i < order.size(); i++)
		{
			sendPacket.writeBits(sizeof(stGoldTrade)<<3, order[i]);
		}
		sendPacket.writeInt(detail.size(), 16);
		for(size_t i = 0; i < detail.size(); i++)
		{
			sendPacket.writeBits(sizeof(stGoldTradeDetail)<<3, detail[i]);
		}
	}
	else
	{
		sendPacket.writeInt(0, 8);
	}
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket);
}

// ----------------------------------------------------------------------------
// ������½��׼�¼
void GoldTradeManage::AddLatestPrice(U32 tradedate, S32 price, U16 num)
{
	mCurrentPrice = price;
	stQuotation::stLatestPrice* pPrice = new stQuotation::stLatestPrice;
	pPrice->TradeDate = tradedate;
	pPrice->Num = num;
	pPrice->Price = price;
	mLatestPrice.push_back(pPrice);
}

// ----------------------------------------------------------------------------
// ���ί�н������ݱ�
void GoldTradeManage::ClearTable()
{
	for each(const std::pair<int, stGoldTrade*>& it in mSellTable)
	{
		if(it.second) delete it.second;
	}

	for each(const std::pair<int, stGoldTrade*>& it in mBuyTable)
	{
		if(it.second) delete it.second;
	}

	for each(stQuotation::stLatestPrice* it in mLatestPrice)
	{
		if(it) delete it;
	}
	mSellTable.clear();
	mBuyTable.clear();
	mLatestPrice.clear();
}

// ----------------------------------------------------------------------------
// ��Zone����ʣ��Ԫ������
void GoldTradeManage::SendWZLeftGold(int PlayerID, int AccountID)
{
	DO_LOCK(Lock::Stock);

	DB_Billing_QueryGold* pDBHandle = new DB_Billing_QueryGold;
	pDBHandle->AccountID	= AccountID;
	pDBHandle->PlayerID		= PlayerID;
	SERVER->GetGoldTradeDBManager()->SendPacket(pDBHandle);
}
*/