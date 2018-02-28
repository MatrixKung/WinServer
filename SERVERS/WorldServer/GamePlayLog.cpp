/*
#include "WorldServer.h"
#include "GamePlayLog.h"
#include "Common/PlayerStruct.h"

// ----------------------------------------------------------------------------
// ��¼�ʺŵ���ʱ����־
void GameplayLog::GL_AccountLogin(int AccountId)
{
	if(!GL_IsRecord(AccountId, 0, LOG_ACCOUNT_LOGIN))
		return;
	stAccountLog log;
	log.setlog(dbStruct::createeid(),0,TARGET_ACCOUNT,AccountId,LOG_ACCOUNT_LOGIN);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
//��¼�ʺŵǳ�ʱ����־
void GameplayLog::GL_AccountLogout(int AccountId)
{
	if(!GL_IsRecord(AccountId, 0, LOG_ACCOUNT_LOGOUT))
		return;
	stAccountLog log;
	log.setlog(dbStruct::createeid(),0,TARGET_ACCOUNT,AccountId,LOG_ACCOUNT_LOGOUT);
	SERVER->GetLog()->writeLog(&log);
}


// ----------------------------------------------------------------------------
// ��¼��ɫ����ʱ����־
void GameplayLog::GL_PlayerLogin(int PlayerId)
{
	if(!GL_IsRecord(0, PlayerId, LOG_PLAYER_LOGIN))
		return;
	stPlayerLog log;
	log.setlog(dbStruct::createeid(),0,TARGET_PLAYER,PlayerId,LOG_PLAYER_LOGIN);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼��ɫ�ǳ�ʱ����־
void GameplayLog::GL_PlayerLogout(int PlayerId)
{
	if(!GL_IsRecord(0, PlayerId, LOG_PLAYER_LOGOUT))
		return;
	stPlayerLog log;
	log.setlog(dbStruct::createeid(),0,TARGET_PLAYER,PlayerId,LOG_PLAYER_LOGOUT);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼������ɫ��־
void GameplayLog::GL_PlayerCreate(int PlayerId)
{
	if(!GL_IsRecord(0, PlayerId, LOG_PLAYER_CREATE))
		return;
	stPlayerLog log;
	log.setlog(dbStruct::createeid(),0,TARGET_PLAYER,PlayerId,LOG_PLAYER_CREATE);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼ɾ����ɫ��־
void GameplayLog::GL_PlayerDelete(int PlayerId, int level)
{
	if(!GL_IsRecord(0, PlayerId, LOG_PLAYER_DELETE))
		return;
	stPlayerLog log;
	log.setlog(dbStruct::createeid(),0,TARGET_PLAYER,PlayerId,LOG_PLAYER_DELETE);
	log.Val[0] = level;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼��ɫԪ���һ�
void GameplayLog::GL_GoldExchange(int PlayerId, int type, int change, int item)
{
	if(!GL_IsRecord(0, PlayerId, LOG_PLAYER_GOLDEXCHANGE))
		return;
	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	log.setlog(eid,PlayerId,TARGET_PLAYER,PlayerId,LOG_PLAYER_GOLDEXCHANGE);
	log.Val[0] = type;
	log.Val[1] = change;
	log.Val[2] = item;
	SERVER->GetLog()->writeLog(&log);

	stMoneyLog moneylog;
	moneylog.setlog(eid,PlayerId,TARGET_PLAYER,PlayerId,LOG_PLAYER_GOLDEXCHANGE);
	moneylog.DataType = stMoneyLog::TYPE_GOLD;
	moneylog.Val[0] = -change;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ��¼��ɢ����
void GameplayLog::GL_PlayerTeamBreak(int leaderId, TeamMemberInfo* teamList)
{
	if(!GL_IsRecord(0, leaderId, LOG_PLAYER_TEAMBREAK))
		return;
	if(!teamList)
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),leaderId,TARGET_PLAYER,teamList[0].playerID,LOG_PLAYER_TEAMBREAK);
	for(int i = 1; i < 6; i++)
		log.Val[i-1] = teamList[i].playerID;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼�����ӳ�
void GameplayLog::GL_PlayerTeamToLeader(int oldLeaderId, int newLeaderId)
{
	if(!GL_IsRecord(0, oldLeaderId, LOG_PLAYER_TEAMTOLEADER))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),oldLeaderId,TARGET_PLAYER,newLeaderId,LOG_PLAYER_TEAMTOLEADER);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼����������
void GameplayLog::GL_PlayerTeamInvate(int playerId, int other)
{
	if(!GL_IsRecord(0, playerId, LOG_PLAYER_TEAMINVATE))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),playerId,TARGET_PLAYER,other,LOG_PLAYER_TEAMINVATE);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼�ӳ��߶�Ա
void GameplayLog::GL_PlayerTeamKick(int leaderId, int playerId)
{
	if(!GL_IsRecord(0, leaderId, LOG_PLAYER_TEAMKICK))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),leaderId,TARGET_PLAYER,playerId,LOG_PLAYER_TEAMKICK);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼��Ա�뿪
void GameplayLog::GL_PlayerTeamExit(int playerId, int leaderId)
{
	if(!GL_IsRecord(0, playerId, LOG_PLAYER_TEAMEXIT))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),playerId,TARGET_PLAYER,leaderId,LOG_PLAYER_TEAMEXIT);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼����������
void GameplayLog::GL_PlayerTeamJoin(int playerId, int leaderId)
{
	if(!GL_IsRecord(0, playerId, LOG_PLAYER_TEAMJOIN))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),playerId,TARGET_PLAYER,leaderId,LOG_PLAYER_TEAMJOIN);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼��ɫ�������ϵ
void GameplayLog::GL_PlayerMakeSocial(int playerId, int targetId, int srcType, int tagType)
{
	if(!GL_IsRecord(0, playerId, LOG_PLAYER_MAKESOCIAL))
		return;

	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	if(srcType > 0)
	{
		log.setlog(eid,playerId,TARGET_PLAYER,targetId,LOG_PLAYER_MAKESOCIAL);
		log.Val[0] = srcType;
		SERVER->GetLog()->writeLog(&log);
	}

	if(tagType > 0)
	{
		log.setlog(eid,targetId,TARGET_PLAYER,playerId,LOG_PLAYER_MAKESOCIAL);
		log.Val[0] = tagType;
		SERVER->GetLog()->writeLog(&log);
	}
}

// ----------------------------------------------------------------------------
// ��¼��ɫ�ı�����ϵ
void GameplayLog::GL_PlayerChangeSocial(int playerId, int targetId, int oldSrcType, int newSrcType, int oldTagType, int newTagType)
{
	if(!GL_IsRecord(0, playerId, LOG_PLAYER_CHANGESOCIAL))
		return;

	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	if(oldSrcType > 0 || newSrcType > 0)
	{
		log.setlog(eid,playerId,TARGET_PLAYER,targetId,LOG_PLAYER_CHANGESOCIAL);
		log.Val[0] = oldSrcType;
		log.Val[1] = newSrcType;
		SERVER->GetLog()->writeLog(&log);
	}

	if(oldTagType > 0 || newTagType > 0)
	{
		log.setlog(eid,targetId,TARGET_PLAYER,playerId,LOG_PLAYER_CHANGESOCIAL);
		log.Val[0] = oldTagType;
		log.Val[1] = newTagType;
		SERVER->GetLog()->writeLog(&log);
	}
}

// ----------------------------------------------------------------------------
// ��¼��ɫɾ������ϵ
void GameplayLog::GL_PlayerDeleteSocial(int playerId, int targetId, int srcType, int tagType)
{
	if(!GL_IsRecord(0, playerId, LOG_PLAYER_DELETESOCIAL))
		return;

	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	if(srcType > 0)
	{
		log.setlog(eid,playerId,TARGET_PLAYER,targetId,LOG_PLAYER_DELETESOCIAL);
		log.Val[0] = srcType;
		SERVER->GetLog()->writeLog(&log);
	}	

	if(tagType > 0)
	{
		log.setlog(eid,targetId,TARGET_PLAYER,playerId,LOG_PLAYER_DELETESOCIAL);
		log.Val[0] = tagType;
		SERVER->GetLog()->writeLog(&log);
	}
}

// ----------------------------------------------------------------------------
// ��ȡԪ��
void GameplayLog::GL_DrawGold(int PlayerID, int Gold, int AccountGold, int LeftGold)
{
	if(!GL_IsRecord(0, PlayerID, LOG_GOLD_DRAWGOLD))
		return;

	stMoneyLog log;
	log.setlog(dbStruct::createeid(),PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_DRAWGOLD);
	log.DataType = stMoneyLog::TYPE_GOLD;
	log.Val[0] = Gold;
	log.Val[1] = AccountGold;
	log.Val[2] = LeftGold;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// �̳ǹ�������Ԫ��
void GameplayLog::GL_BuyGoods(int PlayerID, int FriendID, int Gold, int LeftGold, U64 PriceID, int PriceNum)
{
	if(!GL_IsRecord(0, PlayerID, LOG_GOLD_BUYGOODS))
		return;

	U64 eid = dbStruct::createeid();
	stMoneyLog log;
	log.setlog(eid,PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_BUYGOODS);
	log.DataType = stMoneyLog::TYPE_GOLD;
	log.Val[0] = Gold;
	log.Val[1] = LeftGold;
	SERVER->GetLog()->writeLog(&log);

	stItemLog itemLog;
	itemLog.setlog(eid,PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_BUYGOODS);
	itemLog.ItemUID = PriceID;
	itemLog.SrcNum = PriceNum;
	itemLog.ShopID = 0;		//˵����Ԫ���̳�
	SERVER->GetLog()->writeLog(&itemLog);
}

// ----------------------------------------------------------------------------
// ��ɫ����Ԫ��
void GameplayLog::GL_ReduceGold(int PlayerID, U32 type, int Gold, int LeftGold)
{
	if(!GL_IsRecord(0, PlayerID, type))
		return;

	stMoneyLog log;
	log.setlog(dbStruct::createeid(),PlayerID,TARGET_PLAYER,PlayerID,type);
	log.DataType = stMoneyLog::TYPE_GOLD;
	log.Val[0] = Gold;
	log.Val[1] = LeftGold;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ���׳ɹ�����Ԫ���ҵ�
void GameplayLog::GL_TradeBuyOrder(int PlayerID, int OrderID, int SrcPrice, int BargainNum,
			int TargetID, int TargetOrderID, int TagPrice)
{
	if(!GL_IsRecord(0, PlayerID, LOG_GOLD_TRADEBUYORDER))
		return;

	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	//�ҵ���
	log.setlog(eid,PlayerID,TARGET_PLAYER,TargetID,LOG_GOLD_TRADEBUYORDER);
	log.Val[0] = OrderID;
	log.Val[1] = TargetOrderID;
	log.Val[2] = SrcPrice;
	log.Val[3] = SrcPrice;
	log.Val[4] = BargainNum;
	SERVER->GetLog()->writeLog(&log);

	//������
	log.setlog(eid, TargetID,TARGET_PLAYER,PlayerID,LOG_GOLD_TRADEBUYORDER_TARGET);
	log.Val[0] = TargetOrderID;
	log.Val[1] = OrderID;
	log.Val[2] = TagPrice;
	log.Val[3] = SrcPrice;
	log.Val[4] = BargainNum;
	SERVER->GetLog()->writeLog(&log);

	stMoneyLog moneylog;
	//�ҵ���
	moneylog.setlog(eid,PlayerID,TARGET_PLAYER,TargetID,LOG_GOLD_TRADEBUYORDER);
	moneylog.DataType = stMoneyLog::TYPE_GOLD;
	moneylog.Val[0] = 100*BargainNum;
	SERVER->GetLog()->writeLog(&moneylog);	

	//������
	moneylog.setlog(eid,TargetID,TARGET_PLAYER,PlayerID,LOG_GOLD_TRADEBUYORDER_TARGET);
	moneylog.DataType = stMoneyLog::TYPE_GOLD;
	moneylog.Val[0] = -100*BargainNum;
	SERVER->GetLog()->writeLog(&moneylog);

	//������
	moneylog.setlog(eid,TargetID,TARGET_PLAYER,PlayerID,LOG_GOLD_TRADEBUYORDER_TARGET);
	moneylog.DataType = stMoneyLog::TYPE_TAX;
	moneylog.Val[0] = -SrcPrice*BargainNum/100;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ���׳ɹ�����Ԫ���ҵ�
void GameplayLog::GL_TradeSellOrder(int PlayerID, int OrderID, int SrcPrice, int BargainNum,
									int TargetID, int TargetOrderID, int TagPrice)
{
	if(!GL_IsRecord(0, PlayerID, LOG_GOLD_TRADESELLORDER))
		return;

	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	//�ҵ���
	log.setlog(eid,PlayerID,TARGET_PLAYER,TargetID,LOG_GOLD_TRADESELLORDER);
	log.Val[0] = OrderID;
	log.Val[1] = TargetOrderID;
	log.Val[2] = SrcPrice;
	log.Val[3] = SrcPrice;
	log.Val[4] = BargainNum;
	SERVER->GetLog()->writeLog(&log);

	//������
	log.setlog(eid, TargetID,TARGET_PLAYER,PlayerID,LOG_GOLD_TRADESELLORDER_TARGET);
	log.Val[0] = TargetOrderID;
	log.Val[1] = OrderID;
	log.Val[2] = TagPrice;
	log.Val[3] = SrcPrice;
	log.Val[4] = BargainNum;
	SERVER->GetLog()->writeLog(&log);

	stMoneyLog moneylog;
	//�ҵ���
	moneylog.setlog(eid,PlayerID,TARGET_PLAYER,TargetID,LOG_GOLD_TRADESELLORDER);
	moneylog.DataType = stMoneyLog::TYPE_GOLD;
	moneylog.Val[0] = -100*BargainNum;
	SERVER->GetLog()->writeLog(&moneylog);

	//�ҵ���
	moneylog.setlog(eid,PlayerID,TARGET_PLAYER,TargetID,LOG_GOLD_TRADESELLORDER);
	moneylog.DataType = stMoneyLog::TYPE_TAX;
	moneylog.Val[0] = -SrcPrice*BargainNum/100;
	SERVER->GetLog()->writeLog(&moneylog);

	//������
	moneylog.setlog(eid,TargetID,TARGET_PLAYER,PlayerID,LOG_GOLD_TRADESELLORDER_TARGET);
	moneylog.DataType = stMoneyLog::TYPE_GOLD;
	moneylog.Val[0] = 100*BargainNum;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ί������Ԫ���ҵ�
void GameplayLog::GL_BuyOrder(int PlayerID, int OrderID, int Price, int TradeNum)
{
	if(!GL_IsRecord(0, PlayerID, LOG_GOLD_BUYORDER))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_BUYORDER);
	log.Val[0] = OrderID;
	log.Val[1] = Price;
	log.Val[2] = TradeNum;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ί������Ԫ���ҵ�
void GameplayLog::GL_SellOrder(int PlayerID, int OrderID, int Price, int TradeNum)
{
	if(!GL_IsRecord(0, PlayerID, LOG_GOLD_SELLORDER))
		return;

	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	log.setlog(eid,PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_SELLORDER);
	log.Val[0] = OrderID;
	log.Val[1] = Price;
	log.Val[2] = TradeNum;
	SERVER->GetLog()->writeLog(&log);

	stMoneyLog moneylog;
	moneylog.setlog(eid,PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_SELLORDER);
	moneylog.DataType = stMoneyLog::TYPE_GOLD;
	moneylog.Val[0] = -100*TradeNum;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ���̷�������Ԫ���ҵ�˰��
void GameplayLog::GL_CloseGoldTrade_Tax(int PlayerID, int tax)
{
	if(!GL_IsRecord(0, PlayerID, LOG_GOLD_CLOSEORDER_TAX))
		return;

	stMoneyLog moneylog;
	moneylog.setlog(dbStruct::createeid(),PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_CLOSEORDER_TAX);
	moneylog.DataType = stMoneyLog::TYPE_TAX;
	moneylog.Val[0] = tax;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ��ҳ�����¼��־
void GameplayLog::GL_CancelGoldTrade(int PlayerID, int OrderID, int type, int price, int num )
{
	if(!GL_IsRecord(0, PlayerID, type))
		return;

	U64 eid = dbStruct::createeid();
	if(type == LOG_GOLD_CANCELBUYORDER)		//��������Ԫ���ҵ�
	{		
		stPlayerLog log;
		log.setlog(eid,PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_CANCELBUYORDER);
		log.Val[0] = OrderID;
		log.Val[1] = price;
		log.Val[2] = num;
		SERVER->GetLog()->writeLog(&log);
		stMoneyLog moneylog;
		moneylog.setlog(eid,PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_CANCELBUYORDER);
		moneylog.DataType = stMoneyLog::TYPE_TAX;
		moneylog.Val[0] = price * num /100;
		SERVER->GetLog()->writeLog(&moneylog);
	}
	else									//��������Ԫ���ҵ�
	{
		stPlayerLog log;
		log.setlog(eid,PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_CANCELSELLORDER);
		log.Val[0] = OrderID;
		log.Val[1] = price;
		log.Val[2] = num;
		SERVER->GetLog()->writeLog(&log);
		stMoneyLog moneylog;
		moneylog.setlog(eid,PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_CANCELSELLORDER);
		moneylog.DataType = stMoneyLog::TYPE_GOLD;
		moneylog.Val[0] = num *100;
		SERVER->GetLog()->writeLog(&moneylog);
	}	
}

// ----------------------------------------------------------------------------
// ���̷�������Ԫ���ҵ�ʣ��Ԫ������
void GameplayLog::GL_CloseGoldTrade_Gold(int PlayerID, int Gold)
{
	if(!GL_IsRecord(0, PlayerID, LOG_GOLD_CLOSEORDER_GOLD))
		return;

	stMoneyLog moneylog;
	moneylog.setlog(dbStruct::createeid(),PlayerID,TARGET_PLAYER,PlayerID,LOG_GOLD_CLOSEORDER_GOLD);
	moneylog.DataType = stMoneyLog::TYPE_GOLD;
	moneylog.Val[0] = Gold;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ��¼�ʼ�ɾ��
void GameplayLog::GL_MailDelete(stMailItem& mailitem)
{
	if(!GL_IsRecord(0, mailitem.nSender, LOG_MAIL_DELETE))
		return;

	stMailLog log;
	log.setlog(dbStruct::createeid(), mailitem.nSender, TARGET_PLAYER, mailitem.nRecver, LOG_MAIL_DELETE);
	log.info = mailitem;
	SERVER->GetLog()->writeLog(&log);
}
*/


