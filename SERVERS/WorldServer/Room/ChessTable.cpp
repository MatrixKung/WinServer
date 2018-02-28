#include "ChessTable.h"
#include "FvMask.h"
#include <set>
#include "CommLib/format.h"
#include "../WorldServer.h"
#include "../Common/MemGuard.h"
#include "../WorldServer.h"
#include "WinTCP/IPacket.h"
#include "WinTCP/dtServerSocket.h"
#include "../PlayerMgr.h"
#include "Common/mRandom.h"
#include "RoomMgr.h"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#define IDI_TIMER_XIAO_HU			1 //С��

#define TIME_XIAO_HU  8

//���캯��
CChessTable::CChessTable()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bReady,sizeof(m_bReady));
	ZeroMemory(m_bDisband,sizeof(m_bDisband));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory( m_GangScore,sizeof(m_GangScore) );
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_wLostFanShu,sizeof(m_wLostFanShu) );

	//������Ϣ
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//������Ϣ
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	
	//���б���
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	//״̬����
	m_bSendStatus=false;
	m_bGangStatus = false;
	m_bGangOutStatus = false;

	//�û�״̬
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//����˿�
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//������Ϣ
	m_cbChiHuCard=0;
	ZeroMemory( m_dwChiHuKind,sizeof(m_dwChiHuKind));
	memset( m_wProvider,INVALID_CHAIR,sizeof(m_wProvider));
	memset( m_cbChiHuNum,0,sizeof(m_cbChiHuNum));
	m_cbGameTypeIdex = 0;
	m_dwGameRuleIdex = 0;
	memset( m_TableScore, 0, sizeof(m_TableScore));
	m_cbTableNum = 0;
	SetGameStatus(GS_MJ_FREE);
	m_CreateTime = time(NULL);
	return;
}

//��������
CChessTable::~CChessTable(void)
{
	SERVER->GetWorkQueue()->GetTimerMgr().remove(this, &CChessTable::TimeProcess);
}

//��λ����
void CChessTable::ResetTable()
{
	//��Ϸ����
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bReady,sizeof(m_bReady));
	ZeroMemory(m_bDisband,sizeof(m_bDisband));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory( m_GangScore,sizeof(m_GangScore) );
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_wLostFanShu,sizeof(m_wLostFanShu) );

	//������Ϣ
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//������Ϣ
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;


	//���б���
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;
	//m_wBankerUser = INVALID_CHAIR;

	//״̬����
	m_bSendStatus=false;
	m_bGangStatus = false;
	m_bGangOutStatus = false;

	//�û�״̬
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//����˿�
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//������Ϣ
	m_cbChiHuCard=0;
	ZeroMemory(m_dwChiHuKind,sizeof(m_dwChiHuKind));
	memset( m_wProvider,INVALID_CHAIR,sizeof(m_wProvider) );
	
	for( U16 i = 0; i < GAME_PLAYER; i++ )
	{
		m_ChiHuRight[i].SetEmpty();
	}

	//m_cbGameTypeIdex = 0;
	//m_dwGameRuleIdex = 0;

	return;
}


//��Ϸ��ʼ
bool CChessTable::OnEventGameStart()
{
	GameStart();
	AddGameRule(BIT(GAME_TYPE_ZZ_START));
	SERVER->GetWorkQueue()->GetTimerMgr().notify(this, &CChessTable::TimeProcess, 2000);
	return true;
}

