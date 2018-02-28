#ifndef __GOLDTRADEMANAGE_H__
#define __GOLDTRADEMANAGE_H__
/*
#include "Common/StockBase.h"
#include "base/bitStream.h"
#include "LockCommon.h"
#include "ManagerBase.h"
#include <deque>
#include <vector>

// ============================================================================
// Ԫ�����׹���
//
class GoldTradeManage : public CManagerBase< GoldTradeManage, MSG_GOLDTRADE_BEGIN, MSG_GOLDTRADE_END >
{
public:
	typedef std::deque<stQuotation::stLatestPrice*>  VECPRICE;
	typedef std::vector<stCloseGoldTrade*> VECCLOSE;
	GoldTradeManage();
	~GoldTradeManage();
	bool Initialize();
	void Update();

	void SetBreakOrder(std::vector<int>& buyList, std::vector<int>& sellList);
	void CloseOrder(U32 currentTime);
	void SetClosePrice(int closePrice, U32 nextCloseTime, VECCLOSE& taxList, VECCLOSE& goldList);

	void HandleZWCommissionRequest(int, stPacketHead*, Base::BitStream*);
	void CommissionOrder(bool IsOk, stGoldTrade* trade);
	void CommissionOrderResult(bool IsOk, stGoldTrade* pTrade, std::vector<stGoldTradeEx*> sourceList);
	void SendZoneCommission(int error, int PlayerID);

	void HandleCWCancelOrderRequest(int, stPacketHead*, Base::BitStream*);
	void CancelOrderResult(bool IsOk, stGoldTrade* pOrder);

	void HandleCWQueryOrderRequest(int, stPacketHead*, Base::BitStream*);
	void SendWCQueryOrder(int PlayerID, bool IsOk, std::vector<stGoldTrade*>& order,
		std::vector<stGoldTradeDetail*>& detail);
	void HandleCWQueryQuotation(int, stPacketHead*, Base::BitStream*);
#ifndef _NTJ_UNITTEST
private:
#else
public:
#endif
	void ClearTable();
	void SendWZLeftGold(int PlayerID, int AccountID);
	void SendWCCancelOrder(int, int, bool);
	void SendWCQueryQuotation(int);
	void AddLatestPrice(U32 tradedate, S32 price, U16 num);

#ifndef _NTJ_UNITTEST
private:
#else
public:
#endif
	MAPASC				mSellTable;			//ί�������б�
	MAPDESC				mBuyTable;			//ί�������б�
	VECPRICE			mLatestPrice;		//������׼�¼

	U32					mNextCloseTime;		//��һ������ʱ��
	S32					mCurrentPrice;		//����ɽ���
	S32					mClosePrice;		//������̼�
};
*/
#endif//__GOLDTRADEMANAGE_H__