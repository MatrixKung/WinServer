#include "ChessGX.h"
#include "FvMask.h"
#include <set>
#include "CommLib/format.h"
#include "../WorldServer.h"
#include "../Common/MemGuard.h"
#include "WinTCP/IPacket.h"
#include "WinTCP/dtServerSocket.h"
#include "../PlayerMgr.h"
#include "Common/mRandom.h"

CChessGX::CChessGX() : CChessTable()
{
	ZeroMemory(m_cbRepertoryCard_136,sizeof(m_cbRepertoryCard_136));
	m_dwGameRuleIdex = /*BIT(CChessTable::GAME_TYPE_ZZ_CHI) |*/ BIT(CChessTable::GAME_TYPE_ZZ_QIANGGANGHU);
}

CChessGX::~CChessGX()
{
}

void CChessGX::ResetTable()
{
	Parent::ResetTable();
	ZeroMemory(m_cbRepertoryCard_136,sizeof(m_cbRepertoryCard_136));
}

void CChessGX::GameStart()
{
	Shuffle(m_cbRepertoryCard_136,MAX_REPERTORY_136);
	Parent::GameStart();
}

U8 CChessGX::DoDispatchCardData()
{
	return m_cbRepertoryCard_136[--m_cbLeftCardCount];
}

U16 CChessGX::GetChiHuActionRank(const CChiHuRight & ChiHuRight)
{	
	U16 wFanShu = 1;
	if( !(ChiHuRight&CHR_KA_DAN).IsEmpty() )
	{
		wFanShu = 2;
	}
	else if( !(ChiHuRight&CHR_QI_XIAO_DUI).IsEmpty() )
	{
		wFanShu = 4;
	}
	else if( !(ChiHuRight&CHR_HAOHUA_QI_XIAO_DUI).IsEmpty() )
	{
		wFanShu = 8;
	}
	else if( !(ChiHuRight&CHR_DHAOHUA_QI_XIAO_DUI).IsEmpty() )
	{
		wFanShu = 16;
	}
	else if( !(ChiHuRight&CHR_THAOHUA_QI_XIAO_DUI).IsEmpty() )
	{
		wFanShu = 32;
	}
	else if( !(ChiHuRight&CHR_QUAN_QIU_REN).IsEmpty() )
	{
		wFanShu = 5;
	}
	else if( !(ChiHuRight&CHR_PENGPENG_HU).IsEmpty() )
	{
		wFanShu = 3;
	}
	else if( !(ChiHuRight&CHR_SHU_FAN).IsEmpty() )
	{
		wFanShu = 1;
	}

	if( !(ChiHuRight&CHR_QING_YI_SE).IsEmpty() )
	{
		wFanShu = 5;
		if( !(ChiHuRight&CHR_KA_DAN).IsEmpty() )
			wFanShu = 6;
		else if( !(ChiHuRight&CHR_QI_XIAO_DUI).IsEmpty() )
			wFanShu = 8;
		else if( !(ChiHuRight&CHR_HAOHUA_QI_XIAO_DUI).IsEmpty() )
			wFanShu = 16;
		else if( !(ChiHuRight&CHR_DHAOHUA_QI_XIAO_DUI).IsEmpty() )
			wFanShu = 32;
		else if( !(ChiHuRight&CHR_THAOHUA_QI_XIAO_DUI).IsEmpty() )
			wFanShu = 64;
		else if( !(ChiHuRight&CHR_QUAN_QIU_REN).IsEmpty() )
			wFanShu = 9;
		else if( !(ChiHuRight&CHR_PENGPENG_HU).IsEmpty() )
			wFanShu = 7;
		else if( !(ChiHuRight&CHR_SHU_FAN).IsEmpty() )
			wFanShu = 5;
	}

	if( !(ChiHuRight&CHR_ZI_YI_SE).IsEmpty() )
	{
		wFanShu = 5;

		if( !(ChiHuRight&CHR_QI_XIAO_DUI).IsEmpty() )
			wFanShu = 8;
		else if( !(ChiHuRight&CHR_HAOHUA_QI_XIAO_DUI).IsEmpty() )
			wFanShu = 16;
		else if( !(ChiHuRight&CHR_DHAOHUA_QI_XIAO_DUI).IsEmpty() )
			wFanShu = 32;
		else if( !(ChiHuRight&CHR_THAOHUA_QI_XIAO_DUI).IsEmpty() )
			wFanShu = 64;
		else if( !(ChiHuRight&CHR_QUAN_QIU_REN).IsEmpty() )
			wFanShu = 9;
		else if( !(ChiHuRight&CHR_PENGPENG_HU).IsEmpty() )
			wFanShu = 7;
		else if( !(ChiHuRight&CHR_SHU_FAN).IsEmpty() )
			wFanShu = 5;
	}

	if( !(ChiHuRight&CHR_GANG_SHANG_PAO).IsEmpty() )
	{
		wFanShu *= 2;
	}

	if( !(ChiHuRight&CHR_QIANG_GANG_HU).IsEmpty() )
	{
		wFanShu *= 3;
	}

	ASSERT( wFanShu > 0 );
	return wFanShu;
}

