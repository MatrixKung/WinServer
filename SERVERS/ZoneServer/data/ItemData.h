//
//  ItemData.h
//  HelloLua
//
//  Created by th on 14-2-10.
//
//

#ifndef __HelloLua__ItemData__
#define __HelloLua__ItemData__
#include "Common/DataFile.h"
#include <string.h>
#include <hash_map>


enum enItemType {
	Item_Knife= 1,
	Item_Sword,
	Item_Gun,
	Item_Axe,
	Item_Hammer,
	Item_Firearm,
	Item_Shield,
	Item_Qi,
	Item_Food,
	Item_Charm,//����
	Item_Medicine,//��ҩ
	Item_CuCur,//��«
	Item_Scroll,//����
	Item_Other,
	Item_Max
};

const static char* s_ItemTypeName[Item_Max-1]=
{
	"��ϵ",
	"��ϵ",
	"ǹϵ",
	"��ϵ",
	"��ϵ",
	"����",
	"��",
	"��",
	"ʳ��",
	"����",
	"��ҩ",
	"��«",
	"����",
	"����"
};

class CItemData;
class CItemDataRes {

public:
	enum{
		ITEM_BEGIN_ID=1000001,
		ITEM_END_ID=1405008,
	};
	CItemDataRes();
	~CItemDataRes();

	typedef stdext::hash_map<S32,CItemData*> ITEMDATAMAP;
	typedef ITEMDATAMAP::iterator ITEMDATA_ITR;

	bool read();
	void close();
	void clear();
	CItemData* getItemData(S32 ItemId);

private:
	ITEMDATAMAP m_DataMap;
};

class CItemData {

public:
	CItemData():m_ItemId(0),m_sName(""),m_sIcon(""),m_sType(0),m_sDes(""),m_Limit(0),m_Mpro(0),
		m_AttWidth(0),m_AttLength(0),m_Sale(0),m_Skill(0),m_Timelimit(0),m_UseTimes(0),
		m_MaxDie(0),m_CD(0),m_BuffID1(0),m_Trap(0),m_Effic(0), m_BuffID2(0), m_GemHole(0)
	{};
	~CItemData(){};
	S32 m_ItemId;
	std::string m_sName;
	std::string m_sIcon; //ͼƬ
	U8 m_sType; //����
	std::string m_sDes;//����
	S32 m_Limit;//
	S32 m_Mpro;//
	S32 m_AttWidth;//
	S32 m_AttLength;//
	S32 m_Sale;//
	S32 m_Skill;//
	S32 m_Timelimit;//
	S32 m_UseTimes;//
	S32 m_MaxDie;//
	S32 m_CD;//
	S32 m_BuffID1;//
	S32 m_BuffID2;
	S32 m_Trap;//
	S32 m_Effic;//������Чid
	S32 m_GemHole;//��ʯ����Ŀ
};
extern CItemDataRes* g_ItemDataMgr;
#endif /* defined(__HelloLua__ItemData__) */