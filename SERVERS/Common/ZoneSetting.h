#ifndef __ZONESETTING_H__
#define __ZONESETTING_H__

#include "Base/BitStream.h"
#include "Common/PacketType.h"

#define MAX_RESERVE_FIELDS 6

enum RESERVE
{
	DEFAULT1 = 0,						//Ŀǰ������һ����
	DEFAULT2,							//Ŀǰ��vip���ɻ����
	DEFAULT3,						//vip�ʻ��
	DEFAULT4,        //����Һ�vip�ʻ��
	DEFAULT5,
	DEFAULT6,
	DEFAULT7,
	DEFAULT8,
	MAX_WORLDSETS
};

struct stZoneSetting
{
    stZoneSetting(void)
    {
        memset(this,0,sizeof(stZoneSetting));
    }
    
    bool isApplied;
    bool isChanged;
    
	U8	lineid;					//��ID
	U32 zoneid;					//��ͼ���
	S32 exprate;				//���鱶��
	S32 droprate;				//����
    __time64_t timeStart;       //��ʼʱ��
    __time64_t timeEnd;         //����ʱ��
	S32 reserve[MAX_RESERVE_FIELDS];	//�����ֶ�5��
};

//ȫ����Ϸ�߼�����������
struct stWorldSetting
{
	U8  Years5Assure;			//"��������"
	U8  GodGivePath;			//"����ָ·�"
	U32 TopLevel;				//ȫ�������ߵȼ�
	U32 TopDisaster;			//�����ߵȼ�
	U32	ActivityFlag;			//��Ӫ��ƵĻ��־,Ŀǰ����"ͬһ�������½��������"�
	U32 MaxStalls;				//��ǰ��������
	U32 TotalFlourish;			//��ҵָ��(�����ܷ��ٶ�)
	S32 reserve	[MAX_WORLDSETS];
};

#endif//__ZONESETTING_H__
