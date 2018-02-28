#include "Define.h"
#include "dblib/dbLib.h"
#include "Base/Log.h"
#include "Common/PacketType.h"
#include "Common/MailBase.h"
#include "DBUtility.h"
#include "TBLMailList.h"

// ----------------------------------------------------------------------------
// ɾ���ʼ�����
DBError TBLMailList::Delete(int id)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		M_SQL(GetConn(),"DELETE FROM Tbl_MailList WHERE id=%d",id);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(id)
	return err;
}

// ----------------------------------------------------------------------------
// ɾ������ʼ�����
DBError TBLMailList::DeletePage(int nRecver,std::vector<int>& idlist)
{
	DBError err = DBERR_UNKNOWERR;
	if(idlist.empty())
		return err;
	try
	{		
		char szStr[20],szSQL[4096]={0};
		for(size_t i = 0; i < idlist.size(); i++)
		{
			if( i == 0)
				sprintf_s(szStr, 20, "%d", idlist[i]);
			else
				sprintf_s(szStr, 20, ",%d", idlist[i]);
			strcat_s(szSQL, 4096, szStr);
		}
		M_SQL(GetConn(),"EXECUTE Sp_DeleteMail %d,'%s'",nRecver, szSQL);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBECATCH()
	return err;
}

// ----------------------------------------------------------------------------
// ɾ���ʼ�����(������ȡ�ʼ���ɾ��)
DBError TBLMailList::DeleteAll(int nRecver,std::vector<int> &mailList)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{		
		int id = 0;
		M_SQL(GetConn(),"EXECUTE Sp_DeleteAllMail %d", nRecver);
		while(GetConn()->More())
		{
			id	= GetConn()->GetInt();
			mailList.push_back(id);
		}
		err = DBERR_NONE;
	}
	DBCATCH(nRecver)
	return err;
}

// ----------------------------------------------------------------------------
// �����ʼ�����
DBError TBLMailList::Save(int &nRecver, stMailItem& mailItem)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		U_INT_CHECK(mailItem.nSender);
		INT_CHECK(mailItem.nMoney);
		for(auto i = 0; i < MAX_MAIL_ITEM_NUM; ++i)
		{
			U_INT_CHECK(mailItem.nItemId[i]);
			U_SMALLINT_CHECK(mailItem.nItemCount[i]);
		}
		U_INT_CHECK(mailItem.nRecver);

		if(!StrSafeCheck(mailItem.szSenderName, _countof(mailItem.szSenderName)))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);
		if(!StrSafeCheck(mailItem.szRecverName, _countof(mailItem.szRecverName)))
			throw ExceptionResult(DBERR_DB_SQLPARAMETER);

		unsigned long OutSize = 64;
		TempAlloc<char> pOut(OutSize);
		SafeDBString(mailItem.title, 32, pOut, OutSize);

		char szSendTime[32] = {0};
		unsigned long curTime = (unsigned long)time(NULL);
		GetDBTimeString(curTime, szSendTime);
		M_SQL(GetConn(),"EXECUTE Sp_UpdateMailList  \
			%d,					 %d,				  '%s',					 %d,					%d,					%d,					%d,						%d,\
			%d,					 %d,				   %d,					 %d,					'%s',				%d,					'%s',				    %d,\
			'%s'",\
			mailItem.id,		mailItem.nSender, mailItem.szSenderName,	mailItem.nMoney,	mailItem.nItemId[0],   mailItem.nItemId[1], mailItem.nItemId[2],   mailItem.nItemId[3],\
	mailItem.nItemCount[0], mailItem.nItemCount[1], mailItem.nItemCount[2], mailItem.nItemCount[3],szSendTime,			nRecver,			mailItem.szRecverName, mailItem.isSystem, \
			(char*)pOut);
		if(GetConn()->More())
		{
			int ret = GetConn()->GetInt();
			if(ret == 0)
			{
				int id = GetConn()->GetInt();
				nRecver = GetConn()->GetInt();

				M_SQL(GetConn(),"SELECT msg FROM Tbl_MailList WHERE id=%d", id);
				if(GetConn()->Eval())
				{
					GetConn()->SetBlob("Tbl_MailList","msg",(PBYTE)mailItem.content, MAX_MAIL_ITEM_LENGTH);
				}
				mailItem.id = id;
				err = DBERR_NONE;
			}
			else
			{
				if(ret == 1)		//�ռ���ID������
					throw ExceptionResult(DBERR_MAIL_RECVERID);
				else if(ret == 2)	//�ռ������Ʋ�����
					throw ExceptionResult(DBERR_MAIL_RECVERNAME);
				else if(ret == 3)	//�ռ����ʼ��ﵽ����
					throw ExceptionResult(DBERR_MAIL_TOOMANY);
				else
					throw ExceptionResult(DBERR_UNKNOWERR);
			}
		}
		else
			throw ExceptionResult(DBERR_MAIL_UPDATEFAILED);
	}
	DBCATCH(mailItem.nSender)
	return err;
}

