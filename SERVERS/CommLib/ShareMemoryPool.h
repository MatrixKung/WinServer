#ifndef _SHAREMEMORYPOOL_H_
#define _SHAREMEMORYPOOL_H_

#include "base/Types.h"
#include "ShareMemory.h"
#include <winbase.h>
#include <winsock2.h>

//���ݵĴ洢״̬
enum ShareMemoryStatus
{
    SM_FREE,
    SM_NORMAL,	//�����߼������ʹ��
};

enum ShareMemoryEvents
{
	SM_NOEVENT,
	SM_COPYDATA,
    SM_READYSAVE,		//��ͨ���ݵı���
	SM_QUITSAVE,		//���������ߵ�,��Ҫȷ���Ƿ񱻴浽���ݿ�,ȷ����ɺ��da��ɾ������
    SM_SAVED,
    SM_READYDEL,
    SM_DELED,
};

//sm����ģ��,��װ��sm���ݿ��������״̬�Ļ�ȡ
//����_Ty����֧��sizeof��memcopy
template<typename _Ty>
class ShareMemoryData
{
	template<typename _Ty>
	friend class ShareMemoryPool;

    //����״̬
    enum ShareMemoryLockStatus
    {
        SM_LOCK_FREE,
        SM_LOCK_LOCKED,
    };
public:
    typedef _Ty TYPE;
    
    struct Head
    {
    public:
        int		    poolId;
        int			status;
        volatile  long	lockId;
        int		    event;
		int         mark;
		U32         version;
		bool        dirty;		//���汾�ı��ʱ��,����dirty��־

        Head(void)
        {
            Clear();
        }

        void Clear(void)
        {
            poolId		= -1;
            status		= SM_FREE;
            lockId		= SM_LOCK_FREE;
            event		= SM_NOEVENT;
			mark		= -1;
			version     = 0;
			dirty		= true;
        }
    };
    
    void SetPoolId(int poolID)
	{
		m_head.poolId = poolID;
	}	

	int	 GetPoolID(void) const { return m_head.poolId;}
	
	//���������ݲ���ǰ����Ҫ������,lockguard is needed here
	void SetStatus(int status)
	{	
		m_head.status = status;
	}

	int  GetStatus(void) const
	{   
	    return m_head.status;
	}
	
	void SetEvent(int event)
	{
	    m_head.event = event;
	}

	int  GetEvent(void)
	{   
	    return m_head.event;
	}

	void SetMark(int mark)
	{
	    m_head.mark = mark;
	}

	int  GetMark(void)
	{   
	    return m_head.mark;
	}

	void  SetVersion(U32 id)	 
	{ 
		m_head.version = id;
		m_head.dirty   = true;
	}

	U32   GetVersion(void) const {return m_head.version;}

	void  SetDirty(bool flag)	 { m_head.dirty = flag;}
	bool  GetDirty(void) const   {return m_head.dirty;}

	void DoClear(void)
	{
		m_head.Clear();
		m_data.Clear();
	}

	_Ty* GetData(void) {return &m_data;}

    void Lock(const long& lockId)
    {
bgsm_loop:
        if(m_head.lockId == SM_LOCK_FREE)
		{
			InterlockedCompareExchange(&m_head.lockId,lockId,SM_LOCK_FREE);
		
			if(m_head.lockId  != lockId)
			{
				Sleep(1);
                goto bgsm_loop;
            }
			
			//check another time
			if(m_head.lockId  != lockId)
            {
                Sleep(1);
                goto bgsm_loop;
            }
            
            return;
        }
        else
        {
            Sleep(1);
            goto bgsm_loop;
        }
    }

    void UnLock(void)
    {
        if(m_head.lockId  == SM_LOCK_FREE)
        {
            return;
        }
		
		InterlockedExchange(&m_head.lockId,SM_LOCK_FREE);
        return;
    }
    
protected:
	_Ty  m_data;
    Head m_head;
};

template<typename _Ty>
class CWrapSM
{
public:
	typedef ShareMemoryData<_Ty> RAW_SM_DATA;

	CWrapSM(void)
	{
		m_pObj = 0;
	    m_uid  = (long)GetCurrentProcessId();
	}

	RAW_SM_DATA* m_pObj;
	U32      m_uid;

	void Init(RAW_SM_DATA* pObj)
	{
		m_pObj = pObj;
	}

	void Lock(void)   { m_pObj->Lock(m_uid);}
    void UnLock(void) { m_pObj->UnLock();}

	void SetPoolId(int poolID) { m_pObj->SetPoolId(poolID);}
	int	 GetPoolID(void) const { return m_pObj->GetPoolID();}
	
	void SetStatus(int status) { m_pObj->SetStatus(status);}
	int  GetStatus(void) const { return m_pObj->GetStatus();}
	
	void SetEvent(int event)   { m_pObj->SetEvent(event);}
	int  GetEvent(void)		   { return m_pObj->GetEvent();}

