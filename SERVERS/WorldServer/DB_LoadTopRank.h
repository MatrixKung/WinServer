#ifndef _DBLOADTOPRANK_H_
#define _DBLOADTOPRANK_H_

#include <vector>
#include <hash_map>
#include "CommLib/ThreadPool.h"
#include "DBLayer/Data/TBLTopRank.h"
#include "DBLayer/Data/TBLPlayer.h"

class DB_LoadTopRank : public ThreadBase
{
public:
	DB_LoadTopRank(void);
	
	//method from ThreadBase
	virtual int Execute(int ctxId,void* param);
	bool __isNeedFilt(int type);
   
	int m_iType;
	//bool m_isPlayer;          //�ж������Ƿ���ң������ж��Ƿ��������blob����
	int m_iLoadBlobType;       //����ж�ȡblob���ݣ������ǳ��ﻹ�ǵ���

	std::vector<TBLTopRank::stTopRank> m_toprankList;
	//stdext::hash_map<int,stPlayerStruct> m_palyerStruct;
};

#endif /*_DBSAVETOPRANK_H_*/