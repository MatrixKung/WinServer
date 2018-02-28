#ifndef _THREAD_H_
#define _THREAD_H_

#include "ThreadProcedure.h"

class CThread
{
public:
    enum Priority
    {
        IDLE = 0,               // Base priority level
        LOWEST,                 // 2 points below normal
        BELOW_NORMAL,           // 1 point below normal
        NORMAL,
        ABOVE_NORMAL,           // 1 point above normal
        HIGHEST,                // 2 points above normal
        TIME_CRITICAL,          // Absolute highest OS priority available

        NUM_PRIORITIES
    };

    enum Status
    {
        RUNNING     = 0,
        SUSPENDED,
        COMPLETE
    };

protected:
    CThread(ThreadProcedure* pkProcedure);

public:
    virtual ~CThread(void);

    //�����̺߳���,�̴߳������ڹ���״̬
    static CThread* Create(ThreadProcedure* pkProcedure);

    //�����߳����ȼ�
    bool         SetPriority(Priority ePriority);
    inline const Priority GetPriority() const {return m_ePriority;}
            
    //��ȡ�̵߳�ִ��״̬����ɺ�ķ���ֵ            
    inline const Status GetStatus() const {return m_eStatus;}
    inline const unsigned int GetReturnValue() const {return m_uiReturnValue;}

    int  Suspend();
    int  Resume();
    bool WaitForCompletion();
    
public:
    //internel use!
    void* m_hThread;          //�߳�ʵ��

    bool SystemCreateThread();
    bool SystemSetPriority(Priority ePriority);
    int  SystemSuspend();
    int  SystemResume();
    bool SystemWaitForCompletion();  //�ȴ��߳��Ƿ����

    ThreadProcedure* m_pkProcedure;    //�̻߳ص�����

    Priority        m_ePriority;
    volatile Status m_eStatus;
    volatile unsigned int m_uiReturnValue;
};

#endif  //#ifndef NITHREAD_H
