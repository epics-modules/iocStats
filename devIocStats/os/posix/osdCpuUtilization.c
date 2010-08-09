/*************************************************************************\
* Copyright (c) 2009-2010 Helmholtz-Zentrum Berlin
*     fuer Materialien und Energie GmbH.
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdCpuUtilization.c - CPU utilization info: posix implementation = use getrusage() */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *
 */

#include <sys/time.h>
#include <sys/resource.h>

#include <epicsTime.h>
#include <devIocStats.h>

static epicsTimeStamp oldTime;
static double oldUsage;
static double scale;

int devIocStatsInitCpuUtilization (loadInfo *pval) {
    struct rusage stats;

    epicsTimeGetCurrent(&oldTime);
    getrusage(RUSAGE_SELF, &stats);
    oldUsage = stats.ru_utime.tv_sec + stats.ru_utime.tv_usec / 1e6 +
               stats.ru_stime.tv_sec + stats.ru_stime.tv_usec / 1e6;
    scale = 100.0f / ((double)TICKS_PER_SEC * NO_OF_CPUS);
    pval->noOfCpus = NO_OF_CPUS;
    return 0;
}

int devIocStatsGetCpuUtilization (loadInfo *pval)
{
    epicsTimeStamp curTime;
    double curUsage;
    struct rusage stats;
    double elapsed;
    double cpuFract;

    epicsTimeGetCurrent(&curTime);
    getrusage(RUSAGE_SELF, &stats);
    curUsage = stats.ru_utime.tv_sec + stats.ru_utime.tv_usec / 1e6 +
               stats.ru_stime.tv_sec + stats.ru_stime.tv_usec / 1e6;
    elapsed = epicsTimeDiffInSeconds(&curTime, &oldTime);

    cpuFract = (elapsed > 0) ? (curUsage - oldUsage) * scale / elapsed : 0.0;

    oldTime = curTime;
    oldUsage = curUsage;

    pval->iocLoad = cpuFract;
    return 0;
}
