#ifndef __CHALLENGEBASE_H__
#define __CHALLENGEBASE_H__

#include "Base/BitStream.h"
#include "Common/PlayerStructEx.h"
#include <list>

#define MAX_BATTLE_REPORT 5
#define MAX_BATTLE_RANK   9999

//�������������������߼�����������
class ChallengeData
{
public:
    U32	SaveUID;    //��ɫ�汾��־  

    int quality;    //Ʒ����ֵ
    int nSkills;    //����������
    int nTalents;   //�츳Ͷ�����
    int level;      //��ɫ��ǰ�ȼ�
    int nPetSkills; //�����ۺ�ֵ
    int nPetLevel;  //���޵ȼ�

    void Pack(Base::BitStream* stream)
    {
        stream->writeBits(sizeof(ChallengeData)<<3,this);
    }

    void UnPack(Base::BitStream* stream)
    {
        stream->readBits(sizeof(ChallengeData)<<3, this);
    }
};

class BattleReport
{
public:
    BattleReport(void)
    {
        memset(this,0,sizeof(BattleReport));
    }

    bool self;          //�Ƿ��Լ��������ս
    U32  time;          //��սʱ��
    U32  targetId;      //����ս����ս����Ҷ���
    char targetName[32];
    int  oldRank;       //ǰ��2���������
    int  newRank;
};

class ChallengeMember
{
public:
    ChallengeMember(void)
    {
        playerId    = 0;
        rank        = 9999;
        wins        = 0;
        total       = 0;
        arenaPoints = 10;
        onlineLimit = 0;
        offlineLimit= 0;
        extraLimit  = 0;
        nChallengeBy= 0;
        nLosed      = 0;
        targetId    = 0;
        offlineMode = false;
        starter     = false;
        active      = false;
        curPoints   = 0;
    }

    U32 playerId;
    int rank;               //����
    U32 wins;               //��ʤ
    U32 total;              //�ܼ�
    int arenaPoints;        //��������
    int onlineLimit;        //������ս����
    int offlineLimit;       //������ս����
    int extraLimit;         //������ս����
    U32 nChallengeBy;       //���챻��ս����
    U32 nLosed;             //����ʧ�ܴ���

    std::list<BattleReport> reports;

    U32  targetId;
    bool offlineMode;
    bool starter;
    bool active;
    int  curPoints;

    ChallengeData data;

    
};

#endif//__CHALLENGEBASE_H__