// ----------------------------------------------------------------------------
// ��ҳ��ѯ�ʼ�����(ע��stMailItem�б��ڵ��ú���delete��������й©��
//��nRecver  �ռ���
//  Info  ��ǰһҳ���ʼ�����
//  totalrow �ʼ���¼������
//	noreads �ʼ�δ����¼����
DBError TBLMailList::LoadForPage(int nRecver, std::vector<stMailItem>& Info, int& totalrow, int& noreads)
{
	totalrow = 0;
	noreads = 0;

	DBError err = DBERR_UNKNOWERR;
	try
	{		
		stMailItem mailItem;
		M_SQL(GetConn(),"EXECUTE Sp_LoadMailForPage %d", nRecver);
		while(GetConn()->More())
		{
			mailItem.id				= GetConn()->GetInt();
			mailItem.nSender		= GetConn()->GetInt();
			strncpy_s( mailItem.szSenderName, sizeof( mailItem.szSenderName ), GetConn()->GetString(), _TRUNCATE );
			mailItem.nMoney			= GetConn()->GetInt();
			for(auto i = 0; i < MAX_MAIL_ITEM_NUM; ++i)
				mailItem.nItemId[i]		= GetConn()->GetInt();
			for (auto i = 0; i < MAX_MAIL_ITEM_NUM; ++i)
				mailItem.nItemCount[i]  = GetConn()->GetInt();
			mailItem.nTime			= GetConn()->GetTime();
			mailItem.isReaded		= GetConn()->GetInt() != 0;
			if(!mailItem.isReaded)
				noreads++;
			mailItem.isSystem		= GetConn()->GetInt() != 0;
			if(GetConn()->GetInt()==1)
			{
				mailItem.nMoney			= 0;
				memset(mailItem.nItemId,	0, sizeof(mailItem.nItemId));
				memset(mailItem.nItemCount, 0, sizeof(mailItem.nItemCount));
			}
			strncpy_s( mailItem.title, sizeof( mailItem.title ), GetConn()->GetString(), _TRUNCATE );
			mailItem.nRecver		= nRecver;
			Info.push_back(mailItem);

			totalrow++;
		}

		//��ȡ������Ϣ
		for(std::vector<stMailItem>::iterator itr  = Info.begin(); itr != Info.end(); ++itr)
		{
			stMailItem* mailItem = &(*itr);
			if(mailItem)
			{
				M_SQL(GetConn(),"SELECT msg FROM Tbl_MailList WHERE id=%d", mailItem->id);
				if(GetConn()->Eval())
				{
					// ���ӻ�ȡ��Ʒ�ĸ������� [4/13/2010 LV]
					int len = MAX_MAIL_ITEM_LENGTH;
					GetConn()->GetBlob( len, (PBYTE)mailItem->content);
				}
			}
		}

		err = DBERR_NONE;
	}
	DBCATCH(nRecver)
	return err;
}


// ----------------------------------------------------------------------------
// �����ʼ�����
DBError TBLMailList::Load(int id, stMailItem& mailItem, bool bLoadMsg )
{
	DBError err = DBERR_UNKNOWERR;
	try
	{		
		M_SQL(GetConn(),"SELECT id,sender,sender_name,money,itemId0,itemId1,itemId2,itemId3,\
			item_count0, item_count1, item_count2, item_count3, send_time,\
			readed,isSystem,recvFlag,title,recver FROM Tbl_MailList WHERE id=%d", id);
		if(GetConn()->More())
		{
			mailItem.id				= GetConn()->GetInt();
			mailItem.nSender		= GetConn()->GetInt();
			strncpy_s( mailItem.szSenderName, sizeof( mailItem.szSenderName ), GetConn()->GetString(), _TRUNCATE );
			mailItem.nMoney			= GetConn()->GetInt();
			for (auto i = 0; i < MAX_MAIL_ITEM_NUM; ++i)
				mailItem.nItemId[i]	= GetConn()->GetInt();
			for (auto i = 0; i < MAX_MAIL_ITEM_NUM; ++i)
				mailItem.nItemCount[i]= GetConn()->GetInt();
			mailItem.nTime			= GetConn()->GetTime();
			mailItem.isReaded		= GetConn()->GetInt() != 0;
			mailItem.isSystem		= GetConn()->GetInt() != 0;

			if(GetConn()->GetInt() == 1)
			{
				mailItem.nMoney			= 0;
				memset(mailItem.nItemId,	0, sizeof(mailItem.nItemId));
				memset(mailItem.nItemCount, 0, sizeof(mailItem.nItemCount));
			}

			strncpy_s( mailItem.title, sizeof( mailItem.title ), GetConn()->GetString(), _TRUNCATE );
			mailItem.nRecver		= GetConn()->GetInt();
		}
		else
			throw ExceptionResult(DBERR_ACCOUNT_NOFOUND);

		if( bLoadMsg )
		{
			M_SQL(GetConn(),"SELECT msg FROM Tbl_MailList WHERE id=%d", id);
			if(GetConn()->Eval())
			{
				// ���ӻ�ȡ��Ʒ�ĸ������� [4/13/2010 LV]
				int len = MAX_MAIL_ITEM_LENGTH;
				GetConn()->GetBlob( len, (PBYTE)mailItem.content);
			}
		}
		err = DBERR_NONE;
	}
	DBCATCH(id)
	return err;
}

