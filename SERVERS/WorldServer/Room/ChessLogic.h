#ifndef CHESS_LOGIC_H
#define CHESS_LOGIC_H
#include "ChessBase.h"
#include "BASE/types.h"
#include <deque>
#pragma once
//#pragma pack(1)
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//�߼�����

#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////
//��������

//������־
#define WIK_NULL					0x00								//û������
#define WIK_LEFT					0x01								//�������
#define WIK_CENTER					0x02								//�г�����
#define WIK_RIGHT					0x04								//�ҳ�����
#define WIK_PENG					0x08								//��������
#define WIK_GANG					0x10								//��������
#define WIK_XIAO_HU					0x20//С��							//��������
#define WIK_CHI_HU					0x40								//�Ժ�����
#define WIK_ZI_MO					0x80								//����

//////////////////////////////////////////////////////////////////////////
//���ƶ���

//����
#define CHK_NULL					0x00										//�Ǻ�����
#define CHK_CHI_HU					0x01										//������

// ���
#define CHR_PENGPENG_HU				0x00000001									//������
#define CHR_JIANGJIANG_HU			0x00000002									//������
#define CHR_QING_YI_SE				0x00000004									//��һɫ
#define CHR_HAI_DI_LAO				0x00000008									//������
#define CHR_HAI_DI_PAO				0x00000010									//������
#define CHR_QI_XIAO_DUI				0x00000020									//��С��
#define CHR_HAOHUA_QI_XIAO_DUI		0x00000040									//������С��
#define CHR_GANG_KAI				0x00000080									//���Ͽ���
#define CHR_QIANG_GANG_HU			0x00000100									//���ܺ�
#define CHR_GANG_SHANG_PAO			0x00000200									//������(���Ͽ���)
#define CHR_QUAN_QIU_REN			0x00000400									//ȫ����
#define CHR_DHAOHUA_QI_XIAO_DUI		0x00000800									//˫�����߶�
#define CHR_THAOHUA_QI_XIAO_DUI		0x00001000									//�������߶�
#define CHR_KA_DAN					0x00002000									//����
#define CHR_ZI_YI_SE				0x00004000									//��һɫ

//С��
#define CHR_XIAO_DA_SI_XI			0x00004000									//����ϲ
#define CHR_XIAO_BAN_BAN_HU			0x00008000									//����
#define CHR_XIAO_QUE_YI_SE			0x00010000									//ȱһɫ
#define CHR_XIAO_LIU_LIU_SHUN		0x00020000									//����˳


#define CHR_ZI_MO					0x01000000									//����
#define CHR_SHU_FAN					0x02000000									//�ط�
//////////////////////////////////////////////////////////////////////////

#define ZI_PAI_COUNT	7

//��������
struct tagKindItem
{
	U8								cbWeaveKind;						//�������
	U8								cbCenterCard;						//�����˿�
	U8								cbCardIndex[3];						//�˿�����
	U8								cbValidIndex[3];					//ʵ���˿�����
};

//�������
struct tagWeaveItem
{
	U8								cbWeaveKind;						//�������
	U8								cbCenterCard;						//�����˿�
	U8								cbPublicCard;						//������־
	U16								wProvideUser;						//��Ӧ�û�
};

//���ƽ��
struct tagGangCardResult
{
	U8								cbCardCount;						//�˿���Ŀ
	U8								cbCardData[4];						//�˿�����
};

//��������
struct tagAnalyseItem
{
	U8								cbCardEye;							//�����˿�
	bool                            bMagicEye;                          //�����Ƿ�������
	U8								cbWeaveKind[4];						//�������
	U8								cbCenterCard[4];					//�����˿�
	U8								cbCardData[4][4];                   //ʵ���˿�
};

//////////////////////////////////////////////////////////////////////////

#define MASK_CHI_HU_RIGHT			0x0fffffff

/*
//	Ȩλ�ࡣ
//  ע�⣬�ڲ�����λʱ���ֻ��������Ȩλ.����
//  CChiHuRight chr;
//  chr |= (chr_zi_mo|chr_peng_peng)������������޶���ġ�
//  ֻ�ܵ�������:
//  chr |= chr_zi_mo;
//  chr |= chr_peng_peng;
*/
class CChiHuRight
{	
	//��̬����
private:
	static bool						m_bInit;
	static U32						m_dwRightMask[MAX_RIGHT_COUNT];

	//Ȩλ����
private:
	U32								m_dwRight[MAX_RIGHT_COUNT];

public:
	//���캯��
	CChiHuRight();

	//���������
public:
	//��ֵ��
	CChiHuRight & operator = ( U32 dwRight );

	//�����
	CChiHuRight & operator &= ( U32 dwRight );
	//�����
	CChiHuRight & operator |= ( U32 dwRight );

	//��
	CChiHuRight operator & ( U32 dwRight );
	CChiHuRight operator & ( U32 dwRight ) const;

	//��
	CChiHuRight operator | ( U32 dwRight );
	CChiHuRight operator | ( U32 dwRight ) const;

	//���ܺ���
public:
	//�Ƿ�ȨλΪ��
	bool IsEmpty();

	//����ȨλΪ��
	void SetEmpty();

