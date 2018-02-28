#ifndef CORPS_BASE_H
#define CORPS_BASE_H

enum CorpsError
{
	CORPSOP_OK,
	CORPSERR_Already,
	CORPSERR_NoTeam,
	CORPSERR_OtherNoTeam,
	CORPSERR_NotTeamLeader,
	CORPSERR_AlreadInCorps,
	CORPSERR_OtherAlreadInCorps,
	CORPSERR_NotInCorps,
	CORPSERR_NotAuthorize,
	CORPSERR_Offline,
	CORPSERR_TeamFull,
	CORPSERR_Reject,
	CORPSERR_TeamNotInCorps
};

static const char* strCorpsError[] =
{
	"�ɹ�!",
	"�Ѿ�",
	"��û�ж���",
	"�Է�û�ж���",
	"�����Ƕӳ�",
	"���Ѿ����Ŷ�����",
	"�Է��Ѿ����Ŷ�����",
	"�������Ŷ���",
	"��û��Ȩ��",
	"",
	"�Ŷ��ж�������",
	"�Է��ܾ����������",
	"�Ŷ���û�д˶���"
};

#define MAX_CORPS_TEAM_NUM 6

struct ZoneClientCorps
{
	int masterTeam;

	ZoneClientCorps(int master)
		:masterTeam(master)
	{
	}

	ZoneClientCorps()
	{
		memset(this, 0, sizeof(ZoneClientCorps));
	}

	template<typename _Ty>
	void WriteData(_Ty* packet) const
	{
		packet->writeBits(sizeof(ZoneClientCorps)<<3, this);
	}

	template<typename _Ty>
	void ReadData(_Ty* packet)
	{
		packet->readBits(sizeof(ZoneClientCorps)<<3, this);
	}
};

#endif