#pragma once

#ifndef _IPACKET_H_
#define _IPACKET_H_

#include "Base/Locker.h"
#include "Base/Log.h"
#include "Base/bitStream.h"
#include "MessageCode.h"

#define STX                             0x27        //������ʼ��ʶ
#define CKX                             0x72        //����У���ʶ

#pragma pack(push, IPACKET, 1)
struct stPacketHead
{
	char Stx;					//����ʶ
	char LineServerId;			//��·���
	char DestServerType;		//Ŀ�������
	char Ckx;
    unsigned int Message;  		//������Ϣ
	unsigned int SrcZoneId;     //��ͼ���������
	unsigned int DestZoneId;	//Ŀ��
	int	 Id;					//��ұ�ʶ
	int  RSV;					//TCP����CRCУ��ֵ��UDPת�����IP��
	UINT PacketSize;			//�����ݴ�С    
};
#pragma pack(pop, IPACKET)

class ISocket;
class IPacket
{
	CMyCriticalSection m_cs;    //���̣߳��ٽ�����

	//���ܴ���
	virtual bool EncodePacket(stPacketHead *,char *&);
	virtual bool DecodePacket(stPacketHead *,char *&);

protected:
	//Socket
	ISocket *m_pSocket;
public:
	static int	GetHeadSize();
	static int	GetTotalSize(const char *pInBuffer);
	static bool	CheckHead(const char *pInBuffer);
	static stPacketHead* BuildCheckMark(const char *pInBuffer,int nSize);
	static const char *SeekToHead(const char *pInBuffer,int &nBufferSize);  //Ѱ�Ұ�ͷ������stx��ʾ��
	//static stPacketHead *BuildPacketHead(Base::BitStream &Packeter,unsigned short Msg,int Id = 0,char DestServerType=0,unsigned long DestZoneId=0,unsigned long SrcZoneId=0,char LineServerId=0);
	static stPacketHead *BuildPacketHead(Base::BitStream &Packeter,const char* Msg,int Id = 0,char DestServerType=0,unsigned long DestZoneId=0,unsigned long SrcZoneId=0,char LineServerId=0);
	static char *BuildPacketHead(stPacketHead *pHead,const char* Msg,int Id = 0,char DestServerType=0,unsigned long DestZoneId=0,unsigned long SrcZoneId=0,char LineServerId=0);
	virtual bool HandlePacket(const char *pInData,int InSize) = 0;
	virtual void OnErrorPacket(const char *pInData,int InSize);
public:
	IPacket();
	virtual ~IPacket() {}
	virtual bool Send(char *,int,int ctrlType=0);

	virtual void Initialize();
	void BindSocket(ISocket *pSocket);
	ISocket *GetSocket(){return m_pSocket;};
};

#endif