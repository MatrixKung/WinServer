#ifndef __TBLSERVERMAP_H__
#define __TBLSERVERMAP_H__

#ifndef __DATABASE_H__
#include "DataBase.h"
#endif

#ifndef __TBLBASE_H__
#include "TBLBase.h"
#endif

#include <vector>

class CDBConn;

class TBLServerMap: public TBLBase
{
public:
    //��ͼ������
    enum MapType
    {
        MAP_TYPE_NORMAL = 0,
        MAP_TYPE_COPYMAP,
        
        MAP_TYPE_COUNT
    };
    
	struct stZoneInfo
	{
		int ZoneId;			//��ͼ���������
		int MapType;        //��ͼ������(MapType)
		int MaxPlayerNum;	//����������
		int ServicePort;    //����˿ں�
		int Sequence;
	};
	
	TBLServerMap(DataBase* db):TBLBase(db) {}
	virtual ~TBLServerMap() {}
	
	DBError	Load(std::vector<stZoneInfo>& ZoneInfo);
};

#endif//__TBLSERVERMAP_H__