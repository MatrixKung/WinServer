#include "ChessLogic.h"
#include "math.h"
#include "Common/mRandom.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
bool		CChiHuRight::m_bInit = false;
U32		CChiHuRight::m_dwRightMask[MAX_RIGHT_COUNT];

//���캯��
CChiHuRight::CChiHuRight()
{
	ZeroMemory( m_dwRight,sizeof(m_dwRight) );

	if( !m_bInit )
	{
		m_bInit = true;
		for( U8 i = 0; i < CountArray(m_dwRightMask); i++ )
		{
			if( 0 == i )
				m_dwRightMask[i] = 0;
			else
				m_dwRightMask[i] = (U32(pow((float)2,(float)(i-1))))<<28;
		}
	}
}

//��ֵ������
CChiHuRight & CChiHuRight::operator = ( U32 dwRight )
{
	U32 dwOtherRight = 0;
	//��֤Ȩλ
	if( !IsValidRight( dwRight ) )
	{
		//��֤ȡ��Ȩλ
		ASSERT( IsValidRight( ~dwRight ) );
		if( !IsValidRight( ~dwRight ) ) return *this;
		dwRight = ~dwRight;
		dwOtherRight = MASK_CHI_HU_RIGHT;
	}

	for( U8 i = 0; i < CountArray(m_dwRightMask); i++ )
	{
		if( (dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000) )
			m_dwRight[i] = dwRight&MASK_CHI_HU_RIGHT;
		else m_dwRight[i] = dwOtherRight;
	}

	return *this;
}

//�����
CChiHuRight & CChiHuRight::operator &= ( U32 dwRight )
{
	bool bNavigate = false;
	//��֤Ȩλ
	if( !IsValidRight( dwRight ) )
	{
		//��֤ȡ��Ȩλ
		ASSERT( IsValidRight( ~dwRight ) );
		if( !IsValidRight( ~dwRight ) ) return *this;
		//����Ȩλ
		U32 dwHeadRight = (~dwRight)&0xF0000000;
		U32 dwTailRight = dwRight&MASK_CHI_HU_RIGHT;
		dwRight = dwHeadRight|dwTailRight;
		bNavigate = true;
	}

	for( U8 i = 0; i < CountArray(m_dwRightMask); i++ )
	{
		if( (dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000) )
		{
			m_dwRight[i] &= (dwRight&MASK_CHI_HU_RIGHT);
		}
		else if( !bNavigate )
			m_dwRight[i] = 0;
	}

	return *this;
}

//�����
CChiHuRight & CChiHuRight::operator |= ( U32 dwRight )
{
	//��֤Ȩλ
	if( !IsValidRight( dwRight ) ) return *this;

	for( U8 i = 0; i < CountArray(m_dwRightMask); i++ )
	{
		if( (dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000) )
			m_dwRight[i] |= (dwRight&MASK_CHI_HU_RIGHT);
	}

	return *this;
}

//��
CChiHuRight CChiHuRight::operator & ( U32 dwRight )
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//��
CChiHuRight CChiHuRight::operator & ( U32 dwRight ) const
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//��
CChiHuRight CChiHuRight::operator | ( U32 dwRight )
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//��
CChiHuRight CChiHuRight::operator | ( U32 dwRight ) const
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//�Ƿ�ȨλΪ��
bool CChiHuRight::IsEmpty()
{
	for( U8 i = 0; i < CountArray(m_dwRight); i++ )
		if( m_dwRight[i] ) return false;
	return true;
}

//����ȨλΪ��
void CChiHuRight::SetEmpty()
{
	ZeroMemory( m_dwRight,sizeof(m_dwRight) );
	return;
}

//��ȡȨλ��ֵ
U8 CChiHuRight::GetRightData( U32 dwRight[], U8 cbMaxCount )
{
	ASSERT( cbMaxCount >= CountArray(m_dwRight) );
	if( cbMaxCount < CountArray(m_dwRight) ) return 0;

	CopyMemory( dwRight,m_dwRight,sizeof(U32)*CountArray(m_dwRight) );
	return CountArray(m_dwRight);
}

//����Ȩλ��ֵ
bool CChiHuRight::SetRightData( const U32 dwRight[], U8 cbRightCount )
{
	ASSERT( cbRightCount <= CountArray(m_dwRight) );
	if( cbRightCount > CountArray(m_dwRight) ) return false;

	ZeroMemory( m_dwRight,sizeof(m_dwRight) );
	CopyMemory( m_dwRight,dwRight,sizeof(U32)*cbRightCount );
	
	return true;
}