void CChessTable::GameStart()
{
	//�����˿�
	U32 lSiceCount = MAKEU32(MAKEU16(gRandGen.randI(1,6),gRandGen.randI(1,6)),MAKEU16(gRandGen.randI(1,6),gRandGen.randI(1,6)));
	if(!hasRule(GAME_TYPE_ZZ_START))
		m_wBankerUser = ((U8)(lSiceCount>>24)+(U8)(lSiceCount>>16)-1)%GAME_PLAYER;
	//else
	//	m_wBankerUser=(m_wBankerUser+GAME_PLAYER-1)%GAME_PLAYER;

	//���ñ���
	m_cbProvideCard=0;
	m_wProvideUser=INVALID_CHAIR;
	m_wCurrentUser=m_wBankerUser;

	//��������
	CMD_S_GameStart GameStart;
	GameStart.lSiceCount=lSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.cbLeftCardCount = m_cbLeftCardCount;

	SetGameStatus(GS_MJ_PLAY);
	GameStart.cbXiaoHuTag = 0;

	ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
	//��������
	for (U16 i=0;i<GAME_PLAYER;i++)
	{
		//���ñ���
		GameStart.cbUserAction = WIK_NULL;//m_cbUserAction[i];

		//ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],&GameStart.cbCardData[MAX_COUNT  * i]);
	/*	if( m_pITableFrame->GetTableUserItem(i)->IsAndroidUser() )
		{
			U8 bIndex = 1;
			for( U16 j=0; j<GAME_PLAYER; j++ )
			{
				if( j == i ) continue;
				m_GameLogic.SwitchToCardData(m_cbCardIndex[j],&GameStart.cbCardData[MAX_COUNT*bIndex++]);
			}
		}*/

		//��������

		//m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
			SendTableData("CLIENT_SERVER_SendCards", GameStart);

	m_bSendStatus = true;
    DispatchCardData(m_wCurrentUser);
}

void CChessTable::Shuffle(U8* pRepertoryCard,int nCardCount)
{
	m_cbLeftCardCount = nCardCount;
	m_GameLogic.RandCardData(pRepertoryCard,nCardCount);

	//�ַ��˿�
	for (U16 i=0;i<GAME_PLAYER;i++)
	{
		//if( m_pITableFrame->GetTableUserItem(i) != NULL )
		{
			m_cbLeftCardCount-=(MAX_COUNT-1);
			m_GameLogic.SwitchToCardIndex(&pRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
		}
	}
}

//��Ϸ����
bool CChessTable::OnEventGameConclude(U16 wChairID, U8 cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{
			//��������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.wLeftUser = INVALID_CHAIR;

			//������������
			ZeroMemory(GameEnd.cbCardDataNiao,sizeof(GameEnd.cbCardDataNiao));
			//GameEnd.cbNiaoPick = m_cbNiaoPick;
			//GameEnd.cbNiaoCount = m_cbNiaoCount;
			//for (int i = 0;i<MAX_NIAO_CARD && i< m_cbNiaoCount;i++)
			//{
			//	GameEnd.cbCardDataNiao[i] = m_cbCardDataNiao[i];
			//}
			
			//������Ϣ
			U16 wWinner = INVALID_CHAIR;
			U8 cbLeftUserCount = 0;			//�ж��Ƿ�����
			bool bUserStatus[GAME_PLAYER];		//
			for (U16 i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
				m_ChiHuRight[i].GetRightData( &GameEnd.dwChiHuRight[i],MAX_RIGHT_COUNT );
				//m_StartHuRight[i].GetRightData( &GameEnd.dwStartHuRight[i],MAX_RIGHT_COUNT );
				
				//���������
				if( m_ChiHuRight[i].IsEmpty() ) cbLeftUserCount++;
				//��ǰ���״̬//*********
				//if( NULL != m_pITableFrame->GetTableUserItem(i) ) bUserStatus[i] = true;
				else bUserStatus[i] = false;
			}

			S64 lGangScore[GAME_PLAYER];
			ZeroMemory(&lGangScore,sizeof(lGangScore));
			for( U16 i = 0; i < GAME_PLAYER; i++ )
			{
				for( U8 j = 0; j < m_GangScore[i].cbGangCount; j++ )
				{
					for( U16 k = 0; k < GAME_PLAYER; k++ )
						lGangScore[k] += m_GangScore[i].lScore[j][k];
				}
			}


			if(cbLeftUserCount == GAME_PLAYER)
			{
				//�����¼������ׯ
				m_wBankerUser=(m_wBankerUser+GAME_PLAYER-1)%GAME_PLAYER;
			}
	
			for( U16 i = 0; i < GAME_PLAYER; i++ )
			{
				m_lGameScore[i] += lGangScore[i];				
			}

			//*********
			//�۷�
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			int	lGameTaxs[GAME_PLAYER];				//
			ZeroMemory(&lGameTaxs,sizeof(lGameTaxs));
			//ͳ�ƻ���
			for (U16 i=0;i<GAME_PLAYER;i++)
			{
				//*********
				//if( NULL == m_pITableFrame->GetTableUserItem(i) ) continue;

				//���û���
				if (m_lGameScore[i])
				{
					//*********
					//lGameTaxs[i] = m_pITableFrame->CalculateRevenue(i,m_lGameScore[i]);
					m_lGameScore[i] -= lGameTaxs[i];
					if(hasRule(GAME_TYPE_ZZ_SUOHU))
					{
						m_TableScore[i] += m_lGameScore[i];
					}
					//SERVER->GetPlayerManager()->AddScore(GetPlayer(i), m_lGameScore[i]);
				}
				
				U8 ScoreKind;
				if( m_lGameScore[i] > 0L ) ScoreKind = SCORE_TYPE_WIN;
				else if( m_lGameScore[i] < 0L ) ScoreKind = SCORE_TYPE_LOSE;
				else ScoreKind = SCORE_TYPE_DRAW;

				ScoreInfoArray[i].lScore   = m_lGameScore[i];
				ScoreInfoArray[i].lRevenue = lGameTaxs[i];
				ScoreInfoArray[i].cbType   = ScoreKind;

			}

			if(hasRule(GAME_TYPE_ZZ_CHANGHU))
			{
				--m_cbTableNum;
			}

			//д�����
			//*********
			//m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			CopyMemory( GameEnd.lGameScore,m_lGameScore,sizeof(GameEnd.lGameScore) );
			CopyMemory( GameEnd.lGangScore,lGangScore,sizeof(GameEnd.lGangScore) );
			CopyMemory( GameEnd.wProvideUser,m_wProvider,sizeof(GameEnd.wProvideUser) );
			CopyMemory( GameEnd.lGameTax,lGameTaxs,sizeof(GameEnd.lGameTax));
			for (U16 i=0;i<GAME_PLAYER;i++)
			{
				CopyMemory( GameEnd.wLostFanShu[i],m_wLostFanShu[i],sizeof(m_wLostFanShu[i]) );
				CopyMemory(GameEnd.WeaveItemArray[i],m_WeaveItemArray[i],sizeof(m_WeaveItemArray[i]));
			}
			//CopyMemory( GameEnd.lStartHuScore,m_lStartHuScore,sizeof(GameEnd.lStartHuScore) );

			

			//����˿�
			CopyMemory(GameEnd.cbWeaveCount,m_cbWeaveItemCount,sizeof(GameEnd.cbWeaveCount));

			//���ͽ�����Ϣ
			SendTableData("WC_CHESS_GAME_END", GameEnd);
			//m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd) );

			//������Ϸ
			SetGameStatus(GS_MJ_FREE);

			return true;
		}
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			//��������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.wLeftUser = INVALID_CHAIR;

			//���ñ���
			memset( GameEnd.wProvideUser,INVALID_CHAIR,sizeof(GameEnd.wProvideUser) );

			//�����˿�
			for (U16 i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}

			//������Ϣ	
			SendTableData("WC_CHESS_GAME_END", GameEnd);
			//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//������Ϸ
			SetGameStatus(GS_MJ_FREE);

			return true;
		}
	case GER_NETWORK_ERROR:		//�������
	case GER_USER_LEAVE:		//�û�ǿ��
		{
			m_bTrustee[wChairID] = true;

			return true;
		}
	}

	//�������
	ASSERT(false);
	return false;
}

//���ͳ���
bool CChessTable::OnEventSendGameScene(U16 wChiarID, bool bSendSecret, U64 nTableId, U32 playerId)
{
	switch (GetGameStatus())
	{
	case GS_MJ_FREE:	//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));
			CChessTable* nTable = ROOMMGR->FindChessTable(nTableId);
			if(nTable)
			{
				ROOMMGR->RestartChessTable(playerId, nTableId);
					//if(nTable->ReadyGame(wChiarID))
					//{
					//	nTable->ResetTable();
					//	nTable->OnEventGameStart();
					//}
			}
			for(int i = 0; i < GAME_PLAYER; ++i)
			{
				StatusFree.lGameScore[i] = getTableScore(i);
			}
			//��������
			StatusFree.wBankerUser=m_wBankerUser;
			StatusFree.wCurrentUser = wChiarID;
			StatusFree.nTableID = nTableId;
			//StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusFree.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//���ͳ���
			SendTableData(wChiarID, "WC_CHESS_FREE_GAMESCENE", StatusFree);
			return true;
		}
	case GS_MJ_PLAY:	//��Ϸ״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//��Ϸ����
			StatusPlay.nChairId = wChiarID;
			StatusPlay.nTableID = nTableId;
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=0;//m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//״̬����
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.cbActionMask=(m_bResponse[wChiarID]==false)?m_cbUserAction[wChiarID]:WIK_NULL;

			//��ʷ��¼
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			for (int i = 0;i<GAME_PLAYER;i++)
			{
				CopyMemory(StatusPlay.cbDiscardCard[i],m_cbDiscardCard[i],sizeof(U8)*60);
			}
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));

			//����˿�
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));


			//�˿�����
			StatusPlay.cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex[wChiarID],StatusPlay.cbCardData);
			StatusPlay.cbSendCardData=((m_cbSendCardData!=0)&&(m_wProvideUser==wChiarID))?m_cbSendCardData:0x00;

			for(int i = 0; i < GAME_PLAYER; ++i)
			{
				StatusPlay.wWinOrder[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
				StatusPlay.lGameScore[i] = getTableScore(i);
			}
			//StatusPlay.nTableNum = GetTableNum();
			//���ͳ���
			SendTableData(wChiarID, "WC_CHESS_PLAY_GAMESCENE", StatusPlay);
			return true;
		}
	}

	return false;
}

