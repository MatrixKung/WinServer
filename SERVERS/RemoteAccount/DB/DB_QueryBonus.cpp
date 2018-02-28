//#include "stdafx.h"
#include "DB_QueryBonus.h"
#include "DBContext.h"
#include "Common\BonusBase.h"
#include "quicklz\quicklz.h"

DB_QueryBonus::DB_QueryBonus()
:playerId(0), iBonusType(0), iAccountId(0), socketId(0)
{
}

int DB_QueryBonus::Execute(int ctxId, void* param)
{
	if (DB_CONTEXT_NOMRAL == ctxId)
	{
		CDBConn* pDBConn = (CDBConn*)param;
		IF_ASSERT(pDBConn == NULL)
			return PACKET_ERROR; //TODO: ����Ƿ��ͷ���Դ

		try
		{
			M_SQL(pDBConn, "EXECUTE UPS_QueryItemList %d,%d,%d", iAccountId, playerId, iBonusType);

			int iIndex = 0;

			char* pBuf = new char[MAX_PACKET_SIZE];
			char* pData = new char[MAX_PACKET_SIZE];
			char* zipBuf = new char[MAX_PACKET_SIZE];

			Base::BitStream sendPacket(pBuf, MAX_PACKET_SIZE);
			Base::BitStream dataStream(pData, MAX_PACKET_SIZE);

			while (pDBConn->More()) //ִ��SQL��䲢ȡ�������
			{
				stBonusItem stItem;
				stItem.uId = pDBConn->GetInt();
				stItem.itemId = pDBConn->GetInt();
				stItem.quantity = pDBConn->GetInt();
				stItem.type = iBonusType;
				if (pDBConn->GetInt() > 0)
					stItem.receiveFlag = RECEIVE_END;
				else
					stItem.receiveFlag = RECEIVE_CAN;

				strcpy_s(stItem.szDesc,sizeof(stItem.szDesc), pDBConn->GetString());
				strcat_s(stItem.szDesc,sizeof(stItem.szDesc), pDBConn->GetString());

				iIndex++;
				stItem.WriteData(&dataStream);
				//�޶�����¼��ΪMAX_ITEMBONUS_COUNT(48)��������ʱ�����뷢һ�ΰ�
				//��ΪMAX_PACKET_SIZE=32*1024 = 32768
				//���ͻ��˽���Ļ����С = sizeof(stBonusItem) * MAX_ITEMBONUS_COUNT = 528*48 = 25344
				//������һ�ο��Է��ļ�¼�� = 32768 / 528 = 62
				//֮���Զ���ΪMAX_ITEMBONUS_COUNT = 48��Ӧ���Ǵ��ڲ߻����趨������ǰֻ����ʾ���48�������ÿҳ12������4ҳ
				//������Ҫ��չ������ҳ����һ�η������Ӧ��Ϊ48�����������
				if(iIndex % MAX_ITEMBONUS_COUNT == 0) 
				{
					sendPacket.setPosition(0);
					stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, REMOTEACCOUNT_ZONE_QUERYBONUS_RESPONSE, playerId, SERVICE_ZONESERVER);
					int zipSize = MAX_PACKET_SIZE;
					int iret = SXZ_COMPRESS(zipBuf, &zipSize, pData, dataStream.getPosition());
					if (sendPacket.writeFlag(!iret)) //��ѹ�Ƿ�ɹ�
					{
						sendPacket.writeInt(zipSize, Base::Bit32);
						sendPacket.writeBits(zipSize << 3, zipBuf); //��������3��������8������ȡ��λ����
						sendPacket.writeInt(iIndex, Base::Bit16); //�����ţ�65535
					}

					pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
					CRemoteAccount::getInstance()->getServerSocket()->Send(socketId, sendPacket);
					dataStream.setPosition(0);
					//���(Ϊ��һ�η�����׼��)
					memset(pBuf,0, sizeof(char) * MAX_PACKET_SIZE);
					memset(pData,0, sizeof(char) * MAX_PACKET_SIZE);
					memset(zipBuf,0, sizeof(char) * MAX_PACKET_SIZE);
					sendPacket.clear();
					dataStream.clear();
				}
			}
			//����ѯ�����ݿ��¼��
			if(iIndex > 0 && iIndex % MAX_ITEMBONUS_COUNT != 0)
			{
				sendPacket.setPosition(0);
				stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, REMOTEACCOUNT_ZONE_QUERYBONUS_RESPONSE, playerId, SERVICE_ZONESERVER);
				dataStream.writeInt(iIndex, Base::Bit8);
				int zipSize = MAX_PACKET_SIZE;
				int iret = SXZ_COMPRESS(zipBuf, (size_t *)&zipSize, pData, dataStream.getPosition());
				if (sendPacket.writeFlag(!iret)) //��ѹ�Ƿ�ɹ�
				{
					sendPacket.writeInt(zipSize, Base::Bit32);
					sendPacket.writeBits(zipSize << 3, zipBuf);
					sendPacket.writeInt(iIndex, Base::Bit16);
				}
				pSendHead->PacketSize = sendPacket.getPosition() - sizeof(stPacketHead);
				CRemoteAccount::getInstance()->getServerSocket()->Send(socketId, sendPacket);
			}

			delete[] pBuf;
			delete[] pData;
			delete[] zipBuf;
		}
		catch (CDBException &e)
		{
			g_Log.WriteFocus("<<ERROR>>msg=%s, dberr=%s, ecode=%d, func="__FUNCTION__, e.w_msgtext, e.m_dberrstr, e.m_dberr);
		}
		catch (...)
		{
			g_Log.WriteError("RemoteAccount handle QueryBonus error account=%d,playerId=%d,iType=%d",iAccountId,playerId,iBonusType);
		}
	}
	return PACKET_OK;
}