//����λ�Ƿ���ȷ
bool CChiHuRight::IsValidRight( U32 dwRight )
{
	U32 dwRightHead = dwRight & 0xF0000000;
	for( U8 i = 0; i < CountArray(m_dwRightMask); i++ )
		if( m_dwRightMask[i] == dwRightHead ) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//��̬����

//�˿�����
const U8 CChessLogic::m_cbCardDataArray_108[MAX_REPERTORY_108]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
};

//�˿�����
const U8 CChessLogic::m_cbCardDataArray_112[MAX_REPERTORY_112]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��

	0x35,0x35,0x35,0x35,												//����
};

//�˿�����
const U8 CChessLogic::m_cbCardDataArray_136[MAX_REPERTORY_136]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//�����ϡ����������С�������
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//�����ϡ����������С�������
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//�����ϡ����������С�������
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//�����ϡ����������С�������
};
//////////////////////////////////////////////////////////////////////////

//���캯��
CChessLogic::CChessLogic()
{
	m_cbMagicIndex = MAX_INDEX;
}

//��������
CChessLogic::~CChessLogic()
{
}

//�����˿�
void CChessLogic::RandCardData(U8 cbCardData[], U8 cbMaxCount)
{
	//����׼��
#define RAND_CARD(CardDataArry)         \
	U8 cbCardDataTemp[CountArray(CardDataArry)];\
	CopyMemory(cbCardDataTemp,CardDataArry,sizeof(CardDataArry));\
	U8 cbRandCount=0,cbPosition=0;\
	do\
	{\
		cbPosition= gRandGen.randI(0, cbMaxCount-cbRandCount-1);\
		cbCardData[cbRandCount++]=cbCardDataTemp[cbPosition];\
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbMaxCount-cbRandCount];\
	} while (cbRandCount<cbMaxCount);\


	if ( cbMaxCount == MAX_REPERTORY_108)
	{
		RAND_CARD(m_cbCardDataArray_108);
		SetMagicIndex(MAX_INDEX);
	}
	else if( cbMaxCount == MAX_REPERTORY_112)
	{
		RAND_CARD(m_cbCardDataArray_112);
	}
	else if( cbMaxCount == MAX_REPERTORY_136)
	{
		RAND_CARD(m_cbCardDataArray_136);
	}

	return;
}

//ɾ���˿�
bool CChessLogic::RemoveCard(U8 cbCardIndex[MAX_INDEX], U8 cbRemoveCard)
{
	//Ч���˿�
	ASSERT(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)]>0);

	//ɾ���˿�
	U8 cbRemoveIndex=SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex]>0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//ʧ��Ч��
	ASSERT(false);

	return false;
}

