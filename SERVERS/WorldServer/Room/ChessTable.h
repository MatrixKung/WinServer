#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once
#include <hash_map>
#include "ChessLogic.h"
#include "RoomBase.h"

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//Ч������
enum enEstimatKind
{
	EstimatKind_OutCard,			//����Ч��
	EstimatKind_GangCard,			//����Ч��
};

//���Ƶ÷�
struct tagGangScore
{
	U8		cbGangCount;							//�ܸ���
	S64		lScore[MAX_WEAVE][GAME_PLAYER];			//ÿ���ܵ÷�
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CChessTable : public ITable
{
public:
	static const U32 GAME_TYPE_ZZ_ZIMOHU			 = 0;	//ֻ����ģ��
	static const U32 GAME_TYPE_ZZ_QIANGGANGHU		 = 1;	//�����ܺ�
	static const U32 GAME_TYPE_ZZ_SUOHU				 = 2;	//����
	static const U32 GAME_TYPE_ZZ_CHANGHU			 = 3;	//����
	static const U32 GAME_TYPE_ZZ_DISBAND			 = 4;	//��ɢ��־
	static const U32 GAME_TYPE_ZZ_CHI				 = 5;	//�Եı�־
	static const U32 GAME_TYPE_ZZ_START				 = 6;	//��ʼ��־
	static const U32 NextMask						 = 7;

	//��Ϸ����
protected:
	U16								m_wBankerUser;							//ׯ���û�
	S64								m_lGameScore[GAME_PLAYER];				//��Ϸ�÷�
	U8								m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//�û��˿�
	//****************************************************************************//
	//m_cbCardIndex[i][j]��ֵ����������±����һ��1��������2���Դ�����
	//****************************************************************************//
	U8							    m_bDisband[GAME_PLAYER];				//��ɢ��־
	bool							m_bReady[GAME_PLAYER];					//׼����־
	bool							m_bTrustee[GAME_PLAYER];				//�Ƿ��й�
	tagGangScore					m_GangScore[GAME_PLAYER];				//
	U16								m_wLostFanShu[GAME_PLAYER][GAME_PLAYER];//


	//������Ϣ
protected:
	U16								m_wOutCardUser;							//�����û�
	U8								m_cbOutCardData;						//�����˿�
	U8								m_cbOutCardCount;						//������Ŀ
	U8								m_cbDiscardCount[GAME_PLAYER];			//������Ŀ
	U8								m_cbDiscardCard[GAME_PLAYER][55];		//������¼

	//������Ϣ
protected:
	U8								m_cbSendCardData;						//�����˿�
	U8								m_cbSendCardCount;						//������Ŀ
	U8								m_cbLeftCardCount;						//ʣ����Ŀ

	//���б���
protected:
	U16								m_wResumeUser;							//��ԭ�û�
	U16								m_wCurrentUser;							//��ǰ�û�
	U16								m_wProvideUser;							//��Ӧ�û�
	U8								m_cbProvideCard;						//��Ӧ�˿�

	//״̬����
protected:
	bool							m_bSendStatus;							//����״̬
	bool							m_bGangStatus;							//����״̬
	bool							m_bGangOutStatus;						//

	//�û�״̬
public:
	bool							m_bResponse[GAME_PLAYER];				//��Ӧ��־
	U8								m_cbUserAction[GAME_PLAYER];			//�û�����
	U8								m_cbOperateCard[GAME_PLAYER];			//�����˿�
	U8								m_cbPerformAction[GAME_PLAYER];			//ִ�ж���
	std::string						m_strUserUrlImg[GAME_PLAYER];			//���΢��ͷ��
	std::string						m_strUserNickName[GAME_PLAYER];			//���΢���ǳ�
	//��Ϸͳ��
	U8								m_cbChiHuNum[GAME_PLAYER][3];			 //����ܽ���
	//����˿�
protected:
	U8								m_cbWeaveItemCount[GAME_PLAYER];		 //�����Ŀ
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//����˿�

	//������Ϣ
protected:
	U8								m_cbChiHuCard;							 //�Ժ��˿�
	U32								m_dwChiHuKind[GAME_PLAYER];				 //�Ժ����
	CChiHuRight						m_ChiHuRight[GAME_PLAYER];				 //
	U16								m_wProvider[GAME_PLAYER];				 //

	//�������
protected:
	U32								m_GameStatus;							 //��Ϸ״̬
	CChessLogic						m_GameLogic;							 //��Ϸ�߼�
	U8								m_cbGameTypeIdex;						 //��Ϸ����
	U32								m_dwGameRuleIdex;						 //��Ϸ����
private:
	S64								m_TableScore[GAME_PLAYER];				 //�������
	S32								m_cbTableNum;							 //������
	U32								m_CreateTime;							 //����ʱ��
	//��������
public:
	//���캯��
	CChessTable();
	//��������
	virtual ~CChessTable();

public:
	//��ʼ��
	virtual bool InitTable() = 0;
	//��λ����
	virtual void ResetTable();
	virtual U32 GetMaxPlayers() {return GAME_PLAYER; }
	virtual bool HasDisband()	{return hasRule(GAME_TYPE_ZZ_DISBAND);}

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(U32 lBaseScore){};
	void SetGameStatus(U32 stauts){ m_GameStatus = stauts; }
	U32	 GetGameStatus()		{return m_GameStatus; }
	inline S32	GetTableNum()	{return m_cbTableNum; }
	inline void SetTableNum(S32 cbNum) {m_cbTableNum = cbNum; }
	void SetTableScore(S64 lScore);
	S64  getTableScore(U32 nSlot);
	void SetPrivateInfo(U8 bGameTypeIdex,U32 bGameRuleIdex);
	void AddGameRule(U32 nRule);
	void ClearGameRule(U32 nRule);
	bool ReadyGame(U32 nSlot);
	bool ClearDisbandGame();
	U8   DisbandGame(U32 nSlot, U8 nAgreeFlag);
	U8   AgreeDisband(U32 nSlot) {return m_bDisband[nSlot];}

	//��Ϸ��int i = 0; i < ��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(U16 wChairID,  U8 cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(U16 wChiarID, bool bSendSecret, U64 nTableId, U32 playerId);

	void Shuffle(U8* RepertoryCard,int nCardCount); //ϴ��
	//----һ���·������忴���ֵط���һ��������ʵ��
	virtual void GameStart();					//��Ϸ��ʼ
	virtual U8 DoDispatchCardData() = 0;
	virtual U8 AnalyseChiHuCard(const U8 cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], U8 cbWeaveCount, U8 cbCurrentCard, CChiHuRight &ChiHuRight);//������
	virtual U16 GetChiHuActionRank(const CChiHuRight & ChiHuRight);