	void SetMark(int mark)     { m_pObj->SetMark(mark);}
	int  GetMark(void)		   { return m_pObj->GetMark();}

	void SetVersion(U32 id)	    { m_pObj->SetVersion(id);}
	U32  GetVersion(void) const {return m_pObj->GetVersion();}
	void SetDirty(bool flag)	{ m_pObj->SetDirty(flag);}
	bool GetDirty(void) const   {return m_pObj->GetDirty();}

	_Ty* GetData(void)		   {return m_pObj->GetData();}

	void DoClear(void)		   {m_pObj->DoClear();}
};

#define SM_UID_KEY 0x00f2f3f1

//����������
template<typename _Ty>
class ShareMemoryPool
{
public:
	typedef ShareMemoryData<_Ty> RAW_SM_DATA;
	typedef CWrapSM<_Ty>		 SM_DATA;

	ShareMemoryPool(void)
    {
        m_pShareMemory	  = 0;	
        m_pObjs			  = 0;
        m_nMaxSize		  = -1;
    }
    
    ~ShareMemoryPool(void)
    {
        Close();
    }

	bool IsNewCreated(void) const 
	{
		if (0 == m_pShareMemory)
            return false;

		return m_pShareMemory->IsNewCreated();
	}
	
	bool InitServer(int nMaxCount,int key)
    {   
        if (0 != m_pShareMemory)
            return false;
            
        m_pShareMemory = new ShareMemory();
        assert(m_pShareMemory);

        if(0 == m_pShareMemory)
            return false;

        if (!m_pShareMemory->Create(key,sizeof(RAW_SM_DATA) * nMaxCount + sizeof(ShareMemory::DataHead)))
            return false;

        m_nMaxSize		= nMaxCount;
        m_key           = key;
        m_pObjs			= new SM_DATA[m_nMaxSize];
        
        //����sm�ڴ�
        for(int i = 0; i < m_nMaxSize; ++i)
        {
            RAW_SM_DATA* pData = reinterpret_cast<RAW_SM_DATA*>(m_pShareMemory->GetTypePtr(sizeof(RAW_SM_DATA),i));

            if (pData == NULL)
            {
                assert(pData != NULL);
                return false;
            }
			
			pData->SetPoolId(i);

			m_pObjs[i].Init(pData);
        }

        return true;
    }
    
    bool InitClient(int nMaxCount,int key)
    {
        if (0 != m_pShareMemory)
            return false;
            
        m_pShareMemory = new ShareMemory();
        assert(m_pShareMemory);

        if(0 == m_pShareMemory)
            return false;

        if (!m_pShareMemory->Attach(key,sizeof(RAW_SM_DATA) * nMaxCount + sizeof(ShareMemory::DataHead)))
        {
            //sm������δ����
            delete m_pShareMemory;
            m_pShareMemory = 0;
            return false;
        }

        m_nMaxSize		= nMaxCount;
        m_key           = key;
        m_pObjs			= new SM_DATA[m_nMaxSize];
        
        //����sm�ڴ�
        for(int i = 0; i < m_nMaxSize; ++i)
        {
            RAW_SM_DATA* pData = reinterpret_cast<RAW_SM_DATA*>(m_pShareMemory->GetTypePtr(sizeof(RAW_SM_DATA),i));

            if ( pData == NULL )
            {
                assert( pData != NULL );
                return false;
            }
			
			pData->SetPoolId(i);
			m_pObjs[i].Init(pData);
        }
        
        return true;
    }

    void Close(void)
    {
        if (m_pShareMemory)
        {
            m_pShareMemory->Destory();
            delete m_pShareMemory;
            m_pShareMemory = 0;
        }
        
        if (m_pObjs)
        {
            delete []m_pObjs;
            m_pObjs = 0;
        }
    }

    SM_DATA* GetObj(int iIndex)
    {
        assert(iIndex<m_nMaxSize);
        return &m_pObjs[iIndex];
    }

    bool Dump(char* FilePath)
    {
        if(0 == m_pShareMemory)
        {
            return false;
        }

        return m_pShareMemory->DumpToFile(FilePath);
    }
    
    bool Load(char* FilePath)			
    {
        if(0 == m_pShareMemory)
        {
            return false;
        }
        
        return m_pShareMemory->MergeFromFile(FilePath);
    }

    unsigned int GetHeadVer()
    {
        assert(m_pShareMemory);
        
        if(0 == m_pShareMemory)
        {
            return 0;
        }
        
        return m_pShareMemory->GetHeadVer();
    }
    
    void	SetHeadVer(unsigned int ver)
    {
        assert(m_pShareMemory);
        
        if(0 == m_pShareMemory)
        {
            return;
        }
        
        return m_pShareMemory->SetHeadVer(ver);
    }
private:
    SM_DATA*         m_pObjs;			//�����������
    int				 m_nMaxSize;		//�������
    ShareMemory*	 m_pShareMemory;		
    int				 m_key;
};

#endif /*_SHAREMEMORYPOOL_H_*/