//ɾ���˿�
bool CChessLogic::RemoveCard(U8 cbCardIndex[MAX_INDEX], const U8 cbRemoveCard[], U8 cbRemoveCount)
{
	//ɾ���˿�
	for (U8 i=0;i<cbRemoveCount;i++)
	{
		//Ч���˿�
		ASSERT(IsValidCard(cbRemoveCard[i]));
		ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard[i])]>0);

		//ɾ���˿�
		U8 cbRemoveIndex=SwitchToCardIndex(cbRemoveCard[i]);
		if (cbCardIndex[cbRemoveIndex]==0)
		{
			//�������
			ASSERT(false);

			//��ԭɾ��
			for (U8 j=0;j<i;j++) 
			{
				ASSERT(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//ɾ���˿�
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//ɾ���˿�
bool CChessLogic::RemoveCard(U8 cbCardData[], U8 cbCardCount, const U8 cbRemoveCard[], U8 cbRemoveCount)
{
	//��������
	ASSERT(cbCardCount<=14);
	ASSERT(cbRemoveCount<=cbCardCount);

	//�������
	U8 cbDeleteCount=0,cbTempCardData[14];
	if (cbCardCount>CountArray(cbTempCardData))
		return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (U8 i=0;i<cbRemoveCount;i++)
	{
		for (U8 j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}

	//�ɹ��ж�
	if (cbDeleteCount!=cbRemoveCount) 
	{
		ASSERT(false);
		return false;
	}

	//�����˿�
	U8 cbCardPos=0;
	for (U8 i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) 
			cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//��Ч�ж�
bool CChessLogic::IsValidCard(U8 cbCardData)
{
	U8 cbValue=(cbCardData&MASK_VALUE);
	U8 cbColor=(cbCardData&MASK_COLOR)>>4;
	return (((cbValue>=1)&&(cbValue<=9)&&(cbColor<=2))||((cbValue>=1)&&(cbValue<=7)&&(cbColor==3)));
}

//�˿���Ŀ
U8 CChessLogic::GetCardCount(const U8 cbCardIndex[MAX_INDEX])
{
	//��Ŀͳ��
	U8 cbCardCount=0;
	for (U8 i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbCardIndex[i];

	return cbCardCount;
}

//��ȡ���
U8 CChessLogic::GetWeaveCard(U8 cbWeaveKind, U8 cbCenterCard, U8 cbCardBuffer[4])
{
	//����˿�
	switch (cbWeaveKind)
	{
	case WIK_LEFT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_RIGHT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard-2;

			return 3;
		}
	case WIK_CENTER:	//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case WIK_PENG:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_GANG:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
		}
	default:
		{
			ASSERT(false);
		}
	}

	return 0;
}

//�����ȼ�
U8 CChessLogic::GetUserActionRank(U8 cbUserAction)
{
	//���Ƶȼ�
	if (cbUserAction&WIK_CHI_HU) { return 4; }

	//���Ƶȼ�
	if (cbUserAction&WIK_GANG) { return 3; }

	//���Ƶȼ�
	if (cbUserAction&WIK_PENG) { return 2; }

	//���Ƶȼ�
	if (cbUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT)) { return 1; }

	return 0;
}

//�����ж�
U8 CChessLogic::EstimateEatCard(const U8 cbCardIndex[MAX_INDEX], U8 cbCurrentCard)
{
	//����Ч��
	ASSERT(IsValidCard(cbCurrentCard));

	//�����ж�
	if ( cbCurrentCard>=0x31 || IsMagicCard(cbCurrentCard) ) 
		return WIK_NULL;

	//��������
	U8 cbExcursion[3]={0,1,2};
	U8 cbItemKind[3]={WIK_LEFT,WIK_CENTER,WIK_RIGHT};

	//�����ж�
	U8 cbEatKind=0,cbFirstIndex=0;
	U8 cbCurrentIndex=SwitchToCardIndex(cbCurrentCard);
	for (U8 i=0;i<CountArray(cbItemKind);i++)
	{
		U8 cbValueIndex=cbCurrentIndex%9;
		if ((cbValueIndex>=cbExcursion[i])&&((cbValueIndex-cbExcursion[i])<=6))
		{
			//�����ж�
			cbFirstIndex=cbCurrentIndex-cbExcursion[i];

			//���Ʋ��ܰ���������
			if( m_cbMagicIndex != MAX_INDEX &&
				m_cbMagicIndex >= cbFirstIndex && m_cbMagicIndex <= cbFirstIndex+2 ) continue;

			if ((cbCurrentIndex!=cbFirstIndex)&&(cbCardIndex[cbFirstIndex]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+1))&&(cbCardIndex[cbFirstIndex+1]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+2))&&(cbCardIndex[cbFirstIndex+2]==0))
				continue;

			//��������
			cbEatKind|=cbItemKind[i];
		}
	}

	return cbEatKind;
}

//�����ж�
U8 CChessLogic::EstimatePengCard(const U8 cbCardIndex[MAX_INDEX], U8 cbCurrentCard)
{
	//����Ч��
	ASSERT(IsValidCard(cbCurrentCard));

	//�����ж�
	if ( IsMagicCard(cbCurrentCard) ) 
		return WIK_NULL;

	//�����ж�
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//�����ж�
U8 CChessLogic::EstimateGangCard(const U8 cbCardIndex[MAX_INDEX], U8 cbCurrentCard)
{
	//����Ч��
	ASSERT(IsValidCard(cbCurrentCard));

	//�����ж�
	if ( IsMagicCard(cbCurrentCard) ) 
		return WIK_NULL;

	//�����ж�
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?WIK_GANG:WIK_NULL;
}

//���Ʒ���
U8 CChessLogic::AnalyseGangCard(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount, tagGangCardResult & GangCardResult)
{
	//���ñ���
	U8 cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//���ϸ���
	for (U8 i=0;i<MAX_INDEX;i++)
	{
		if( i == m_cbMagicIndex ) continue;
		if (cbCardIndex[i]==4)
		{
			cbActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//��ϸ���
	for (U8 i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].cbWeaveKind==WIK_PENG)
		{
			if (cbCardIndex[SwitchToCardIndex(WeaveItem[i].cbCenterCard)]==1)
			{
				cbActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return cbActionMask;
}

//�Ժ�����
U8 CChessLogic::AnalyseChiHuCard(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount, U8 cbCurrentCard, CChiHuRight &ChiHuRight)
{
	//��������
	U8 cbChiHuKind=WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	//���ñ���
	AnalyseItemArray.clear();
	ChiHuRight.SetEmpty();

	//�����˿�
	U8 cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//cbCurrentCardһ����Ϊ0			!!!!!!!!!
	ASSERT( cbCurrentCard != 0 );
	if( cbCurrentCard == 0 ) return WIK_NULL;

	/*
	//	���ⷬ��
	*/
	//��С����
	int nGenCount = 0;
	if( IsQiXiaoDui(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard,nGenCount) ) 
		ChiHuRight |= CHR_QI_XIAO_DUI;

	if( !ChiHuRight.IsEmpty() )
		cbChiHuKind = WIK_CHI_HU;

	//�����˿�
	if (cbCurrentCard!=0)
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	//�����˿�
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	//���Ʒ���
	if (AnalyseItemArray.size()>0)
	{
		//
		cbChiHuKind = WIK_CHI_HU;

		//���ͷ���
		while(AnalyseItemArray.size())
		{
			//��������
			tagAnalyseItem pAnalyseItem= (AnalyseItemArray.front());
			AnalyseItemArray.pop_front();
			/*
			//	�жϷ���
			*/
			//������
			if( IsPengPeng(&pAnalyseItem) ) 
				ChiHuRight |= CHR_PENGPENG_HU;
			//����
			//if( IsDaiYao(pAnalyseItem) )
			//	ChiHuRight |= CHR_DAI_YAO;
			//������
			if( IsJiangJiangHu(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard) )
				ChiHuRight |= CHR_JIANGJIANG_HU;
		}
	}

	//�ط�
	if( cbChiHuKind == WIK_CHI_HU && ChiHuRight.IsEmpty() )
		ChiHuRight |= CHR_SHU_FAN;

	if( cbChiHuKind == WIK_CHI_HU )
	{
		//��һɫ��
		if( IsQingYiSe(cbCardIndex,WeaveItem,cbWeaveCount,cbCurrentCard) )
			ChiHuRight |= CHR_QING_YI_SE;
	}

	return cbChiHuKind;
}

//���Ʒ���
U8 CChessLogic::AnalyseTingCard( const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount )
{
	return WIK_NULL;
}

//�Ƿ�����
bool CChessLogic::IsTingCard( const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount )
{
	//��������
	U8 cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	CChiHuRight chr;
	for( U8 i = 0; i < MAX_INDEX-7; i++ )
	{
		U8 cbCurrentCard = SwitchToCardData( i );
		if( WIK_CHI_HU == AnalyseChiHuCard( cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr ) )
			return true;
	}
	return false;
}

//�˿�ת��
U8 CChessLogic::SwitchToCardData(U8 cbCardIndex)
{
	ASSERT(cbCardIndex<34);
	return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
}

//�˿�ת��
U8 CChessLogic::SwitchToCardIndex(U8 cbCardData)
{
	ASSERT(IsValidCard(cbCardData));
	return ((cbCardData&MASK_COLOR)>>4)*9+(cbCardData&MASK_VALUE)-1;
}

//�˿�ת��
U8 CChessLogic::SwitchToCardData(const U8 cbCardIndex[MAX_INDEX], U8 cbCardData[MAX_COUNT])
{
	//ת���˿�
	U8 cbPosition=0;
	//����
	if( m_cbMagicIndex != MAX_INDEX )
	{
		for( U8 i = 0; i < cbCardIndex[m_cbMagicIndex]; i++ )
			cbCardData[cbPosition++] = SwitchToCardData(m_cbMagicIndex);
	}
	for (U8 i=0;i<MAX_INDEX;i++)
	{
		if( i == m_cbMagicIndex ) continue;
		if (cbCardIndex[i]!=0)
		{
			for (U8 j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(cbPosition<MAX_COUNT);
				cbCardData[cbPosition++]=SwitchToCardData(i);
			}
		}
	}

	return cbPosition;
}

//�˿�ת��
U8 CChessLogic::SwitchToCardIndex(const U8 cbCardData[], U8 cbCardCount, U8 cbCardIndex[MAX_INDEX])
{
	//���ñ���
	ZeroMemory(cbCardIndex,sizeof(U8)*MAX_INDEX);

	//ת���˿�
	for (U8 i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}

//�����˿�
bool CChessLogic::AnalyseCard(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	//������Ŀ
	U8 cbCardCount=GetCardCount(cbCardIndex);

	//Ч����Ŀ
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0))
		return false;

	//��������
	U8 cbKindItemCount=0;
	tagKindItem KindItem[27*2+7+14];
	ZeroMemory(KindItem,sizeof(KindItem));

	//�����ж�
	U8 cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==4);

	//�����ж�
	if (cbLessKindItem==0)
	{
		//Ч�����
		ASSERT((cbCardCount==2)&&(cbWeaveCount==4));

		//�����ж�
		for (U8 i=0;i<MAX_INDEX;i++)
		{
			if (cbCardIndex[i]==2 || 
				( m_cbMagicIndex != MAX_INDEX && i != m_cbMagicIndex && cbCardIndex[m_cbMagicIndex]+cbCardIndex[i]==2 ) )
			{
				//��������
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//���ý��
				for (U8 j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.cbWeaveKind[j]=WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
					GetWeaveCard( WeaveItem[j].cbWeaveKind,WeaveItem[j].cbCenterCard,AnalyseItem.cbCardData[j] );
				}
				AnalyseItem.cbCardEye=SwitchToCardData(i);
				if( cbCardIndex[i] < 2 || i == m_cbMagicIndex )
					AnalyseItem.bMagicEye = true;
				else AnalyseItem.bMagicEye = false;

				//������
				AnalyseItemArray.push_back(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//��ַ���
	U8 cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));
	U8 cbMagicCardCount = 0;
	if( m_cbMagicIndex != MAX_INDEX )
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex];
		if( cbMagicCardIndex[m_cbMagicIndex] ) cbMagicCardIndex[m_cbMagicIndex] = 1;		//��С�������
	}
	if (cbCardCount>=3)
	{
		for (U8 i=0;i<MAX_INDEX;i++)
		{
			//ͬ���ж�
			if (cbMagicCardIndex[i]+cbMagicCardCount>=3)
			{
				ASSERT( cbKindItemCount < CountArray(KindItem) );
				KindItem[cbKindItemCount].cbCardIndex[0]=i;
				KindItem[cbKindItemCount].cbCardIndex[1]=i;
				KindItem[cbKindItemCount].cbCardIndex[2]=i;
				KindItem[cbKindItemCount].cbWeaveKind=WIK_PENG;
				KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
				KindItem[cbKindItemCount].cbValidIndex[0] = cbMagicCardIndex[i]>0?i:m_cbMagicIndex;
				KindItem[cbKindItemCount].cbValidIndex[1] = cbMagicCardIndex[i]>1?i:m_cbMagicIndex;
				KindItem[cbKindItemCount].cbValidIndex[2] = cbMagicCardIndex[i]>2?i:m_cbMagicIndex;
				cbKindItemCount++;
				if(cbMagicCardIndex[i]+cbMagicCardCount>=6)
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					KindItem[cbKindItemCount].cbCardIndex[0]=i;
					KindItem[cbKindItemCount].cbCardIndex[1]=i;
					KindItem[cbKindItemCount].cbCardIndex[2]=i;
					KindItem[cbKindItemCount].cbWeaveKind=WIK_PENG;
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
					KindItem[cbKindItemCount].cbValidIndex[0] = cbMagicCardIndex[i]>3?i:m_cbMagicIndex;
					KindItem[cbKindItemCount].cbValidIndex[1] = m_cbMagicIndex;
					KindItem[cbKindItemCount].cbValidIndex[2] = m_cbMagicIndex;
					cbKindItemCount++;
				}
			}

			//�����ж�
			if ((i<(MAX_INDEX-9))&&((i%9)<7))
			{
				//ֻҪ������������3��˳���������������ڵ���3,��������
				if( cbMagicCardCount+cbMagicCardIndex[i]+cbMagicCardIndex[i+1]+cbMagicCardIndex[i+2] >= 3 )
				{
					U8 cbIndex[3] = { i==m_cbMagicIndex?0:cbMagicCardIndex[i],(i+1)==m_cbMagicIndex?0:cbMagicCardIndex[i+1],
						(i+2)==m_cbMagicIndex?0:cbMagicCardIndex[i+2] };
					int nMagicCountTemp = cbMagicCardCount;
					U8 cbValidIndex[3];
					while( nMagicCountTemp+cbIndex[0]+cbIndex[1]+cbIndex[2] >= 3 )
					{
						for( U8 j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
								cbValidIndex[j] = i+j;
							}
							else 
							{
								nMagicCountTemp--;
								cbValidIndex[j] = m_cbMagicIndex;
							}
						}
						if( nMagicCountTemp >= 0 )
						{
							ASSERT( cbKindItemCount < CountArray(KindItem) );
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
							KindItem[cbKindItemCount].cbWeaveKind=WIK_LEFT;
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							CopyMemory( KindItem[cbKindItemCount].cbValidIndex,cbValidIndex,sizeof(cbValidIndex) );
							cbKindItemCount++;
						}
						else break;
					}
				}
			}
		}
	}

	//��Ϸ���
	if (cbKindItemCount>=cbLessKindItem)
	{
		//��������
		U8 cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//��������
		U8 cbIndex[4]={0,1,2,3};
		tagKindItem * pKindItem[4];
		ZeroMemory(&pKindItem,sizeof(pKindItem));

		//��ʼ���
		do
		{
			//���ñ���
			CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
			for (U8 i=0;i<cbLessKindItem;i++)
				pKindItem[i]=&KindItem[cbIndex[i]];

			//�����ж�
			bool bEnoughCard=true;
			for (U8 i=0;i<cbLessKindItem*3;i++)
			{
				//�����ж�
				U8 cbCardIndex=pKindItem[i/3]->cbValidIndex[i%3]; 
				if (cbCardIndexTemp[cbCardIndex]==0)
				{
					bEnoughCard=false;
					break;
				}
				else 
					cbCardIndexTemp[cbCardIndex]--;
			}

			//�����ж�
			if (bEnoughCard==true)
			{
				//�����ж�
				U8 cbCardEye=0;
				bool bMagicEye = false;
				for (U8 i=0;i<MAX_INDEX;i++)
				{
					if (cbCardIndexTemp[i]==2)
					{
						cbCardEye=SwitchToCardData(i);
						if( i == m_cbMagicIndex ) bMagicEye = true;
						break;
					}
					else if( i!=m_cbMagicIndex && 
						m_cbMagicIndex != MAX_INDEX && cbCardIndexTemp[i]+cbCardIndexTemp[m_cbMagicIndex]==2 )
					{
						cbCardEye = SwitchToCardData(i);
						bMagicEye = true;
					}
				}

				//�������
				if (cbCardEye!=0)
				{
					//��������
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

					//�������
					for (U8 i=0;i<cbWeaveCount;i++)
					{
						AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
						AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
						GetWeaveCard( WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,
							AnalyseItem.cbCardData[i] );
					}

					//��������
					for (U8 i=0;i<cbLessKindItem;i++) 
					{
						AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
						AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
						AnalyseItem.cbCardData[cbWeaveCount+i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
						AnalyseItem.cbCardData[cbWeaveCount+i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
						AnalyseItem.cbCardData[cbWeaveCount+i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
					}

					//��������
					AnalyseItem.cbCardEye=cbCardEye;
					AnalyseItem.bMagicEye = bMagicEye;

					//������
					AnalyseItemArray.push_back(AnalyseItem);
				}
			}

			//��������
			if (cbIndex[cbLessKindItem-1]==(cbKindItemCount-1))
			{
				U8 i=cbLessKindItem-1;
				for (;i>0;i--)
				{
					if ((cbIndex[i-1]+1)!=cbIndex[i])
					{
						U8 cbNewIndex=cbIndex[i-1];
						for (U8 j=(i-1);j<cbLessKindItem;j++) 
							cbIndex[j]=cbNewIndex+j-i+2;
						break;
					}
				}
				if (i==0)
					break;
			}
			else
				cbIndex[cbLessKindItem-1]++;
		} while (true);

	}

	return (AnalyseItemArray.size()>0);
}

//����
bool CChessLogic::IsMagicCard( U8 cbCardData )
{
	if( m_cbMagicIndex != MAX_INDEX )
		return SwitchToCardIndex(cbCardData) == m_cbMagicIndex;
	return false;
}

//����,������ֵ����
bool CChessLogic::SortCardList( U8 cbCardData[MAX_COUNT], U8 cbCardCount )
{
	//��Ŀ����
	if (cbCardCount==0||cbCardCount>MAX_COUNT) return false;

	//�������
	bool bSorted=true;
	U8 cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (U8 i=0;i<cbLast;i++)
		{
			if (cbCardData[i]>cbCardData[i+1])
			{
				//���ñ�־
				bSorted=false;

				//�˿�����
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return true;
}

/*
// ������������
*/

//�����
bool CChessLogic::IsPengPeng( const tagAnalyseItem *pAnalyseItem )
{
	for( U8 i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
			return false;
	}
	return true;
}

//��һɫ��
bool CChessLogic::IsQingYiSe(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const U8 cbItemCount,const U8 cbCurrentCard)
{
	//�����ж�
	U8 cbCardColor=0xFF;

	for (U8 i=0;i<MAX_INDEX;i++)
	{
		if(i==m_cbMagicIndex) continue;
		if (cbCardIndex[i]!=0)
		{
			//��ɫ�ж�
			if (cbCardColor!=0xFF)
				return false;

			//���û�ɫ
			cbCardColor=(SwitchToCardData(i)&MASK_COLOR);

			//��������
			i=(i/9+1)*9-1;
		}
	}

	//����������
	if((cbCardColor == 0x30))
	{
		return false;
	}

	//�������ֻ������
	if( cbCardColor == 0xFF )
	{
		ASSERT( m_cbMagicIndex != MAX_INDEX && cbCardIndex[m_cbMagicIndex] > 0 );
		//������
		ASSERT( cbItemCount > 0 );
		cbCardColor = WeaveItem[0].cbCenterCard&MASK_COLOR;
	}

	if((cbCurrentCard&MASK_COLOR)!=cbCardColor && !IsMagicCard(cbCurrentCard) ) return false;

	//����ж�
	for (U8 i=0;i<cbItemCount;i++)
	{
		U8 cbCenterCard=WeaveItem[i].cbCenterCard;
		if ((cbCenterCard&MASK_COLOR)!=cbCardColor)	return false;
	}

	//����������
	if((cbCardColor == 0x30))
	{
		return false;
	}

	return true;
}

//��һɫ��
bool CChessLogic::IsZiYiSe(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const U8 cbItemCount,const U8 cbCurrentCard)
{
	//�����ж�
	U8 cbCardColor=0xFF;

	for (U8 i=0;i<MAX_INDEX;i++)
	{
		if(i==m_cbMagicIndex) continue;
		if (cbCardIndex[i]!=0)
		{
			//��ɫ�ж�
			if (cbCardColor!=0xFF)
				return false;

			//���û�ɫ
			cbCardColor=(SwitchToCardData(i)&MASK_COLOR);

			//��������
			i=(i/9+1)*9-1;
		}
	}

	//����
	if(cbCardColor != 0x30)
	{
		return false;
	}

	//�������ֻ������
	if( cbCardColor == 0xFF )
	{
		ASSERT( m_cbMagicIndex != MAX_INDEX && cbCardIndex[m_cbMagicIndex] > 0 );
		//������
		ASSERT( cbItemCount > 0 );
		cbCardColor = WeaveItem[0].cbCenterCard&MASK_COLOR;
	}

	if((cbCurrentCard&MASK_COLOR)!=cbCardColor && !IsMagicCard(cbCurrentCard) ) return false;

	//����ж�
	for (U8 i=0;i<cbItemCount;i++)
	{
		U8 cbCenterCard=WeaveItem[i].cbCenterCard;
		if ((cbCenterCard&MASK_COLOR)!=cbCardColor)	return false;
	}

	//����
	if(cbCardColor != 0x30)
	{
		return false;
	}

	return true;
}

//��С����
bool CChessLogic::IsQiXiaoDui(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const U8 cbWeaveCount,const U8 cbCurrentCard,int& nGenCount)
{
	//����ж�
	if (cbWeaveCount!=0) return false;

	//������Ŀ
	U8 cbReplaceCount = 0;
	nGenCount = 0;

	//��ʱ����
	U8 cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//��������
	U8 cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
	cbCardIndexTemp[cbCurrentIndex]++;

	//���㵥��
	for (U8 i=0;i<MAX_INDEX;i++)
	{
		U8 cbCardCount=cbCardIndexTemp[i];

		//���ƹ���
		if( i == m_cbMagicIndex ) continue;

		//����ͳ��
		if( cbCardCount == 1 || cbCardCount == 3 ) 	cbReplaceCount++;

		if (cbCardCount == 4 )
		{
			nGenCount++;
		}
	}
	
	//���Ʋ���
	if( m_cbMagicIndex != MAX_INDEX && cbReplaceCount > cbCardIndexTemp[m_cbMagicIndex] ||
		m_cbMagicIndex == MAX_INDEX && cbReplaceCount > 0 )
		return false;

	return true;

}

//����
bool CChessLogic::IsKaDan( const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount )
{
	//��������
	U32 nCurNum = 0;
	U8 cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	CChiHuRight chr;
	for( U8 i = 0; i < MAX_INDEX; i++ )
	{
		U8 cbCurrentCard = SwitchToCardData( i );
		if( WIK_CHI_HU == AnalyseChiHuCard( cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr ) )
			nCurNum++;

		if(nCurNum >= 2)
			return true;
	}
	return false;
}

//������
bool CChessLogic::IsJiangJiangHu(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[],const  U8 cbWeaveCount,const U8 cbCurrentCard)
{
	//������Ŀ
	U8 cbReplaceCount = 0;

	//��ʱ����
	U8 cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//��������
	U8 cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
	cbCardIndexTemp[cbCurrentIndex]++;

	//���㵥��
	for (U8 i=0;i<MAX_INDEX;i++)
	{
		U8 cbValue =(SwitchToCardData(i) & MASK_VALUE);

		//����ͳ��
		if( cbValue != 2 && cbValue != 5 && cbValue == 8 )
		{
			return false;
		}
	}
	return true;
}

bool CChessLogic::IsDanDiao(const U8 cbCardIndex[MAX_INDEX],const U8 cbCurrentCard)
{
	//������Ŀ
	U8 cbReplaceCount = 0;

	//��ʱ����
	U8 cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//��������
	U8 cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
	cbCardIndexTemp[cbCurrentIndex]++;

	//���㵥��
	int nTaltal = 0;
	bool bDuizi = false;
	for (U8 i=0;i<MAX_INDEX;i++)
	{
		U8 cbCardCount=cbCardIndexTemp[i];

		//���ƹ���
		if( i == m_cbMagicIndex ) continue;

		//����ͳ��
		if( cbCardCount == 2) 	
		{
			bDuizi = true;
		}
		nTaltal += cbCardCount;
	}

	if (bDuizi && nTaltal == 2)
	{
		return true;
	}
	return false;
}

//����
bool CChessLogic::IsDaiYao( const tagAnalyseItem *pAnalyseItem )
{
	//�������
	U8 cbCardValue = pAnalyseItem->cbCardEye&MASK_VALUE;
	if( cbCardValue != 1 && cbCardValue != 9 ) return false;

	for( U8 i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			U8 j = 0;
			for(; j < 3; j++ )
			{
				cbCardValue = pAnalyseItem->cbCardData[i][j]&MASK_VALUE;
				if( cbCardValue == 1 || cbCardValue == 9 ) break;
			}
			if( j == 3 ) return false;
		}
		else
		{
			cbCardValue = pAnalyseItem->cbCenterCard[i]&MASK_VALUE;
			if( cbCardValue != 1 && cbCardValue != 9 ) return false;
		}
	}
	return true;
}

//����
bool CChessLogic::IsJiangDui( const tagAnalyseItem *pAnalyseItem )
{
	//�Ƿ�����
	if( !IsPengPeng(pAnalyseItem) ) return false;

	//�������
	U8 cbCardValue = pAnalyseItem->cbCardEye&MASK_VALUE;
	if( cbCardValue != 2 && cbCardValue != 5 && cbCardValue != 8 ) return false;

	for( U8 i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++ )
	{
		if( pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			U8 j = 0;
			for(; j < 3; j++ )
			{
				cbCardValue = pAnalyseItem->cbCardData[i][j]&MASK_VALUE;
				if( cbCardValue == 2 || cbCardValue == 5 || cbCardValue == 8 ) break;
			}
			if( j == 3 ) return false;
		}
		else
		{
			cbCardValue = pAnalyseItem->cbCenterCard[i]&MASK_VALUE;
			if( cbCardValue != 2 && cbCardValue != 5 && cbCardValue != 8 ) return false;
		}
	}
	return true;
}

//�Ƿ���
bool CChessLogic::IsHuaZhu( const U8 cbCardIndex[], const tagWeaveItem WeaveItem[], U8 cbWeaveCount )
{
	U8 cbColor[3] = { 0,0,0 };
	for( U8 i = 0; i < MAX_INDEX; i++ )
	{
		if( cbCardIndex[i] > 0 )
		{
			U8 cbCardColor = SwitchToCardData(i)&MASK_COLOR;
			cbColor[cbCardColor>>4]++;

			i = (i/9+1)*9-1;
		}
	}
	for( U8 i = 0; i < cbWeaveCount; i++ )
	{
		U8 cbCardColor = WeaveItem[i].cbCenterCard&MASK_COLOR;
		cbColor[cbCardColor>>4]++;
	}
	//ȱһ�žͲ��ǻ���
	for( U8 i = 0; i < CountArray(cbColor); i++ )
		if( cbColor[i] == 0 ) return false;

	return true;
}


U8 CChessLogic::GetPickNiaoCount(U8 cbCardData[MAX_NIAO_CARD],U8 cbCardNum)
{
	U8 cbPickNum = 0;
	for (int i=0;i<cbCardNum;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));

		U8 nValue = cbCardData[i]&MASK_VALUE;
		if (nValue == 1 || nValue == 5 || nValue == 9)
		{
			cbPickNum++;
		}
		
	}
	return cbPickNum;
}



//////////////////////////////////////////////////////////////////////////
