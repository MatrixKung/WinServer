#ifndef __TBLUSERMACHINE_H__
#define __TBLUSERMACHINE_H__

#ifndef __DATABASE_H__
#include "DataBase.h"
#endif

#ifndef __TBLBASE_H__
#include "TBLBase.h"
#endif

#include <vector>

class CDBConn;

class TBLUserMachine: public TBLBase
{
public:
	typedef __int64 U64;
	
	struct stUserMahine
	{
		U64		id;							//ΨһID
		unsigned int accountId;
		char	IPAddress[32];				//IP��ַ
		char	MacAddress[64];				//Mac���������ַ
		char	OSVersion[64];				//����ϵͳ�汾
		char	CPUName[64];				//CPU����
		U64		CPUFreq;					//CPUƵ��
		char	VideoCardName[64];			//�Կ�����
		char	VideoDriverVersion[64];	    //�Կ������汾
		char	VideoMemory[64];			//�Դ��С
		char	DirectXVersion[64];	        //DX�汾
		U64		TotalPhysMemory;			//�������ڴ��С
		U64		CurrentPhysMemory;			//��ǰ�����ڴ��С
		U64		TotalVirtualMemory;			//�������ڴ��С
		U64		CurrentVirtualMemory;		//��ǰ�����ڴ��С
		U64		UsePhysMemory;				//��ǰ����ʹ�������ڴ�
		U64		UseVirtualMemory;			//��ǰ����ʹ�������ڴ�
		U64		DiskSize;					//Ӳ�̴�С
		U64		CurrentDriveSize;			//��ǰ��������С
		U64		CurrentDriveFreeSize;		//��ǰ������ʣ���С
	};
	
	TBLUserMachine(DataBase* db):TBLBase(db) {}
	virtual ~TBLUserMachine() {}
	DBError Load(std::vector<stUserMahine>& Info);
	DBError Save(stUserMahine& Info);
	DBError QueryMAC(int AccountId, std::string& strMacAddress);
};

#endif//__TBLUSERMACHINE_H__