////��ʱ���¼�
struct stChessData
{
	U8 index;
	U8 value;

	bool operator < (const stChessData& Other) const
	{
		if(Other.index == value)
			return index < Other.index;

		return value < Other.value;
	}
};

void CChessTable::TimeProcess(U32 value)
{
	{
		U32 nCurTime = time(NULL);
		if(HasDisband())
		{
			return;
		}
		else if(nCurTime - m_CreateTime >= 24 * 60 * 60)
		{
			AddGameRule(BIT(GAME_TYPE_ZZ_DISBAND));
			this->DisbandGameSendtoPlayer(this);
		}
		//********�Զ�
		else if(GetGameStatus() == GS_MJ_PLAY )
		{
			//ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			//SetGameStatus(GS_MJ_PLAY);

			//m_bSendStatus = true;
			if(m_wCurrentUser == 0 || m_wCurrentUser == INVALID_CHAIR)
				return;
			U8 cIndex = 0;
			std::set<stChessData> ChessSet;
			for(int i = 0; i < MAX_INDEX; ++i)
			{
				if(m_cbCardIndex[m_wCurrentUser][i] != 0)
				{
					stChessData data;
					data.index = i;
					data.value = m_cbCardIndex[m_wCurrentUser][i];
					ChessSet.insert(data);
				}
			}

			if(ChessSet.empty())
				return;

			if(m_wCurrentUser == 0 || m_wCurrentUser == 1)
				cIndex = (*ChessSet.begin()).index;
			else 
				cIndex = (*ChessSet.rbegin()).index;

			if(OnUserOutCard(m_wCurrentUser, m_GameLogic.SwitchToCardData(cIndex)))
			{
				
			}
			return;
		}
		else if(GetGameStatus() == GS_MJ_FREE)
		{
			if(hasRule(GAME_TYPE_ZZ_SUOHU))
			{
				bool bDisband = false;
				for(int i = 0; i < GAME_PLAYER; ++i)
				{
					if(m_TableScore[i] < 0)
					{
						bDisband = true;
						break;
					}
				}

				if(bDisband)
				{
					AddGameRule(BIT(GAME_TYPE_ZZ_DISBAND));
					this->DisbandGameSendtoPlayer(this);
				}
			}
			else if(hasRule(GAME_TYPE_ZZ_CHANGHU))
			{
				if(m_cbTableNum  <= 0)
				{
					AddGameRule(BIT(GAME_TYPE_ZZ_DISBAND));
					this->DisbandGameSendtoPlayer(this);
				}
			}
		}
	}
	return;
}
//
////��Ϸ��Ϣ����
//bool CChessTable::OnGameMessage(U16 wSubCmdID, VOID* pDataBuffer, U16 wDataSize, IServerUserItem * pIServerUserItem)
//{
//	switch (wSubCmdID)
//	{
//	case SUB_C_OUT_CARD:		//������Ϣ
//		{
//			//Ч����Ϣ
//			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
//			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;
//
//			//�û�Ч��
//			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;
//
//			//��Ϣ����
//			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
//			return OnUserOutCard(pIServerUserItem->GetChairID(),pOutCard->cbCardData);
//		}
//	case SUB_C_OPERATE_CARD:	//������Ϣ
//		{
//			//Ч����Ϣ
//			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
//			if (wDataSize!=sizeof(CMD_C_OperateCard)) return false;
//
//			//�û�Ч��
//			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;
//
//			//��Ϣ����
//			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pDataBuffer;
//			return OnUserOperateCard(pIServerUserItem->GetChairID(),pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);
//		}
//	case SUB_C_TRUSTEE:
//		{
//			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pDataBuffer;
//			if(wDataSize != sizeof(CMD_C_Trustee)) return false;
//
//
//			m_bTrustee[pIServerUserItem->GetChairID()]=pTrustee->bTrustee;
//			CMD_S_Trustee Trustee;
//			Trustee.bTrustee=pTrustee->bTrustee;
//			Trustee.wChairID = pIServerUserItem->GetChairID();
//			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
//			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
//
//			return true;
//		}
//	case SUB_C_XIAOHU:
//		{
//			if (m_pITableFrame->GetGameStatus() != GS_MJ_XIAOHU)
//			{
//				return true;
//			}
//			U16 wChairID = pIServerUserItem->GetChairID();
//			m_dwChiHuKind[wChairID] = AnalyseChiHuCardCS_XIAOHU(m_cbCardIndex[wChairID],m_StartHuRight[wChairID]);
//
//			LONGLONG lStartHuScore = 0;
//			int wFanShu = m_GameLogic.GetChiHuActionRank_CS(m_StartHuRight[wChairID]);
//			lStartHuScore  = wFanShu*m_pGameServiceOption->lCellScore;
//			m_lStartHuScore[wChairID] += lStartHuScore*3;
//
//			for (int i=0;i<GAME_PLAYER;i++)
//			{
//				if (i == wChairID)continue;
//				m_lStartHuScore[i] -= lStartHuScore;
//			}
//
//			m_cbUserAction[wChairID] = WIK_NULL;
//
//			bool bEnd = true;
//			for (int i = 0;i < GAME_PLAYER;i++)
//			{
//				if (m_cbUserAction[i] != WIK_NULL)
//				{
//					bEnd = false;
//				}
//			}
//			if (bEnd)
//			{
//				ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
//				m_pITableFrame->SetGameStatus(GS_MJ_PLAY);
//				m_bSendStatus = true;
//				DispatchCardData(m_wCurrentUser);
//				m_pITableFrame->KillGameTimer(IDI_TIMER_XIAO_HU);
//
//			}
//			return true;
//		}
//	}
//
//	return false;
//}
//
////�����Ϣ����
//bool CChessTable::OnFrameMessage(U16 wSubCmdID, VOID * pDataBuffer, U16 wDataSize, IServerUserItem * pIServerUserItem)
//{
//	return false;
//}
//
////�û�����
//bool CChessTable::OnActionUserSitDown(U16 wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
//{
//	return true;
//}
//
////�û�����
//bool CChessTable::OnActionUserStandUp(U16 wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
//{
//	//ׯ������
//	if (bLookonUser==false)
//	{
//		m_bTrustee[wChairID]=false;
//		CMD_S_Trustee Trustee;
//		Trustee.bTrustee=false;
//		Trustee.wChairID = wChairID;
//		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
//		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
//	}
//
//	return true;
//}