	//��ȡȨλ��ֵ
	U8 GetRightData( U32 dwRight[], U8 cbMaxCount );

	//����Ȩλ��ֵ
	bool SetRightData( const U32 dwRight[], U8 cbRightCount );

private:
	//���Ȩλ�Ƿ���ȷ
	bool IsValidRight( U32 dwRight );
};


//////////////////////////////////////////////////////////////////////////

//����˵��
typedef std::deque<tagAnalyseItem> CAnalyseItemArray;

//��Ϸ�߼���
class CChessLogic
{
	//��������
protected:
	static const U8				m_cbCardDataArray_108[MAX_REPERTORY_108];	    //�˿�����
	static const U8				m_cbCardDataArray_112[MAX_REPERTORY_112];	//�˿�����
	static const U8				m_cbCardDataArray_136[MAX_REPERTORY_136];	//�˿�����
	U8							m_cbMagicIndex;						//��������

	//��������
public:
	//���캯��
	CChessLogic();
	//��������
	virtual ~CChessLogic();

	//���ƺ���
public:
	//�����˿�
	void RandCardData(U8 cbCardData[], U8 cbMaxCount);
	//ɾ���˿�
	bool RemoveCard(U8 cbCardIndex[MAX_INDEX], U8 cbRemoveCard);
	//ɾ���˿�
	bool RemoveCard(U8 cbCardIndex[MAX_INDEX], const U8 cbRemoveCard[], U8 cbRemoveCount);
	//ɾ���˿�
	bool RemoveCard(U8 cbCardData[], U8 cbCardCount, const U8 cbRemoveCard[], U8 cbRemoveCount);
	//��������
	void SetMagicIndex( U8 cbMagicIndex ) { m_cbMagicIndex = cbMagicIndex; }
	//����
	bool IsMagicCard( U8 cbCardData );

	//��������
public:
	//��Ч�ж�
	bool IsValidCard(U8 cbCardData);
	//�˿���Ŀ
	U8 GetCardCount(const U8 cbCardIndex[MAX_INDEX]);
	//����˿�
	U8 GetWeaveCard(U8 cbWeaveKind, U8 cbCenterCard, U8 cbCardBuffer[4]);

	//�ȼ�����
public:
	//�����ȼ�
	U8 GetUserActionRank(U8 cbUserAction);

	//�����ж�
public:
	//�����ж�
	U8 EstimateEatCard(const  U8 cbCardIndex[MAX_INDEX], U8 cbCurrentCard);
	//�����ж�
	U8 EstimatePengCard(const U8 cbCardIndex[MAX_INDEX], U8 cbCurrentCard);
	//�����ж�
	U8 EstimateGangCard(const U8 cbCardIndex[MAX_INDEX], U8 cbCurrentCard);

	//�����ж�
public:
	//���Ʒ���
	U8 AnalyseGangCard(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount, tagGangCardResult & GangCardResult);
	//�Ժ�����
	U8 AnalyseChiHuCard(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount, U8 cbCurrentCard, CChiHuRight &ChiHuRight);
	//���Ʒ���
	U8 AnalyseTingCard( const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount );
	//�Ƿ�����
	bool IsTingCard( const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount );
	//�Ƿ���
	bool IsHuaZhu( const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount );

	//ת������
public:
	//�˿�ת��
	U8 SwitchToCardData(U8 cbCardIndex);
	//�˿�ת��
	U8 SwitchToCardIndex(U8 cbCardData);
	//�˿�ת��
	U8 SwitchToCardData(const U8 cbCardIndex[MAX_INDEX], U8 cbCardData[MAX_COUNT]);
	//�˿�ת��
	U8 SwitchToCardIndex(const U8 cbCardData[], U8 cbCardCount, U8 cbCardIndex[MAX_INDEX]);

	//��������
public:
	//�����
	bool IsPengPeng( const tagAnalyseItem *pAnalyseItem );
	//��һɫ��
	bool IsQingYiSe(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const U8 cbItemCount,const U8 cbCurrentCard);
	//��һɫ��
	bool IsZiYiSe(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const U8 cbItemCount,const U8 cbCurrentCard);
	//��С����
	bool IsQiXiaoDui(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[],const  U8 cbWeaveCount,const U8 cbCurrentCard,int& nGenCount);
	//����
	bool IsKaDan( const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount );

	//����
	bool IsDaiYao( const tagAnalyseItem *pAnalyseItem );
	//����
	bool IsJiangDui( const tagAnalyseItem *pAnalyseItem );
	//������
	bool IsJiangJiangHu(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[],const  U8 cbWeaveCount,const U8 cbCurrentCard);
	bool IsDanDiao(const U8 cbCardIndex[MAX_INDEX],const U8 cbCurrentCard);

	//�����˿�
	bool AnalyseCard(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbItemCount, CAnalyseItemArray & AnalyseItemArray);
	//����,������ֵ����
	bool SortCardList( U8 cbCardData[MAX_COUNT], U8 cbCardCount );

	U8 GetPickNiaoCount( U8 cbCardData[MAX_NIAO_CARD],U8 cbCardNum);
};

//////////////////////////////////////////////////////////////////////////
//#pragma pack()
#endif
