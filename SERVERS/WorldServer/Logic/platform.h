#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <stdio.h>
#include <string>
#include "BASE/types.h"

struct Platform
{
	enum enTimeSetInt
	{
		INTERVAL_DAY	= 1,	//��ǰʱ��ĵڶ���
		INTERVAL_WEEK	= 2,	//��ǰʱ�������һ
		INTERVAL_MONTH	= 3,	//��ǰʱ�����һ�µ�һ��)
		INTERVAL_YEAR	= 4,	//��ǰʱ�����һ���һ��)
		TIME_SET_MAX_VAL,       //�������ֵ
	};

	// Time
	struct LocalTime
	{
		U8  sec;        ///< Seconds after minute (0-59)
		U8  min;        ///< Minutes after hour (0-59)
		U8  hour;       ///< Hours after midnight (0-23)
		U8  month;      ///< Month (0-11; 0=january)
		U8  monthday;   ///< Day of the month (1-31)
		U8  weekday;    ///< Day of the week (0-6, 6=sunday)
		U16 year;       ///< Current year minus 1900
		U16 yearday;    ///< Day of year (0-365)
		bool isdst;     ///< True if daylight savings time is active

		LocalTime(){ memset(this,0,sizeof(LocalTime)); }
	};

	static void getLocalTime(LocalTime &);
	static U32  getTime();
	static void getDateTime(U32, LocalTime&);
	static bool isInTime(U32 start, U32 end, U32 current);
    static bool isInMonthTime(U32 start, U32 end, U32 current);
	static bool isInPeriod(U32 startdate, U32 starttime, U32 enddate, U32 endtime, U32 curTime);
	static S32 getMonthLastDay(S32 year, S32 month);
	static U32 getNextTime(S32 intervalType);
};

#endif