void CChessGX::setSpecialType()
{
	//����ׯ

}

void CChessGX::ProcessChiHuUser( U16 wChairId, bool bGiveUp)
{
	U16 nBankerUser = m_wBankerUser;
	if(!hasRule(GAME_TYPE_ZZ_MOBAO) || m_cbLeftCardCount == 0)
	{
		Parent::ProcessChiHuUser(wChairId, bGiveUp);
		return;
	}

	U16 wCurMoBaoUser = wChairId;
	if (nBankerUser != wChairId && m_wProvideUser != wChairId)
	{
		wCurMoBaoUser = m_wProvideUser;
	}
	//��������
	U8 cbCardIndexTemp = m_cbRepertoryCard_136[gRandGen.randI(0, m_cbLeftCardCount)];
	if( !bGiveUp )
	{
		//����Ȩλ
		CChiHuRight &chr = m_ChiHuRight[wChairId];

		FiltrateRight( wChairId,chr );
		U16 wFanShu = 1;
		wFanShu = GetChiHuActionRank(chr);
		S64 lChiHuScore = wFanShu*1;//m_pGameServiceOption->lCellScore;
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
				++m_cbChiHuNum[wChairId][0];
				//�����
				//m_lGameScore[i] -= m_cbNiaoPick* 1;//;m_pGameServiceOption->lCellScore;
				//m_lGameScore[wChairId] += m_cbNiaoPick* 1;//m_pGameServiceOption->lCellScore;
			}
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
		ChiHu.cbBaoPai = cbCardIndexTemp;
		SendTableData("WC_CHESS_CHI_HU", ChiHu);
		//m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_CHI_HU,&ChiHu,sizeof(ChiHu) );
	}

	bool bBankerUser = (nBankerUser == wCurMoBaoUser);//�Ƿ�Ϊׯ��
	bool bFanShu = false;
	if(bBankerUser)
	{
		//ׯ�ң������С�һ���塢��
		if(cbCardIndexTemp == 0x31 || cbCardIndexTemp == 0x35 || cbCardIndexTemp == 0x35 \
			||  cbCardIndexTemp == 0x01 || cbCardIndexTemp == 0x11 || cbCardIndexTemp == 0x21 \
			||  cbCardIndexTemp == 0x05 || cbCardIndexTemp == 0x15 || cbCardIndexTemp == 0x25 \
			||  cbCardIndexTemp == 0x09 || cbCardIndexTemp == 0x19 || cbCardIndexTemp == 0x29 \
			)
		{
			bFanShu = true;
		}


		if( bFanShu)
		{
			if(m_wProvideUser == wChairId)
			{
				for( U16 i = 0; i < GAME_PLAYER; i++ )
				{
					//if( i == wChairId ) continue;

					//���Ʒ�
					m_lGameScore[i] *= 2;
				}
			}	
			else//����
			{
				m_lGameScore[wChairId] += abs(m_lGameScore[m_wProvideUser]);
				m_lGameScore[m_wProvideUser] *= 2;
			}
		}
	}
	else//�м�
	{
		//ׯ�ҵĶԼҶ�Ӧ������Ϊ�������ס�������
		if(((nBankerUser + 2) % GAME_PLAYER)  == wCurMoBaoUser)//�Լ�
		{
			if(cbCardIndexTemp == 0x33 || cbCardIndexTemp == 0x37 \
				||  cbCardIndexTemp == 0x03 || cbCardIndexTemp == 0x13 || cbCardIndexTemp == 0x23 \
				||  cbCardIndexTemp == 0x07 || cbCardIndexTemp == 0x17 || cbCardIndexTemp == 0x27 \
				)
			{
				bFanShu = true;
			}
		}
		//ׯ�ҵ��¼Ҷ�Ӧ������Ϊ���ϡ�����������
		else if(((nBankerUser + 3) % GAME_PLAYER)  == wCurMoBaoUser)//�¼�
		{
			if(cbCardIndexTemp == 0x32 || cbCardIndexTemp == 0x36  \
				||  cbCardIndexTemp == 0x02 || cbCardIndexTemp == 0x12 || cbCardIndexTemp == 0x22 \
				||  cbCardIndexTemp == 0x06 || cbCardIndexTemp == 0x16 || cbCardIndexTemp == 0x26
				)
			{
				bFanShu = true;
			}
		}
		//ׯ�ҵ��ϼҶ�Ӧ������Ϊ�������ġ���
		else if(((nBankerUser + 1) % GAME_PLAYER)  == wCurMoBaoUser)//�ϼ�
		{
			if(cbCardIndexTemp == 0x34  \
				||  cbCardIndexTemp == 0x04 || cbCardIndexTemp == 0x14 || cbCardIndexTemp == 0x24 \
				||  cbCardIndexTemp == 0x08 || cbCardIndexTemp == 0x18 || cbCardIndexTemp == 0x28
				)
			{
				bFanShu = true;
			}
		}

		if(bFanShu)
		{
			if(m_wProvideUser == wChairId)
			{
			}	
			else//����
			{
				m_lGameScore[wChairId] += abs(m_lGameScore[nBankerUser] - m_lGameScore[m_wProvideUser]);
				m_lGameScore[nBankerUser] = m_lGameScore[m_wProvideUser];
			}
		}
		else
		{
			if(m_wProvideUser == wChairId)
			{
				m_lGameScore[wChairId] += abs(m_lGameScore[nBankerUser]);
				m_lGameScore[nBankerUser] *= 2;
			}	
			else//����
			{
			}
		}
	
	}
}

