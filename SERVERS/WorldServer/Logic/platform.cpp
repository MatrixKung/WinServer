#include "platform.h"
#include <time.h>
#include <stdio.h>

void Platform::getLocalTime(LocalTime &lt)
{
	struct tm *systime;
	time_t long_time;

	time( &long_time );                // Get time as long integer.
	systime = localtime( &long_time ); // Convert to local time.

	lt.sec      = systime->tm_sec;
	lt.min      = systime->tm_min;
	lt.hour     = systime->tm_hour;
	lt.month    = systime->tm_mon;
	lt.monthday = systime->tm_mday;
	lt.weekday  = systime->tm_wday;
	lt.year     = systime->tm_year;
	lt.yearday  = systime->tm_yday;
	lt.isdst    = systime->tm_isdst;
}

U32 Platform::getTime()
{
	time_t long_time;
	time( &long_time );
	return long_time;
}

void Platform::getDateTime(U32 t, LocalTime &lt)
{
	errno_t err;
	tm systime;

	__time32_t long_time = (__time32_t)t;
	err = _localtime32_s(&systime,&long_time);
	if(err)
		return;

	lt.sec      = systime.tm_sec;
	lt.min      = systime.tm_min;
	lt.hour     = systime.tm_hour;
	lt.month    = systime.tm_mon;
	lt.monthday = systime.tm_mday;
	lt.weekday  = systime.tm_wday;
	lt.year     = systime.tm_year;
	lt.yearday  = systime.tm_yday;
	lt.isdst    = systime.tm_isdst;
}

// ----------------------------------------------------------------------------
// �ж�curTime�Ƿ���ָ��ʱ�����
// ����1�Ͳ���2��ʽ��ͬ��Ϊʱ��ֵ����: 093059
bool Platform::isInTime(U32 start, U32 end, U32 current)
{	
	tm t;
	if(_localtime32_s(&t,(__time32_t*)&current))
		return false;

	current = t.tm_hour*10000 + t.tm_min*100 + t.tm_sec;
	return (current >= start && current <= end);
}

// ----------------------------------------------------------------------------
// �ж�curTime�Ƿ���ָ����ʱ��
// ����1�Ͳ���2��ʽ��ͬ��Ϊ����ʱ���룬��:0429093059 -->���¶�ʮ����9��30��59��
bool Platform::isInMonthTime(U32 start, U32 end, U32 current)
{
	tm t;
	if(_localtime32_s(&t,(__time32_t*)&current))
		return false;

	current = (t.tm_mon+1)*100000000 + t.tm_mday*1000000 + 
		t.tm_hour* 10000 + t.tm_min*100 + t.tm_sec;
	return (current >= start && current <= end);
}

// ----------------------------------------------------------------------------
// �ж�curTime�Ƿ���ָ���ڼ���
// ����1�Ͳ���3��ʽ��ͬ��Ϊ����ֵ���磺20120411
// ����2�Ͳ���4��ʽ��ͬ��Ϊʱ��ֵ����: 093059
bool Platform::isInPeriod(U32 startdate, U32 starttime, U32 enddate, U32 endtime, U32 curTime)
{
	tm t;
	if(_localtime32_s(&t,(__time32_t*)&curTime))
		return false;

	U32 curDate = (t.tm_year+1900)*10000 + (t.tm_mon+1)*100 + t.tm_mday;
	curTime = t.tm_hour* 10000 + t.tm_min*100 + t.tm_sec;
	return (curDate >= startdate && curDate <= enddate) && 
		(curTime >= starttime && curTime <= endtime);
}

// ----------------------------------------------------------------------------
// ��ȡ�·����һ��
S32 Platform::getMonthLastDay(S32 year, S32 month)
{
	static S32 Month[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	month %= 12;
	S32 day = Month[month-1];

	if((year%400 == 0) || (year%4 == 0 && year%100 != 0))
	{
		if(month==2)
			day += 1;
	}
	return day;
}

// ----------------------------------------------------------------------------
// �����ͻ�ȡ�ض�ʱ��(����ֵ,��: 1201010000  --->2012��1��1��0��0��
U32 Platform::getNextTime(S32 intervalType)
{
	U32 cur;
	U32 year, month, day;
	tm t;

	_time32((__time32_t*)&cur);
	if(_localtime32_s(&t,(__time32_t*)&cur))
		return 0;

	if(intervalType == INTERVAL_YEAR)
	{
		year = t.tm_year + 1;
		month = day = 1;
	}
	else if(intervalType == INTERVAL_MONTH)
	{
		year = t.tm_year;
		month = t.tm_mon  + 1;
		if(month >= 12)
		{
			month = 0;
			year++;
		}
		day = 1;
	}
	else if(intervalType == INTERVAL_WEEK)
	{
		if(t.tm_wday != 0)
		{
			cur += ((8-t.tm_wday) * 24 * 3600);
		}
		else
		{
			cur += (24*3600);
		}

		if(_localtime32_s(&t,(__time32_t*)&cur))
			return 0;

		year = t.tm_year;
		month = t.tm_mon;
		day = t.tm_mday;
	}
	else
	{
		cur += (24*3600);
		if(_localtime32_s(&t,(__time32_t*)&cur))
			return 0;

		year = t.tm_year;
		month = t.tm_mon;
		day = t.tm_mday;
	}

	t.tm_year = year;
	t.tm_mon = month;
	t.tm_mday = day;
	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 0;

	cur = _mktime32(&t);
	return cur;
}