void CChessTable::SetPrivateInfo(U8 bGameTypeIdex,U32	bGameRuleIdex)
{
	m_cbGameTypeIdex = bGameTypeIdex;
	m_dwGameRuleIdex = bGameRuleIdex;
}

void CChessTable::AddGameRule(U32 nRule)
{
	m_dwGameRuleIdex |= nRule;
}

void CChessTable::ClearGameRule(U32 nRule)
{
	m_dwGameRuleIdex &= (~nRule);
}

bool CChessTable::ReadyGame(U32 nSlot)
{
	m_bReady[nSlot] = true;
	bool bAllReady = true;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(m_bReady[i] == false)
		{
			bAllReady = false;
			break;
		}
	}
	return bAllReady;
}

bool CChessTable::ClearDisbandGame()
{
	ZeroMemory(m_bDisband,sizeof(m_bDisband));
	return true;
}

U8 CChessTable::DisbandGame(U32 nSlot, U8 nAgreeFlag)
{
	m_bDisband[nSlot] = nAgreeFlag;

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(m_bDisband[i] != 1)
		{
			return m_bDisband[i];
			break;
		}
	}
	return 1;
}

bool CChessTable::IsUserVaildCard(U16 wChairID, U8 cbCardData)
{
	if (m_GameLogic.IsValidCard(cbCardData)==false) 
		return false;

	if (wChairID >= GetMaxPlayers())
		return false;

	U8 cbIndex = m_GameLogic.SwitchToCardIndex(cbCardData);
	return m_cbCardIndex[wChairID][cbIndex];
}

bool CChessTable::IsCurrentUser(U16 wChairID)
{
	return m_wCurrentUser == wChairID;
}

template<class T>
bool CChessTable::SendTableData(U16 wChairID, const char* msg, T& obj)
{
	AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(GetPlayer(wChairID));
	if (NULL == pAccount)
		return false;

	CMemGuard Buffer(512 MEM_GUARD_PARAM);
	Base::BitStream sendPacket(Buffer.get(), 512);
	stPacketHead* pHead = IPacket::BuildPacketHead(sendPacket, msg, pAccount->GetAccountId(), SERVICE_CLIENT);
	sendPacket.writeBits(sizeof(obj)<<3,&obj);

	pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
	SERVER->GetServerSocket()->Send(pAccount->GetGateSocketId(),sendPacket);
	return true;
}

template<class T>
bool CChessTable::SendTableData(const char* msg, T& obj)
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(GetPlayer(i));
		if (NULL == pAccount)
			continue;;

		CMemGuard Buffer(512 MEM_GUARD_PARAM);
		Base::BitStream sendPacket(Buffer.get(), 512);
		stPacketHead* pHead = IPacket::BuildPacketHead(sendPacket, msg, pAccount->GetAccountId(), SERVICE_CLIENT);
		sendPacket.writeBits(sizeof(obj)<<3,&obj);

		pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
		SERVER->GetServerSocket()->Send(pAccount->GetGateSocketId(),sendPacket);
	}
	return true;
}

bool CChessTable::DisbandGameSendtoPlayer(CChessTable* pTable)
{
	for(int i = 0; i < pTable->GetMaxPlayers(); ++i)
	{

		U32 nJPlayerId = pTable->GetJoinPlayer(i);
		if(nJPlayerId)
		{
			AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(nJPlayerId);
			if(pAccount)
			{
				CMemGuard Buffer(256 MEM_GUARD_PARAM);
				Base::BitStream sendPacket(Buffer.get(), 256);
				stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, "WC_RefuseDisbandChessTable", pAccount->GetAccountId(), SERVICE_CLIENT,1);
				for(int j = 0; j < pTable->GetMaxPlayers(); ++j)
				{
					for(int k = 0; k < 3; ++k)
					{
						sendPacket.writeInt(pTable->m_cbChiHuNum[j][k],  Base::Bit8);
					}
					S64 score = pTable->getTableScore(j);
					sendPacket.writeBits(Base::Bit64, &score);
				}
				pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
				SERVER->GetServerSocket()->Send(pAccount->GetGateSocketId(), sendPacket);
			}
		}

	}
	return true;
}


//�û�����
bool CChessTable::OnUserOutCard(U16 wChairID, U8 cbCardData)
{
	//std::printf("���[%d],���һ��[%d]")
	//Ч��״̬
	ASSERT(GetGameStatus()==GS_MJ_PLAY);
	if (GetGameStatus()!=GS_MJ_PLAY) return true;

	//�������
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//Ч�����
	if (wChairID!=m_wCurrentUser) return true;
	if (m_GameLogic.IsValidCard(cbCardData)==false) return true;

	//ɾ���˿�
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(false);
		return true;
	}

	//���ñ���
	m_bSendStatus=true;
	if( m_bGangStatus )
	{
		m_bGangStatus = false;
		m_bGangOutStatus = true;
	}
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;

	//���Ƽ�¼
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;

	//��������
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;

	//������Ϣ
	//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	SendTableData("WC_CHESS_OUT_CARD", OutCard);

	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;

	//�û��л�
	m_wCurrentUser=(wChairID+GAME_PLAYER+1)%GAME_PLAYER;

	//��Ӧ�ж�
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//�ɷ��˿�
	if (bAroseAction==false) DispatchCardData(m_wCurrentUser);

	return true;
}

