/*************************************************************************\
* Copyright (c) 2009 Helmholtz-Zentrum Berlin fuer Materialien und Energie.
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdCpuUsage.c - CPU usage info: posix implementation = use clock() */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  based on code written by Ken Evans for the CA Gateway
 *
 *  Modification History
 *  2009-05-27 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#ifdef SOLARIS
/* Is in stdlib.h elsewhere, not available on WIN32 */
#include <sys/loadavg.h>
#endif

#ifdef WIN32
#else
#include <unistd.h>
#endif

#include <limits.h>
#include <epicsTime.h>
#include <devIocStats.h>

#define ULONG_DIFF(n1,n2) (((n1) >= (n2))?((n1)-(n2)):((n1)+(ULONG_MAX-(n2))))

static epicsTimeStamp prevTime;
static unsigned long cpuPrevCount;

int devIocStatsInitCpuUsage (void) {
    epicsTimeGetCurrent(&prevTime);
    cpuPrevCount = (unsigned long)clock();
    return 0;
}

int devIocStatsGetCpuUsage (double *pval)
{
    epicsTimeStamp curTime;
    double delTime;
    double cpuFract;
    /* For WIN32, clock returns wall clock so cpuFract always is 1. */
    unsigned long cpuCurCount = (unsigned long)clock();

    epicsTimeGetCurrent(&curTime);
    delTime = epicsTimeDiffInSeconds(&curTime, &prevTime);

    /* Calculate the CPU Fract
    Note: clock() returns (long)-1 if it can't find the time;
    however, we can't distinguish that -1 from a -1 owing to
    wrapping.  So treat the return value as an unsigned long and
    don't check the return value. */
    cpuFract = (delTime > 0) ? (double)(ULONG_DIFF(cpuCurCount,cpuPrevCount))/delTime/CLOCKS_PER_SEC : 0.0;
    if (cpuFract > 1.0) cpuFract = 1.0;

    /* Reset the previous values */
    prevTime = curTime;
    cpuPrevCount = cpuCurCount;

    *pval = cpuFract * 100.0;
    return 0;
}
