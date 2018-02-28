#ifndef CHESS_BASE_H
#define CHESS_BASE_H
#include "BASE/types.h"
#include <assert.h>
//////////////////////////////////////////////////////////
//�����궨��
//#pragma pack(1)

//�������
#define GAME_PLAYER					4									//��Ϸ����
enum GS_GAME_STATUS
{
	GS_MJ_FREE = 0,//����״̬
	GS_MJ_PLAY,//��Ϸ״̬
	GS_MJ_XIAOHU,//С��״̬
};

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
//��������
#define MAX_WEAVE					4									//������
#define MAX_INDEX					34									//�������
#define MAX_COUNT					14									//�����Ŀ
#define MAX_REPERTORY_108			108									//�����(����ͬ)
#define MAX_REPERTORY_112			112									//�����齫�����(����ͬ��)
#define MAX_REPERTORY_136			136									//�����(����ͬ���������з���)
//��������
#define INVALID_CHAIR				0xFFFF								//��Ч����
#define INVALID_TABLE				0xFFFF								//��Ч����

#define MAX_NIAO_CARD				 6									//���������


#define MAX_RIGHT_COUNT				1									//���ȨλU32����	

#define GAME_TYPE_108				0		//
#define GAME_TYPE_112				1		//
#define GAME_TYPE_136				2       //


#define MAKEU16(a, b)      ((U16)(((U8)(((U32)(a)) & 0xff)) | ((U16)((U8)(((U32)(b)) & 0xff))) << 8))
#define MAKEU32(a, b)      ((U32)(((U16)(((U32)(a)) & 0xffff)) | ((U32)((U16)(((U32)(b)) & 0xffff))) << 16))

#define ZeroMemory(dst,size) \
{\
	memset(dst, 0, size);\
	}\

#define ASSERT(x) assert((x))
#define VERIFY(x) if(!(x)) {assert((x)); }

#define CopyMemory(dst,src,size) \
	{\
		memcpy(dst,src,size);\
	}\
//////////////////////////////////////////////////////////////////////////

//�������
struct CMD_WeaveItem
{
	U8							cbWeaveKind;						//�������
	U8							cbCenterCard;						//�����˿�
	U8							cbPublicCard;						//������־
	U16							wProvideUser;						//��Ӧ�û�
};

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_OUT_CARD				101									//��������
#define SUB_S_SEND_CARD				102									//�����˿�
#define SUB_S_OPERATE_NOTIFY		104									//������ʾ
#define SUB_S_OPERATE_RESULT		105									//��������
#define SUB_S_GAME_END				106									//��Ϸ����
#define SUB_S_TRUSTEE				107									//�û��й�
#define SUB_S_CHI_HU				108									//
#define SUB_S_GANG_SCORE			110									//

//����ԭ��
#define GER_NORMAL					0x00								//�������
#define GER_DISMISS					0x01								//��Ϸ��ɢ
#define GER_USER_LEAVE				0x02								//�û��뿪
#define GER_NETWORK_ERROR			0x03								//�������

//��������
#define SCORE_TYPE_NULL				0x00								//��Ч����
#define SCORE_TYPE_WIN				0x01								//ʤ�ֻ���
#define SCORE_TYPE_LOSE				0x02								//��ֻ���
#define SCORE_TYPE_DRAW				0x03								//�;ֻ���
#define SCORE_TYPE_FLEE				0x04								//�Ӿֻ���
#define SCORE_TYPE_PRESENT			0x10								//���ͻ���
#define SCORE_TYPE_SERVICE			0x11								//�������

struct tagScoreInfo
{
	U8								cbType;								//��������
	S64								lScore;								//�û�����
	S64								lGrade;								//�û��ɼ�
	S64								lRevenue;							//��Ϸ˰��
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
	S64								lCellScore;							//�������
	U16								wBankerUser;						//ׯ���û�
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�
	U16								wCurrentUser;						//��ǰ�û�
	U64								nTableID;							//����ID
	S64								lGameScore[GAME_PLAYER];			//��Ϸ����
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��Ϸ����
	S64								lCellScore;									//��Ԫ����
	U16								wBankerUser;								//ׯ���û�
	U16								wCurrentUser;								//��ǰ�û�

	//״̬����
	U8								cbActionCard;								//�����˿�
	U8								cbActionMask;								//��������
	U8								cbLeftCardCount;							//ʣ����Ŀ
	bool							bTrustee[GAME_PLAYER];						//�Ƿ��й�
	U16								wWinOrder[GAME_PLAYER];						//

	//������Ϣ
	U16								wOutCardUser;								//�����û�
	U8								cbOutCardData;								//�����˿�
	U8								cbDiscardCount[GAME_PLAYER];				//������Ŀ
	U8								cbDiscardCard[GAME_PLAYER][60];				//������¼