/********************************************************************************************************/
/********************************************************************************************************/
/********************************************************************************************************/
/************************        ��  ��  ��  GM  ��  ��  ��  ־        **********************************/
/********************************************************************************************************/
/********************************************************************************************************/
/********************************************************************************************************/
// ----------------------------------------------------------------------------
/*
// ��¼GM����
void GameplayLog::GL_GMAccountLogin(int GMAccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_LOGIN))
		return;
	stAccountLog log;
	log.setlog(dbStruct::createeid(),0,TARGET_ACCOUNT,GMAccountId,LOG_GM_LOGIN);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�ǳ�
void GameplayLog::GL_GMAccountLogout(int GMAccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_LOGOUT))
		return;
	stAccountLog log;
	log.setlog(dbStruct::createeid(),0,TARGET_ACCOUNT,GMAccountId,LOG_GM_LOGOUT);
	SERVER->GetLog()->writeLog(&log);
}


// ----------------------------------------------------------------------------
// ��¼GM��ѯ�ʺ�
void GameplayLog::GL_GMAccountQuery(int GMAccountId, int AccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_QUERYACCOUNT))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(),GMAccountId,TARGET_ACCOUNT,AccountId,LOG_GM_QUERYACCOUNT);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�޸��˺�Ԫ��
void GameplayLog::GL_GMUpdateAccountGold(int GMAccountId, int AccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_UPDATEACCOUNTGOLD))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(),GMAccountId,TARGET_ACCOUNT,AccountId,LOG_GM_UPDATEACCOUNTGOLD);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�޸Ľ�ɫԪ��
void GameplayLog::GL_GMUpdateActorGold(int GMAccountId, int AccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_UPDATEACTORGOLD))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(),GMAccountId,TARGET_ACCOUNT,AccountId,LOG_GM_UPDATEACTORGOLD);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�����ʺ�
void GameplayLog::GL_GMFreezeAccount(int GMAccountId, int AccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_FREEZEACCOUNT))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(),GMAccountId,TARGET_ACCOUNT,AccountId,LOG_GM_FREEZEACCOUNT);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�ⶳ�ʺ�
void GameplayLog::GL_GMUnfreezeAccount(int GMAccountId, int AccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_UNFREEEZEACCOUNT))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(),GMAccountId,TARGET_ACCOUNT,AccountId,LOG_GM_UNFREEEZEACCOUNT);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�����ʺ�
void GameplayLog::GL_GMBanAccount(int GMAccountId, int AccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_BANACCOUNT))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(),GMAccountId,TARGET_ACCOUNT,AccountId,LOG_GM_BANACCOUNT);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�����ʺ�
void GameplayLog::GL_GMUnbanAccount(int GMAccountId, int AccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_UNBANACCOUNT))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_ACCOUNT,AccountId,LOG_GM_UNBANACCOUNT);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM������־
void GameplayLog::GL_GMKickAccount(int GMAccountId, int AccountId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_KICKACCOUNT))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_ACCOUNT,AccountId,LOG_GM_KICKACCOUNT);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM���Խ�ɫ
void GameplayLog::GL_GMBanActor(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_BANACTOR))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,playerId,LOG_GM_BANACTOR);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�����ɫ
void GameplayLog::GL_GMUnbanActor(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_UNBANACTOR))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,playerId,LOG_GM_UNBANACTOR);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�����ɫ
void GameplayLog::GL_GMFreezeActor(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_FREEZEACTOR))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,playerId,LOG_GM_FREEZEACTOR);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�ⶳ��ɫ
void GameplayLog::GL_GMUnfreezeActor(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_UNFREEZEACTOR))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,playerId,LOG_GM_UNFREEZEACTOR);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�߳���ɫ
void GameplayLog::GL_GMKickActor(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_KICKACTOR))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,playerId,LOG_GM_KICKACTOR);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM���ͽ�ɫ
void GameplayLog::GL_GMTransferActor(int GMAccountId, int playerId, int zoneId, float x, float y, float z)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_TRANSPORTACTOR))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, zoneId, x, y, z, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_TRANSPORTACTOR);
	SERVER->GetLog()->writeLog(&log);

	stPlayerLog playerlog;
	playerlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_TRANSPORTACTOR);
	playerlog.DataType = stPlayerLog::TYPE_POSITION;
	playerlog.Val[0] = zoneId;
	playerlog.Val[1] = (int)x;
	playerlog.Val[2] = (int)y;
	playerlog.Val[3] = (int)z;
	SERVER->GetLog()->writeLog(&playerlog);
}

// ----------------------------------------------------------------------------
// ��¼GM��ѯ��ɫ
void GameplayLog::GL_GMActorQuery(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_QUERYACTOR))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,playerId,LOG_GM_QUERYACTOR);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM��ѯ�ѱ�ɾ����ɫ
void GameplayLog::GL_GMDelActorQuery(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_QUERYDELACTOR))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,playerId,LOG_GM_QUERYDELACTOR);
	SERVER->GetLog()->writeLog(&log);
}


// ----------------------------------------------------------------------------
// ��¼GM�ָ�ɾ����ɫ
void GameplayLog::GL_GMUnDelActor(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_UNDELACTOR))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,playerId,LOG_GM_UNDELACTOR);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GMɾ����ɫ
void GameplayLog::GL_GMDelActor(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_DELACTOR))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,playerId,LOG_GM_DELACTOR);
	SERVER->GetLog()->writeLog(&log);
}


// ----------------------------------------------------------------------------
// ��¼GM�޸Ľ�ɫ�ȼ�
void GameplayLog::GL_GMModifyLevel(int GMAccountId, int playerId, int oldlevel, int newlevel)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MODIFYACTORLEVEL))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MODIFYACTORLEVEL);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"oldlevel:%d,newlevel:%d",oldlevel,newlevel);
	dStrcpy(log.content,sizeof(log.content),txt);
	SERVER->GetLog()->writeLog(&log);

	stPlayerLog playerlog;
	playerlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MODIFYACTORLEVEL);
	playerlog.DataType = stPlayerLog::TYPE_LEVEL;
	playerlog.Val[0] = oldlevel;
	playerlog.Val[1] = newlevel;
	SERVER->GetLog()->writeLog(&playerlog);
}

// ----------------------------------------------------------------------------
// ��¼GM�޸Ľ�ɫ����
void GameplayLog::GL_GMModifyEXP(int GMAccountId, int playerId, int oldexp, int newexp)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_EXPSET))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_EXPSET);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"oldexp:%d,newexp:%d",oldexp,newexp);
	dStrcpy(log.content,sizeof(log.content),txt);
	SERVER->GetLog()->writeLog(&log);

	stPlayerLog playerlog;
	playerlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_EXPSET);
	playerlog.DataType = stPlayerLog::TYPE_EXP;
	playerlog.Val[0] = oldexp;
	playerlog.Val[1] = newexp;
	SERVER->GetLog()->writeLog(&playerlog);
}
//GM�޸ľ���������
void GameplayLog::GL_GMPvpScoreUpdate(int GMAccountId, int playerId, int oldPvpScore, int newPvpScore)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_PVPSCOREUPDATE))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_PVPSCOREUPDATE);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"oldPvpScore:%d,newPvpScore:%d",oldPvpScore,newPvpScore);
	dStrcpy(log.content,sizeof(log.content),txt);
	SERVER->GetLog()->writeLog(&log);
}
// ----------------------------------------------------------------------------
// ��¼GM�޸Ľ�ɫ��Ǯ
void GameplayLog::GL_GMModifyGold(int GMAccountId, int playerId, int oldgold, int newgold, int oldBindMoney, int newBindMoney, int oldBindGold, int newBindGold)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_GOLDSET))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_GOLDSET);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"oldgold:%d,newgold:%d oldBindMoney:%d,newBindMoney:%d oldBindGold:%d,newBindGold:%d",oldgold,newgold,oldBindMoney,newBindMoney,oldBindGold,newBindGold);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stMoneyLog moneylog;
	moneylog.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_GOLDSET);
	moneylog.DataType = stMoneyLog::TYPE_MONEY;
	moneylog.Val[0] = newgold - oldgold;
	moneylog.Val[1] = newgold;
	moneylog.Val[2] = oldgold;
	SERVER->GetLog()->writeLog(&moneylog);

	stMoneyLog bindMoneylog;
	bindMoneylog.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_GOLDSET);
	bindMoneylog.DataType = stMoneyLog::TYPE_BINDMONEY;
	bindMoneylog.Val[0] = newBindMoney - oldBindMoney;
	bindMoneylog.Val[1] = newBindMoney;
	bindMoneylog.Val[2] = oldBindMoney;
	SERVER->GetLog()->writeLog(&bindMoneylog);

	stMoneyLog bindGoldlog;
	bindGoldlog.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_GOLDSET);
	bindGoldlog.DataType = stMoneyLog::TYPE_BINDGOLD;
	bindGoldlog.Val[0] = newBindGold - oldBindGold;
	bindGoldlog.Val[1] = newBindGold;
	bindGoldlog.Val[2] = oldBindGold;
	SERVER->GetLog()->writeLog(&bindGoldlog);


}

void GameplayLog::GL_GMActorRename(int GMAccountId, int playerId, const char* oldName, const char* newName)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_RENAME))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_RENAME);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"oldName:%s,newName:%s",oldName,newName);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

}
//GM���������Ҷ�������
void GameplayLog::GL_GMActorClrSecondPassword(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_CLRSECONDPASSWORLD))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_CLRSECONDPASSWORLD);
	char text[250] = "��ն�������";
	dStrcpy(log.content,sizeof(log.content),text);

	SERVER->GetLog()->writeLog(&log);

}
//GM�������ý�ɫ��������
void GameplayLog::GL_GMActorResetXianShu(int GMAccountId, int playerId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_RESETXIANSHU))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_RESETXIANSHU);
	char text[250] = "���ý�ɫ��������";
	dStrcpy(log.content,sizeof(log.content),text);

	SERVER->GetLog()->writeLog(&log);

}


// ----------------------------------------------------------------------------
// ��¼GM�޸�Ȩ��
void GameplayLog::GL_GMChgRole(int GMAccountId, int accountId,int gmFlag)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_ROLECHANGE))
		return;
	stGMLog log;
	log.setlog(dbStruct::createeid(), GMAccountId,TARGET_PLAYER,accountId,LOG_GM_ROLECHANGE);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"gmFlag:%d",gmFlag);
	dStrcpy(log.content,sizeof(log.content),txt);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼GM�����ʼ�
void GameplayLog::GL_GMSendMail(int GMAccountId, int playerId, int itemId, int itemNum, int money)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MAILSEND))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MAILSEND);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"itemId:%d,itemNum:%d,money:%d",itemId,itemNum,money);
	dStrcpy(log.content,sizeof(log.content),txt);
	SERVER->GetLog()->writeLog(&log);

	time_t t;
	time(&t);
	stMailLog maillog;
	maillog.setlog(eid, GMAccountId,TARGET_PLAYER, playerId, LOG_GM_MAILSEND);
	maillog.info.isSystem = 1;
	maillog.info.nItemId = itemId;
	maillog.info.nItemCount = itemNum;
	maillog.info.nMoney = money;
	maillog.info.nTime = (int)t;
	maillog.info.nRecver = playerId;
	SERVER->GetLog()->writeLog(&maillog);
}

// ----------------------------------------------------------------------------
// ��¼GM��ѯ�ʼ�
void GameplayLog::GL_GMQueryMail(int GMAccountId, int playerId, int itemId, int itemNum, int money)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MAILQUERY))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MAILQUERY);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"itemId:%d,itemNum:%d,money:%d",itemId,itemNum,money);
	dStrcpy(log.content,sizeof(log.content),txt);
	SERVER->GetLog()->writeLog(&log);

	time_t t;
	time(&t);
	stMailLog maillog;
	maillog.setlog(eid, GMAccountId,TARGET_PLAYER, playerId, LOG_GM_MAILQUERY);
	maillog.info.isSystem = 1;
	maillog.info.nItemId = itemId;
	maillog.info.nItemCount = itemNum;
	maillog.info.nMoney = money;
	maillog.info.nTime = (int)t;
	maillog.info.nRecver = playerId;
	SERVER->GetLog()->writeLog(&maillog);
}


// ----------------------------------------------------------------------------
// ��¼GMɾ���ʼ�
void GameplayLog::GL_GMDeleteMail(int GMAccountId, stMailItem& mailitem)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MAILDEL))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,mailitem.nRecver,LOG_GM_MAILDEL);
	char txt[250] = {0};
    sprintf_s(txt,sizeof(txt),"mailId:%d ItemId:%d ItemCount:%d mailMoney:%d",mailitem.id,mailitem.nItemId,mailitem.nItemCount,mailitem.nMoney);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stMailLog maillog;
	maillog.setlog(eid, GMAccountId, TARGET_PLAYER, mailitem.nRecver, LOG_MAIL_DELETE);
	maillog.info = mailitem;
	SERVER->GetLog()->writeLog(&maillog);
}

// ----------------------------------------------------------------------------
// ��¼GM�޸��ʼ�
void GameplayLog::GL_GMModifyMail(int GMAccountId, stMailItem& mailitem)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MAILMODIFY))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,mailitem.nRecver,LOG_GM_MAILMODIFY);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"mailId:%d",mailitem.id);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stMailLog maillog;
	maillog.setlog(eid, GMAccountId, TARGET_PLAYER, mailitem.nRecver, LOG_GM_MAILMODIFY);
	maillog.info = mailitem;
	SERVER->GetLog()->writeLog(&maillog);
}

// ----------------------------------------------------------------------------
// ��¼GM����ɫ����
void GameplayLog::GL_GMGetMission(int GMAccountId, int playerId, int missionId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MISSIONGET))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_MISSIONGET);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"missionId:%d",missionId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stMissionLog missionlog;
	missionlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MISSIONGET);
	missionlog.MissionID = missionId;
	SERVER->GetLog()->writeLog(&missionlog);
}

// ----------------------------------------------------------------------------
// ��¼GMɾ����ɫ����
void GameplayLog::GL_GMDeleteMission(int GMAccountId, int playerId, int missionId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MISSIONDEL))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_MISSIONDEL);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"missionId:%d",missionId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stMissionLog missionlog;
	missionlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MISSIONDEL);
	missionlog.MissionID = missionId;
	SERVER->GetLog()->writeLog(&missionlog);
}

// ----------------------------------------------------------------------------
// ��¼GM�ý�ɫ�������
void GameplayLog::GL_GMGetCompleteMission(int GMAccountId, int playerId, int missionId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MISSIONCOMPLETE))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_MISSIONCOMPLETE);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"missionId:%d",missionId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stMissionLog missionlog;
	missionlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MISSIONCOMPLETE);
	missionlog.MissionID = missionId;
	SERVER->GetLog()->writeLog(&missionlog);
}

// ----------------------------------------------------------------------------
// ��¼GM����ɫ����
void GameplayLog::GL_GMGetSkill(int GMAccountId, int playerId, int skillId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_SKILLGET))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_SKILLGET);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"skillId:%d",skillId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stSkillLog skilllog;
	skilllog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_SKILLGET);
	skilllog.SkillID = skillId;
	SERVER->GetLog()->writeLog(&skilllog);
}

// ----------------------------------------------------------------------------
// ��¼GMɾ��ɫ����
void GameplayLog::GL_GMDeleteSkill(int GMAccountId, int playerId, int skillId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_SKILLDEL))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_SKILLDEL);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"skillId:%d",skillId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stSkillLog skilllog;
	skilllog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_SKILLDEL);
	skilllog.SkillID = skillId;
	SERVER->GetLog()->writeLog(&skilllog);
}

// ----------------------------------------------------------------------------
// ��¼GM����ɫ�����
void GameplayLog::GL_GMGetLivingSkill(int GMAccountId, int playerId, int skillId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_LIVINGSKILLGET))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_LIVINGSKILLGET);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"skillId:%d",skillId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stSkillLog skilllog;
	skilllog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_LIVINGSKILLGET);
	skilllog.SkillID = skillId;
	SERVER->GetLog()->writeLog(&skilllog);
}

// ----------------------------------------------------------------------------
// ��¼GMɾ��ɫ�����
void GameplayLog::GL_GMDeleteLivingSkill(int GMAccountId, int playerId, int skillId)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_LIVINGSKILLDEL))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_LIVINGSKILLDEL);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"skillId:%d",skillId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	stSkillLog skilllog;
	skilllog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_LIVINGSKILLDEL);
	skilllog.SkillID = skillId;
	SERVER->GetLog()->writeLog(&skilllog);
}


// ----------------------------------------------------------------------------
// ��¼GM����Ʒ
void GameplayLog::GL_GMGetItem(int GMAccountId, int playerId, stItemInfo& item)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_ITEMGET))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_ITEMGET);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"itemId:%d",item.ItemID);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	if(item.ItemID > 100000000 && item.ItemID < 150000000)
	{
		U64 ITEMNO = dbStruct::createeid();
		stItemLog itemlog;
		itemlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_ITEMGET);
		itemlog.ItemID = item.ItemID;
		itemlog.TagNum = 1;
		itemlog.ChangeNum = 1;
		itemlog.ItemNo = ITEMNO;
		SERVER->GetLog()->writeLog(&itemlog);

		stItemDetailLog itemdetaillog;
		itemdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_ITEMGET);
		itemdetaillog.ItemNo = ITEMNO;
		itemdetaillog.info = item;
		SERVER->GetLog()->writeLog(&itemdetaillog);
	}
	else
	{
		g_Log.WriteFocus("GL_GMGetItem>>>>type=%d, playerid=%d", LOG_GM_ITEMGET,playerId);
	}	
}

// ----------------------------------------------------------------------------
// ��¼GM�޸���Ʒ
void GameplayLog::GL_GMModifyItem(int GMAccountId, int playerId, stItemInfo& olditem, stItemInfo& newitem)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_ITEMMODIFY))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_ITEMMODIFY);
	char txt[250] = {0};
	std::string content;
	sprintf_s(txt,sizeof(txt),"itemId:%d|",newitem.ItemID);
	content += txt;
	
	if(olditem.Quantity != newitem.Quantity)
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|",olditem.Quantity,newitem.Quantity);
		content += txt;
	}
	if(olditem.LapseTime != newitem.LapseTime)
	{
		sprintf_s(txt,sizeof(txt),"ʣ��ʱ��:%d��%d|",olditem.LapseTime,newitem.LapseTime);
		content += txt;
	}
	if(olditem.RemainUseTimes != newitem.RemainUseTimes)
	{
		sprintf_s(txt,sizeof(txt),"ʣ��ʹ�ô���:%d��%d|",olditem.LapseTime,newitem.LapseTime);
		content += txt;
	}
	if(olditem.BindPlayer != newitem.BindPlayer)
	{
		sprintf_s(txt,sizeof(txt),"����id:%d��%d|",olditem.BindPlayer,newitem.BindPlayer);
		content += txt;
	}
	if(olditem.Quality != newitem.Quality)
	{
		sprintf_s(txt,sizeof(txt),"��ƷƷ��:%d��%d|",olditem.Quality,newitem.Quality);
		content += txt;
	}
	if(olditem.CurWear != newitem.CurWear)
	{
		sprintf_s(txt,sizeof(txt),"��ǰ�;�:%d��%d|",olditem.CurWear,newitem.CurWear);
		content += txt;
	}
	if(olditem.CurMaxWear != newitem.CurMaxWear)
	{
		sprintf_s(txt,sizeof(txt),"��ǰ����;�:%d��%d|",olditem.CurMaxWear,newitem.CurMaxWear);
		content += txt;
	}
	if(olditem.EquipStrengthens != newitem.EquipStrengthens)
	{
		sprintf_s(txt,sizeof(txt),"ǿ���ȼ�:%d��%d|",olditem.EquipStrengthens,newitem.EquipStrengthens);
		content += txt;
	}
	if(olditem.CurAdroit != newitem.CurAdroit)
	{
		sprintf_s(txt,sizeof(txt),"��ǰ������:%d��%d|",olditem.CurAdroit,newitem.CurAdroit);
		content += txt;
	}
	if(olditem.CurAdroitLv != newitem.CurAdroitLv)
	{
		sprintf_s(txt,sizeof(txt),"�����ȵȼ�:%d��%d|",olditem.CurAdroitLv,newitem.CurAdroitLv);
		content += txt;
	}
	if(olditem.ActivatePro != newitem.ActivatePro)
	{
		sprintf_s(txt,sizeof(txt),"Ч�������־:%d��%d|",olditem.ActivatePro,newitem.ActivatePro);
		content += txt;
	}
	if(olditem.RandPropertyID != newitem.RandPropertyID)
	{
		sprintf_s(txt,sizeof(txt),"�Ƿ������������:%d��%d|",olditem.RandPropertyID,newitem.RandPropertyID);
		content += txt;
	}
	if(olditem.LockedLeftTime != newitem.LockedLeftTime)
	{
		sprintf_s(txt,sizeof(txt),"��/����״̬:%d��%d|",olditem.LockedLeftTime,newitem.LockedLeftTime);
		content += txt;
	}
	if(olditem.WuXingPro != newitem.WuXingPro)
	{
		sprintf_s(txt,sizeof(txt),"��������״̬:%d��%d|",olditem.WuXingPro,newitem.WuXingPro);
		content += txt;
	}
	if(olditem.WuXingID != newitem.WuXingID)
	{
		sprintf_s(txt,sizeof(txt),"����id:%d��%d|",olditem.WuXingID,newitem.WuXingID);
		content += txt;
	}
	if(olditem.WuXingPro != newitem.WuXingPro)
	{
		sprintf_s(txt,sizeof(txt),"����Ч��id:%d��%d|",olditem.WuXingPro,newitem.WuXingPro);
		content += txt;
	}
	if(olditem.SkillAForEquip != newitem.SkillAForEquip)
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|",olditem.SkillAForEquip,newitem.SkillAForEquip);
		content += txt;
	}
	if(olditem.SkillBForEquip != newitem.SkillBForEquip)
	{
		sprintf_s(txt,sizeof(txt),"��������:%d��%d|",olditem.SkillBForEquip,newitem.SkillBForEquip);
		content += txt;
	}
	for (int i = 0; i < MAX_IDEPROS; ++i)
	{
		if (olditem.IDEProValue[i] != newitem.IDEProValue[i])
		{
			sprintf_s(txt,sizeof(txt),"�����󸽼�����ֵ%d��:%d��%d|",i+1,olditem.IDEProValue[i],newitem.IDEProValue[i]);
			content += txt;
		}
	}

	dStrcpy(log.content,sizeof(log.content),content.c_str());

	SERVER->GetLog()->writeLog(&log);

	if(olditem.ItemID > 100000000 && olditem.ItemID < 150000000 &&
		newitem.ItemID > 100000000 && newitem.ItemID < 150000000)
	{
		U64 ITEMNO0 = dbStruct::createeid();
		U64 ITEMNO1 = dbStruct::createeid();
		stItemLog itemlog;
		itemlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_ITEMMODIFY);
		itemlog.ItemID = newitem.ItemID;
		itemlog.ItemNo = ITEMNO0;
		itemlog.ItemNo1 = ITEMNO1;
		SERVER->GetLog()->writeLog(&itemlog);

		stItemDetailLog itemdetaillog;
		itemdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_ITEMMODIFY);
		itemdetaillog.info = olditem;
		itemdetaillog.ItemNo = ITEMNO0;
		SERVER->GetLog()->writeLog(&itemdetaillog);

		itemdetaillog.info = newitem;
		itemdetaillog.ItemNo = ITEMNO1;
		SERVER->GetLog()->writeLog(&itemdetaillog);
	}
	else
	{
		g_Log.WriteFocus("GL_GMModifyItem>>>>type=%d, playerid=%d", LOG_GM_ITEMMODIFY,playerId);
	}
}

// ----------------------------------------------------------------------------
// ��¼GMɾ����Ʒ
void GameplayLog::GL_GMDelItem(int GMAccountId, int playerId, stItemInfo& item)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_ITEMDEL))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_ITEMDEL);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"itemId:%d itemCount:%d",item.ItemID,item.Quantity);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	if(item.ItemID > 100000000 && item.ItemID < 150000000)
	{
		U64 ITEMNO = dbStruct::createeid();
		stItemLog itemlog;
		itemlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_ITEMDEL);
		itemlog.ItemID = item.ItemID;
		itemlog.SrcNum = 1;
		itemlog.ChangeNum = -item.Quantity;
		itemlog.ItemNo = ITEMNO;
		SERVER->GetLog()->writeLog(&itemlog);

		stItemDetailLog itemdetaillog;
		itemdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_ITEMDEL);
		itemdetaillog.ItemNo = ITEMNO;
		itemdetaillog.info = item;
		SERVER->GetLog()->writeLog(&itemdetaillog);
	}
	else
	{
		g_Log.WriteFocus("GL_GMDelItem>>>>type=%d, playerid=%d", LOG_GM_ITEMDEL,playerId);
	}
}

// ----------------------------------------------------------------------------
// ��¼GM������
void GameplayLog::GL_GMGetPet(int GMAccountId, int playerId, stPetInfo& pet)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_PETGET))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_PETGET);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"petId:%d",pet.id);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	U64 PETNO = dbStruct::createeid();
	stPetLog petlog;
	petlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_PETGET);
	petlog.PetId = pet.id;
	petlog.Val[0] = pet.petDataId;
	petlog.PetNo = PETNO;
	SERVER->GetLog()->writeLog(&petlog);

	stPetDetailLog petdetaillog;
	petdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_PETGET);
	petdetaillog.info = pet;
	petdetaillog.PetNo = PETNO;
	SERVER->GetLog()->writeLog(&petdetaillog);
}

// ----------------------------------------------------------------------------
// ��¼GMɾ������
void GameplayLog::GL_GMDeletePet(int GMAccountId, int playerId, stPetInfo& pet)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_PETDEL))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_PETDEL);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"petId:%d",pet.id);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	U64 PETNO = dbStruct::createeid();
	stPetLog petlog;
	petlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_PETDEL);
	petlog.PetId = pet.id;
	petlog.Val[0] = pet.petDataId;
	petlog.PetNo = PETNO;
	SERVER->GetLog()->writeLog(&petlog);

	stPetDetailLog petdetaillog;
	petdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_PETDEL);
	petdetaillog.info = pet;
	petdetaillog.PetNo = PETNO;
	SERVER->GetLog()->writeLog(&petdetaillog);
}

// ----------------------------------------------------------------------------
// ��¼GM�޸���������
void GameplayLog::GL_GMModifyPet(int GMAccountId, int playerId, stPetInfo& oldpet, stPetInfo& newpet)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_PETMODIFY))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_PETMODIFY);
	char txt[256] = {0};
	std::string content;

	sprintf_s(txt,sizeof(txt),"petId:%d|",newpet.id);
	content += txt;
	if(oldpet.petDataId != newpet.petDataId)
	{
		sprintf_s(txt,sizeof(txt),"ID:%d��%d|",oldpet.petDataId,newpet.petDataId);
		content += txt;
	}
	if (oldpet.petEggId != newpet.petEggId)
	{
		sprintf_s(txt,sizeof(txt),"��ӡID:%d��%d|",oldpet.petEggId,newpet.petEggId);
		content += txt;
	}
	if (oldpet.id != newpet.id)
	{
		sprintf_s(txt,sizeof(txt),"ΨһID:%I64d��%I64d|",oldpet.id,newpet.id);
		content += txt;
	}
	if (0 != strcmp(oldpet.name,newpet.name))
	{
		sprintf_s(txt,sizeof(txt),"����:%s��%s|",oldpet.name,newpet.name);
		content += txt;
	}
	if (oldpet.level != newpet.level)
	{
		sprintf_s(txt,sizeof(txt),"�ȼ�:%d��%d|",oldpet.level,newpet.level);
		content += txt;
	}
	if (oldpet.curHP != newpet.curHP)
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|",oldpet.curHP,newpet.curHP);
		content += txt;
	}
	if (oldpet.curMP != newpet.curMP)
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|",oldpet.curMP,newpet.curMP);
		content += txt;
	}
	if (oldpet.curPP != newpet.curPP)
	{
		sprintf_s(txt,sizeof(txt),"Ԫ��:%d��%d|",oldpet.curPP,newpet.curPP);
		content += txt;
	}
	if (oldpet.staminaPts != newpet.staminaPts)
	{
		sprintf_s(txt,sizeof(txt),"�������Ե�:%d��%d|",oldpet.staminaPts,newpet.staminaPts);
		content += txt;
	}
	if (oldpet.ManaPts != newpet.ManaPts)
	{
		sprintf_s(txt,sizeof(txt),"�������Ե�:%d��%d|",oldpet.ManaPts,newpet.ManaPts);
		content += txt;
	}
	if (oldpet.strengthPts != newpet.strengthPts)
	{
		sprintf_s(txt,sizeof(txt),"�������Ե�:%d��%d|",oldpet.strengthPts,newpet.strengthPts);
		content += txt;
	}
	if (oldpet.intellectPts != newpet.intellectPts)
	{
		sprintf_s(txt,sizeof(txt),"�������Ե�:%d��%d|",oldpet.intellectPts,newpet.intellectPts);
		content += txt;
	}
	if (oldpet.agilityPts != newpet.agilityPts)
	{
		sprintf_s(txt,sizeof(txt),"�����Ե�:%d��%d|",oldpet.agilityPts,newpet.agilityPts);
		content += txt;
	}	
	if (oldpet.pneumaPts != newpet.pneumaPts)
	{
		sprintf_s(txt,sizeof(txt),"Ԫ�����Ե�:%d��%d|",oldpet.pneumaPts,newpet.pneumaPts);
		content += txt;
	}
	if (oldpet.statsPoints != newpet.statsPoints)
	{
		sprintf_s(txt,sizeof(txt),"ʣ�����Ե�:%d��%d|",oldpet.statsPoints,newpet.statsPoints);
		content += txt;
	}
	if (oldpet.insight != newpet.insight)
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|",oldpet.insight,newpet.insight);
		content += txt;
	}
	if (oldpet.petTalent != newpet.petTalent)
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|",oldpet.petTalent,newpet.petTalent);
		content += txt;
	}
	if (oldpet.partnerId != newpet.partnerId)
	{
		sprintf_s(txt,sizeof(txt),"��żID:%I64d��%I64d|",oldpet.partnerId,newpet.partnerId);
		content += txt;
	}
	if (oldpet.sex != newpet.sex)
	{
		sprintf_s(txt,sizeof(txt),"�Ա�:%d��%d|",oldpet.sex,newpet.sex);
		content += txt;
	}
	if (oldpet.mature != newpet.mature)
	{
		sprintf_s(txt,sizeof(txt),"�Ƿ����:%d��%d|",oldpet.mature,newpet.mature);
		content += txt;
	}
	if (oldpet.happiness != newpet.happiness)
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|",oldpet.happiness,newpet.happiness);
		content += txt;
	}
	if (oldpet.generation != newpet.generation)
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|",oldpet.generation,newpet.generation);
		content += txt;
	}
	if (oldpet.title != newpet.title)
	{
		sprintf_s(txt,sizeof(txt),"�ƺű�ʶ:%I64d��%I64d|",oldpet.title,newpet.title);
		content += txt;
	}
	if (oldpet.titleEnabled != newpet.titleEnabled)
	{
		sprintf_s(txt,sizeof(txt),"��ǰ�ƺ�:%d��%d|",oldpet.titleEnabled,newpet.titleEnabled);
		content += txt;
	}
	if (oldpet.lives != newpet.lives)
	{
		sprintf_s(txt,sizeof(txt),"��ǰ����:%d��%d|",oldpet.lives,newpet.lives);
		content += txt;
	}
	if (oldpet.IsIdentify != newpet.IsIdentify)
	{
		sprintf_s(txt,sizeof(txt),"�Ƿ����:%d��%d|",oldpet.IsIdentify,newpet.IsIdentify);
		content += txt;
	}
	if (oldpet.chengZhangLv != newpet.chengZhangLv)
	{
		sprintf_s(txt,sizeof(txt),"�ɳ���:%d��%d|",oldpet.chengZhangLv,newpet.chengZhangLv);
		content += txt;
	}
	if (oldpet.qianLi != newpet.qianLi)
	{
		sprintf_s(txt,sizeof(txt),"Ǳ��:%d��%d|",oldpet.qianLi,newpet.qianLi);
		content += txt;
	}
	if (oldpet.quality != newpet.quality)
	{
		sprintf_s(txt,sizeof(txt),"Ʒ��:%d��%d|",oldpet.quality,newpet.quality);
		content += txt;
	}
	if (oldpet.staminaGift != newpet.staminaGift)
	{
		sprintf_s(txt,sizeof(txt),"��������:%d��%d|",oldpet.staminaGift,newpet.staminaGift);
		content += txt;
	}
	if (oldpet.ManaGift != newpet.ManaGift)
	{
		sprintf_s(txt,sizeof(txt),"��������:%d��%d|",oldpet.ManaGift,newpet.ManaGift);
		content += txt;
	}
	if (oldpet.strengthGift != newpet.strengthGift)
	{
		sprintf_s(txt,sizeof(txt),"��������:%d��%d|",oldpet.strengthGift,newpet.strengthGift);
		content += txt;
	}
	if (oldpet.intellectGift != newpet.intellectGift)
	{
		sprintf_s(txt,sizeof(txt),"��������:%d��%d|",oldpet.intellectGift,newpet.intellectGift);
		content += txt;
	}
	if (oldpet.agilityGift != newpet.agilityGift)
	{
		sprintf_s(txt,sizeof(txt),"������:%d��%d|",oldpet.agilityGift,newpet.agilityGift);
		content += txt;
	}
	if (oldpet.pneumaGift != newpet.pneumaGift)
	{
		sprintf_s(txt,sizeof(txt),"Ԫ������:%d��%d|",oldpet.pneumaGift,newpet.pneumaGift);
		content += txt;
	}
	if (oldpet.exp != newpet.exp)
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|",oldpet.exp,newpet.exp);
		content += txt;
	}
	if (oldpet.charPoints != newpet.charPoints)
	{
		sprintf_s(txt,sizeof(txt),"ʣ���Ը����:%d��%d|",oldpet.charPoints,newpet.charPoints);
		content += txt;
	}
	if (oldpet.LastBreedLv != newpet.LastBreedLv)
	{
		sprintf_s(txt,sizeof(txt),"�ϴη�ֳ�ȼ�:%d��%d|",oldpet.LastBreedLv,newpet.LastBreedLv);
		content += txt;
	}
	if (oldpet.BabyPetDataId != newpet.BabyPetDataId)
	{
		sprintf_s(txt,sizeof(txt),"�Ӵ�ģ��id:%d��%d|",oldpet.BabyPetDataId,newpet.BabyPetDataId);
		content += txt;
	}
	if (oldpet.BreedStartTime != newpet.BreedStartTime)
	{
		sprintf_s(txt,sizeof(txt),"��ֳ��ʼʱ��id:%d��%d|",oldpet.BreedStartTime,newpet.BreedStartTime);
		content += txt;
	}
	for (int i = 0; i < stPetInfo::MaxPetSkills; ++i)
	{
		if (oldpet.Combat[i] != newpet.Combat[i])
		{
			sprintf_s(txt,sizeof(txt),"����ս������%d��:%d��%d|",i+1,oldpet.Combat[i],newpet.Combat[i]);
			content += txt;
		}
	}
	for (int j = 0; j < stPetInfo::MaxPetEquips; ++j)
	{
		if (oldpet.equipitem[j] != newpet.equipitem[j])
		{
			sprintf_s(txt,sizeof(txt),"����װ��%d��:%d��%d|",j+1,oldpet.equipitem[j],newpet.equipitem[j]);
			content += txt;
		}
	}

	dStrcpy(log.content,sizeof(log.content),content.c_str());

	SERVER->GetLog()->writeLog(&log);

	U64 PETNO0 = dbStruct::createeid();
	U64 PETNO1 = dbStruct::createeid();
	stPetLog petlog;
	petlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_PETMODIFY);
	petlog.PetId = oldpet.id;
	petlog.Val[0] = oldpet.petDataId;
	petlog.PetNo = PETNO0;
	petlog.PetNo1 = PETNO1;
	SERVER->GetLog()->writeLog(&petlog);

	stPetDetailLog petdetaillog;
	petdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_PETMODIFY);
	petdetaillog.info = oldpet;
	petdetaillog.PetNo = PETNO0;
	SERVER->GetLog()->writeLog(&petdetaillog);

	petdetaillog.info = newpet;
	petdetaillog.PetNo = PETNO1;
	SERVER->GetLog()->writeLog(&petdetaillog);
}


// ----------------------------------------------------------------------------
// ��¼GM�����
void GameplayLog::GL_GMGetMount(int GMAccountId, int playerId, stMountInfo& mount)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MOUNTADD))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_MOUNTADD);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"MountId:%d",mount.mDataId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	U64 MOUNTNO = dbStruct::createeid();
	stMountLog mountlog;
	mountlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MOUNTADD);
	mountlog.MountNo = MOUNTNO;
	SERVER->GetLog()->writeLog(&mountlog);

	stMountDetailLog mountdetaillog;
	mountdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MOUNTADD);
	mountdetaillog.info = mount;
	mountdetaillog.MountNo = MOUNTNO;
	SERVER->GetLog()->writeLog(&mountdetaillog);
}

// ----------------------------------------------------------------------------
// ��¼GMɾ�����
void GameplayLog::GL_GMDeleteMount(int GMAccountId, int playerId, stMountInfo& mount)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MOUNTDEL))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_MOUNTDEL);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"MountId:%d",mount.mDataId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	U64 MOUNTNO = dbStruct::createeid();
	stMountLog mountlog;
	mountlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MOUNTDEL);
	mountlog.MountNo = MOUNTNO;
	SERVER->GetLog()->writeLog(&mountlog);

	stMountDetailLog mountdetaillog;
	mountdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MOUNTDEL);
	mountdetaillog.info = mount;
	mountdetaillog.MountNo = MOUNTNO;
	SERVER->GetLog()->writeLog(&mountdetaillog);
}

// ----------------------------------------------------------------------------
// ��¼GM�޸��������
void GameplayLog::GL_GMModifyMount(int GMAccountId, int playerId, stMountInfo& oldmount, stMountInfo& newmount)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_MOUNTMODIFY))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_MOUNTMODIFY);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"MountId:%d",newmount.mDataId);
	dStrcpy(log.content,sizeof(log.content),txt);

	SERVER->GetLog()->writeLog(&log);

	U64 MOUNTNO0 = dbStruct::createeid();
	U64 MOUNTNO1 = dbStruct::createeid();
	stMountLog mountlog;
	mountlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MOUNTMODIFY);
	mountlog.MountNo = MOUNTNO0;
	mountlog.MountNo1 = MOUNTNO1;
	SERVER->GetLog()->writeLog(&mountlog);

	stMountDetailLog mountdetaillog;
	mountdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_MOUNTMODIFY);
	mountdetaillog.info = oldmount;
	mountdetaillog.MountNo = MOUNTNO0;
	SERVER->GetLog()->writeLog(&mountdetaillog);

	mountdetaillog.info = newmount;
	mountdetaillog.MountNo = MOUNTNO1;
	SERVER->GetLog()->writeLog(&mountdetaillog);
}

// ----------------------------------------------------------------------------
// ��¼GM������Ϣ
void GameplayLog::GL_GMSendMSG(int GMAccountId, int playerId, stChatMessage& msg)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_SENDMSG))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;

	log.setlog(eid,GMAccountId,TARGET_NONE,playerId,LOG_GM_SENDMSG);
	log.flag = msg.btMessageType;
	strncpy_s(log.content,sizeof(log.content),msg.szMessage,sizeof(msg.szMessage));
	SERVER->GetLog()->writeLog(&log);

}

// ----------------------------------------------------------------------------
// ��¼GM�Զ�������Ϣ
void GameplayLog::GL_GMSendAutoMSG(int GMAccountId, int playerId, stChatMessage& msg)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_SENDAUTOMSG))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;

	log.setlog(eid,GMAccountId,TARGET_NONE,playerId,LOG_GM_SENDAUTOMSG);
	log.flag = msg.btMessageType;
	strncpy_s(log.content,sizeof(log.content),msg.szMessage,sizeof(msg.szMessage));
	SERVER->GetLog()->writeLog(&log);

}

//��¼GM��ɫ��������
void GameplayLog::GL_GMPlayerOp(int GMAccountId, int GMplayerId, int TarPlayerId,const char* cmd)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_GMPLAYEROP))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, GMplayerId, LOG_GM_GMPLAYEROP);
	char txt[250] = {0};
	dStrcpy(log.content,sizeof(log.content),cmd);

	SERVER->GetLog()->writeLog(&log);
}

void GameplayLog::GL_GMXingXiangMODIFY(int GMAccountId, int playerId, stItemInfo& olditem, stItemInfo& newitem,int pos)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_XINGXIANGMODIFY))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_XINGXIANGMODIFY);
	char txt[250] = {0};
	std::string content;
	sprintf_s(txt,sizeof(txt),"itemId:%d|",newitem.ItemID);
	content += txt;

	if((olditem.CurAdroit != newitem.CurAdroit))
	{
		sprintf_s(txt,sizeof(txt),"����:%d��%d|λ�ã�",olditem.CurAdroit,newitem.CurAdroit);
		content += txt;
	}

	switch (pos)
	{
	case 0:
		content += "��������|";
		break;
	case 1:
		content += "�������|";
		break;
	case 2:
		content += "����|";
		break;
	}

	dStrcpy(log.content,sizeof(log.content),content.c_str());

	SERVER->GetLog()->writeLog(&log);

	if(olditem.ItemID > 100000000 && olditem.ItemID < 150000000 &&
		newitem.ItemID > 100000000 && newitem.ItemID < 150000000)
	{
		U64 ITEMNO0 = dbStruct::createeid();
		U64 ITEMNO1 = dbStruct::createeid();
		stItemLog itemlog;
		itemlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_XINGXIANGMODIFY);
		itemlog.ItemID = newitem.ItemID;
		itemlog.ItemNo = ITEMNO0;
		itemlog.ItemNo1 = ITEMNO1;
		SERVER->GetLog()->writeLog(&itemlog);

		stItemDetailLog itemdetaillog;
		itemdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_XINGXIANGMODIFY);
		itemdetaillog.info = olditem;
		itemdetaillog.ItemNo = ITEMNO0;
		SERVER->GetLog()->writeLog(&itemdetaillog);

		itemdetaillog.info = newitem;
		itemdetaillog.ItemNo = ITEMNO1;
		SERVER->GetLog()->writeLog(&itemdetaillog);
	}
	else
	{
		g_Log.WriteFocus("GL_GMXingXiangMODIFY>>>>type=%d, playerid=%d", LOG_GM_XINGXIANGMODIFY,playerId);
	}
}

// ----------------------------------------------------------------------------
// ��¼GMɾ����Ʒ
void GameplayLog::GL_GMXingXiangDEL(int GMAccountId, int playerId, stItemInfo& item,int pos)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_XINGXIANGDEL))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_XINGXIANGDEL);
	char txt[250] = {0};
	std::string content;
	sprintf_s(txt,sizeof(txt),"itemId:%d|λ�ã�",item.ItemID);
	content += txt;

	switch (pos)
	{
	case 0:
		content += "��������|";
		break;
	case 1:
		content += "�������|";
		break;
	case 2:
		content += "����|";
		break;
	}

	dStrcpy(log.content,sizeof(log.content),content.c_str());

	SERVER->GetLog()->writeLog(&log);

	if(item.ItemID > 100000000 && item.ItemID < 150000000)
	{
		U64 ITEMNO = dbStruct::createeid();
		stItemLog itemlog;
		itemlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_XINGXIANGDEL);
		itemlog.ItemID = item.ItemID;
		itemlog.SrcNum = 1;
		itemlog.ChangeNum = -item.Quantity;
		itemlog.ItemNo = ITEMNO;
		SERVER->GetLog()->writeLog(&itemlog);

		stItemDetailLog itemdetaillog;
		itemdetaillog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_XINGXIANGDEL);
		itemdetaillog.ItemNo = ITEMNO;
		itemdetaillog.info = item;
		SERVER->GetLog()->writeLog(&itemdetaillog);
	}
	else
	{
		g_Log.WriteFocus("GL_GMXingXiangDEL>>>>type=%d, playerid=%d", LOG_GM_XINGXIANGDEL,playerId);
	}
}

// ----------------------------------------------------------------------------
// ��¼GM�޸���ӡ
void GameplayLog::GL_GMModifyGodPrint(int GMAccountId, int playerId, stGodPrintInfo *oldGPinfo, stGodPrintInfo *newGPinfo)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_GODPRINTMODIFY))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_GODPRINTMODIFY);
	char txt[250] = {0};
	std::string content;
	//sprintf_s(txt,sizeof(txt),"itemId:%d|",newitem.ItemID);
	//content += txt;

	content = "";
// 	stGodPrintInfo oldinfo[GODPRINT_MAXSLOTS];
// 	stGodPrintInfo newinfo[GODPRINT_MAXSLOTS];
	for (int i = 0; i < GODPRINT_MAXSLOTS; ++i)
	{
// 		memcpy(oldinfo[i],*oldGPinfo,sizeof(stGodPrintInfo));
// 		memcpy(newinfo[i],*newGPinfo,sizeof(stGodPrintInfo));

		if (oldGPinfo->status != newGPinfo->status)
		{
			sprintf_s(txt,sizeof(txt),"��%d��״̬:%d->%d|",i+1,oldGPinfo->status,newGPinfo->status);
			content += txt;
		}
		++oldGPinfo;
		++newGPinfo;
	}

	dStrcpy(log.content,sizeof(log.content),content.c_str());

	SERVER->GetLog()->writeLog(&log);
}
// ----------------------------------------------------------------------------
// ��¼GM�޸����
void GameplayLog::GL_GMLingChongEdit(int GMAccountId, int playerId, stGodPetInfo *oldInfo, stGodPetInfo *newInfo)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_LINGCHONGEDIT))
		return;

	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId, TARGET_PLAYER, playerId, LOG_GM_LINGCHONGEDIT);
	char txt[250] = {0};
	std::string content;
	//sprintf_s(txt,sizeof(txt),"itemId:%d|",newitem.ItemID);
	//content += txt;

	sprintf_s(txt,sizeof(txt),"�ȼ�:%d->%d | Ʒ��:%d->%d | ����״̬:%d->%d |",oldInfo->Level,newInfo->Level,\
		oldInfo->Quality,newInfo->Quality,(int)oldInfo->active,(int)newInfo->active);
	content += txt;

	dStrcpy(log.content,sizeof(log.content),content.c_str());

	SERVER->GetLog()->writeLog(&log);
}*/
/********************************************************************************************************/
/********************************************************************************************************/
/***************************        ��  ��  ��  GM  ��  ��  ��  ־        *******************************/
/********************************************************************************************************/
/********************************************************************************************************/
/********************************************************************************************************/


