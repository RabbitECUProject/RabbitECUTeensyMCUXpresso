//*******************************************************************
// <time.h>             Copyright (C) Codemist 2007
//
// Portions Copyright (c) Code Red Technologies Ltd., 2008-13
// Portions Copyright (c) NXP Semiconductors, 2013-14
//*******************************************************************

#ifndef __TIME_H_INCLUDED
#define __TIME_H_INCLUDED

#include <sys/redlib_version.h>

#ifndef __CODEMIST
#define __CODEMIST
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#include <sys/libconfig.h>

#define CLOCKS_PER_SEC __CLK_TCK

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
#if __SIZEOF_INT < __SIZEOF_VOID_P
typedef unsigned long size_t;
#else
typedef unsigned int size_t;
#endif
#endif

/*
 * The types here are probably pretty dubious and would lead to
 * significant risk of overflow.
 */

#ifndef __CLOCK_T_DEFINED
#define __CLOCK_T_DEFINED
typedef unsigned int clock_t;    /* cpu time type */
#endif

#ifndef __TIME_T_DEFINED
#define __TIME_T_DEFINED
typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */
#endif

#ifndef __STRUCT_TM_DEFINED
#define __STRUCT_TM_DEFINED

struct tm 
{
    int tm_sec;   /* seconds after the minute, 0 to 60
                     (0 - 60 allows for the occasional leap second) */
    int tm_min;   /* minutes after the hour, 0 to 59 */
    int tm_hour;  /* hours since midnight, 0 to 23 */
    int tm_mday;  /* day of the month, 1 to 31 */
    int tm_mon;   /* months since January, 0 to 11 */
    int tm_year;  /* years since 1900 */
    int tm_wday;  /* days since Sunday, 0 to 6 */
    int tm_yday;  /* days since January 1, 0 to 365 */
    int tm_isdst; /* Daylight Savings Time flag */
};
   /* struct tm holds the components of a calendar time, called the broken-down
    * time. The value of tm_isdst is positive if Daylight Savings Time is in
    * effect, zero if Daylight Savings Time is not in effect, and negative if
    * the information is not available.
    */
#endif

extern clock_t clock(void);
extern double difftime(time_t time1, time_t time0);
extern time_t mktime(struct tm *timeptr);
extern time_t time(time_t *timer);
extern char *asctime(const struct tm *timeptr);
extern char *ctime(const time_t *timer);
extern struct tm *gmtime(const time_t *timer);
extern struct tm *localtime(const time_t *timer);
extern size_t strftime(char * restrict s,
                       size_t maxsize,
                       const char * restrict format,
                       const struct tm * restrict timeptr);

#endif /* __TIME_H_INCLUDED */

/* end of <time.h> */
