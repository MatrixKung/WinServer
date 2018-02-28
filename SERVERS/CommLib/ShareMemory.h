#ifndef _SHAREMEMORY_H_
#define _SHAREMEMORY_H_

#include <assert.h>

/************************************************************************/
/* ��װshare memory api����
   header
   data
   
   header�еİ汾��,������ض�Ӧ�Ŀͻ��˳����Ƿ��������쳣�˳�
/************************************************************************/
class ShareMemory
{	
public:
    //��ǰ�ڴ���״̬
    enum Status
    {
        STATUS_NORMAL,
        STATUS_CLOSE,
        
        STATUS_COUNT
    };
    
    struct DataHead
    {
    public:
        int m_Key;
        int m_Size;
        int m_version;		
        int m_status;

        DataHead(void)
        {
            m_Size	  = 0;
            m_version = 0;
            m_status  = 0;
        }
    };
   
public:
	ShareMemory(void)
	{
		m_pData	    = 0;
		m_smHandle	= 0;
		m_Size		= 0;
		m_pHeader	= 0;	
		m_isNewCreated = 0;
		m_createEvent = 0;
	}
	
	~ShareMemory() { Destory(); };
	
	//��������Ѿ����ڵ�
	bool	Create(int key,int Size);
	void	Destory(void);
	bool	Attach(int key,int Size);
	bool	Detach();

	bool	IsNewCreated(void) const {return m_isNewCreated;}
	
	int	    GetHeadVer(void);
	void    SetHeadVer(int ver);
	
	int     GetHeadStatus(void);
	void    SetHeadStatus(int status);
	
	//�洢�ʹ��ļ���ȡ
	bool    DumpToFile(char* FilePath);
	bool    MergeFromFile(char* FilePath);

	char*	GetData(void) { return m_pData;}
	int	    GetSize(void) {return m_Size;}

	char*	GetTypePtr(int tSize,int tIndex)
	{
		assert(tSize > 0);
		assert(tSize*tIndex < m_Size);
		
		if( tSize<=0 || tIndex>=m_Size )
			return 0;
			
		return m_pData + tSize*tIndex;
	}
private:
	bool __DoCreate(int key,int Size);
	bool __DoAttach(int key,int Size);

	bool    m_isNewCreated;
	void*   m_smHandle;
	int     m_Size;
	char*   m_pData;
	
	DataHead* m_pHeader;
	
	//��������ڴ洴��,����ʼ��������,�򴴽�����¼�����
	void*	m_createEvent;
};

#endif /*_SHAREMEMORY_H_*/