/*************************************************************************\
* Copyright (c) 2009-2010 Helmholtz-Zentrum Berlin
*     für Materialien und Energie.
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdCpuUsage.c - CPU usage info: posix implementation = use getrusage() */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-27 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *  2010-07-09 Ralph Lange (HZB/BESSY)
 *     Changed implementation to use getrusage()
 *
 */

#include <sys/time.h>
#include <sys/resource.h>

#include <epicsTime.h>
#include <devIocStats.h>

static epicsTimeStamp prevTime;
static double prevUsage;

int devIocStatsInitCpuUsage (void) {
    struct rusage stats;
    epicsTimeGetCurrent(&prevTime);
    getrusage(RUSAGE_SELF, &stats);
    prevUsage = stats.ru_utime.tv_sec + stats.ru_utime.tv_usec / 1e6;
    return 0;
}

int devIocStatsGetCpuUsage (double *pval)
{
    epicsTimeStamp curTime;
    double curUsage;
    double diffUsage;
    struct rusage stats;
    double diffTime;
    double cpuFract;

    epicsTimeGetCurrent(&curTime);
    getrusage(RUSAGE_SELF, &stats);
    curUsage = stats.ru_utime.tv_sec + stats.ru_utime.tv_usec / 1e6;
    diffTime = epicsTimeDiffInSeconds(&curTime, &prevTime);
    diffUsage = curUsage - prevUsage;

    cpuFract = (diffTime > 0) ? diffUsage / diffTime : 0.0;

    prevTime = curTime;
    prevUsage = curUsage;

    *pval = cpuFract * 100.0;
    return 0;
}