	//�˿�����
	U8								cbCardCount;								//�˿���Ŀ
	U8								cbCardData[MAX_COUNT];						//�˿��б�
	U8								cbSendCardData;								//�����˿�

	//����˿�
	U8								cbWeaveCount[GAME_PLAYER];					//�����Ŀ
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//����˿�
	U64								nTableID;							        //����ID
	U16								nChairId;							        //slotID
	S64								lGameScore[GAME_PLAYER];			//��Ϸ����
	//U32								nTableNum;
	//std::string						m_strUserUrlImg[GAME_PLAYER];			//���΢��ͷ��
	//std::string						m_strUserNickName[GAME_PLAYER];			//���΢���ǳ�
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	U32								lSiceCount;									//���ӵ���
	U32								wBankerUser;								//ׯ���û�
	U32								wCurrentUser;								//��ǰ�û�
	U8								cbUserAction;								//�û�����
	U8								cbCardData[MAX_COUNT*GAME_PLAYER];			//�˿��б�
	U8								cbLeftCardCount;							//
	U8								cbXiaoHuTag;                           //С����� 0 ûС�� 1 ��С����

};

//��������
struct CMD_S_OutCard
{
	U16								wOutCardUser;						//�����û�
	U8								cbOutCardData;						//�����˿�
};

//�����˿�
struct CMD_S_SendCard
{
	U8								cbCardData;							//�˿�����
	U8								cbActionMask;						//��������
	U16								wCurrentUser;						//��ǰ�û�
	bool							bTail;								//ĩβ����
};


//������ʾ
struct CMD_S_OperateNotify
{
	U16								wResumeUser;						//��ԭ�û�
	U8								cbActionMask;						//��������
	U8								cbActionCard;						//�����˿�
};

//��������
struct CMD_S_OperateResult
{
	U16								wOperateUser;						//�����û�
	U16								wProvideUser;						//��Ӧ�û�
	U8								cbOperateCode;						//��������
	U8								cbOperateCard;						//�����˿�
};

//��Ϸ����
struct CMD_S_GameEnd
{
	U8								cbCardCount[GAME_PLAYER];			//
	U8								cbCardData[GAME_PLAYER][MAX_COUNT];	//
	//������Ϣ
	U16								wProvideUser[GAME_PLAYER];			//��Ӧ�û�
	U32								dwChiHuRight[GAME_PLAYER];			//��������
	U32								dwStartHuRight[GAME_PLAYER];		//���ֺ�������
	S64								lStartHuScore[GAME_PLAYER];			//���ֺ��Ʒ���

	//������Ϣ
	S64								lGameScore[GAME_PLAYER];			//��Ϸ����
	int								lGameTax[GAME_PLAYER];				//

	U16								wWinOrder[GAME_PLAYER];				//��������

	S64								lGangScore[GAME_PLAYER];//��ϸ�÷�
	U8								cbGenCount[GAME_PLAYER];			//
	U16								wLostFanShu[GAME_PLAYER][GAME_PLAYER];
	U16								wLeftUser;	//

	//����˿�
	U8								cbWeaveCount[GAME_PLAYER];					//�����Ŀ
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//����˿�


	U8								cbCardDataNiao[MAX_NIAO_CARD];	// ����
	U8								cbNiaoCount;	//���Ƹ���
	U8								cbNiaoPick;	//�������
};

//�û��й�
struct CMD_S_Trustee
{
	bool							bTrustee;							//�Ƿ��й�
	U32								wChairID;							//�й��û�
};

//
struct CMD_S_ChiHu
{
	U16								wChiHuUser;							//
	U16								wProviderUser;						//
	U8								cbChiHuCard;						//
	U8								cbCardCount;						//
	S64								lGameScore;							//
	U8								cbWinOrder;							//
	U8						        cbBaoPai;
};

//
struct CMD_S_GangScore
{
	U16								wChairId;							//
	U8								cbXiaYu;							//
	S64								lGangScore[GAME_PLAYER];			//
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_OUT_CARD				1									//��������
#define SUB_C_OPERATE_CARD			3									//�����˿�
#define SUB_C_TRUSTEE				4									//�û��й�
#define SUB_C_XIAOHU				5									//С��

//��������
struct CMD_C_OutCard
{
	U8								cbCardData;							//�˿�����
};

//��������
struct CMD_C_OperateCard
{
	U8								cbOperateCode;						//��������
	U8								cbOperateCard;						//�����˿�
};

//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;							//�Ƿ��й�	
};

//����С��
struct CMD_C_XiaoHu
{
	U8								cbOperateCode;						//��������
	U8								cbOperateCard;						//�����˿�
};


//////////////////////////////////////////////////////////////////////////
//#pragma pack()
#endif
