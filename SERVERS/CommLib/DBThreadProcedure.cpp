#include "stdafx.h"

#include "DBThreadProcedure.h"

int DBThreadProcedure::run(void)
{
    while(!m_isEnded)
    {
        m_pktProcess.run(m_pDBConn);
        
        Sleep(1);
    }
    
    //��������������
    m_pktProcess.run(m_pDBConn);

    return 0;
}