//�û�����
bool CChessTable::OnUserOperateCard(U16 wChairID, U8 cbOperateCode, U8 cbOperateCard)
{
	//Ч��״̬
	ASSERT(GetGameStatus()!=GS_MJ_FREE);
	if (GetGameStatus()==GS_MJ_FREE)
		return true;

	//Ч���û�	ע�⣺�������п��ܷ����˶���
	//ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));
	if ((wChairID!=m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR)) 
		return true;

	//��������
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//Ч��״̬
		if (m_bResponse[wChairID]==true) 
			return true;
		if ((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)==0))
			return true;

		//��������
		U16 wTargetUser=wChairID;
		U8 cbTargetAction=cbOperateCode;

		//���ñ���
		m_bResponse[wChairID]=true;
		m_cbPerformAction[wChairID]=cbOperateCode;
		m_cbOperateCard[wChairID]=m_cbProvideCard;

		//ִ���ж�
		for (U16 i=0;i<GAME_PLAYER;i++)
		{
			//��ȡ����
			U8 cbUserAction=(m_bResponse[i]==false)?m_cbUserAction[i]:m_cbPerformAction[i];

			//���ȼ���
			U8 cbUserActionRank=m_GameLogic.GetUserActionRank(cbUserAction);
			U8 cbTargetActionRank=m_GameLogic.GetUserActionRank(cbTargetAction);

			//�����ж�
			if (cbUserActionRank>cbTargetActionRank)
			{
				wTargetUser=i;
				cbTargetAction=cbUserAction;
			}
		}
		if (m_bResponse[wTargetUser]==false) 
			return true;

		//�Ժ��ȴ�
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (U16 i=0;i<GAME_PLAYER;i++)
			{
				if ((m_bResponse[i]==false)&&(m_cbUserAction[i]&WIK_CHI_HU))
				{
					int jjjj = 0;
				}
				//	return true;
			}
		}

		//��������
		if (cbTargetAction==WIK_NULL)
		{
			//�û�״̬
			ZeroMemory(m_bResponse,sizeof(m_bResponse));
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
			ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

			//�����˿�
			DispatchCardData(m_wResumeUser);

			return true;
		}

		//��������
		U8 cbTargetCard=m_cbOperateCard[wTargetUser];

		//���Ʊ���
		m_cbOutCardData=0;
		m_bSendStatus=true;
		m_wOutCardUser=INVALID_CHAIR;

		//���Ʋ���
		if (cbTargetAction==WIK_CHI_HU)
		{
			setSpecialType();//��������������
			//������Ϣ
			m_cbChiHuCard=cbTargetCard;

			for (U16 i=(m_wProvideUser+GAME_PLAYER-1)%GAME_PLAYER;i!=m_wProvideUser;i = (i+GAME_PLAYER-1)%GAME_PLAYER)
			{
				//�����ж�
				if ((m_cbPerformAction[i]&WIK_CHI_HU)==0)
					continue;

				//�����ж�
				U8 cbWeaveItemCount=m_cbWeaveItemCount[i];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
				m_dwChiHuKind[i] = AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,m_ChiHuRight[i]);

				//�����˿�
				if (m_dwChiHuKind[i]!=WIK_NULL) 
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
					ProcessChiHuUser( i,false);
				}
			}

			OnEventGameConclude( INVALID_CHAIR,GER_NORMAL );

			return true;
		}

		//�û�״̬
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

		//����˿�
		ASSERT(m_cbWeaveItemCount[wTargetUser]<4);
		U16 wIndex=m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard=true;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind=cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//ɾ���˿�
		switch (cbTargetAction)
		{
		case WIK_LEFT:		//���Ʋ���
			{
				//ɾ���˿�
				U8 cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_LEFT,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );
			}
			break;
		case WIK_RIGHT:		//���Ʋ���
			{
				//ɾ���˿�
				U8 cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_RIGHT,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );
			}
			break;
		case WIK_CENTER:	//���Ʋ���
			{
				//ɾ���˿�
				U8 cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_CENTER,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );
			}
			break;
		case WIK_PENG:		//���Ʋ���
			{
				//ɾ���˿�
				U8 cbRemoveCard[]={cbTargetCard,cbTargetCard};
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );
			}
			break;
		case WIK_GANG:		//���Ʋ���
			{
				//ɾ���˿�,��������ֻ���ڷŸ�
				U8 cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard)) );
			}
			break;
		default:
			ASSERT( false );
			return false;
		}

		//������
		CMD_S_OperateResult OperateResult;
		OperateResult.wOperateUser=wTargetUser;
		OperateResult.cbOperateCard=cbTargetCard;
		OperateResult.cbOperateCode=cbTargetAction;
		OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//������Ϣ
		SendTableData("WC_CHESS_OPERATE_RESULT", OperateResult);
		//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
		//�����û�
		m_wCurrentUser=wTargetUser;

		//���ƴ���
		if (cbTargetAction==WIK_GANG)
		{
			//������Ϣ
			CMD_S_GangScore gs;
			ZeroMemory( &gs,sizeof(gs) );
			gs.wChairId = wTargetUser;
			gs.cbXiaYu = false;
			gs.lGangScore[wTargetUser] += 1L;//m_pGameServiceOption->lCellScore*2L;
			gs.lGangScore[m_wProvideUser] -= 1L;//-m_pGameServiceOption->lCellScore*2L;
			SendTableData("WC_CHESS_GANG_SCORE", gs);
			//m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );

			//�ܵ÷�
			U8 cbIndex = m_GangScore[wTargetUser].cbGangCount++;
			m_GangScore[wTargetUser].lScore[cbIndex][wTargetUser] += 1L;//m_pGameServiceOption->lCellScore*2;
			m_GangScore[wTargetUser].lScore[cbIndex][m_wProvideUser] -= 1L;//-m_pGameServiceOption->lCellScore*2;
			m_bGangStatus = true;
			DispatchCardData(wTargetUser,true);
		}

		return true;
	}

	//��������
	if (m_wCurrentUser==wChairID)
	{
		//Ч�����
		if ((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)==0))
			return true;

		//�˿�Ч��
		ASSERT((cbOperateCode==WIK_NULL)||(cbOperateCode==WIK_CHI_HU)||
			(m_GameLogic.IsValidCard(cbOperateCard)==true));
		if ((cbOperateCode!=WIK_NULL)&&(cbOperateCode!=WIK_CHI_HU)
			&&(m_GameLogic.IsValidCard(cbOperateCard)==false)) 
			return true;

		//���ñ���
		m_bSendStatus=true;
		m_cbUserAction[m_wCurrentUser]=WIK_NULL;
		m_cbPerformAction[m_wCurrentUser]=WIK_NULL;

		bool bPublic=false;
		U16  wGangProvideUser = INVALID_CHAIR;

		//ִ�ж���
		switch (cbOperateCode)
		{
		case WIK_GANG:			//���Ʋ���
			{
				//��������
				U8 cbWeaveIndex=0xFF;
				U8 cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard);

				//���ƴ���
				if (m_cbCardIndex[wChairID][cbCardIndex]==1)
				{
					//Ѱ�����
					for (U8 i=0;i<m_cbWeaveItemCount[wChairID];i++)
					{
						U8 cbWeaveKind=m_WeaveItemArray[wChairID][i].cbWeaveKind;
						U8 cbCenterCard=m_WeaveItemArray[wChairID][i].cbCenterCard;
						if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
						{
							bPublic=true;
							cbWeaveIndex=i;
							break;
						}
					}

					//Ч�鶯��
					ASSERT(cbWeaveIndex!=0xFF);
					if (cbWeaveIndex==0xFF) return false;

					//����˿�
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=true;
					//m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					wGangProvideUser = m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
				}
				else
				{
					//�˿�Ч��
					ASSERT(m_cbCardIndex[wChairID][cbCardIndex]==4);
					if (m_cbCardIndex[wChairID][cbCardIndex]!=4) 
						return false;

					//���ñ���
					bPublic=false;
					cbWeaveIndex=m_cbWeaveItemCount[wChairID]++;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=false;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
				}

				//ɾ���˿�
				m_cbCardIndex[wChairID][cbCardIndex]=0;

				m_bGangStatus = true;

				//���Ƶ÷�
				S64 lScore = 1L;
				U8 cbGangIndex = m_GangScore[wChairID].cbGangCount++;
				if(bPublic)
				{
					m_GangScore[wChairID].lScore[cbGangIndex][wChairID] += lScore;//m_pGameServiceOption->lCellScore*2;
					if(wGangProvideUser != INVALID_CHAIR)
					{
						m_GangScore[wChairID].lScore[cbGangIndex][wGangProvideUser] -= lScore;//-m_pGameServiceOption->lCellScore*2;
					}
				}
				else
				{
					for( U16 i = 0; i < GAME_PLAYER; i++ )
					{
						if(  i == wChairID ) continue;

						m_GangScore[wChairID].lScore[cbGangIndex][i] = -lScore;
						m_GangScore[wChairID].lScore[cbGangIndex][wChairID] += lScore;
					}
				}

				//������
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=cbOperateCode;
				OperateResult.cbOperateCard=cbOperateCard;

				//������Ϣ
				SendTableData("WC_CHESS_OPERATE_RESULT", OperateResult);
				//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));


				//������Ϣ
				CMD_S_GangScore gs;
				gs.wChairId = wChairID;
				ZeroMemory( &gs,sizeof(gs) );
				gs.cbXiaYu = bPublic?false:true;
				for( U16 i = 0; i < GAME_PLAYER; i++ )
				{
					if( i == wChairID ) continue;

					gs.lGangScore[i] = -lScore;
					gs.lGangScore[wChairID] += lScore;
				}

				SendTableData("WC_CHESS_GANG_SCORE", gs);
				//m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );

				//Ч�鶯��
				bool bAroseAction=false;
				if (bPublic==true) bAroseAction=EstimateUserRespond(wChairID,cbOperateCard,EstimatKind_GangCard);

				//�����˿�
				if (bAroseAction==false)
				{
					DispatchCardData(wChairID,true);
				}
				return true;
			}
		case WIK_CHI_HU:		//�Ժ�����
			{
				setSpecialType();//��������������
				//����Ȩλ
				if (m_cbOutCardCount==0)
				{
					m_wProvideUser = m_wCurrentUser;
					m_cbProvideCard = m_cbSendCardData;
				}

				//��ͨ����
				U8 cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];
				m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],&m_cbProvideCard,1);
				m_dwChiHuKind[wChairID] = AnalyseChiHuCard(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,m_cbProvideCard,
					m_ChiHuRight[wChairID]);
				m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;

				//������Ϣ
				m_cbChiHuCard=m_cbProvideCard;

				ProcessChiHuUser( wChairID,false);

				
				OnEventGameConclude( INVALID_CHAIR,GER_NORMAL );

				return true;
			}
		}

		return true;
	}

	return false;
}

