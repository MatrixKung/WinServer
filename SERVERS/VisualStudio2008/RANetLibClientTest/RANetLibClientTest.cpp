// RANetLibClientTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ranetlib.h"
#include <iostream>
#include <windows.h>

char* strError[] = 
{
    "���������ڣ��޷���ѯ",
    "���ڵȴ�����������",
    "���߲�����ʱ",
    "������",
    "�ʺŲ�����Ϸ��",
    "δ֪����",
};

int _tmain(int argc, _TCHAR* argv[])
{
    RA_Connect("192.168.0.65",45100);

    while(true)
    {
        char name1[256] = {0};
        std::cin >> name1;

        int kick1 = RA_KickAccount(0,name1);
        
        while (true)
        {
            int rslt = RA_GetKickRespond(kick1);

            if (E_KICK_NORESP == rslt)
            {
                std::cout << "���������������Ѿ������" << std::endl;
                break;
            }

            if (E_KICK_WAITTING == rslt)
            {
                std::cout << "���ڵȴ��������..." << std::endl;
                Sleep(1000);
            }
            else
            {
                std::cout << "�յ����߷��ؽ��:" << strError[rslt] << std::endl;
                break;
            }
        }
    }


	return 0;
}