/*
void GameplayLog::GL_OrgLog(int playerID, const OrgLogInfo& info, enOpType op)
{
	if(!GL_IsRecord(0, playerID, op))
		return;

	U64 eid = dbStruct::createeid();
	stOrgLog log;

	log.setlog(eid, playerID, TARGET_ORG, 0, op);
	log.info = info;
	SERVER->GetLog()->writeLog(&log);
}

void GameplayLog::GL_LeagueLog(int playerID, const LeagueLogInfo& info, enOpType op)
{
	if(!GL_IsRecord(0, playerID, op))
		return;

	U64 eid = dbStruct::createeid();
	stLeagueLog log;

	log.setlog(eid, playerID, TARGET_LEAGUE, 0, op);
	log.info = info;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼�������̲���
void GameplayLog::GL_ShopBuyShop(U32 PlayerID, U16 stallId, S32 money, U32 itemId)
{
	if(!GL_IsRecord(0, PlayerID, LOG_SHOP_BUYSHOP_OK))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),PlayerID,TARGET_STALLSHOP,stallId, LOG_SHOP_BUYSHOP_OK);
	log.Val[0] = money;
	log.Val[1] = itemId;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼���������ʽ��ȡ
void GameplayLog::GL_ShopFlowMoney(U32 PlayerID, U16 stallId, S32 srcMoney, S32 tagMoney)
{
	if(!GL_IsRecord(0, PlayerID, LOG_SHOP_FLOWMONEY_OK))
		return;

	stMoneyLog moneylog;
	moneylog.setlog(dbStruct::createeid(),stallId,TARGET_STALLSHOP,PlayerID,LOG_SHOP_FLOWMONEY_OK);
	moneylog.DataType = stMoneyLog::TYPE_MONEY;
	moneylog.Val[0] = tagMoney-srcMoney;
	moneylog.Val[1] = tagMoney;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ��¼���̶̹��ʽ��ȡ
void GameplayLog::GL_ShopFixedMoney(U32 PlayerID, U16 stallId, S32 srcMoney, S32 tagMoney)
{
	if(!GL_IsRecord(0, PlayerID, LOG_SHOP_FIXEDMONEY_OK))
		return;

	stMoneyLog moneylog;
	moneylog.setlog(dbStruct::createeid(),stallId,TARGET_STALLSHOP,PlayerID,LOG_SHOP_FIXEDMONEY_OK);
	moneylog.DataType = stMoneyLog::TYPE_MONEY;
	moneylog.Val[0] = tagMoney-srcMoney;
	moneylog.Val[1] = tagMoney;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ��¼����������λ����
void GameplayLog::GL_ShopBuySlots(U32 PlayerID, U16 stallId, U8 type, U8 page)
{
	if(!GL_IsRecord(0, PlayerID, LOG_SHOP_BUYSLOT_OK))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),PlayerID,TARGET_STALLSHOP,stallId, LOG_SHOP_BUYSLOT_OK);
	log.Val[0] = type;
	log.Val[1] = page;
	log.Val[2] = page * 10000;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼���̳�����Ʒ����Ʒ���¼ܲ���
void GameplayLog::GL_ShopSetSellItem(int PlayerID, U32 type,stFixedStallItem* item)
{
	if(!GL_IsRecord(0, PlayerID, type))
		return;
	
	if(!item)
		return;

	U64 eid = dbStruct::createeid();
	U64 ITEMNO = dbStruct::createeid();
	stItemLog itemlog;
	itemlog.setlog(eid, item->StallID, TARGET_STALLSHOP, PlayerID, type);
	itemlog.TagPos = item->Pos;
	itemlog.SrcNum = LOG_SHOP_UPSELLITEM_OK?0:item->Quantity;
	itemlog.TagNum = LOG_SHOP_UPSELLITEM_OK?item->Quantity:0;
	itemlog.ItemID = item->ItemID;
	itemlog.Val0   = item->Price;
	itemlog.ItemNo = ITEMNO;
	SERVER->GetLog()->writeLog(&itemlog);

	stItemDetailLog itemdetaillog;
	itemdetaillog.setlog(eid, item->StallID,TARGET_STALLSHOP, PlayerID,type);
	itemdetaillog.ItemNo = ITEMNO;
	memcpy(&itemdetaillog.info, item, sizeof(stItemInfo));
	SERVER->GetLog()->writeLog(&itemdetaillog);
}

// ----------------------------------------------------------------------------
// ��¼���̳����������������¼ܲ���
void GameplayLog::GL_ShopSetSellPet(int PlayerID, U32 type,stFixedStallPet* pet)
{
	if(!GL_IsRecord(0, PlayerID, type))
		return;

	if(!pet)
		return;

	U64 eid = dbStruct::createeid();
	U64 PETNO = dbStruct::createeid();
	stPetLog petlog;
	petlog.setlog(eid, pet->StallID, TARGET_STALLSHOP, PlayerID, type);
	petlog.PetId = pet->id;
	petlog.Val[0] = pet->petDataId;
	petlog.Val[1] = pet->Price;
	petlog.Val[2] = pet->Pos;
	petlog.PetNo = PETNO;
	SERVER->GetLog()->writeLog(&petlog);

	stPetDetailLog petdetaillog;
	petdetaillog.setlog(eid, pet->StallID,TARGET_STALLSHOP, PlayerID,type);
	petdetaillog.PetNo = PETNO;
	memcpy(&petdetaillog.info, pet, sizeof(stPetInfo));
	SERVER->GetLog()->writeLog(&petdetaillog);
}

// ----------------------------------------------------------------------------
// ��¼�����չ���Ʒ����Ʒ���¼ܲ���
void GameplayLog::GL_ShopSetBuyItem(int PlayerID, U32 type,stFixedStallItem* item)
{
	if(!GL_IsRecord(0, PlayerID, type))
		return;

	if(!item)
		return;

	U64 eid = dbStruct::createeid();
	stItemLog itemlog;
	itemlog.setlog(eid, item->StallID, TARGET_STALLSHOP, PlayerID, type);
	itemlog.TagPos = item->Pos;
	itemlog.SrcNum = item->BuyNum;
	itemlog.TagNum = item->LeftNum;
	itemlog.ItemID = item->ItemID;
	itemlog.Val0 = item->Price;
	SERVER->GetLog()->writeLog(&itemlog);
}

// ----------------------------------------------------------------------------
// ��¼���̺�����������Ʒ���¼ܲ���
void GameplayLog::GL_ShopSetShoutItem(int PlayerID, U32 type, U32 stallId, U32 itemId, S32 itemNum)
{
	if(!GL_IsRecord(0, PlayerID, type))
		return;

	U64 eid = dbStruct::createeid();
	stItemLog itemlog;
	itemlog.setlog(eid, stallId, TARGET_STALLSHOP, PlayerID, type);
	itemlog.SrcNum = type == LOG_SHOP_UPSHOUTITEM_OK? 0:itemNum;
	itemlog.TagNum = type == LOG_SHOP_UPSHOUTITEM_OK? itemNum : 0;
	itemlog.ItemID = itemId;
	SERVER->GetLog()->writeLog(&itemlog);
}

// ----------------------------------------------------------------------------
// ��¼���̹��������Ʒ����Ʒ
void GameplayLog::GL_ShopBuySellItem(int PlayerID, U32 type, U32 stallId, U32 stallerId, S32 srcFlowMoney, 
						S32 tagFlowMoney, S32 money, S32 tax, S32 taxRate,
						stItemInfo* item, S32 index, S32 price, S32 itemNum)
{
	if(!GL_IsRecord(0, PlayerID, type))
		return;

	if(!item)
		return;

	U64 eid = dbStruct::createeid();
	U64 ITEMNO = dbStruct::createeid();

	stItemLog itemlog;
	itemlog.setlog(eid, stallId, TARGET_STALLSHOP, PlayerID, type);
	itemlog.TagPos = index;
	itemlog.SrcNum = item->Quantity;
	itemlog.TagNum = type == LOG_SHOP_BUYSELLITEM_OK?item->Quantity-itemNum:item->Quantity;
	itemlog.ItemID = item->ItemID;
	itemlog.Val0   = price;
	itemlog.ItemNo = type == LOG_SHOP_BUYSELLITEM_OK?ITEMNO:0;
	SERVER->GetLog()->writeLog(&itemlog);

	if(type != LOG_SHOP_BUYSELLITEM_OK)
		return;

	stItemDetailLog itemdetaillog;
	itemdetaillog.setlog(eid, stallId,TARGET_STALLSHOP,PlayerID,type);
	itemdetaillog.ItemNo = ITEMNO;
	memcpy(&itemdetaillog.info, item, sizeof(stItemInfo));
	SERVER->GetLog()->writeLog(&itemdetaillog);

	stMoneyLog moneylog;
	moneylog.setlog(eid,stallId,TARGET_STALLSHOP,PlayerID,type);
	moneylog.DataType = stMoneyLog::TYPE_MONEY;
	moneylog.Val[0] = tagFlowMoney-srcFlowMoney;
	moneylog.Val[1] = tagFlowMoney;
	SERVER->GetLog()->writeLog(&moneylog);

	//˰��
	moneylog.setlog(eid,stallId,TARGET_STALLSHOP,PlayerID,type);
	moneylog.DataType = stMoneyLog::TYPE_TAX;
	moneylog.Val[0] = -tax;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ��¼���̹����������������
void GameplayLog::GL_ShopBuySellPet(int PlayerID, U32 type, U32 stallId, U32 stallerId, S32 srcFlowMoney,
					   S32 tagFlowMoney, S32 money, S32 tax, S32 taxRate, stPetInfo* pet, S32 index, S32 price)
{
	if(!GL_IsRecord(0, PlayerID, type))
		return;

	if(!pet)
		return;

	U64 eid = dbStruct::createeid();
	U64 PETNO = dbStruct::createeid();
	stPetLog petlog;
	petlog.setlog(eid, stallId, TARGET_STALLSHOP, PlayerID, type);
	petlog.PetId = pet->id;
	petlog.Val[0] = pet->petDataId;
	petlog.Val[1] = price;
	petlog.Val[2] = index;
	petlog.PetNo = type == LOG_SHOP_BUYSELLPET_OK? PETNO: 0;
	SERVER->GetLog()->writeLog(&petlog);	

	if(type != LOG_SHOP_BUYSELLPET_OK)
		return;

	stPetDetailLog petdetaillog;
	petdetaillog.setlog(eid, stallId,TARGET_STALLSHOP, PlayerID,type);
	petdetaillog.PetNo = PETNO;
	memcpy(&petdetaillog.info, pet, sizeof(stPetInfo));
	SERVER->GetLog()->writeLog(&petdetaillog);

	stMoneyLog moneylog;
	moneylog.setlog(eid, stallId,TARGET_STALLSHOP,PlayerID,type);
	moneylog.DataType = stMoneyLog::TYPE_MONEY;
	moneylog.Val[0] = tagFlowMoney-srcFlowMoney;
	moneylog.Val[1] = srcFlowMoney;
	moneylog.Val[2] = tagFlowMoney;
	SERVER->GetLog()->writeLog(&moneylog);

	//˰��
	moneylog.setlog(eid, stallId,TARGET_STALLSHOP,PlayerID,type);
	moneylog.DataType = stMoneyLog::TYPE_TAX;
	moneylog.Val[0] = -tax;
	moneylog.Val[1] = -money;
	moneylog.Val[2] = taxRate;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ��¼���������չ���Ʒ����Ʒ
void GameplayLog::GL_ShopSellBuyItem(int PlayerID, U32 type, U32 stallId,U32 stallerId, S32 srcFlowMoney,
						S32 tagFlowMoney, stItemInfo* item, S32 index, S32 srcNum, S32 leftNum, S32 price)
{
	if(!GL_IsRecord(0, PlayerID, type))
		return;

	if(!item)
		return;

	U64 eid = dbStruct::createeid();

	stItemLog itemlog;
	itemlog.setlog(eid, stallId, TARGET_STALLSHOP, PlayerID, type);
	itemlog.TagPos = index;
	itemlog.SrcNum = srcNum;
	itemlog.TagNum = type == LOG_SHOP_SELLBUYITEM_OK?leftNum:srcNum;
	itemlog.ItemID = item->ItemID;
	itemlog.Val0   = price;
	SERVER->GetLog()->writeLog(&itemlog);

	if(type != LOG_SHOP_SELLBUYITEM_OK)
		return;

	stMoneyLog moneylog;
	moneylog.setlog(eid, stallId,TARGET_STALLSHOP,PlayerID,type);
	moneylog.DataType = stMoneyLog::TYPE_MONEY;
	moneylog.Val[0] = tagFlowMoney-srcFlowMoney;
	moneylog.Val[1] = srcFlowMoney;
	moneylog.Val[2] = tagFlowMoney;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ��¼ϵͳ�۳�����ÿ��˰
void GameplayLog::GL_ShopDayTax(U32 PlayerID, U32 stallId, S32 dayTax, S32 flourish, S32 totalFlourish, S32 srcMoney, S32 tagMoney)
{
	if(!GL_IsRecord(0, PlayerID, LOG_SHOP_DAYTAX))
		return;

	U64 eid = dbStruct::createeid();
	stMoneyLog moneylog;
	moneylog.setlog(eid, stallId, TARGET_STALLSHOP, stallId,LOG_SHOP_DAYTAX);
	moneylog.DataType = stMoneyLog::TYPE_MONEY;
	moneylog.Val[0] = tagMoney-srcMoney;
	moneylog.Val[1] = srcMoney;
	moneylog.Val[2] = tagMoney;
	SERVER->GetLog()->writeLog(&moneylog);

	//˰��
	moneylog.setlog(eid, stallId,TARGET_STALLSHOP,stallId,LOG_SHOP_DAYTAX);
	moneylog.DataType = stMoneyLog::TYPE_TAX;
	moneylog.Val[0] = -dayTax;
	moneylog.Val[1] = flourish;
	moneylog.Val[2] = totalFlourish;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// ��¼���̺������ĵ���
void GameplayLog::GL_ShopCostShoutItem(U32 PlayerID, U32 stallId, U32 itemId, S32 itemNum)
{
	if(!GL_IsRecord(0, PlayerID, LOG_SHOP_COSTSHOUTITEM))
		return;

	U64 eid = dbStruct::createeid();

	stItemLog itemlog;
	itemlog.setlog(eid, stallId, TARGET_STALLSHOP, PlayerID, LOG_SHOP_COSTSHOUTITEM);
	itemlog.SrcNum = itemNum;
	itemlog.TagNum = itemNum-1;
	itemlog.ChangeNum = -1;
	itemlog.ItemID = itemId;
	SERVER->GetLog()->writeLog(&itemlog);
}

// ----------------------------------------------------------------------------
// ��¼���̱�ϵͳ����
void GameplayLog::GL_ShopDumpStall(U32 PlayerID, U32 stallId)
{
	if(!GL_IsRecord(0, PlayerID, LOG_SHOP_DUMPSTALL))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),stallId,TARGET_PLAYER,PlayerID,LOG_SHOP_DUMPSTALL);
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼������һ�ֱ������
void GameplayLog::GL_ArenaFight(U32 PlayerId, S32 Arena, S32 Season, S32 TotalScore, S32 CurrentScore)
{
	if(!GL_IsRecord(0, PlayerId, LOG_PLAYER_ARENAFIGHT))
		return;

	stPlayerLog log;
	log.setlog(dbStruct::createeid(),PlayerId,TARGET_PLAYER,0,LOG_PLAYER_ARENAFIGHT);
	log.DataType = stPlayerLog::TYPE_ARENASCORE;
	log.Val[0] = CurrentScore;
	log.Val[1] = TotalScore;
	log.Val[2] = Arena;
	log.Val[3] = Season;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼��ɫԪ���һ�
void GameplayLog::GL_DrawSRBonus(int PlayerId, U32 SRId, S32 PerMonth, S32 gold, S32 bindgold,
				S32 money, S32 bindmoney, S32 exp)
{
	if(!GL_IsRecord(0, PlayerId, LOG_PLAYER_DRAWSRBONUS))
		return;
	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	log.setlog(eid,PlayerId,TARGET_PLAYER,PlayerId,LOG_PLAYER_DRAWSRBONUS);
	log.Val[0] = SRId;
	log.Val[1] = PerMonth;
	log.Val[2] = exp;
	SERVER->GetLog()->writeLog(&log);

	stMoneyLog moneylog;
	moneylog.setlog(eid,PlayerId,TARGET_PLAYER,PlayerId,LOG_PLAYER_DRAWSRBONUS);
	moneylog.DataType = stMoneyLog::TYPE_GOLD;
	moneylog.Val[0] = gold;
	moneylog.Val[1] = PerMonth;
	SERVER->GetLog()->writeLog(&moneylog);

	moneylog.DataType = stMoneyLog::TYPE_BINDGOLD;
	moneylog.Val[0] = bindgold;
	moneylog.Val[1] = PerMonth;
	SERVER->GetLog()->writeLog(&moneylog);

	moneylog.DataType = stMoneyLog::TYPE_MONEY;
	moneylog.Val[0] = money;
	moneylog.Val[1] = PerMonth;
	SERVER->GetLog()->writeLog(&moneylog);

	moneylog.DataType = stMoneyLog::TYPE_BINDMONEY;
	moneylog.Val[0] = bindmoney;
	moneylog.Val[1] = PerMonth;
	SERVER->GetLog()->writeLog(&moneylog);
}

// ----------------------------------------------------------------------------
// �����ʼ�
void GameplayLog::GL_MailSend(int PlayerId, S32 recver, stMailItem& mailitem)
{
	if(!GL_IsRecord(0, PlayerId, LOG_MAIL_SEND))
		return;

	U64 eid = dbStruct::createeid();

	stMailLog log;
	log.setlog(eid, mailitem.nSender, TARGET_PLAYER, mailitem.nRecver, LOG_MAIL_SEND);
	log.info = mailitem;
	SERVER->GetLog()->writeLog(&log);

	if(mailitem.nItemId != 0)
	{
		U64 ITEMNO = dbStruct::createeid();		
		if(mailitem.item[0] != 0)
		{
			stItemInfo* item = (stItemInfo*)mailitem.item;
			stItemDetailLog itemdetaillog;
			itemdetaillog.setlog(eid, PlayerId,TARGET_PLAYER,recver,LOG_MAIL_SEND);
			itemdetaillog.ItemNo = ITEMNO;
			itemdetaillog.info = *item;
			SERVER->GetLog()->writeLog(&itemdetaillog);
		}

		stItemLog itemlog;
		itemlog.setlog(eid, PlayerId, TARGET_PLAYER, recver, LOG_MAIL_SEND);
		itemlog.SrcNum = mailitem.nItemCount;
		itemlog.TagNum = 0;
		itemlog.ChangeNum = -mailitem.nItemCount;
		itemlog.ItemID = mailitem.nItemId;
		itemlog.ItemNo = ITEMNO;
		SERVER->GetLog()->writeLog(&itemlog);
	}

	if(mailitem.nMoney != 0)
	{
		stMoneyLog moneylog;
		moneylog.setlog(eid,PlayerId,TARGET_PLAYER,recver,LOG_MAIL_SEND);
		moneylog.DataType = stMoneyLog::TYPE_MONEY;
		moneylog.Val[0] = -mailitem.nMoney;
		SERVER->GetLog()->writeLog(&moneylog);
	}
}

// ----------------------------------------------------------------------------
// ����ȫ���ʼ�
void GameplayLog::GL_ServerMailSend(S32 recver, stMailItem& mailitem)
{
	if(!GL_IsRecord(0, mailitem.nSender, LOG_SERVER_MAIL_SEND))
		return;

	U64 eid = dbStruct::createeid();

	stMailLog log;
	log.setlog(eid, mailitem.nSender, TARGET_PLAYER, mailitem.nRecver, LOG_SERVER_MAIL_SEND);
	log.info = mailitem;
	SERVER->GetLog()->writeLog(&log);

	if(mailitem.nItemId != 0)
	{
		U64 ITEMNO = dbStruct::createeid();		
		if(mailitem.item[0] != 0)
		{
			stItemInfo* item = (stItemInfo*)mailitem.item;
			stItemDetailLog itemdetaillog;
			itemdetaillog.setlog(eid, mailitem.nSender,TARGET_PLAYER,recver,LOG_SERVER_MAIL_SEND);
			itemdetaillog.ItemNo = ITEMNO;
			itemdetaillog.info = *item;
			SERVER->GetLog()->writeLog(&itemdetaillog);
		}

		stItemLog itemlog;
		itemlog.setlog(eid, mailitem.nSender, TARGET_PLAYER, recver, LOG_SERVER_MAIL_SEND);
		itemlog.SrcNum = mailitem.nItemCount;
		itemlog.TagNum = 0;
		itemlog.ChangeNum = -mailitem.nItemCount;
		itemlog.ItemID = mailitem.nItemId;
		itemlog.ItemNo = ITEMNO;
		SERVER->GetLog()->writeLog(&itemlog);
	}

	if(mailitem.nMoney != 0)
	{
		stMoneyLog moneylog;
		moneylog.setlog(eid,mailitem.nSender,TARGET_PLAYER,recver,LOG_SERVER_MAIL_SEND);
		moneylog.DataType = stMoneyLog::TYPE_MONEY;
		moneylog.Val[0] = -mailitem.nMoney;
		SERVER->GetLog()->writeLog(&moneylog);
	}
}


// ----------------------------------------------------------------------------
// ��¼��������Ʒ���ɹ�
void GameplayLog::GL_AuctionAddOK(int PlayerId, U32 ItemId, stItemInfo* pItem, S32 ItemNum, S32 price)
{
	if(!GL_IsRecord(PlayerId, 0, LOG_AUCTION_ADD_WORLD_OK))
		return;

	U64 eid = dbStruct::createeid();
	U64 ITEMNO = 0;
	if(pItem)
	{
		ITEMNO = dbStruct::createeid();
		stItemDetailLog itemdetaillog;
		itemdetaillog.setlog(eid, PlayerId,TARGET_PLAYER,PlayerId,LOG_AUCTION_ADD_WORLD_OK);
		itemdetaillog.ItemNo = ITEMNO;
		itemdetaillog.info = *pItem;
		SERVER->GetLog()->writeLog(&itemdetaillog);
	}

	if(ItemId > 100000000 && ItemId < 150000000)
	{
		stItemLog itemlog;
		itemlog.setlog(eid, PlayerId,TARGET_PLAYER,PlayerId,LOG_AUCTION_ADD_WORLD_OK);
		itemlog.ItemID = ItemId;
		itemlog.SrcNum = ItemNum;
		itemlog.TagNum = ItemNum;
		itemlog.ChangeNum = 0;
		itemlog.ShopID = price;
		itemlog.ItemNo = ITEMNO;
		SERVER->GetLog()->writeLog(&itemlog);
	}
}

// ----------------------------------------------------------------------------
// ��¼��������Ʒ���ʧ��
void GameplayLog::GL_AuctionAddFailed(int PlayerId, U32 ItemId, stItemInfo* pItem, S32 ItemNum, S32 price)
{
	if(!GL_IsRecord(PlayerId, 0, LOG_AUCTION_ADD_WORLD_FAILED))
		return;

	U64 eid = dbStruct::createeid();
	U64 ITEMNO = 0;
	if(pItem)
	{
		ITEMNO = dbStruct::createeid();
		stItemDetailLog itemdetaillog;
		itemdetaillog.setlog(eid, PlayerId,TARGET_PLAYER,PlayerId,LOG_AUCTION_ADD_WORLD_FAILED);
		itemdetaillog.ItemNo = ITEMNO;
		itemdetaillog.info = *pItem;
		SERVER->GetLog()->writeLog(&itemdetaillog);
	}

	if(ItemId > 100000000 && ItemId < 150000000)
	{
		stItemLog itemlog;
		itemlog.setlog(eid, PlayerId,TARGET_PLAYER,PlayerId,LOG_AUCTION_ADD_WORLD_FAILED);
		itemlog.ItemID = ItemId;
		itemlog.SrcNum = ItemNum;
		itemlog.TagNum = ItemNum;
		itemlog.ChangeNum = 0;
		itemlog.ShopID = price;
		itemlog.ItemNo = ITEMNO;
		SERVER->GetLog()->writeLog(&itemlog);
	}
}

// ----------------------------------------------------------------------------
// ��¼��������Ʒ�������ɹ�
void GameplayLog::GL_AuctionBuyOK(int PlayerId, U32 Id, S32 price)
{
	if(!GL_IsRecord(PlayerId, 0, LOG_AUCTION_BUY_WORLD_OK))
		return;

	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	log.setlog(eid,PlayerId,TARGET_PLAYER,PlayerId,LOG_AUCTION_BUY_WORLD_OK);
	log.Val[0] = Id;
	log.Val[1] = price;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼��������Ʒ�������ʧ��
void GameplayLog::GL_AuctionBuyFailed(int PlayerId, U32 Id, S32 price)
{
	if(!GL_IsRecord(PlayerId, 0, LOG_AUCTION_BUY_WORLD_FAILED))
		return;

	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	log.setlog(eid,PlayerId,TARGET_PLAYER,PlayerId,LOG_AUCTION_BUY_WORLD_FAILED);
	log.Val[0] = Id;
	log.Val[1] = price;
	SERVER->GetLog()->writeLog(&log);
}

// ----------------------------------------------------------------------------
// ��¼������ȡ�����۳ɹ�
void GameplayLog::GL_AuctionCancelOK(int PlayerId, U32 Id)
{
	if(!GL_IsRecord(PlayerId, 0, LOG_AUCTION_CANCEL_OK))
		return;

	U64 eid = dbStruct::createeid();
	stPlayerLog log;
	log.setlog(eid,PlayerId,TARGET_PLAYER,PlayerId,LOG_AUCTION_CANCEL_OK);
	log.Val[0] = Id;
	SERVER->GetLog()->writeLog(&log);
}

// ---------------------------------------------------------------------------
// ��¼��������޸�
void GameplayLog::GL_GMGodShipMdf(int GMAccountId, int playerId, U64 oldexp, U64 newexp, int oldLevel, int newLevel)
{
	if(!GL_IsRecord(GMAccountId, 0, LOG_GM_GODSHIPMODIFY))
		return;
	U64 eid = dbStruct::createeid();
	stGMLog log;
	log.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_GODSHIPMODIFY);
	char txt[250] = {0};
	sprintf_s(txt,sizeof(txt),"OldGodShipExp:%d,NewGodShipExp:%d;OldGodShipLevel:%d,NewGodShipLevel:%d",
		oldexp, newexp, oldLevel, newLevel);
	dStrcpy(log.content,sizeof(log.content),txt);
	SERVER->GetLog()->writeLog(&log);

	stPlayerLog playerlog;
	playerlog.setlog(eid, GMAccountId,TARGET_PLAYER,playerId,LOG_GM_GODSHIPMODIFY);
	playerlog.DataType = stPlayerLog::TYPE_OTHER;
	playerlog.Val[0] = oldexp;
	playerlog.Val[1] = newexp;
	playerlog.Val[2] = oldLevel;
	playerlog.Val[3] = newLevel;
	SERVER->GetLog()->writeLog(&playerlog);
}*/