//���Ͳ���
bool CChessTable::SendOperateNotify()
{
	//������ʾ
	for (U16 i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbUserAction[i]!=WIK_NULL)
		{
			//��������
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.wResumeUser=m_wResumeUser;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//��������
			SendTableData(i, "WC_CHESS_OPERATE_NOTIFY", OperateNotify);
			//m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		}
	}

	return true;
}

//�ɷ��˿�
bool CChessTable::DispatchCardData(U16 wCurrentUser,bool bTail)
{
	//״̬Ч��
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR)
		return false;

	//�����˿�
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}

	//��ׯ����
	if (m_cbLeftCardCount==0)
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser,GER_NORMAL);

		return true;
	}

	//���ñ���
	m_cbOutCardData=0;
	m_wCurrentUser=wCurrentUser;
	m_wOutCardUser=INVALID_CHAIR;
	tagGangCardResult GangCardResult;

	//�ܺ���
	if( m_bGangOutStatus )
	{
		m_bGangOutStatus = false;
	}

	//���ƴ���
	if (m_bSendStatus==true)
	{
		//�����˿�
		m_cbSendCardCount++;
		m_cbSendCardData= DoDispatchCardData();

		//�����ж�
		CChiHuRight chr;
		m_wProvideUser = wCurrentUser;
		m_cbUserAction[wCurrentUser]|= AnalyseChiHuCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],m_cbSendCardData,chr);

		//����
		m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//���ñ���
		m_cbProvideCard = m_cbSendCardData;

		//�����ж�
		if (m_cbLeftCardCount>0)
		{
			m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],GangCardResult);
		}
	}


	//��������
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.bTail = bTail;
	SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
	SendCard.cbCardData=(m_bSendStatus==true)?m_cbSendCardData:0x00;
	//AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(GetPlayer(wCurrentUser));
	//if (NULL == pAccount)
	//	return false;

	//CMemGuard Buffer(256 MEM_GUARD_PARAM);
	//Base::BitStream sendPacket(Buffer.get(), 256);
	//stPacketHead* pHead = IPacket::BuildPacketHead(sendPacket, "CLIENT_SERVER_SendOneCards", GetPlayer(wCurrentUser), SERVICE_CLIENT);
	//sendPacket.writeBits(sizeof(SendCard)<<3,&SendCard);

	//pHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
	//SERVER->GetServerSocket()->Send(pAccount->GetGateSocketId(),sendPacket);
	//��������
	//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	SendTableData("WC_CHESS_SEND_CARD", SendCard);

	return true;
}

