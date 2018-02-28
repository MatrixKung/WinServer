// AccountServer.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <winsock2.h>
#include <TChar.h>
#include "ZoneServer.h"
#include "Common/Script.h"

int _tmain(int argc, _TCHAR* argv[])
{
	SERVER_CLASS_NAME *pServerInstance = SERVER_CLASS_NAME::GetInstance();

	if(!pServerInstance->Initialize())
	{
		delete pServerInstance;
		return 0;
	}

	pServerInstance->StartService();
	pServerInstance->Maintenance();
	pServerInstance->DenyService();
	pServerInstance->StopService();

	delete pServerInstance;
	return 0;
}