// ----------------------------------------------------------------------------
// ���������ʼ�����
DBError TBLMailList::LoadAll(int nRecver, std::vector<stMailItem*>& Info)
{
	DBError err = DBERR_UNKNOWERR;
	stMailItem* mailItem;

	try
	{
		M_SQL(GetConn(),"SELECT id,sender,sender_name,money,itemId0,itemId1,itemId2,itemId3,\
			item_count0,item_count1,item_count2,item_count3,send_time,\
			readed,isSystem,recvFlag,title, recver FROM Tbl_MailList WHERE recver=%d", nRecver);
		while(GetConn()->More())
		{
			mailItem = new stMailItem;
			mailItem->id			= GetConn()->GetInt();
			mailItem->nSender		= GetConn()->GetInt();
			strncpy_s( mailItem->szSenderName, sizeof( mailItem->szSenderName ), GetConn()->GetString(), _TRUNCATE );
			mailItem->nMoney		= GetConn()->GetInt();
			for (auto i = 0; i < MAX_MAIL_ITEM_NUM; ++i)
				mailItem->nItemId[i]= GetConn()->GetInt();
			for (auto i = 0; i < MAX_MAIL_ITEM_NUM; ++i)
				mailItem->nItemCount[i]= GetConn()->GetInt();
			mailItem->nTime			= GetConn()->GetTime();
			mailItem->isReaded		= GetConn()->GetInt() != 0;
			mailItem->isSystem		= GetConn()->GetInt() != 0;
			if(GetConn()->GetInt() == 1)
			{
				mailItem->nMoney			= 0;
				memset(mailItem->nItemId,	0, sizeof(mailItem->nItemId));
				memset(mailItem->nItemCount, 0, sizeof(mailItem->nItemCount));
			}
			strncpy_s( mailItem->title, sizeof( mailItem->title ), GetConn()->GetString(), _TRUNCATE );
			mailItem->nRecver		= GetConn()->GetInt();
			Info.push_back(mailItem);
		}

		for(size_t i = 0; i < Info.size(); i++)
		{
			mailItem = Info[i];
			M_SQL(GetConn(),"SELECT msg FROM Tbl_MailList WHERE id=%d", mailItem->id);
			if (GetConn()->Eval())
			{
				// ���ӻ�ȡ��Ʒ�ĸ������� [4/13/2010 LV]
				int len = MAX_MAIL_ITEM_LENGTH;
				GetConn()->GetBlob(len, (PBYTE)mailItem->content);
			}
		}
		err = DBERR_NONE;
	}
	DBCATCH(nRecver)
	return err;
}

// ----------------------------------------------------------------------------
// �����ʼ��Ѷ���־
DBError TBLMailList::UpdateReaded(int id)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{
		
		M_SQL(GetConn(),"UPDATE Tbl_MailList SET readed=1 WHERE id=%d", id);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(id)
	return err;
}

// ----------------------------------------------------------------------------
// ��ȡ�ʼ���Ǯ����Ʒ����
DBError TBLMailList::setMailRecv(int id)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{		
		M_SQL(GetConn(),"UPDATE Tbl_MailList SET recvFlag=1 WHERE id=%d", id);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(id)
	return err;
}

// ----------------------------------------------------------------------------
// ������ȡ�ʼ���Ǯ����Ʒ����
DBError TBLMailList::resetMailRecv(int id)
{
	DBError err = DBERR_UNKNOWERR;
	try
	{		
		M_SQL(GetConn(),"UPDATE Tbl_MailList SET recvFlag=0 WHERE id=%d", id);
		GetConn()->Exec();
		err = DBERR_NONE;
	}
	DBCATCH(id)
		return err;
}