//��Ӧ�ж�
bool CChessTable::EstimateUserRespond(U16 wCenterUser, U8 cbCenterCard, enEstimatKind EstimatKind)
{
	//��������
	bool bAroseAction=false;

	//�û�״̬
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//�����ж�
	for (U16 i=0;i<GAME_PLAYER;i++)
	{
		//�û�����
		if (wCenterUser==i) continue;

		//��������
		if (EstimatKind==EstimatKind_OutCard)
		{
			////�����ж�
			m_cbUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

			//�����ж�
			if (m_cbLeftCardCount>0) 
			{
				m_cbUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
			}
		}

		//�Ժ��ж�
		CChiHuRight chr;
		U8 cbWeaveCount=m_cbWeaveItemCount[i];
		m_cbUserAction[i]|= AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,chr);
		

		//����ж�
		if (m_cbUserAction[i]!=WIK_NULL) 
			bAroseAction=true;
	}

	//��ɳ�齫�Բ���
	//if (m_cbGameTypeIdex == GAME_TYPE_108)
	if(!hasRule(GAME_TYPE_ZZ_CHI))
	{
		m_cbUserAction[m_wCurrentUser]|= m_GameLogic.EstimateEatCard(m_cbCardIndex[m_wCurrentUser],cbCenterCard);

		//����ж�
		if (m_cbUserAction[m_wCurrentUser]!=WIK_NULL) 
			bAroseAction=true;
	}

	//�������
	if (bAroseAction==true) 
	{
		//���ñ���
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;
		
		//********�Զ�
		for (U16 i=1;i<GAME_PLAYER;i++)
		{
			if(m_cbUserAction[i])
			{
				if(m_cbUserAction[i] & WIK_CHI_HU)
					OnUserOperateCard(i,WIK_CHI_HU, m_cbProvideCard);
				else if(m_cbUserAction[i] & WIK_XIAO_HU)
					OnUserOperateCard(i,WIK_XIAO_HU, m_cbProvideCard);
				else if(m_cbUserAction[i] & WIK_GANG)
					OnUserOperateCard(i,WIK_GANG, m_cbProvideCard);
				else if(m_cbUserAction[i] & WIK_PENG)
					OnUserOperateCard(i,WIK_PENG, m_cbProvideCard);
				else if(m_cbUserAction[i] & WIK_RIGHT)
					OnUserOperateCard(i,WIK_RIGHT, m_cbProvideCard);
				else if(m_cbUserAction[i] & WIK_CENTER)
					OnUserOperateCard(i,WIK_CENTER, m_cbProvideCard);
				else if(m_cbUserAction[i] & WIK_LEFT)
					OnUserOperateCard(i,WIK_LEFT, m_cbProvideCard);
				else
					OnUserOperateCard(i,m_cbUserAction[i], m_cbProvideCard);

				if(GetGameStatus() == GS_MJ_FREE)
					break;
			}
		}

		//if(m_wCurrentUser == INVALID_CHAIR && GetGameStatus() != GS_MJ_FREE)
		//	ASSERT(false);
		//������ʾ
		SendOperateNotify();
		return true;
	}

	return false;
}

U16 CChessTable::GetChiHuActionRank(const CChiHuRight & ChiHuRight)
{	
	U16 wFanShu = 0;
	//if( !(ChiHuRight&CHR_ZI_MO).IsEmpty() )
	//	wFanShu *= 2;
	if( !(ChiHuRight&CHR_GANG_KAI).IsEmpty() )
		wFanShu = 2;
	if( !(ChiHuRight&CHR_QIANG_GANG_HU).IsEmpty() )
		wFanShu = 1;

	if (wFanShu == 0)
	{
		if( !(ChiHuRight&CHR_SHU_FAN).IsEmpty() )
			wFanShu = 1;
	}

	ASSERT( wFanShu > 0 );
	return wFanShu;
}

//
void CChessTable::ProcessChiHuUser( U16 wChairId, bool bGiveUp)
{
	if( !bGiveUp )
	{
		//����Ȩλ
		CChiHuRight &chr = m_ChiHuRight[wChairId];

		FiltrateRight( wChairId,chr );
		U16 wFanShu = 1;
		wFanShu = GetChiHuActionRank(chr);
		S64 lChiHuScore = wFanShu*1;//m_pGameServiceOption->lCellScore;
		
		//������,����ת��
		//if( !(chr&CHR_GANG_SHANG_PAO).IsEmpty() )
		//{
		//	U8 cbGangIndex = (m_GangScore[m_wProvideUser].cbGangCount>1)?(m_GangScore[m_wProvideUser].cbGangCount-1):0;
		//	//һ�ڶ���������,������ƽ�ָܵ÷�
		//	U8 cbChiHuCount = 0;
		//	for( U16 i = 0; i < GAME_PLAYER; i++ )
		//		if( m_cbPerformAction[i]==WIK_CHI_HU ) cbChiHuCount++;
		//	if( cbChiHuCount == 1 )
		//	{
		//		S64 lScore = m_GangScore[m_wProvideUser].lScore[cbGangIndex][wChairId];
		//		m_GangScore[m_wProvideUser].lScore[cbGangIndex][wChairId] = m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser];
		//		m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser] = lScore;
		//	}
		//	else
		//	{
		//		S64 lGangScore = m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser]/cbChiHuCount;
		//		lGangScore = getMax(lGangScore,(S64)2);//m_pGameServiceOption->lCellScore);
		//		for( U16 i = 0; i < GAME_PLAYER; i++ )
		//		{
		//			if( m_cbPerformAction[i]==WIK_CHI_HU )
		//				m_GangScore[m_wProvideUser].lScore[cbGangIndex][i] = lGangScore;
		//		}
		//		m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser] = 0;
		//		for( U16 i = 0; i < GAME_PLAYER; i++ )
		//		{
		//			if( i != m_wProvideUser)
		//				m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser] += m_GangScore[m_wProvideUser].lScore[cbGangIndex][i];
		//		}
		//	}
		//}
		//���ֲܸܷ���
		//else if( !(chr&CHR_QIANG_GANG_HU).IsEmpty() )
		if( !(chr&CHR_QIANG_GANG_HU).IsEmpty() )
		{
			m_GangScore[m_wProvideUser].cbGangCount--;
		}

		if( m_wProvideUser != wChairId )
			m_wLostFanShu[m_wProvideUser][wChairId] = (U16)lChiHuScore;
		else
		{
			for( U16 i = 0; i < GAME_PLAYER; i++ )
			{
				if( i == wChairId ) continue;

				m_wLostFanShu[i][wChairId] = (U16)lChiHuScore;
			}
		}

		//ƽ�����ܵ���
		if( m_wProvideUser == wChairId || lChiHuScore == 1)
		{
			for( U16 i = 0; i < GAME_PLAYER; i++ )
			{
				if( i == wChairId ) continue;

				//���Ʒ�
				m_lGameScore[i] -= lChiHuScore;
				m_lGameScore[wChairId] += lChiHuScore;
				
				//�����
				//m_lGameScore[i] -= m_cbNiaoPick* 1;//;m_pGameServiceOption->lCellScore;
				//m_lGameScore[wChairId] += m_cbNiaoPick* 1;//m_pGameServiceOption->lCellScore;
			}
			++m_cbChiHuNum[wChairId][0];
		}
		//����
		else
		{

			m_lGameScore[m_wProvideUser] -= lChiHuScore;
			m_lGameScore[wChairId] += lChiHuScore;
			++m_cbChiHuNum[wChairId][1];
			++m_cbChiHuNum[m_wProvideUser][2];
			//m_lGameScore[m_wProvideUser] -= m_cbNiaoPick*1;// m_pGameServiceOption->lCellScore;
			//m_lGameScore[wChairId] += m_cbNiaoPick* 1;//m_pGameServiceOption->lCellScore;
		}

		//���ñ���
		m_wProvider[wChairId] = m_wProvideUser;
		m_bGangStatus = false;
		m_bGangOutStatus = false;
		m_wBankerUser = wChairId;

		//������Ϣ
		CMD_S_ChiHu ChiHu;
		ChiHu.wChiHuUser = wChairId;
		ChiHu.wProviderUser = m_wProvideUser;
		ChiHu.lGameScore = m_lGameScore[wChairId];
		ChiHu.cbCardCount = m_GameLogic.GetCardCount( m_cbCardIndex[wChairId] );
		ChiHu.cbChiHuCard = m_cbProvideCard;
		ChiHu.cbBaoPai = 0;
		SendTableData("WC_CHESS_CHI_HU", ChiHu);
		//m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_CHI_HU,&ChiHu,sizeof(ChiHu) );
	}

	return;
}

