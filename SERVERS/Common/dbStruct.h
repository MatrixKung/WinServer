#ifndef DB_STRUCT_H
#define DB_STRUCT_H

#include <unordered_map>
#include "DBLayer/Data/TBLExecution.h"
#include "BASE/bitStream.h"
class DataBase;

#define DBSTRUCT_VERSION 20110518
#define MAX_VAL_SIZE 5
#define MAX_STRING_LEN 64

inline float CC_FLOAT(float val, float low, float high)
{
	return (float)getMax(getMin(val, high), low);
}

inline int CC_INT(int val, int low, int high)
{
	return (int)getMax(getMin(val, high), low);
}

//���������ͱ�־
enum enOpType
{
	LOG_UNKNOW						=	0,						//δ֪����

	//================�ƽ�����(1-999) ================================================	
	LOG_GOLD_BUY					=	100,
	LOG_GOLD_DRAW,
	LOG_GOLD_DINNER,
	LOG_GOLD_ADVANCE,
	LOG_GOLD_HIRECARD,
	LOG_GOLD_PRINCESS,
	LOG_GOLD_PRINCE,
	LOG_GOLD_PVP,
	LOG_GOLD_ORG,

	LOG_MAX,
};

static std::string GetLocalString(enOpType Type) {
	static std::unordered_map<enOpType, std::string> s_LocalStr;
	static bool bInit = false;
	auto Register = [](enOpType Type, std::string Str) {
		s_LocalStr[Type] = Str;
	};

	if (!bInit) {
		Register(LOG_UNKNOW, "δ֪����");
		Register(LOG_GOLD_BUY, "������Ʒ");
		Register(LOG_GOLD_DRAW, "�齱");
		Register(LOG_GOLD_DINNER, "���");
		Register(LOG_GOLD_ADVANCE, "����");
		Register(LOG_GOLD_HIRECARD, "����ļ");
		Register(LOG_GOLD_PRINCESS, "����");
		Register(LOG_GOLD_PRINCE, "����");
		Register(LOG_GOLD_PVP, "PVP");
		Register(LOG_GOLD_PVP, "����");
		bInit = true;
	}

	return s_LocalStr[Type];
}

#define GETLOGSTR(x) GetLocalString(x)

typedef __int64 GUID_T;

class CDBConn;

#pragma pack(push, dbsl, 1)

//��־�ṹ����ö��
enum
{
	STPLAYERLOG,			//STPLAYERLOG
	STNULLLOG,				//NULL
};

//������־�ṹ
struct dbStruct
{
	U32				sourceid;					//Դ������ID
	U32				targetid;					//Ŀ�������ID
	std::string		optype;						//��������
	U32				logtime;					//�¼�����ʱ��
	
	dbStruct();

	static dbStruct*	creator(int structType);
    void				setlog(U32 _playerid,  U32 _targetid, std::string _optype);
	void				clear() {};

	virtual size_t		getStructType() = 0;
	virtual void		pack(Base::BitStream* stream);
	virtual void		unpack(Base::BitStream* stream);

	#ifdef NTJ_LOGSERVER
	virtual bool		save(CDBConn* conn) = 0;
	#endif//NTJ_LOGSERVER
};

//��־����ģ��ṹ
#ifdef NTJ_LOGSERVER
template<size_t T1, typename T2>
#else
template<size_t T1>
#endif//NTJ_LOGSERVER
struct stBaseLog : dbStruct
{
	size_t getStructType() { return T1; }
#ifdef NTJ_LOGSERVER
	bool save(CDBConn* conn)
	{
		T2 data(0);
		data.AttachConn(conn);
		return (data.Save(this) == DBERR_NONE);
	}
#endif
};

#ifdef NTJ_LOGSERVER
#define TTCC(a,b) stBaseLog<a,b>
#else
#define TTCC(a,b) stBaseLog<a>
#endif//NTJ_LOGSERVER

class LOGPlayer : public TBLExecution
{
public:
	LOGPlayer(DataBase* db) :TBLExecution(db) {}
	virtual ~LOGPlayer() {}
	DBError Save(void* Info);
};

//��Ʒ��־���ݽṹ
struct stPlayerLog : TTCC(STPLAYERLOG, LOGPlayer)
{
	std::string		val[MAX_VAL_SIZE];
	stPlayerLog() {};

	void pack(Base::BitStream* stream);
	void unpack(Base::BitStream* stream);
};

#pragma pack(pop, dbsl)
#endif//DB_STRUCT_H