	//��ʱ���¼�
	virtual void TimeProcess(U32 value);
public:
	bool IsUserVaildCard(U16 wChairID, U8 cbCardData);
	bool IsCurrentUser(U16 wChairID);
	//�û�����
	bool OnUserOutCard(U16 wChairID, U8 cbCardData);
	//�û�����
	bool OnUserOperateCard(U16 wChairID, U8 cbOperateCode, U8 cbOperateCard);
	//֪ͨ�ͻ��˱���
	template<class T>
	bool SendTableData(U16 wChairID, const char* msg, T& obj);
	template<class T>
	bool SendTableData(const char* msg, T& obj);
	bool hasRule(U8 cbRule);
	bool DisbandGameSendtoPlayer(CChessTable* pTable);

protected:
	//
	virtual void ProcessChiHuUser( U16 wChairId, bool bGiveUp);
	virtual void setSpecialType() {};
	//���Ͳ���
	bool SendOperateNotify();
	//�ɷ��˿�
	bool DispatchCardData(U16 wCurrentUser,bool bTail=false);
	//��Ӧ�ж�
	bool EstimateUserRespond(U16 wCenterUser, U8 cbCenterCard, enEstimatKind EstimatKind);
	//
	void FiltrateRight( U16 wChairId,CChiHuRight &chr );
};

//////////////////////////////////////////////////////////////////////////

#endif