U8 CChessGX::AnalyseChiHuCard(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount, U8 cbCurrentCard, CChiHuRight &ChiHuRight)
{
	bool bSelfSendCard = (m_wProvideUser == m_wCurrentUser);
	//��������
	U8 cbChiHuKind=WIK_NULL;
	bool bQiXiao = false;
	bool bZiYiSe = false;
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
		bQiXiao = true;
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

	//��һɫ
	if( m_GameLogic.IsZiYiSe(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard))
	{
		ChiHuRight |= CHR_ZI_YI_SE;
		cbChiHuKind = WIK_CHI_HU;
		bZiYiSe = true;
	}

	//�����˿�
	bool bValue = m_GameLogic.AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);
	if (!bValue && !bQiXiao)
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
	}
	
	//ƽ��ֻ������
	/*if(!bQiXiao && (ChiHuRight&CHR_QUAN_QIU_REN).IsEmpty() &&\
		(ChiHuRight&CHR_PENGPENG_HU).IsEmpty())
	{
		if(m_GameLogic.IsKaDan(cbCardIndex, WeaveItem, cbWeaveCount))
		{
			if(!bSelfSendCard && !bZiYiSe && (ChiHuRight&CHR_QING_YI_SE).IsEmpty())
				return WIK_NULL;
			else
			{
				ChiHuRight |= CHR_SHU_FAN;
				return WIK_CHI_HU;
			}
		}
		else
		{
			ChiHuRight |= CHR_KA_DAN;
		}
	}*/

	//if (bSelfSendCard)
	//{
	//	cbChiHuKind = WIK_CHI_HU;
	//	ChiHuRight = CHR_SHU_FAN;
	//	ChiHuRight |= CHR_ZI_MO;
	//}

	return cbChiHuKind;
}