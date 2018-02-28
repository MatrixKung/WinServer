/*
#include "StallListManager.h"
#include "WorldServer.h"
#include "PlayerMgr.h"
#include "ServerMgr.h"
#include "MailManager.h"
#include "DBFixedStall.h"
#include "ChatHelper.h"
#include "GamePlayLog.h"
#include "wintcp/dtServerSocket.h"

CStallManager::CStallManager(void)
{
	registerEvent(ZW_STALL_QUERYSTALLLISTINFO_REQUEST,	&CStallManager::HandleQueryStallListInfoRequest);
	registerEvent(ZW_STALL_QUERYSTALLBASEINFO_REQUEST,	&CStallManager::HandleQueryStallBaseInfoRequest);
	registerEvent(ZW_STALL_QUERYSTALLDATA_REQUEST,		&CStallManager::HandleQueryStallDataRequest);
	registerEvent(ZW_STALL_STALLBUY_REQUEST,			&CStallManager::HandleStallBuyRequest);
	registerEvent(ZW_STALL_SETSTATUS_REQUEST,			&CStallManager::HandleSetStallStatusRequest);
	registerEvent(ZW_STALL_SETNAME_REQUEST,				&CStallManager::HandleSetStallNameRequest);
	registerEvent(ZW_STALL_SETATTORNPRICE_REQUEST,		&CStallManager::HandleSetAttornPriceRequest);
	registerEvent(ZW_STALL_NOTICE_REQUEST,				&CStallManager::HandleNoticeRequest);
	registerEvent(ZW_STALL_SETFLOWMONEY_REQUEST,		&CStallManager::HandleSetFlowMoneyRequest);
	registerEvent(ZW_STALL_SETFIXEDMONEY_REQUEST,		&CStallManager::HandleSetFixedMoneyRequest);
	registerEvent(ZW_STALL_SHOUTCONTENT_REQUEST,		&CStallManager::HandleShoutContentRequest);
	registerEvent(ZW_STALL_STALLTYPE_REQUEST,			&CStallManager::HandleStallTypeRequest);
	registerEvent(ZW_STALL_ISATTORN_REQUEST,			&CStallManager::HandleIsAttornRequest);
	registerEvent(ZW_STALL_ISSHOUTALL_REQUEST,			&CStallManager::HandleIsShoutAllRequest);
	registerEvent(ZW_STALL_SHOUTINTERVAL_REQUEST,		&CStallManager::HandleShoutIntervalRequest);
	registerEvent(ZW_STALL_BUYSTALLSLOTS_REQUEST,		&CStallManager::HandleBuyStallSlotsRequest);
	registerEvent(ZW_STALL_SELLITEM_REQUEST,			&CStallManager::HandleSellItemRequest);
	registerEvent(ZW_STALL_SELLPET_REQUEST,				&CStallManager::HandleSellPetRequest);
	registerEvent(ZW_STALL_BUYITEM_REQUEST,				&CStallManager::HandleBuyItemRequest);
	registerEvent(ZW_STALL_SETSHOUTITEM_REQUEST,		&CStallManager::HandleSetShoutItemRequest);
	registerEvent(ZW_STALL_PLAYERITEM_REQUEST,			&CStallManager::HandlePlayerItemRequest);
	registerEvent(ZW_STALL_SETSTALLMONEY_REQUEST,		&CStallManager::HandleSetStallMoneyRequest);
	
	registerEvent(CW_STALL_SENDCHAT_REQUEST,			&CStallManager::HandleSendChatRequest);
	registerEvent(CW_STALL_QUERYCHAT_REQUEST,			&CStallManager::HandleQueryChatRequest);
	registerEvent(CW_STALL_SENDCLEARCHAT_REQUEST,		&CStallManager::HandleSendClearChatRequest);
}

CStallManager::~CStallManager()
{
	m_stallMap.clear();
}

// ----------------------------------------------------------------------------
// �������ڴ����������ݣ�������ݿ���µ������ڴ�
bool CStallManager::LoadAllStalls(void)
{
	bool bLoaded = false;
    stFixedStall* pInfo         = new stFixedStall[MAX_FIXEDSTALLS];
	stFixedStallItem* pSellItem = new stFixedStallItem[MAX_FIXEDSTALLS * MAX_FIXEDSTALL_SELLITEM];
	stFixedStallItem* pBuyItem  = new stFixedStallItem[MAX_FIXEDSTALLS * MAX_FIXEDSTALL_BUYITEM];
	stFixedStallPet* pSellPet   = new stFixedStallPet[MAX_FIXEDSTALLS * MAX_FIXEDSTALL_SELLPET];
	
    TBLStallList tb_stalllist(SERVER->GetActorDB());
	
	//��ʼ�����̻�����Ϣ����
	for(U32 i = 0; i < MAX_FIXEDSTALLS; i++)
    {
		pInfo[i].Init(i);
    }

	//�������̻�����Ϣ����
	DBError err = tb_stalllist.LoadAllInfo(pInfo);
	if(err != DBERR_NONE)
	{
		g_Log.WriteFocus("LoadAllInfo error.");
		goto clearall;
	}
	
	//��ʼ�����̳�����Ʒ������
	for(int i = 0; i < MAX_FIXEDSTALLS * MAX_FIXEDSTALL_SELLITEM; i++)
    {
		pSellItem[i].InitData(i/MAX_FIXEDSTALL_SELLITEM, i%MAX_FIXEDSTALL_SELLITEM);
    }

	//�������̳�����Ʒ������
	err = tb_stalllist.LoadAllSellItem(pSellItem);
	if(err != DBERR_NONE)
	{
		g_Log.WriteFocus("LoadAllSellItem error.");
		goto clearall;
	}

	//��ʼ�������չ���Ʒ������
	for(int i = 0; i < MAX_FIXEDSTALLS * MAX_FIXEDSTALL_BUYITEM; i++)
    {
		pBuyItem[i].InitData(i/MAX_FIXEDSTALL_BUYITEM, i%MAX_FIXEDSTALL_BUYITEM);
    }

	//���������չ���Ʒ������
	err = tb_stalllist.LoadAllBuyItem(pBuyItem);
	if(err != DBERR_NONE)
	{
		g_Log.WriteFocus("LoadAllBuyItem error.");
		goto clearall;
	}

	//��ʼ�����̳�������������
	for(int i = 0; i < MAX_FIXEDSTALLS * MAX_FIXEDSTALL_SELLPET; i++)
    {
		pSellPet[i].InitData(i/MAX_FIXEDSTALL_SELLPET, i%MAX_FIXEDSTALL_SELLPET);
    }

	//�������̳�������������
	err = tb_stalllist.LoadAllSellPet(pSellPet);
	if(err != DBERR_NONE)
	{
		g_Log.WriteFocus("LoadAllSellPet error.");
		goto clearall;
	}

	//���µ������ڴ�,��������ͬ��
	U32 k = 0;
	StallHandle pStall;
	for(U32 i = 0; i < MAX_FIXEDSTALLS; i++)
	{
        //��������Ѿ����ڴ棬����Դ�DB���ص�����
		pStall = SERVER->GetStallManager()->GetStall(i);
        if(0 == pStall)
        {
            pStall = new CStall(i);
            if (0 == pStall)
			{
				g_Log.WriteFocus("No enough memory be alloced for stallshop(%d)", i);
                goto clearall;
			}

			//Note:�������Ϊ���̷��乲���ڴ����ݿ飬Ȼ�����󶨹�ϵ
            if (!pStall->InitSM(i))
            {
                assert(0);
				g_Log.WriteFocus("Attch SM MEMORY failed for stallshop(%d)", i);
                delete pStall;
                goto clearall;
            }

            pStall->SetInfo(&pInfo[i]);

            for(int j = 0; j < MAX_FIXEDSTALL_SELLITEM; j++)
            {
				k = pInfo[i].StallID * MAX_FIXEDSTALL_SELLITEM + j;
				pStall->SetOneSellItem(&pSellItem[k]);
			}

			for(int j = 0; j < MAX_FIXEDSTALL_BUYITEM; j++)
			{
				k = pInfo[i].StallID * MAX_FIXEDSTALL_BUYITEM + j;
				pStall->SetOneBuyItem(&pBuyItem[k]);
			}

			for(int j = 0; j < MAX_FIXEDSTALL_SELLPET; j++)
			{
				k = pInfo[i].StallID * MAX_FIXEDSTALL_SELLPET + j;
				pStall->SetOneSellPet(&pSellPet[k]);
			}

			//Note:��Ϊ��������������set���ݿ����ݶ���ñ�������븴�Ʊ������ݵ�����.
			//�ڴ棬������savetodb,,���Լ�ʹ�˿�WorldDB�˳���Ҳ�����������森
			pStall->m_pSMData->Save(false, false);

			m_stallMap[i] = pStall;			
		}
	}
	bLoaded = true;
clearall:
	SAFE_DELETE_ARRAY(pInfo);
	SAFE_DELETE_ARRAY(pSellItem);
	SAFE_DELETE_ARRAY(pBuyItem);
	SAFE_DELETE_ARRAY(pSellPet);
	return bLoaded;
}

bool CStallManager::InitSMData(void)
{
	while(!m_stallSMPool.Init(SMKEY_STALLLIST))
	{
		g_Log.WriteLog("���ڵȴ��������ݹ����ڴ��ʼ�����...");
		Sleep(1000);
	}	

	SM_STALL_DATA* pSMData = 0;

	for (U32 i = 0; i < SM_MAXSTALLLIST; ++i)
	{
		pSMData = m_stallSMPool.Get(i);
		if(0 == pSMData || !pSMData->IsUsed())
			continue;

		//�����Ƚ���,��ֹ�ر�ʱ��û���ͷ�
		pSMData->UnLock();

		if (pSMData->IsCopying())
		{
			//pSMData->Reset();
			g_Log.WriteFocus("�������ݴӹ����ڴ����ʧ�ܣ�ԭ��:IsCopying, id=%d)", i);
			return false;
		}

		StallHandle pStall = new CStall(i);
		if (!pStall->InitSM(pSMData))
		{
			//pSMData->Reset();
			delete pStall;
			g_Log.WriteFocus("�������ݴӹ����ڴ����ʧ�ܣ�ԭ��:LoadFrom, id=%d)", i);
			return false;
		}

		if(pStall->GetStallID() != i)
		{
			g_Log.WriteFocus("�������ݴӹ����ڴ�������ݻ���(ԭ��:LoadFrom, srcid=%d, nowid=%d",
						i, pStall->GetStallID());
			delete pStall;
			return false;
		}

		m_stallMap[i] = pStall;
	}
	
	stWorldSetting& setting = SERVER->GetServerManager()->GetWorldSetting();
	m_TotalFlourish = setting.TotalFlourish;
	m_MaxStalls = setting.MaxStalls;
	return true;
}

StallHandle CStallManager::GetStall(U32 stallId)
{
	STALLMAP::iterator it = m_stallMap.find( stallId );
	if( it != m_stallMap.end() )
		return it->second;
	return StallHandle();
}

// ----------------------------------------------------------------------------
// ��������ٶȣ���ҵָ����
void CStallManager::SetTotalFlourish(U32 iAllFlourish)
{
	if(iAllFlourish < 2000)
	{
		SetMaxStalls(50);
	}
	else if(iAllFlourish >= 2000 && iAllFlourish < 4000)
	{
		SetMaxStalls(100);
	}
	else if(iAllFlourish >= 4000 && iAllFlourish < 6000)
	{
		SetMaxStalls(150);
	}
	else if(iAllFlourish >= 6000 && iAllFlourish < 8000)
	{
		SetMaxStalls(200);
	}
	else
	{
		SetMaxStalls(250);
	}

	m_TotalFlourish = iAllFlourish;

	SERVER->GetServerManager()->SetStallShopSetting(GetMaxStalls(), m_TotalFlourish);

	UpdateAllStallTax();
}

// ----------------------------------------------------------------------------
// �����������̵�����˰��
void CStallManager::UpdateAllStallTax()
{
	S32 TotalFlourish = GetTotalFlourish();
	for(U32 i = 0; i < m_MaxStalls; i++)
	{
		StallHandle pStall = GetStall(i);
		if(pStall)
		{
			pStall->SetStallTax(GetDayTax(pStall->GetFlourish()));
			pStall->SetInfoVer();
		}
	}
}

// ----------------------------------------------------------------------------
// ��������Zone�����ѯ������������ҵָ������
void CStallManager::HandleQueryStallListInfoRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );
	char buf[128];
	Base::BitStream sendPacket( buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_QUERYSTALLLISTINFO_RESPOND, GetTotalFlourish(), SERVICE_ZONESERVER, pHead->SrcZoneId);
	sendPacket.writeInt(GetMaxStalls(), 16);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�����ѯ�����������̻�����Ϣ����Ϣ
void CStallManager::HandleQueryStallBaseInfoRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );
	U32 ver[MAX_FIXEDSTALLS];
	Packet->readBits(sizeof(U32)*8*MAX_FIXEDSTALLS, ver);

	std::vector<StallHandle> vecInfo;
	for(size_t i = 0; i < GetMaxStalls(); i++)
	{
		StallHandle pStall = GetStall(i);
		if(!pStall)
			continue;

		if(ver[i] != pStall->GetInfoVer())
		{
			vecInfo.push_back(pStall);
		}
	}

	SendStallInfoData(SocketHandle, pHead->SrcZoneId, vecInfo);
	vecInfo.clear();
}

// ----------------------------------------------------------------------------
// ��������Zone�����ѯ���������������ݵ���Ϣ
void CStallManager::HandleQueryStallDataRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );

	size_t srcsize = Packet->readInt(32);
	char* src = new char[srcsize];
	Packet->readBits(srcsize<<3, src);

	size_t tagsize = sizeof(stStallVersion) * MAX_FIXEDSTALLS;
	stStallVersion ver[MAX_FIXEDSTALLS];

	if(SXZ_UNCOMPRESS(ver, &tagsize, src, srcsize) != 0)
	{
		g_Log.WriteFocus("HandleQueryStallDataRequest ��ѹ�����̰汾����ʧ��!");
		delete[] src;
		return;
	}

	delete[] src;
	std::vector<stFixedStallItem*> vecSellItem;
	std::vector<stFixedStallItem*> vecBuyItem;
	std::vector<stFixedStallPet*> vecSellPet;
	for(size_t i = 0; i < GetMaxStalls(); i++)
	{
		StallHandle pStall = GetStall(i);
		if(!pStall)
			continue;

		//��������Ʒ�����ݰ汾��ͬ,�����
		for(int j = 0; j < MAX_FIXEDSTALL_SELLITEM; j++)
		{
			if(ver[i].SellItemVer[j] != pStall->GetSellItemVer(j))
				vecSellItem.push_back(pStall->GetOneSellItem(j));
		}

		for(int j = 0; j < MAX_FIXEDSTALL_BUYITEM; j++)
		{
			if(ver[i].BuyItemVer[j] != pStall->GetBuyItemVer(j))
				vecBuyItem.push_back(pStall->GetOneBuyItem(j));
		}

		for(int j = 0; j < MAX_FIXEDSTALL_SELLPET; j++)
		{
			if(ver[i].SellPetVer[j] != pStall->GetSellPetVer(j))
				vecSellPet.push_back(pStall->GetOneSellPet(j));
		}
	}

	SendSellItemData(SocketHandle, pHead->SrcZoneId, vecSellItem);
	SendBuyItemData(SocketHandle, pHead->SrcZoneId, vecBuyItem);
	SendSellPetData(SocketHandle, pHead->SrcZoneId, vecSellPet);
	vecSellItem.clear();
	vecBuyItem.clear();
	vecSellPet.clear();
}

// ----------------------------------------------------------------------------
// ��Zone�������̻�����Ϣ����
void CStallManager::SendStallInfoData(S32 SocketHandle, U32 srcZoneId, std::vector<StallHandle>& vecInfo)
{
	if(vecInfo.empty())
	{
		char buf[128];
		Base::BitStream sendPacket(buf, sizeof(buf));
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_QUERYINFO_RESPOND, 0, SERVICE_ZONESERVER, srcZoneId);
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
		return;
	}

	U16 stallcount = vecInfo.size();
	char* buffer = new char[MAX_PLAYER_STRUCT_SIZE];
	Base::BitStream sendPacket(buffer, MAX_PLAYER_STRUCT_SIZE);
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_QUERYINFO_RESPOND, stallcount, SERVICE_ZONESERVER, srcZoneId);
	
	//�������̻�����Ϣ����ѹ��������ʽ
	size_t srcsize = sizeof(stFixedStall) * stallcount;
	stFixedStall* src = new stFixedStall[stallcount];
	for(size_t i = 0; i < vecInfo.size(); i++)
	{
		memcpy(&src[i], vecInfo[i]->GetInfo(), sizeof(stFixedStall));
	}

	size_t tagsize = srcsize + 400;
	char* tag = new char[tagsize];
	if(SXZ_COMPRESS(tag, &tagsize, (char*)src, srcsize) == 0 && tagsize <= srcsize)
	{
		sendPacket.writeInt(tagsize, 32);
		sendPacket.writeBits(tagsize<<3, tag);
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
	}
	else
	{
		g_Log.WriteFocus("ѹ���������̻�����Ϣ����ʧ��!");
	}

	delete[] src;
	delete[] tag;
	delete[] buffer;	
}

// ----------------------------------------------------------------------------
// ��Zone�������̳�����Ʒ������
void CStallManager::SendSellItemData(S32 SocketHandle, U32 srcZoneId, std::vector<stFixedStallItem*>& vecItem)
{
	if(vecItem.empty())
		return;

	int count = vecItem.size();
	size_t srcsize = sizeof(stFixedStallItem) * count;
	stFixedStallItem* src = new stFixedStallItem[count];
	for(size_t i = 0; i < vecItem.size(); i++)
	{
		memcpy(&src[i], vecItem[i], sizeof(stFixedStallItem));
	}

	size_t tagsize = srcsize + 400;
	char* tag = new char[tagsize];
	if(SXZ_COMPRESS(tag, &tagsize, (char*)src, srcsize) != 0 || tagsize > srcsize)
	{
		g_Log.WriteFocus("ѹ���������̳�����Ʒ������ʧ��!");
		delete[] src;
		delete[] tag;
		return;
	}

	//ѹ�����ݷֿ鷢��
	const U32 MAX_BLOCKS = MAX_PLAYER_STRUCT_SIZE-128;

	int blockcount = tagsize / MAX_BLOCKS;
	if(tagsize % MAX_BLOCKS != 0)
		blockcount ++;
	
	char* buffer = new char[MAX_PLAYER_STRUCT_SIZE];
	char* offset = tag;
	int writeSize;
	for(int i = 0; i < blockcount; i++)
	{
		memset(buffer, 0, MAX_PLAYER_STRUCT_SIZE);
		Base::BitStream sendPacket( buffer, MAX_PLAYER_STRUCT_SIZE);
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_QUERYSELLITEM_RESPOND, count, SERVICE_ZONESERVER, srcZoneId);
		sendPacket.writeInt(blockcount, 8);
		sendPacket.writeInt(i, 8);
		if(i == blockcount-1)
			writeSize = tagsize-i*MAX_BLOCKS;
		else
			writeSize = MAX_BLOCKS;
		sendPacket.writeInt(writeSize, 32);
		sendPacket.writeBits(writeSize<<3, offset);
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
		offset += MAX_BLOCKS;
	}

	delete[] src;
	delete[] tag;
	delete[] buffer;
}

// ----------------------------------------------------------------------------
// ��Zone���������չ���Ʒ������
void CStallManager::SendBuyItemData(S32 SocketHandle, U32 srcZoneId, std::vector<stFixedStallItem*>& vecItem)
{
	if(vecItem.empty())
		return;

	int count = vecItem.size();
	size_t srcsize = sizeof(stFixedStallItem) * count;
	stFixedStallItem* src = new stFixedStallItem[count];
	for(size_t i = 0; i < vecItem.size(); i++)
	{
		memcpy(&src[i], vecItem[i], sizeof(stFixedStallItem));
	}

	size_t tagsize = srcsize + 400;
	char* tag = new char[tagsize];
	if(SXZ_COMPRESS(tag, &tagsize, (char*)src, srcsize) != 0 || tagsize > srcsize)
	{
		g_Log.WriteFocus("ѹ�����������չ���Ʒ������ʧ��!");
		delete[] src;
		delete[] tag;
		return;
	}

	//ѹ�����ݷֿ鷢��
	const U32 MAX_BLOCKS = MAX_PLAYER_STRUCT_SIZE-128;

	int blockcount = tagsize / MAX_BLOCKS;
	if(tagsize % MAX_BLOCKS != 0)
		blockcount ++;

	char* buffer = new char[MAX_PLAYER_STRUCT_SIZE];
	char* offset = tag;
	int writeSize;
	for(int i = 0; i < blockcount; i++)
	{
		memset(buffer, 0, MAX_PLAYER_STRUCT_SIZE);
		Base::BitStream sendPacket( buffer, MAX_PLAYER_STRUCT_SIZE);
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_QUERYBUYITEM_RESPOND, count, SERVICE_ZONESERVER, srcZoneId);
		sendPacket.writeInt(blockcount, 8);
		sendPacket.writeInt(i, 8);
		if(i == blockcount-1)
			writeSize = tagsize-i*MAX_BLOCKS;
		else
			writeSize = MAX_BLOCKS;
		sendPacket.writeInt(writeSize, 32);
		sendPacket.writeBits(writeSize<<3, offset);
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
		offset += MAX_BLOCKS;
	}

	delete[] src;
	delete[] tag;
	delete[] buffer;
}

// ----------------------------------------------------------------------------
// ��Zone�������̳�������������
void CStallManager::SendSellPetData(S32 SocketHandle, U32 srcZoneId, std::vector<stFixedStallPet*>& vecPet)
{
	if(vecPet.empty())
		return;

	int count = vecPet.size();
	size_t srcsize = sizeof(stFixedStallPet) * count;
	stFixedStallPet* src = new stFixedStallPet[count];
	for(size_t i = 0; i < vecPet.size(); i++)
	{
		memcpy(&src[i], vecPet[i], sizeof(stFixedStallPet));
	}

	size_t tagsize = srcsize + 400;
	char* tag = new char[tagsize];
	if(SXZ_COMPRESS(tag, &tagsize, (char*)src, srcsize) != 0 || tagsize > srcsize)
	{
		g_Log.WriteFocus("ѹ���������̳�������������ʧ��!");
		delete[] src;
		delete[] tag;
		return;
	}

	//ѹ�����ݷֿ鷢��
	const U32 MAX_BLOCKS = MAX_PLAYER_STRUCT_SIZE-128;

	int blockcount = tagsize / MAX_BLOCKS;
	if(tagsize % MAX_BLOCKS != 0)
		blockcount ++;

	char* buffer = new char[MAX_PLAYER_STRUCT_SIZE];
	char* offset = tag;
	int writeSize;
	for(int i = 0; i < blockcount; i++)
	{
		memset(buffer, 0, MAX_PLAYER_STRUCT_SIZE);
		Base::BitStream sendPacket( buffer, MAX_PLAYER_STRUCT_SIZE);
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_QUERYSELLPET_RESPOND, count, SERVICE_ZONESERVER, srcZoneId);
		sendPacket.writeInt(blockcount, 8);
		sendPacket.writeInt(i, 8);
		if(i == blockcount-1)
			writeSize = tagsize-i*MAX_BLOCKS;
		else
			writeSize = MAX_BLOCKS;
		sendPacket.writeInt(writeSize, 32);
		sendPacket.writeBits(writeSize<<3, offset);
		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
		offset += MAX_BLOCKS;
	}

	delete[] src;
	delete[] tag;
	delete[] buffer;
}

// ----------------------------------------------------------------------------
// ��������Zone���������̵���Ϣ
void CStallManager::HandleStallBuyRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );

	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//�����������ID
	S32 money		= Packet->readInt(32);	//���������ĵĽ�Ԫ
	U32 itemId		= Packet->readInt(32);	//���������ĵ������ؼ�
	char PlayerName[32] = {0};				//�������
	Packet->readString(PlayerName, 32);

	U32 curtime;
	_time32((__time32_t*)&curtime);

	int error = 0;
	StallHandle pStall = GetStall(stallId);

	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;
		}

		//�ж��Ƿ��ϵͳ����
		if(pStall->GetStallerID() != 0)
		{
			if(pStall->GetStallerID() == PlayerId)
			{
				error = 3417;
				break;//���ܹ����Լ�������
			}

			if(pStall->GetStatus() != SS_OPEN)
			{
				error = 3426;
				break;//����δ����,���ܹ���
			}

			if(!pStall->IsAttorn())
			{
				error = 3403;
				break;//����δת��,���ܹ���
			}

			if(curtime - pStall->GetAttornStartTime() < 3600)
			{
				error = 3403;
				break;//����δת��,���ܹ���
			}

			if(money != (pStall->GetAttornPrice() + BUYSTALLIMPREST))
			{
				error = 3400;
				break;//�������̽�Ԫ����
			}
		}
		else
		{
			if(money != (GetBuySystemStall() + BUYSTALLIMPREST))
			{
				error = 3400;
				break;//�������̽�Ԫ����
			}
		}
	} while (false);

	if(error != 0)
	{
		char szMsg[1024] = {0};
		sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
			"    ���ԡ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>���Ĺ�������"\
			"����ʧ��,ϵͳ�˻���Ǯ����Ʒ����,�뼰ʱ��ȡ.</t>", PlayerName, stallId+1);
		SERVER->GetMailManager()->sendMail(0, PlayerId, money, itemId, 1, "�����̹���", szMsg, 1);
		//��ͻ���֪ͨ�ʼ�����
		SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );

		//��Zone��Ӧ����������Ϣ
		SendStallBuyRespond(PlayerId, stallId, 0, NULL, error);
	}
	else
	{
		money -= BUYSTALLIMPREST;
		//�ȴ���ԭ����
		if(pStall->GetStallerID() != 0)
		{
			struct tm today;
			_localtime32_s(&today, (__time32_t*)&curtime);
			char szMsg[1024] = {0};

			sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>����%d��%2d��%2d��"\
				"�����[</t><t c='0x00ff00ff'>%s</t><t c='0xff9900ff'>]�ɹ��չ���ϵͳ�˻���"\
				"�������ʽ�,�뼰ʱ��ȡ.</t>", pStall->GetStallerName(), stallId+1,
				today.tm_year+1900, today.tm_mon+1, today.tm_mday, PlayerName);
			SERVER->GetMailManager()->sendMail(0, pStall->GetStallerID(), pStall->GetFlowMoney(), 0, 0, "������ת�á�", szMsg, 1);

			sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>����%d��%2d��%2d��"\
				"�����[</t><t c='0x00ff00ff'>%s</t><t c='0xff9900ff'>]�ɹ��չ���ϵͳ�˻���"\
				"�Ĺ̶��ʽ�,�뼰ʱ��ȡ.</t>", pStall->GetStallerName(), stallId+1,
				today.tm_year+1900, today.tm_mon+1, today.tm_mday, PlayerName);
			SERVER->GetMailManager()->sendMail(0, pStall->GetStallerID(), pStall->GetFixedMoney(), 0, 0, "������ת�á�", szMsg, 1);

			sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>����%d��%2d��%2d��"\
				"�����[</t><t c='0x00ff00ff'>%s</t><t c='0xff9900ff'>]�ɹ��չ���ϵͳ��������"\
				"�������̼�Ǯ,�뼰ʱ��ȡ.</t>", pStall->GetStallerName(), stallId+1,
				today.tm_year+1900, today.tm_mon+1, today.tm_mday, PlayerName);
			SERVER->GetMailManager()->sendMail(0, pStall->GetStallerID(), money, 0, 0, "������ת�á�", szMsg, 1);

			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(pStall->GetStallerID(),WORLD_CLIENT_MailNotify );

			//���չ���ת�ü۸���ϵ�10%
			pStall->SetAttornPrice(money*11/10);
		}
		else
			pStall->SetAttornPrice(money);

		//�ٴ�������
		char stallName[32];
		sprintf_s(stallName, sizeof(stallName), "%s������", PlayerName);
		pStall->SetStallName(stallName);
		pStall->SetPreStallerID(pStall->GetStallerID());
		pStall->SetStallerID(PlayerId);
		pStall->SetStallerName(PlayerName);
		pStall->SetStatus(SS_MODIFY);
		pStall->SetPayTaxTime(getZeroTime(curtime));
		pStall->SetIsAttorn(false);
		pStall->SetAttornStartTime(0);
		pStall->SetFixedMoney(BUYSTALLIMPREST);
		pStall->SetFlowMoney(0);
		pStall->SetNotice("");
		pStall->SetBuyTime(curtime);

		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();

		//��־��¼����������
		GameplayLog::GL_ShopBuyShop(PlayerId, pStall->GetStallID(), money+BUYSTALLIMPREST, BUYSTALLITEM);

		//��Zone��Ӧ����������Ϣ
		SendStallBuyRespond(PlayerId, stallId, pStall->GetInfoVer(), pStall->GetInfo(), error);
	}	
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ����������Ϣ
void CStallManager::SendStallBuyRespond(U32 PlayerId, U16 stallId, U32 infoVer, stFixedStall* pInfo, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[4096];
	Base::BitStream sendPacket( buf, sizeof(buf) );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_STALLBUY_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(infoVer, 32);
	sendPacket.writeInt(error, 32);
	if(error == 0)
	{
		sendPacket.writeBits(sizeof(stFixedStall)<<3, pInfo);
	}
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone������������״̬����Ϣ
void CStallManager::HandleSetStallStatusRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	U8  state		= Packet->readInt(8);	//����޸�����״̬

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж�״̬�Ƿ���ȷ
		if(state != SS_MODIFY && state != SS_OPEN)
		{
			error = 3432;
			break;//����״̬����ȷ
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetStatus(state);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}

	SendSetStallStatusRespond(PlayerId, stallId, state, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ��������״̬��Ϣ
void CStallManager::SendSetStallStatusRespond(U32 PlayerId, U16 stallId, U8 state, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[128];
	Base::BitStream sendPacket( buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SETSTATUS_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(state, 8);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�����޸��������Ƶ���Ϣ
void CStallManager::HandleSetStallNameRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	char szName[32] = {0};
	Packet->readString(szName, 32);			//��������

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3401;
			break;
		}
		
		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;
		}

		//�ж������Ƿ����޸�ģʽ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetStallName(szName);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}

	SendSetStallNameRespond(PlayerId, stallId, szName, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ��������������Ϣ
void CStallManager::SendSetStallNameRespond(U32 PlayerId, U16 stallId, const char* name, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[200];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SETNAME_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeString(name, 32);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone������������ת�ü۸����Ϣ
void CStallManager::HandleSetAttornPriceRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	S32 money		= Packet->readInt(32);	//����޸����̵�ת�ü۸�

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall  || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж�״̬�Ƿ���ȷ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//����������̯״̬,�����޸�
		}

		if(money < ATTORNPRICE)
		{
			error = 3404;
			break;//��������ȷ��ת�ü۸�
		}

		 if(money > MAX_MONEY_NUM)
		 {
			 error = 617;
			 break;//���õĽ�Ǯ�ﵽ����
		 }
	}while (false);

	if(error == 0)
	{
		pStall->SetAttornPrice(money);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}
	SendSetAttornPriceRespond(PlayerId, stallId, money, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ��������״̬��Ϣ
void CStallManager::SendSetAttornPriceRespond(U32 PlayerId, U16 stallId, S32 money, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[128];
	Base::BitStream sendPacket(buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SETATTORNPRICE_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(money, 32);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�����޸����̸�ʾ����Ϣ
void CStallManager::HandleNoticeRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	char szWords[128] = {0};
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	Packet->readString(szWords, 128);		//����޸����̸�ʾ

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�����
		}

		//�ж������Ƿ����޸�ģʽ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 3432;
			break;//���̲���ʧ��
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetNotice(szWords);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}

	SendNoticeRespond(PlayerId, stallId, szWords, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ��������������Ϣ
void CStallManager::SendNoticeRespond(U32 PlayerId, U16 stallId, const char* words, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[250];
	Base::BitStream sendPacket( buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_NOTICE_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeString(words, 128);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�������������ʽ����Ϣ
void CStallManager::HandleSetFlowMoneyRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	S32 money		= Packet->readInt(32);	//����޸����̵������ʽ�
	bool flag		= Packet->readFlag();	//�����ʽ��Ƿ�ȡ��
	char szPlayerName[32];
	Packet->readString(szPlayerName, 32);	//�������

	int error = 0;
	S32 iFlowMoney = 0, srcFlowMoney = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж�״̬�Ƿ���ȷ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//����Ӧ����̯���޸�
		}

		if(money <= 0)
		{
			error = 580;//��������ȷ����ֵ
			break;			
		}

		iFlowMoney = pStall->GetFlowMoney();
		srcFlowMoney = iFlowMoney;
		if(flag == 0)		//���������ʽ�
		{
			S64 iMoney = (S64)iFlowMoney + money;			
			if (iMoney > MAX_MONEY_NUM)
				error = 3434;//���������ʽ��Ѵ�����
			else
				iFlowMoney = (S32)iMoney;
		}
		else				//ȡ�������ʽ�
		{
			if(iFlowMoney < money)
				error = 3436;//��������ȷ����ֵ
			else
				iFlowMoney -= money;
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetFlowMoney(iFlowMoney);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();

		if(flag == 1)
		{
			char szMsg[1024] = {0};
			sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>��ȡ�������ʽ�ɹ�,ϵͳ�������Ľ�Ԫ���뼰ʱ��ȡ!</t>",
				szPlayerName, stallId + 1);
			//ȡ���������ʽ����ʼ��黹
			SERVER->GetMailManager()->sendMail(0, pStall->GetStallerID(), money, 0, 0, "��ȡ�������ʽ�", szMsg, 1);
			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );
		}

		//��־��¼:���������ʽ��ȡ
		GameplayLog::GL_ShopFlowMoney(PlayerId, pStall->GetStallID(), srcFlowMoney, iFlowMoney);
	}
	else
	{
		if(flag == 0)
		{
			char szMsg[1024] = {0};
			sprintf_s(szMsg, 1024, "<t c='0xff990ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>�����������ʽ�ʧ��,ϵͳ�˻����Ľ�Ԫ���뼰ʱ��ȡ!</t>",
				szPlayerName, stallId + 1);
			//ʧ�ܱ������ʼ��˻�Ԥ�۵Ľ�Ǯ
			SERVER->GetMailManager()->sendMail(0, PlayerId, money, 0, 0, "�����������ʽ�", szMsg, 1);
			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );
		}
	}

	SendSetFlowMoneyRespond(PlayerId, stallId, money, flag, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ�������������ʽ���Ϣ
void CStallManager::SendSetFlowMoneyRespond(U32 PlayerId, U16 stallId, S32 money, bool flag, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[128];
	Base::BitStream sendPacket( buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SETFLOWMONEY_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(money, 32);
	sendPacket.writeFlag(flag);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�������ù̶��ʽ����Ϣ
void CStallManager::HandleSetFixedMoneyRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	S32 money		= Packet->readInt(32);	//����޸����̵������ʽ�
	bool flag		= Packet->readFlag();
	char szPlayerName[32];
	Packet->readString(szPlayerName, 32);	//�������

	int error = 0;
	S32 iFixedMoney = 0, srcFixedMoney = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж�״̬�Ƿ���ȷ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//����Ӧ����̯���޸�
		}

		if(money <= 0)
		{
			error = 580;//��������ȷ����ֵ
			break;			
		}

		iFixedMoney = pStall->GetFixedMoney();
		srcFixedMoney = iFixedMoney;
		if(flag == 0)		//���������ʽ�
		{
			S64 iMoney = (S64)iFixedMoney + money;			
			if (iMoney > MAX_MONEY_NUM)
				error = 3434;//���������ʽ��Ѵ�����
			else
				iFixedMoney = (S32)iMoney;
		}
		else				//ȡ�������ʽ�
		{
			if(iFixedMoney < money)
				error = 3436;//��������ȷ����ֵ
			else
				iFixedMoney -= money;
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetFixedMoney(iFixedMoney);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();

		//if(flag == 1)
		//{
		//	char szMsg[1024] = {0};
		//	sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
		//		"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>��ȡ���̶��ʽ�ɹ�,ϵͳ�������Ľ�Ԫ���뼰ʱ��ȡ!</t>",
		//		szPlayerName, stallId + 1);
		//	//ȡ���Ĺ̶��ʽ����ʼ��黹
		//	SERVER->GetMailManager()->sendMail(0, pStall->GetStallerID(), money, 0, 0, "��ȡ�������ʽ�", szMsg, 1);
		//	//��ͻ���֪ͨ�ʼ�����
		//	SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );
		//}

		//��־��¼:���̶̹��ʽ��ȡ
		GameplayLog::GL_ShopFixedMoney(PlayerId, pStall->GetStallID(), srcFixedMoney, iFixedMoney);
	}
	else
	{
		if(flag == 0)
		{
			char szMsg[1024] = {0};
			sprintf_s(szMsg, 1024, "<t c='0xff990ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>������̶��ʽ�ʧ��,ϵͳ�˻����Ľ�Ԫ���뼰ʱ��ȡ!</t>",
				szPlayerName, stallId + 1);
			//ʧ�ܱ������ʼ��˻�Ԥ�۵Ľ�Ǯ
			SERVER->GetMailManager()->sendMail(0, PlayerId, money, 0, 0, "�����������ʽ�", szMsg, 1);
			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );
		}
	}

	SendSetFixedMoneyRespond(PlayerId, stallId, money, flag, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ�������̶̹��ʽ���Ϣ
void CStallManager::SendSetFixedMoneyRespond(U32 PlayerId, U16 stallId, S32 money, bool flag, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	CMemGuard buf( 128 MEM_GUARD_PARAM );
	Base::BitStream sendPacket( buf.get(), 128 );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SETFIXEDMONEY_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(money, 32);
	sendPacket.writeFlag(flag);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�����޸����̺������ݵ���Ϣ
void CStallManager::HandleShoutContentRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	char szShoutContent[128] = {0};
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	Packet->readString(szShoutContent, 128);//����޸����̵ĺ�������

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж������Ƿ����޸�ģʽ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//������̯,�ٲ���
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetShoutContent(szShoutContent);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}

	SendShoutContentRespond(PlayerId, stallId, szShoutContent, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ�������̺���������Ϣ
void CStallManager::SendShoutContentRespond(U32 PlayerId, U16 stallId, const char* words, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[250];
	Base::BitStream sendPacket(buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SHOUTCONTENT_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeString(words, 128);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone���������������͵���Ϣ
void CStallManager::HandleStallTypeRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	U8  type		= Packet->readInt(8);	//����޸���������

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж������Ƿ����޸�ģʽ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//������̯,�ٲ���
		}

		//�ж������Ƿ���ȷ
		if(type > STALLTYPE_MATERAIL)
		{
			error = 3432;
			break;//���ܲ���
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetStallType(type);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}

	SendSetStallTypeRespond(PlayerId, stallId, type, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ��������������Ϣ
void CStallManager::SendSetStallTypeRespond(U32 PlayerId, U16 stallId, U8 type, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[128];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_STALLTYPE_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(type, 8);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�������������Ƿ�ת�õ���Ϣ
void CStallManager::HandleIsAttornRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	bool IsAttorn	= Packet->readFlag();	//�Ƿ�ת�ñ�־

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж������Ƿ����޸�ģʽ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//������̯,�ٲ���
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetIsAttorn(IsAttorn);
		if(IsAttorn)
		{
			U32 curtime;
			_time32((__time32_t*)&curtime);
			pStall->SetAttornStartTime(curtime);
		}
		else
		{
			pStall->SetAttornStartTime(0);
		}	

		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}

	SendIsAttornRespond(PlayerId, stallId, IsAttorn, pStall->GetAttornStartTime(), error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ���������Ƿ�ת����Ϣ
void CStallManager::SendIsAttornRespond(U32 PlayerId, U16 stallId, bool flag,U32 attorntime, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[128];
	Base::BitStream sendPacket(buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_ISATTORN_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeFlag(flag);
	sendPacket.writeInt(attorntime, 32);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�������������Ƿ����ú�������Ϣ
void CStallManager::HandleIsShoutAllRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	bool IsShoutAll	= Packet->readFlag();	//�Ƿ����ú�����־

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж������Ƿ����޸�ģʽ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//������̯,�ٲ���
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetIsShoutAll(IsShoutAll);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}

	SendIsShoutAllRespond(PlayerId, stallId, IsShoutAll, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ���������Ƿ�ת����Ϣ
void CStallManager::SendIsShoutAllRespond(U32 PlayerId, U16 stallId, bool flag, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[128];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_ISSHOUTALL_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeFlag(flag);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�����������̺����������Ϣ
void CStallManager::HandleShoutIntervalRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	U16  interval	= Packet->readInt(16);	//����޸����̺������

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж������Ƿ����޸�ģʽ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//������̯,�ٲ���
		}

		//�жϺ�������Ƿ���ȷ
		if(interval != SHOUTINTERVAL0 && interval != SHOUTINTERVAL1 && interval != SHOUTINTERVAL2)
		{
			error = 3432;
			break;//���̲���ʧ��
		}
	}while (false);

	if(error == 0)
	{
		pStall->SetShoutInterval(interval);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}

	SendShoutIntervalRespond(PlayerId, stallId, interval, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ�������̺��������Ϣ
void CStallManager::SendShoutIntervalRespond(U32 PlayerId, U16 stallId, U16 interval, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[128];
	Base::BitStream sendPacket(buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SHOUTINTERVAL_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(interval, 16);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�����������̹�����λ������Ϣ
void CStallManager::HandleBuyStallSlotsRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	U8  type		= Packet->readInt(8);	//�Ƿ������Ʒ��OR����������
	U8	page		= Packet->readInt(8);	//��λҳ��
	char szPlayerName[32];
	Packet->readString(szPlayerName, 32);	//�������

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		if((type != 1 && type != 2) || page > 10)
		{
			error = 3414;
			break;
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж������Ƿ����޸�ģʽ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//������̯,�ٲ���
		}

		S32 curPage = 0;
		if(type == 1)
			curPage = pStall->GetBuySellItems() / INITSELLITEMS;
		else if(type == 2)
			curPage = pStall->GetBuySellPets() /INITSELLPETS;

		//�ж���λҳ���Ƿ�Ϸ�
		if(curPage == 0 || page != (curPage + 1))
		{
			error = 3432;
			break;//���̲���ʧ��
		}
	}while (false);

	if(error == 0)
	{
		if(!pStall)
		{
			g_Log.WriteError("����������λ��δ��ѯ�����̶���!(stallid=%d",stallId);
			return;
		}
		if(type == 1)
		{
			pStall->SetBuySellItems(page * INITSELLITEMS);
		}
		else
		{
			pStall->SetBuySellPets(page * INITSELLPETS);
		}
		U8 flourish = pStall->GetFlourish();
		pStall->SetFlourish(flourish + 5);
		S32 tax = 12 - (pStall->GetFlourish() * 0.1f);
		pStall->SetTradeTax(tax);		
		pStall->SetStallTax(GetDayTax(pStall->GetFlourish()));
		//U32 totalFlourish = GetTotalFlourish();
		//SetTotalFlourish(totalFlourish + 5);
		pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();

		//��־��¼��������λ
		GameplayLog::GL_ShopBuySlots(PlayerId, stallId, type, page);

		SendBuyStallSlotsRespond(PlayerId, stallId, type, page, pStall->GetFlourish(), pStall->GetStallTax(),
				pStall->GetTradeTax(), error);
	}
	else
	{
		S32 money = page * 10000;
		//ʧ����Ҫ�˻ع�����λ��Ǯ
		char szMsg[1024] = {0};
		sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
			"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>������%s��%dҳ�Ĳ���ʧ�ܣ�ϵͳ�˻����Ľ�Ǯ,�뼰ʱ��ȡ!</t>",
			szPlayerName, stallId+1, type==1?"������Ʒ��":"����������", page);
		SERVER->GetMailManager()->sendMail(0, PlayerId, money, 0, 0, "������������λ��", szMsg, 1);
		//��ͻ���֪ͨ�ʼ�����
		SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );

		SendBuyStallSlotsRespond(PlayerId, stallId, type, page, 0, 0, 0, error);
	}	
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ�������̹�����λ����Ϣ
void CStallManager::SendBuyStallSlotsRespond(U32 PlayerId, U16 stallId, U8 type, U8 page, U8 flourish,
							S32 stallTax, S32 tradeTax, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[128];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_BUYSTALLSLOTS_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(type, 8);
	sendPacket.writeInt(page, 8);
	sendPacket.writeInt(flourish, 8);
	sendPacket.writeInt(stallTax, 32);
	sendPacket.writeInt(tradeTax, 32);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone���������Ʒ����Ʒ���¼ܵ���Ϣ
void CStallManager::HandleSellItemRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	S16	srcIndex	= Packet->readInt(16);	//���̳���������λ��
	S16	tagIndex	= Packet->readInt(16);	//����������λ��
	bool isUpItem	= Packet->readFlag();	//�Ƿ��ϼ���Ʒ
	S32	price		= Packet->readInt(32);	//��Ʒ�۸�
	char szPlayerName[32];
	Packet->readString(szPlayerName, 32);	//�������

	stFixedStallItem item;
	Packet->readBits(sizeof(stItemInfo)<<3, &item);

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//�����Ѳ����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//���������������޸Ĳ���
		}

		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2308;
			break;//��ǰ�ѳ�̯������̯����
		}

		if(isUpItem)	//��Ʒ�ϼ�
		{
			if(tagIndex != -1)
			{
				//�ж��Ƿ������λλ��Խ��
				if(tagIndex < 0 || tagIndex >= pStall->GetBuySellItems())
				{
					error = 3023;
					break;//�޷�����Ŀ����λ
				}

				//Ŀ����Ʒ���ϲ����е���Ʒ
				stFixedStallItem* pItem = pStall->GetOneSellItem(tagIndex);
				if(pItem && pItem->ItemID != 0)
				{
					error = 3022;
					break;//Ŀ���Ѿ�����Ʒ
				}				
			}
			else
			{
				tagIndex = pStall->FindEmptyPosFromSellItem();
				if(tagIndex == -1)
				{
					error = 3023;
					break;//�޷���ӵ�Ŀ����λ
				}
			}
			item.StallID = pStall->GetStallID();
			item.Pos = tagIndex;
			item.Price = price;
			pStall->SetOneSellItem(&item);
			pStall->SetSellItemVer(tagIndex);
			pStall->UpdateSMData(StallSMData::MASK_SELLITEM, tagIndex);

			//��¼��־:�����ϼܳ�����Ʒ����Ʒ�ɹ�
			GameplayLog::GL_ShopSetSellItem(PlayerId, LOG_SHOP_UPSELLITEM_OK, &item);
		}
		else
		{
			//�ж��Ƿ������λλ��Խ��
			if(srcIndex < 0 || srcIndex >= pStall->GetBuySellItems())
			{
				error = 3072;
				break;//Ŀ��ѡ�����
			}
			stFixedStallItem* pSrcItem = pStall->GetOneSellItem(srcIndex);
			//�ж�λ�����Ƿ�����Ʒ
			if(!pSrcItem || pSrcItem->ItemID != item.ItemID)
			{
				error = 3020;
				break;//Ŀ�겻���ڣ��޷�����
			}
			item = *pSrcItem;
			pSrcItem->InitData(stallId, pSrcItem->Pos);
			pStall->SetSellItemVer(srcIndex);
			pStall->UpdateSMData(StallSMData::MASK_SELLITEM, srcIndex);

			//��¼��־:�����¼ܳ�����Ʒ����Ʒ�ɹ�
			GameplayLog::GL_ShopSetSellItem(PlayerId, LOG_SHOP_DOWNSELLITEM_OK, &item);
		}		
	}while (false);

	if(error != 0)
	{
		if(isUpItem)
		{
			char szMsg[1024] = {0};
			sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>����Ʒ�ϼ�ʧ��,ϵͳ�˻���Ʒ,�뼰ʱ��ȡ.</t>",
				szPlayerName, stallId+1);
			SERVER->GetMailManager()->sendMail(0, PlayerId, 0, item, "��������Ʒ���á�", szMsg, 1);
			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );

			//��¼��־:�����ϼܳ�����Ʒ����Ʒʧ��
			GameplayLog::GL_ShopSetSellItem(PlayerId,LOG_SHOP_UPSELLITEM_FAIL, &item);
		}
	}

	SendSellItemRespond(PlayerId, szPlayerName, stallId, isUpItem, item, price, srcIndex, tagIndex, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ�������̳�����Ʒ����Ʒ�ϼ���Ϣ
void CStallManager::SendSellItemRespond(U32 PlayerId, const char* PlayerName, U16 stallId, bool isUpItem, stItemInfo& item,
						S32 price, S32 srcIndex, S32 tagIndex, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[4096];
	Base::BitStream sendPacket(buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SELLITEM_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(srcIndex, 16);
	sendPacket.writeInt(tagIndex, 16);
	sendPacket.writeFlag(isUpItem);
	sendPacket.writeInt(price, 32);
	sendPacket.writeString(PlayerName, MAX_NAME_LENGTH);
	sendPacket.writeBits(sizeof(stItemInfo)<<3, &item);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone��������������������¼ܵ���Ϣ
void CStallManager::HandleSellPetRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	S16	srcIndex	= Packet->readInt(16);	//��������������λ��
	S16 tagIndex	= Packet->readInt(16);	//������Ʒ������λ��
	bool isUpPet	= Packet->readFlag();	//�Ƿ��ϴ�����
	S32 price		= Packet->readInt(32);	//���޼۸�

	stFixedStallPet pet;
	Packet->readBits(sizeof(stPetInfo)<<3, &pet);

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;
		}

		//�ж������Ƿ���̯�޸�״̬
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2308;
			break;
		}

		if(isUpPet)	//�����ϼ�
		{
			if(tagIndex != -1)
			{
				//�ж��Ƿ������λλ��Խ��
				if(tagIndex < 0 || tagIndex >= pStall->GetBuySellPets())
				{
					error = 3023;
					break;
				}
				stFixedStallPet* pTagPet = pStall->GetOneSellPet(tagIndex);
				if(pTagPet && pTagPet->petDataId != 0)
				{
					error = 3022;
					break;//Ŀ��λ����������
				}
			}
			else
			{
				tagIndex = pStall->FindEmptyPosFromSellPet();
				if(tagIndex == -1)
				{
					error = 3432;
					break;//���̲�����Ч
				}
			}
			pet.StallID = stallId;
			pet.Pos = tagIndex;
			pet.Price = price;
			pStall->SetOneSellPet(&pet);
			pStall->SetSellPetVer(tagIndex);
			pStall->UpdateSMData(StallSMData::MASK_SELLPET, tagIndex);

			//��¼��־:�����ϼܳ������������޳ɹ�
			GameplayLog::GL_ShopSetSellPet(PlayerId, LOG_SHOP_UPSELLPET_OK, &pet);
		}
		else
		{
			//�ж��Ƿ������λλ��Խ��
			if(srcIndex < 0 || srcIndex >= pStall->GetBuySellPets())
			{
				error = 3072;
				break;
			}
			stFixedStallPet* pSrcPet = pStall->GetOneSellPet(srcIndex);
			//�ж�λ�����Ƿ�������
			if(!pSrcPet || pSrcPet->petDataId != pet.petDataId)
			{
				error = 3020;
				break;
			}
			pet = *pSrcPet;
			pSrcPet->InitData(stallId, pSrcPet->Pos);
			pStall->SetSellPetVer(srcIndex);
			pStall->UpdateSMData(StallSMData::MASK_SELLPET, srcIndex);

			//��¼��־:�����¼ܳ������������޳ɹ�
			GameplayLog::GL_ShopSetSellPet(PlayerId, LOG_SHOP_DOWNSELLPET_OK, &pet);
		}
		
	}while (false);

	if(error != 0)
	{
		if(isUpPet)
		{
			//��ʱδ�����ʼ����޹���

			//��¼��־:�����ϼܳ�������������ʧ��
			GameplayLog::GL_ShopSetSellPet(PlayerId, LOG_SHOP_UPSELLPET_FAIL, &pet);
		}		
	}

	SendSellPetRespond(PlayerId, stallId, isUpPet, pet, price, srcIndex, tagIndex, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ�������̳����������������¼���Ϣ
void CStallManager::SendSellPetRespond(U32 PlayerId, U16 stallId, bool isUpPet, stPetInfo& pet, 
							S32 price, S32 srcIndex, S32 tagIndex, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[1024];
	Base::BitStream sendPacket(buf,sizeof(buf));

	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SELLPET_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(srcIndex, 16);
	sendPacket.writeInt(tagIndex, 16);
	sendPacket.writeFlag(isUpPet);
	sendPacket.writeInt(price, 32);
	sendPacket.writeBits(sizeof(stPetInfo)<<3, &pet);
	sendPacket.writeInt(error, 32);

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone�����չ���Ʒ�����¼ܵ���Ϣ
void CStallManager::HandleBuyItemRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	bool isUpItem	= Packet->readFlag();	//�Ƿ��ϼ���Ʒ
	S32	price		= Packet->readInt(32);	//��Ʒ�۸�
	S16	num			= Packet->readInt(16);	//����
	S16 index		= Packet->readInt(8);	//��λ����λ��
	char szPlayerName[MAX_NAME_LENGTH];
	Packet->readString(szPlayerName, MAX_NAME_LENGTH);

	stFixedStallItem item;
	Packet->readBits(sizeof(stItemInfo)<<3, &item);

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//�����Ѳ����б���
		}

		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//�����������޷��޸Ĳ���
		}

		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2020;
			break;//����̯����
		}

		if(isUpItem)	//��Ʒ�ϼ�
		{
			if(pStall->FindSameItemInBuyItem(item.ItemID))
			{
				error = 2312;
				break;//����Ʒ�Ѿ����չ�����
			}

			index = pStall->FindEmptyPosFromBuyItem();
			if(index == -1)
			{
				error = 2312;
				break;//�չ���λ����
			}

			item.StallID = pStall->GetStallID();
			item.Pos = index;
			item.Price = price;
			item.BuyNum = num;
			item.LeftNum = num;
			pStall->SetOneBuyItem(&item);

			//��¼��־:�����ϼܳ����չ�����Ʒ�ɹ�
			GameplayLog::GL_ShopSetBuyItem(PlayerId, LOG_SHOP_UPBUYITEM_OK, &item);
		}
		else
		{
			//�ж��Ƿ������λλ��Խ��
			if(index < 0 || index >= MAX_FIXEDSTALL_BUYITEM)
			{
				error = 3072;
				break;
			}

			stFixedStallItem* pSrcItem = pStall->GetOneBuyItem(index);
			//�ж�λ�����Ƿ�����Ʒ
			if(!pSrcItem || pSrcItem->ItemID != item.ItemID)
			{
				error = 3020;
				break;
			}
			item = *pSrcItem;
			pSrcItem->InitData(stallId, pSrcItem->Pos);

			//��¼��־:�����¼ܳ����չ�����Ʒ�ɹ�
			GameplayLog::GL_ShopSetBuyItem(PlayerId, LOG_SHOP_DOWNBUYITEM_OK, &item);
		}		
	}while (false);

	if(error == 0)
	{
        pStall->UpdateSMData(StallSMData::MASK_BUYITEM,index);
		pStall->SetBuyItemVer(index);
	}
	else
	{
		if(isUpItem)
		{
			//��¼��־:�����ϼܳ����չ�����Ʒʧ��
			GameplayLog::GL_ShopSetBuyItem(PlayerId, LOG_SHOP_UPBUYITEM_FAIL, &item);
		}
	}

	SendBuyItemRespond(PlayerId, szPlayerName, stallId, isUpItem, item, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ���������չ���Ʒ����Ʒ���¼���Ϣ
void CStallManager::SendBuyItemRespond(U32 PlayerId, const char* PlayerName, U16 stallId, bool isUpItem, stFixedStallItem& item, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[1024];
	Base::BitStream sendPacket( buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_BUYITEM_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeFlag(isUpItem);
	sendPacket.writeBits(sizeof(stFixedStallItem)<<3, &item);
	sendPacket.writeInt(error, 32);
	sendPacket.writeString(PlayerName, MAX_NAME_LENGTH);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone���󺰻�������Ʒ���¼ܵ���Ϣ
void CStallManager::HandleSetShoutItemRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	bool isUpItem	= Packet->readFlag();	//�Ƿ��ϼ���Ʒ
	U32	itemId		= Packet->readInt(32);	//��������ID
	S32 num			= Packet->readInt(16);	//������������
	S32 index		= Packet->readInt(16);	//������λ����λ��
	char szPlayerName[MAX_NAME_LENGTH];
	Packet->readString(szPlayerName, MAX_NAME_LENGTH);

	int error = 0;
	U32 shoutItem = 0;
	S32 shoutNum = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;
		}

		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2308;
			break;
		}

		shoutItem = pStall->GetShoutItemId();
		shoutNum = pStall->GetShoutItemNum();
		if(isUpItem)	//��Ʒ�ϼ�
		{
			if(shoutItem != 0)
			{
				error = 3022;
				break;//�������������Ѿ�����Ʒ
			}
			if(itemId != SHOUTITEM0 && itemId != SHOUTITEM1)
			{
				error = 3023;
				break;//�޷�����Ʒ��Ŀ��λ����
			}

			pStall->SetShoutItemId(itemId);
			pStall->SetShoutItemNum(num);

			//��¼��־:�����ϼܺ�����������Ʒ�ɹ�
			GameplayLog::GL_ShopSetShoutItem(PlayerId, LOG_SHOP_UPSHOUTITEM_OK, stallId, itemId, num);
		}
		else
		{
			num = shoutNum;
			pStall->SetShoutItemId(0);
			pStall->SetShoutItemNum(0);

			//��¼��־:�����¼ܺ�����������Ʒ�ɹ�
			GameplayLog::GL_ShopSetShoutItem(PlayerId, LOG_SHOP_DOWNSHOUTITEM_OK, stallId, shoutItem, shoutNum);
		}		
	}while (false);

	if(error == 0)
	{
        pStall->UpdateSMData(StallSMData::MASK_INFO);
		pStall->SetInfoVer();
	}
	else
	{
		if(isUpItem)
		{
			char szMsg[1024] = {0};
			sprintf_s(szMsg,1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>�������������ò���ʧ�ܣ�ϵͳ�˻�������Ʒ,�뼰ʱ��ȡ.</t>",
				szPlayerName, stallId + 1);
			SERVER->GetMailManager()->sendMail(0, PlayerId, 0, itemId, num, "��������Ʒ���á�", szMsg, 1);
			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );

			//��¼��־:�����ϼܺ�����������Ʒʧ��
			GameplayLog::GL_ShopSetShoutItem(PlayerId, LOG_SHOP_UPSHOUTITEM_FAIL, stallId, shoutItem, shoutNum);
		}
	}

	SendSetShoutItemRespond(PlayerId, szPlayerName, stallId, isUpItem, itemId, num, index, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ�������̺����������¼���Ϣ
void CStallManager::SendSetShoutItemRespond(U32 PlayerId, const char* PlayerName, U16 stallId, bool isUpItem, U32 itemId,
							S32 num, S32 index, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[200];
	Base::BitStream sendPacket( buf, sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SETSHOUTITEM_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeFlag(isUpItem);
	sendPacket.writeInt(itemId, 32);
	sendPacket.writeInt(num, 16);
	sendPacket.writeInt(index, 16);
	sendPacket.writeInt(error,32);
	sendPacket.writeString(PlayerName, MAX_NAME_LENGTH);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone������ҹ��������Ʒ�����ޣ��Լ�������Ʒ�������Ĳ�������Ϣ
void CStallManager::HandlePlayerItemRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	U8	type		= Packet->readInt(8);	//������Ʒ/��������/������Ʒ
	S16 index		= Packet->readInt(16);	//��Ʒ��λ����ֵ
	S16	num			= Packet->readInt(16);	//��Ʒ����
	S32 money		= Packet->readInt(32);	//���ѵĽ�Ǯ
	U32 checkId		= Packet->readInt(32);	//�����Ŀ����ƷID
	S32 checkPrice	= Packet->readInt(32);	//�����Ŀ����Ʒ�۸�
	char szPlayerName[MAX_NAME_LENGTH];
	Packet->readString(szPlayerName, MAX_NAME_LENGTH);

	stItemInfo item;
	stPetInfo pet;
	S32 srcNum = 0, tagNum = 0, price = 0;

	int error = 0;
	StallHandle pStall = GetStall(stallId);
	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;
		}

		//��ǰ��������˰��
		S32 TaxRate = pStall->GetTradeTax();
		if(TaxRate <= 0 || TaxRate >= 100)
			TaxRate = TRADETAX;

		//��ǰ���������ʽ�
		S64 flowMoney = pStall->GetFlowMoney();

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() == 0 || pStall->GetStallerID() == PlayerId)
		{
			error = 3418;
			break;
		}

		if(pStall->GetStatus() != SS_OPEN)
		{
			error = 2308;
			break;
		}

		if(type == 1)		//������Ʒ
		{
			if(index < 0 || index >= pStall->GetBuySellItems())
			{
				error = 3072;
				break;//��Ʒ����λ�ò���ȷ
			}

			stFixedStallItem* pItem = pStall->GetOneSellItem(index);
			if(!pItem || pItem->Quantity < num)
			{
				error = 3072;
				break;//��Ʒ��������
			}

			if(pItem->ItemID != checkId || pItem->Price != checkPrice)
			{
				error = 3489;
				break;//��Ʒ��λ�ѷ����ı䣬������ȷ������
			}

			//��Ҫ���ѵĽ�Ǯ
			S64 iNeedMoney = pItem->Price * num; 
			if(iNeedMoney <= 0)
			{
				error = 580;
				break;;	//����Ľ�Ǯ����
			}

			if(iNeedMoney > MAX_MONEY_NUM)
			{
				error = 3406;
				break;	//��Ǯ��Ŀ̫��
			}

			//����˰Ǯ
			S64 iTax = TaxRate * iNeedMoney / 100;
			if(iTax < 1)
				iTax = 1;
			S32 srcFlowMoney = flowMoney;
			flowMoney += (iNeedMoney - iTax);
			S32 tagFlowMoney = flowMoney;
			if(flowMoney < 0)
			{
				error = 3432;//���̲���ʧ��
				break;
			}
			
			if(flowMoney > MAX_MONEY_NUM)
			{
				error = 3438;
				break;//������Ǯ�Ѵ�����
			}

			memcpy(&item, pItem, sizeof(stItemInfo));
			price = pItem->Price;
			if(pItem->Quantity == num)
			{
				srcNum = 0;
				pItem->InitData(stallId, pItem->Pos);
			}
			else
			{
				pItem->Quantity -= num;
				srcNum = pItem->Quantity;
			}
			tagNum = num;
			pStall->SetFlowMoney(flowMoney);
			pStall->SetInfoVer();
			pStall->SetSellItemVer(index);
            pStall->UpdateSMData(StallSMData::MASK_INFO | StallSMData::MASK_SELLITEM,index);
			
			//��¼�����������׼�¼
			pStall->AddRecordToList(0, iNeedMoney, iTax, szPlayerName, &item, num, NULL);

			//��¼��־:���̹��������Ʒ����Ʒ�ɹ�
			GameplayLog::GL_ShopBuySellItem(PlayerId, LOG_SHOP_BUYSELLITEM_OK, pStall->GetStallID(),
				pStall->GetStallerID(),	srcFlowMoney, tagFlowMoney, iNeedMoney, iTax, 
				TaxRate, &item, index, price, num);
		}
		else if(type == 2) //��������
		{
			if(index < 0 || index >= pStall->GetBuySellPets())
			{
				error = 3072;
				break;//���޲�λ����λ�ò���ȷ
			}

			stFixedStallPet* pPet = pStall->GetOneSellPet(index);
			if(!pPet)
			{
				error = 3072;
				break;//�������ݲ���
			}

			if(pPet->petDataId != checkId || pPet->Price != checkPrice)
			{
				error = 3489;
				break;//��Ʒ��λ�ѷ����ı䣬������ȷ������
			}

			//��Ҫ���ѵĽ�Ǯ
			S64 iNeedMoney = pPet->Price; 
			//����˰Ǯ
			S64 iTax = TaxRate * iNeedMoney / 100;
			if(iTax < 1)
				iTax = 1;
			S32 srcFlowMoney = flowMoney;
			flowMoney += (iNeedMoney - iTax);
			S32 tagFlowMoney = flowMoney;
			if(flowMoney < 0)
			{
				error = 3432;//���̲���ʧ��
				break;
			}
			
			if(flowMoney > MAX_MONEY_NUM)
			{
				error = 3438;
				break;//��Ǯ�Ѵ�����
			}

			memcpy(&pet, pPet, sizeof(stPetInfo));
			price = pPet->Price;
			pStall->SetFlowMoney(flowMoney);
			pStall->SetInfoVer();
			pPet->InitData(stallId, pPet->Pos);
			pStall->SetSellPetVer(index);
            pStall->UpdateSMData(StallSMData::MASK_INFO | StallSMData::MASK_SELLPET,index);

			//��¼�����������׼�¼
		   pStall->AddRecordToList(1, iNeedMoney, iTax, szPlayerName, NULL, 0, &pet);
			//��¼��־:���̹���������������޳ɹ�
			GameplayLog::GL_ShopBuySellPet(PlayerId, LOG_SHOP_BUYSELLPET_OK, pStall->GetStallID(),
				pStall->GetStallerID(),	srcFlowMoney, tagFlowMoney, iNeedMoney, iTax,
				TaxRate, &pet, index, price);
		}
		else
		{
			if(index < 0 || index >= MAX_FIXEDSTALL_BUYITEM)
			{
				error = 3072;
				break;//��Ʒ����λ�ò���ȷ
			}
			stFixedStallItem* pItem = pStall->GetOneBuyItem(index);
			if(!pItem || pItem->LeftNum < num)
			{
				error = 3072;
				break;//��Ʒ�Ѿ��չ����
			}

			if(pItem->ItemID != checkId || pItem->Price != checkPrice)
			{
				error = 3489;
				break;//��Ʒ��λ�ѷ����ı䣬������ȷ������
			}

			S64 iNeedMoney = pItem->Price * num;
			if(iNeedMoney <= 0)
			{
				error = 580;
				break;//����Ľ�Ǯ����
			}

			if(iNeedMoney > MAX_MONEY_NUM)
			{
				error = 617;
				break;//��Ǯ�Ѵ�����
			}

			if(flowMoney < iNeedMoney)
			{
				error = 3413;
				break;//���㹻�������ʽ����չ���Ʒ
			}

			S32 srcFlowMoney = flowMoney;
			flowMoney -= iNeedMoney;
			S32 tagFlowMoney = flowMoney;

			memcpy(&item, pItem, sizeof(stItemInfo));
			pItem->LeftNum -= num;
			srcNum = pItem->LeftNum;
			tagNum = num;
			price = pItem->Price;
			pStall->SetFlowMoney(flowMoney);
			pStall->SetInfoVer();
			pStall->SetBuyItemVer(index);
            pStall->UpdateSMData(StallSMData::MASK_INFO | StallSMData::MASK_BUYITEM,index);

			//��¼�����������׼�¼
			pStall->AddRecordToList(2,-iNeedMoney, 0, szPlayerName, &item, num, NULL);
			//��¼��־:���������չ���Ʒ����Ʒ�ɹ�
			GameplayLog::GL_ShopSellBuyItem(PlayerId, LOG_SHOP_SELLBUYITEM_OK,	pStall->GetStallID(),
				pStall->GetStallerID(), srcFlowMoney, tagFlowMoney, &item, index, srcNum+tagNum, srcNum, price);
		}
	}while (false);

	if(error != 0)
	{
		if(!pStall)
		{
			g_Log.WriteError("�������������������δ��ѯ����Ч�����̶���!");
			SendPlayerItemRespond(PlayerId, szPlayerName, stallId, type, index, price, srcNum, tagNum, NULL, NULL, 0,0,error);
			return;
		}

		if(type == 1)		//������Ʒ����ʧ�ܣ������˻���Ԫ�����
		{
			char szMsg[1024] = {0};
			sprintf_s(szMsg,1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ����</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>��������Ʒʧ�ܣ�ϵͳ�˻����Ľ�Ԫ,�뼰ʱ��ȡ.</t>",
				szPlayerName, stallId + 1);
			SERVER->GetMailManager()->sendMail(0, PlayerId, money, 0, 0, "��������Ʒ����", szMsg, 1);
			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );
			
			//��¼��־:���̹��������Ʒ����Ʒʧ��
			GameplayLog::GL_ShopBuySellItem(PlayerId, LOG_SHOP_BUYSELLITEM_FAIL, stallId,
				pStall->GetStallerID(), 0, 0, 0, 0, 0, &item, index,price, num);
		}
		else if(type == 2)	//�������޲���ʧ�ܱ����˻���Ԫ�����
		{
			char szMsg[1024] = {0};
			sprintf_s(szMsg,1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ����</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>����������ʧ�ܣ�ϵͳ�˻����Ľ�Ԫ,�뼰ʱ��ȡ.</t>",
				szPlayerName, stallId + 1);
			SERVER->GetMailManager()->sendMail(0, PlayerId, money, 0, 0, "���������޹���", szMsg, 1);
			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );

			//��¼��־:���̹����������������ʧ��
			GameplayLog::GL_ShopBuySellPet(PlayerId, LOG_SHOP_BUYSELLPET_FAIL, stallId,
				pStall->GetStallerID(), 0, 0, 0, 0,0, &pet, index, price);
		}
		else		//������Ʒ����������ʧ�ܣ������˻���Ʒ�����
		{
			char szMsg[1024] = {0};
			sprintf_s(szMsg,1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
				"    ����</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>��������Ʒ����ʧ�ܣ�ϵͳ�˻�������Ʒ,����ȡ.</t>",
				szPlayerName, stallId + 1);
			SERVER->GetMailManager()->sendMail(0, PlayerId, 0, item, "������������Ʒ��", szMsg, 1);
			//��ͻ���֪ͨ�ʼ�����
			SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );

			//��¼��־:���������չ���Ʒ����Ʒʧ��
			GameplayLog::GL_ShopSellBuyItem(PlayerId, LOG_SHOP_SELLBUYITEM_FAIL, stallId,
				pStall->GetStallerID(),	0, 0, &item, index, srcNum+tagNum, srcNum, price);
		}
		SendPlayerItemRespond(PlayerId, szPlayerName, stallId, type, index, price, srcNum, tagNum,
			&item, &pet, 0, 0, error);
	}
	else
	{
		SendPlayerItemRespond(PlayerId, szPlayerName, stallId, type, index, price, srcNum, tagNum,
			&item, &pet, pStall->GetFlowMoney(), pStall->GetTradeTax(), error);
	}	
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ������ҹ��������Ʒ�����ޣ��Լ�������Ʒ����������Ϣ
void CStallManager::SendPlayerItemRespond(U32 PlayerId, const char* PlayerName, U16 stallId, U8 type, S32 index, S32 price, S16 srcNum,
						S16 tagNum, stItemInfo* pItem, stPetInfo* pPet, S32 flowmoney, S32 tax, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[1024];
	Base::BitStream sendPacket(buf,sizeof(buf));
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_PLAYERITEM_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(type, 8);
	sendPacket.writeInt(index, 16);
	sendPacket.writeInt(price, 32);
	sendPacket.writeInt(flowmoney,32);
	sendPacket.writeInt(tax, 32);
	sendPacket.writeString(PlayerName, MAX_NAME_LENGTH);
	if(type == 1 || type == 3)
	{
		sendPacket.writeInt(srcNum, 16);
		sendPacket.writeInt(tagNum, 16);
		sendPacket.writeBits(sizeof(stItemInfo)<<3, pItem);
	}
	else
		sendPacket.writeBits(sizeof(stPetInfo)<<3, pPet);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������Zone���������޸ļ۸����Ϣ
void CStallManager::HandleSetStallMoneyRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId	= pHead->Id;			//���ID
	U16 stallId		= Packet->readInt(16);	//����޸�����ID
	U8	type		= Packet->readInt(8);	//��������
	S16 index		= Packet->readInt(16);	//Ŀ��λ��
	S32 money		= Packet->readInt(32);	//����޸�������Ʒ�����޵ļ۸�

	int error = 0;
	StallHandle pStall = GetStall(stallId);

	do 
	{
		if(!pStall || stallId >= GetMaxStalls())
		{
			error = 3414;
			break;//���̲����б���
		}

		//�ж��Ƿ������Լ�
		if(pStall->GetStallerID() != PlayerId)
		{
			error = 3431;
			break;//��������,�����޸�
		}

		//�ж�״̬�Ƿ���ȷ
		if(pStall->GetStatus() != SS_MODIFY)
		{
			error = 2302;
			break;//����Ӧ����̯���޸�
		}

		if(money <= 0)
		{
			error = 580;
			break;
		}
		
		if(money > MAX_MONEY_NUM)
		{
			error = 617;
			break;//����Ľ�Ǯ����������
		}

		if(type == 1)		//������Ʒ��
		{
			if(index < 0 || index >= pStall->GetBuySellItems())
			{
				error = 3072;
				break;//��Ʒ����λ�ò���ȷ
			}

			stFixedStallItem* pItem = pStall->GetOneSellItem(index);
			if(!pItem || pItem->ItemID == 0)
			{
				error = 3020;
				break;//��Ʒ������
			}
			pItem->Price = money;
			pStall->SetSellItemVer(index);
            pStall->UpdateSMData(StallSMData::MASK_SELLITEM, index);
		}
		else if(type == 2)		//����������
		{
			if(index < 0 || index >= pStall->GetBuySellPets())
			{
				error = 3072;
				break;//��Ʒ����λ�ò���ȷ
			}
			stFixedStallPet* pPet = pStall->GetOneSellPet(index);
			if(!pPet || pPet->petDataId == 0)
			{
				error = 3020;
				break;//Ŀ�겻����
			}
			pPet->Price = money;
			pStall->SetSellPetVer(index);
            pStall->UpdateSMData(StallSMData::MASK_SELLPET, index);
		}
		else
		{
			if(index < 0 || index >= MAX_FIXEDSTALL_BUYITEM)
			{
				error = 3072;
				break;//��Ʒ����λ�ò���ȷ
			}
			stFixedStallItem* pItem = pStall->GetOneBuyItem(index);
			if(!pItem || pItem->LeftNum == 0)
			{
				error = 3020;
				break;//Ŀ�겻����
			}
			pItem->Price = money;
			pStall->SetBuyItemVer(index);
            pStall->UpdateSMData(StallSMData::MASK_BUYITEM, index);
		}
	}while (false);

	SendSetStallMoneyRespond(PlayerId, stallId, type, index, money, error);
}

// ----------------------------------------------------------------------------
// ��Zone��Ӧ�������������ʽ���Ϣ
void CStallManager::SendSetStallMoneyRespond(U32 PlayerId, U16 stallId,	U8 type, S32 index, S32 money, S32 error)
{
	int LineId = SERVER->GetPlayerManager()->GetPlayerLineId(PlayerId);
	int srcZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);
	int SocketHandle = SERVER->GetServerManager()->GetGateSocket( LineId, srcZoneId );

	char buf[128];
	Base::BitStream sendPacket( buf, sizeof(buf) );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WZ_STALL_SETSTALLMONEY_RESPOND, PlayerId, SERVICE_ZONESERVER, srcZoneId);
	sendPacket.writeInt(stallId, 16);
	sendPacket.writeInt(type, 8);
	sendPacket.writeInt(index, 16);
	sendPacket.writeInt(money, 32);
	sendPacket.writeInt(error, 32);
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket);
}

// ----------------------------------------------------------------------------
// ��������ÿ���߼�
void CStallManager::UpdateStall()
{
	U32 curtime;
	_time32((__time32_t*)&curtime);

	DO_LOCK( Lock::Stall );

	static U32 lastDay = 0;		

	STALLMAP::iterator it = m_stallMap.begin();
	for(; it != m_stallMap.end(); ++it)
	{
		StallHandle pStall = it->second;
		//���̯λ�Ƿ񿪷Ų�����̯��
		if(pStall && pStall->GetStallerID() != 0)
		{
			bool isUpdate = false;
			//����ȫ������
			if(pStall->GetStatus() == SS_OPEN && pStall->GetIsShoutAll())
			{
				U32 ItemId  = pStall->GetShoutItemId();
				U32 ItemNum = pStall->GetShoutItemNum();
				if(ItemNum > 0 && curtime > pStall->GetShoutLastTime() + pStall->GetShoutInterval())
				{
					stChatMessage msg;
					msg.nSender = pStall->GetStallerID();
					msg.btMessageType = CHAT_MSG_TYPE_WORLD;
					msg.nRecver = 1;
					dStrcpy(msg.szMessage, _countof(msg.szMessage), pStall->GetShoutContent());
					dStrcpy(msg.szSenderName, 32, pStall->GetStallerName());
					gHelper.SendMessageToWorld(msg);
					pStall->SetShoutLastTime(curtime);
					pStall->SetShoutItemNum(ItemNum-1);
					if(pStall->GetShoutItemNum() == 0)
						pStall->SetShoutItemId(0);
					pStall->SetInfoVer();
					isUpdate = true;

					//��¼��־�����̶�ʱ�������ĵ���
					GameplayLog::GL_ShopCostShoutItem(pStall->GetStallerID(), pStall->GetStallID(), ItemId, ItemNum);
				}				
			}

			//����۳�ÿ��˰
			if(curtime - pStall->GetPayTaxTime() > 24*3600)
			{
				S32 money = pStall->GetFixedMoney();
				S32 srcFixedMoney = money;
				S32 DayTax = pStall->GetStallTax();
				if(money >= DayTax)
				{
					money -= DayTax;
					pStall->SetFixedMoney(money);
					pStall->SetPayTaxTime(getZeroTime(curtime));
					pStall->SetInfoVer();
					isUpdate = true;
					if(money < 7*DayTax)
					{
						S32 days = money / DayTax;
						char szMsg[1024] = {0};
						sprintf_s(szMsg, 1024, "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
							"    ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>���Ĺ̶��ʽ�"\
							"�Ѿ�������֧��</t><t c='0xff0000ff'>%d��</t><t c='0xff9900ff'>��ÿ��˰�գ�������"\
							"ʱΪ���������㹻�Ĺ̶��ʽ�,����ϵͳ�����޷������ĵ��ջ��մ�����.</t><b/>"\
							"<t c='0xff9900ff'>    �����ע!</t>",
							pStall->GetStallerName(), pStall->GetStallID() + 1, days);
						SERVER->GetMailManager()->sendMail(0, pStall->GetStallerID(), 0, 0, 0, "�����̽�˰��", szMsg, 1);
						//��ͻ���֪ͨ�ʼ�����
						SERVER->GetPlayerManager()->SendSimplePacket(pStall->GetStallerID(),WORLD_CLIENT_MailNotify );
					}
					//��¼��־:����ÿ�տ�˰
					GameplayLog::GL_ShopDayTax(pStall->GetStallerID(), pStall->GetStallID(), DayTax, pStall->GetFlourish(),
						GetTotalFlourish(), srcFixedMoney, pStall->GetFixedMoney()); 
				}
				else
				{
					char szInfo[1024] = {0};
					struct tm today;
					_localtime32_s(&today, (__time32_t*)&curtime);
					sprintf_s(szInfo, sizeof(szInfo), "<t c='0xff9900ff'>�װ���%s:</t><b/><t c='0xff9900ff'>"\
						"    ��Ϊ���ġ�</t><t c='0xff0000ff'>%d������</t><t c='0xff9900ff'>���Ĺ̶��ʽ�����֧�������˰�գ�ϵͳ��"\
						"%d-%02d-%02d���մ����̣�</t>", pStall->GetStallerName(), pStall->GetStallID() + 1,
						today.tm_year + 1900, today.tm_mon + 1, today.tm_mday);

					S32 PlayerId = pStall->GetStallerID();
					pStall->InitInfo();

					//��Ϊ����һ����ȫ�����£�����isUpdateû��Ҫ��Ϊtrue
					isUpdate = false;

					//ȫ�����ݱ���
                    pStall->UpdateSMData(StallSMData::MASK_ALL);
					
					SERVER->GetMailManager()->sendMail(0, PlayerId, 0, 0, 0, "�����̻��ա�", szInfo, 1);
					//��ͻ���֪ͨ�ʼ�����
					SERVER->GetPlayerManager()->SendSimplePacket(PlayerId,WORLD_CLIENT_MailNotify );

					//��¼��־:ϵͳ��������
					GameplayLog::GL_ShopDumpStall(PlayerId, pStall->GetStallID());
				}
			}

			if(isUpdate)
			{
                pStall->UpdateSMData(StallSMData::MASK_INFO);
			}
		}
	}

	U32 today = getZeroTime(curtime);
	if(today != lastDay && m_stallMap.size() >= INITSTALLS)
	{
		S32 iTotalFlourish = 0;
		//����Ҫ����һ����ҵָ��
		STALLMAP::iterator it = m_stallMap.begin();
		for(; it != m_stallMap.end(); ++it)
		{
			StallHandle pStall = it->second;
			//���̯λ�Ƿ񿪷Ų�����̯��
			if(pStall && pStall->GetStallID() < m_MaxStalls)
			{
				iTotalFlourish += pStall->GetFlourish();
			}
		}

		//������ҵָ��
		SetTotalFlourish(iTotalFlourish);
		lastDay = today;
	}
}

// ----------------------------------------------------------------------------
// �������Կͻ��˷��͵�������Ϣ
void CStallManager::HandleSendChatRequest(S32 SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId = pHead->Id;
	U16 stallId = Packet->readInt(16);
	StallHandle pStall = GetStall(stallId);
	if(pStall && pStall->GetStallerID() != PlayerId &&
			pStall->GetStatus() == SS_OPEN)
	{
		char szName[MAX_NAME_LENGTH];
		char szMsg[128];
		Packet->readString(szName, 32);
		Packet->readString(szMsg, 128);
		pStall->AddChatToList(szName, szMsg);
	}
}

// ----------------------------------------------------------------------------
// �������Կͻ��˷����������������Ϣ
void CStallManager::HandleSendClearChatRequest(S32 SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId = pHead->Id;
	U16 stallId = Packet->readInt(16);
	StallHandle pStall = GetStall(stallId);
	if(pStall && pStall->GetStallerID() == PlayerId)
	{
		pStall->ClearChat();
	}
}

// ----------------------------------------------------------------------------
// �������Կͻ��˷��������ѯ���Ի�����Ϣ
void CStallManager::HandleQueryChatRequest(S32 SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Stall );	
	U32 PlayerId = pHead->Id;
	U16 stallId = Packet->readInt(16);
	U8	type = Packet->readInt(8);
	U32 timePos = Packet->readInt(32);
	StallHandle pStall = GetStall(stallId);
	if(pStall)
	{
		U32 findPos = 0;
		AccountRef pAccount = SERVER->GetPlayerManager()->GetOnlinePlayer(PlayerId);
		if(NULL == pAccount)
		{
			g_Log.WriteWarn("Ϊ��ѯ�������������Ϣ������������ʷ��¼��ȡ���������������!(PlayerId=%d)", PlayerId);
			return;
		}

		CMemGuard buf( MAX_PACKET_SIZE MEM_GUARD_PARAM );
		Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );

		if(type == 1)		//����������ʷ��¼
		{
			if(timePos != 0)
			{
				for(size_t i = 0; i < pStall->RecordList.size(); i++)
				{
					stStallRecord* pRecord = pStall->RecordList[i];
					if(pRecord && pRecord->time > timePos)
					{
						findPos = i;
						break;
					}
				}
			}

			size_t count = pStall->RecordList.size() - findPos;

			stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_STALL_QUERYCHAT_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
			sendPacket.writeInt(stallId, 16);
			sendPacket.writeInt(type, 8);
			sendPacket.writeInt(count, 32);
			for(size_t i = 0; i < count; i++)
			{
				stStallRecord* pRecord = pStall->RecordList[findPos];
				if(sendPacket.writeFlag(pRecord != NULL))
				{
					sendPacket.writeBits(sizeof(stStallRecord)<<3, pRecord);
					if(pRecord->type == 0 || pRecord->type == 2)
					{
						if(sendPacket.writeFlag(pRecord->trade.item != NULL))
							sendPacket.writeBits(sizeof(stItemInfo)<<3, pRecord->trade.item);
					}
					else
					{
						if(sendPacket.writeFlag(pRecord->trade.pet != NULL))
							sendPacket.writeBits(sizeof(stPetInfo)<<3, pRecord->trade.pet);
					}
				}
				findPos++;
			}
			pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
		}
		else				//������Լ�¼
		{
			if(timePos != 0)
			{
				for(size_t i = 0; i < pStall->ChatList.size(); i++)
				{
					stStallChat* pChat = pStall->ChatList[i];
					if(pChat && pChat->time > timePos)
					{
						findPos = i;
						break;
					}
				}
			}

			size_t count = pStall->ChatList.size() - findPos;
			
			stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WC_STALL_QUERYCHAT_RESPOND, pAccount->AccountId, SERVICE_CLIENT);
			sendPacket.writeInt(stallId, 16);
			sendPacket.writeInt(type, 8);
			sendPacket.writeInt(count, 32);
			for(size_t i = 0; i < count; i++)
			{
				if(sendPacket.writeFlag(pStall->ChatList[findPos] != NULL))
					sendPacket.writeBits(sizeof(stStallChat)<<3, pStall->ChatList[findPos]);
				findPos++;
			}
			pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );			
		}		
		
		SERVER->GetServerSocket()->Send( pAccount->GetGateSocketId(), sendPacket );
	}
}

//==============================================================================//
// ���̵Ĺ�������																//
//==============================================================================//
CStall::CStall()
{
	m_pSMData = NULL;
}

CStall::CStall(U16 id)
{
	m_pSMData = NULL;
	m_Info.StallID = id;
	InitInfo();
}

CStall::~CStall()
{
	if (NULL != m_pSMData)
	{
		SERVER->GetStallManager()->GetStallSMPool()->Free(m_pSMData);
		m_pSMData = NULL;
	}
	ClearChat();
	ClearRecord();
}

bool CStall::InitSM(U32 stallId)
{
	if (NULL != m_pSMData)
	{
		assert(NULL == m_pSMData);
		return false;
	}

	m_pSMData = SERVER->GetStallManager()->GetStallSMPool()->Alloc(stallId,0);
	if (NULL == m_pSMData)
	{
		assert(NULL != m_pSMData);
		return false;
	}
	
	m_pSMData->Attach(this);
	return true;
}

bool CStall::InitSM(SM_STALL_DATA* pSMData)
{
	if (NULL == pSMData)
		return false;

	if (NULL != m_pSMData)
	{
		assert(NULL == m_pSMData);
		return false;
	}

	m_pSMData = pSMData;
	m_pSMData->Attach(this);

	//��SM�����ȡ����
	if (!m_pSMData->LoadFromSM())
		return false;
    
	return true;
}

void CStall::UpdateSMData(int mask,U32 posMask)
{
	if (NULL == m_pSMData)
	{
		g_Log.WriteError("����[%d]SM DataΪ��,�޷����浽share memory", GetStallID());
		return;
	}
    
    //���֮ǰ�������Ѿ�������,��������
    //�����Ǽ��㲻��ȷ��Ӧ��ֻ�����ݱ�ʶ���ظ����ã�����
    //��Ӧ�Ĳ�����ִ�ж�Σ�Ӧ�ò�����������
    if (m_pSMData->IsSaved())
    {
        this->ClearSaveMask();
    }

    m_dirtyMask |= mask;

    //��Ҫ��¼�޸�����ЩԪ��
    if (mask & StallSMData::MASK_BUYITEM)
    {
        if(posMask >= MAX_FIXEDSTALL_BUYITEM)
        {
            assert(0);
		    return;
        }

        m_maskBuyItem[posMask] = true;
    }
    
    if (mask & StallSMData::MASK_SELLPET)
    {
        if(posMask > MAX_FIXEDSTALL_SELLPET)
        {
            assert(0);
		    return;
        }

        m_maskSellPet[posMask] = true;
    }
    
    if (mask & StallSMData::MASK_SELLITEM)
    {
        if(posMask > MAX_FIXEDSTALL_SELLITEM)
        {
            assert(0);
		    return;
        }

        m_maskSellItem[posMask] = true;
    }

    m_pSMData->Save(true,false);
}

void CStall::SetInfo(stFixedStall* pInfo)
{
	if(!pInfo)
		return;

	memcpy(&m_Info, pInfo, sizeof(stFixedStall));
	SetInfoVer();
}

void CStall::InitInfo()
{
	m_Info.Init(m_Info.StallID);
	for(int i = 0; i < MAX_FIXEDSTALL_SELLITEM; i++)
	{
		m_SellItem[i].InitData(m_Info.StallID, i);
	}
	for(int i = 0; i < MAX_FIXEDSTALL_SELLPET; i++)
	{
		m_SellPet[i].InitData(m_Info.StallID, i);
	}
	for(int i = 0; i < MAX_FIXEDSTALL_BUYITEM; i++)
	{
		m_BuyItem[i].InitData(m_Info.StallID, i);
	}
	ClearChat();
	ClearRecord();
}

void CStall::SetOneSellItem(stFixedStallItem* pItem)
{
	if(!pItem || pItem->Pos >= MAX_FIXEDSTALL_SELLITEM)
		return;
	memcpy(&m_SellItem[pItem->Pos], pItem, sizeof(stFixedStallItem));
}

void CStall::SetOneBuyItem(stFixedStallItem* pItem)
{
	if(!pItem || pItem->Pos >= MAX_FIXEDSTALL_BUYITEM)
		return;
	memcpy(&m_BuyItem[pItem->Pos], pItem, sizeof(stFixedStallItem));
}

void CStall::SetOneSellPet(stFixedStallPet* pPet)
{
	if(!pPet || pPet->Pos >= MAX_FIXEDSTALL_SELLPET)
		return;
	memcpy(&m_SellPet[pPet->Pos], pPet, sizeof(stFixedStallPet));
}

stFixedStall* CStall::GetInfo()
{
	return &m_Info;
}

stFixedStallItem* CStall::GetOneSellItem(U32 pos)
{
	return pos < MAX_FIXEDSTALL_SELLITEM ? &m_SellItem[pos] : NULL;
}

stFixedStallItem* CStall::GetOneBuyItem(U32 pos)
{
	return pos < MAX_FIXEDSTALL_BUYITEM ? &m_BuyItem[pos] : NULL;
}

stFixedStallPet* CStall::GetOneSellPet(U32 pos)
{
	return pos < MAX_FIXEDSTALL_SELLPET? &m_SellPet[pos] : NULL;
}

U32 CStall::GetSellItemVer(U32 pos)
{
	return pos < MAX_FIXEDSTALL_SELLITEM ? m_SellItem[pos].DataVersion : 0;
}

void CStall::SetSellItemVer(U32 pos)
{
	if(pos < MAX_FIXEDSTALL_SELLITEM) m_SellItem[pos].DataVersion++;
}

U32 CStall::GetBuyItemVer(U32 pos)
{
	return pos < MAX_FIXEDSTALL_BUYITEM ? m_BuyItem[pos].DataVersion : 0;
}

void CStall::SetBuyItemVer(U32 pos)
{
	if(pos < MAX_FIXEDSTALL_BUYITEM) m_BuyItem[pos].DataVersion++;
}

U32 CStall::GetSellPetVer(U32 pos)
{
	return pos < MAX_FIXEDSTALL_SELLPET ? m_SellPet[pos].DataVersion : 0;
}

void CStall::SetSellPetVer(U32 pos)
{
	if(pos < MAX_FIXEDSTALL_SELLPET) m_SellPet[pos].DataVersion++;
}

// ----------------------------------------------------------------------------
// ������������Ϣ(��ౣ�����50��)
void CStall::AddChatToList(const char* sender, const char* content)
{
	stStallChat* pChat = NULL;
	if(ChatList.size() >= MAXLEAVEWORDS)
	{
		pChat = ChatList.front();
		ChatList.pop_front();
	}
	else
		pChat = new stStallChat;
	
	_time32((__time32_t*)&pChat->time);
	dStrcpy(pChat->sender, MAX_NAME_LENGTH, sender);
	dStrcpy(pChat->content, 128, content);
	ChatList.push_back(pChat);
}

// ----------------------------------------------------------------------------
// �������������Լ�¼
void CStall::ClearChat()
{
	for(size_t i = 0; i < ChatList.size(); i++)
	{
		if(ChatList[i])
			delete ChatList[i];
	}
	ChatList.clear();
}

// ----------------------------------------------------------------------------
// �����ҽ�����Ϣ(��ౣ�����200��)
void CStall::AddRecordToList(U32 type, S32 money, S32 tax, const char* target, stItemInfo* item, S32 num, stPetInfo* pet)
{
	stStallRecord* pRecord = NULL;
	if(RecordList.size() >= MAXTRADERECORDS)
	{
		pRecord = RecordList.front();
		RecordList.pop_front();
		if(pRecord->type == 0 || pRecord->type == 2)
			delete pRecord->trade.item;
		else
			delete pRecord->trade.pet;
	}
	else
	{
		pRecord = new stStallRecord;
	}

	_time32((__time32_t*)&pRecord->time);
	pRecord->type = type;
	pRecord->money = money;
	pRecord->tax = tax;
	dStrcpy(pRecord->targetName, MAX_NAME_LENGTH, target);
	if(type == 0 || type == 2)
	{
		pRecord->trade.item = new stItemInfo;
		memcpy(pRecord->trade.item,item, sizeof(stItemInfo));
		pRecord->trade.item->Quantity = num;
	}
	else
	{
		pRecord->trade.pet = new stPetInfo;
		memcpy(pRecord->trade.pet, pet, sizeof(stPetInfo));
	}
	
	RecordList.push_back(pRecord);
}

// ----------------------------------------------------------------------------
// ����������׼�¼
void CStall::ClearRecord()
{
	for(size_t i = 0; i < RecordList.size(); i++)
	{
		if(RecordList[i])
			delete RecordList[i];
	}
	RecordList.clear();
}

// ============================================================================
// ��������
// ============================================================================
// ----------------------------------------------------------------------------
// �����չ���Ʒ���Ƿ����ָ����ͬ����Ʒ
bool CStall::FindSameItemInBuyItem(U32 itemId)
{
	for(int i = 0; i < MAX_FIXEDSTALL_BUYITEM; i++)
	{
		if(m_BuyItem[i].ItemID == itemId)
			return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
// ���̳�����Ʒ���ڲ���һ���ղ�λ
S16 CStall::FindEmptyPosFromSellItem()
{
	for(U16 i = 0; i < GetBuySellItems(); i++)
	{
		if(m_SellItem[i].ItemID == 0)
			return i;
	}
	return -1;
}

// ----------------------------------------------------------------------------
// ���̳����������ڲ���һ���ղ�λ
S16 CStall::FindEmptyPosFromSellPet()
{
	for(U16 i = 0; i < GetBuySellPets(); i++)
	{
		if(m_SellPet[i].petDataId == 0)
			return i;
	}
	return -1;
}

// ----------------------------------------------------------------------------
// �����չ���Ʒ���ڲ���һ���ղ�λ
S16 CStall::FindEmptyPosFromBuyItem()
{
	for(U16 i = 0; i < MAX_FIXEDSTALL_BUYITEM; i++)
	{
		if(m_BuyItem[i].ItemID == 0)
			return i;
	}
	return -1;
}
*/