bool CChessTable::hasRule(U8 cbRule)
{
	return FvMask::HasAny(m_dwGameRuleIdex,_MASK_(cbRule));
}

U8 CChessTable::AnalyseChiHuCard(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount, U8 cbCurrentCard, CChiHuRight &ChiHuRight)
{
	bool bSelfSendCard = (m_wProvideUser == m_wCurrentUser);
	//��������
	U8 cbChiHuKind=WIK_NULL;
	bool bQiXiaoDui = false;
	CAnalyseItemArray AnalyseItemArray;

	if (hasRule(GAME_TYPE_ZZ_ZIMOHU) && !bSelfSendCard)
	{
		return WIK_NULL;
	}

	//���ñ���
	AnalyseItemArray.clear();
	ChiHuRight.SetEmpty();
	//ChiHuRight |= CHR_SHU_FAN;
	int nGenCount = 0;

	//���ܺ�
	if( m_wCurrentUser == INVALID_CHAIR && m_bGangStatus)
	{
		if (hasRule(GAME_TYPE_ZZ_QIANGGANGHU))
		{
			ChiHuRight |= CHR_QIANG_GANG_HU;
		}
		else
		{
			ChiHuRight.SetEmpty();
			return WIK_NULL;
		}

	}
	//С�߶����⴦��
	if( m_GameLogic.IsQiXiaoDui(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard,nGenCount) )
	{
		if(nGenCount == 1)
		{
			ChiHuRight |= CHR_HAOHUA_QI_XIAO_DUI;
		}
		else if(nGenCount == 2)
		{
			ChiHuRight |= CHR_DHAOHUA_QI_XIAO_DUI;
		}
		else if (nGenCount == 3)
		{
			ChiHuRight |= CHR_THAOHUA_QI_XIAO_DUI;
		}
		else 
		{
			ChiHuRight |= CHR_QI_XIAO_DUI;
		}

		cbChiHuKind = WIK_CHI_HU;
		bQiXiaoDui = true;
	}	

	//if( !ChiHuRight.IsEmpty() )
	//	cbChiHuKind = WIK_CHI_HU;
	//�����˿�
	U8 cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//cbCurrentCardһ����Ϊ0			!!!!!!!!!
	ASSERT( cbCurrentCard != 0 );
	if( cbCurrentCard == 0 ) return WIK_NULL;

	//�����˿�
	if (cbCurrentCard!=0)
		cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCurrentCard)]++;

	//�����˿�
	bool bValue = m_GameLogic.AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);
	if (!bValue && !bQiXiaoDui)
	{
		ChiHuRight.SetEmpty();
		return WIK_NULL;
	}


	//���Ʒ���
	//���ͷ���
	while(AnalyseItemArray.size())
	{
		//��������
		tagAnalyseItem pAnalyseItem=AnalyseItemArray.front();
		AnalyseItemArray.pop_front();

		//������
		if( m_GameLogic.IsPengPeng(&pAnalyseItem) ) 
			ChiHuRight |= CHR_PENGPENG_HU;
		cbChiHuKind = WIK_CHI_HU;
		break;
	}

	if( cbChiHuKind == WIK_CHI_HU )
	{
		if(cbWeaveCount == 4 &&  m_GameLogic.IsDanDiao(cbCardIndex,cbCurrentCard))
		{
			ChiHuRight |= CHR_QUAN_QIU_REN;
		}
		//��һɫ��
		if( m_GameLogic.IsQingYiSe(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard) )
		{
			ChiHuRight |= CHR_QING_YI_SE;
		}
		if( m_GameLogic.IsZiYiSe(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard))
			ChiHuRight |= CHR_ZI_YI_SE;
	}

	//if (bSelfSendCard)
	//{
	//	cbChiHuKind = WIK_CHI_HU;
	//	ChiHuRight = CHR_SHU_FAN;
	//	ChiHuRight |= CHR_ZI_MO;
	//}

	return cbChiHuKind;
}

//
void CChessTable::FiltrateRight( U16 wChairId,CChiHuRight &chr )
{
	//Ȩλ����
	//����
	if( m_wCurrentUser == INVALID_CHAIR && m_bGangStatus )
	{
		chr |= CHR_QIANG_GANG_HU;
	}
	if (m_cbLeftCardCount==0)
	{
		chr |= CHR_HAI_DI_LAO;
	}
	//����Ȩλ
	//���ϻ�
	if( m_wCurrentUser==wChairId && m_bGangStatus )
	{
		chr |= CHR_GANG_KAI;
	}
	//������
	if( m_bGangOutStatus && !m_bGangStatus )
	{
		chr |= CHR_GANG_SHANG_PAO;
	}
}

void CChessTable::SetTableScore(S64 lScore)
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		m_TableScore[i] = lScore;
	}
}

S64 CChessTable::getTableScore(U32 nSlot)
{
	return m_TableScore[nSlot];
}

//////////////////////////////////////////////////////////////////////////
