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

/* osdCpuUtilization.c - CPU utilization info: linux implementation = read /proc/self/stat */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *
 */

#include <unistd.h>
#include <stdio.h>

#include <epicsTime.h>
#include <devIocStats.h>

static epicsTimeStamp oldTime;
static double oldUsage;
static double scale;

static double usageFromProc (void) {
    static char statfile[] = "/proc/self/stat";
    char sd[80];
    char cd;
    int id;
    unsigned int ud;
    long ld;
    long sticks = 0;
    long uticks = 0;
    FILE *fp;

    fp = fopen(statfile, "r");
    if (fp) {
        fscanf(fp, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu",
               &id, sd, &cd, &id, &id, &id, &id, &id, &ud, &ld, &ld, &ld, &ld, &uticks, &sticks );
        fclose(fp);
    }
    return (uticks + sticks) / (double)TICKS_PER_SEC;
}

int devIocStatsInitCpuUtilization (loadInfo *pval) {
    epicsTimeGetCurrent(&oldTime);
    oldUsage = usageFromProc();
    scale = 100.0f / NO_OF_CPUS;
    pval->noOfCpus = NO_OF_CPUS;
    return 0;
}

int devIocStatsGetCpuUtilization (loadInfo *pval)
{
    epicsTimeStamp curTime;
    double curUsage;
    double elapsed;
    double cpuFract;

    epicsTimeGetCurrent(&curTime);
    curUsage = usageFromProc();
    elapsed = epicsTimeDiffInSeconds(&curTime, &oldTime);

    cpuFract = (elapsed > 0) ? (curUsage - oldUsage) * scale / elapsed : 0.0;

    oldTime = curTime;
    oldUsage = curUsage;

    pval->iocLoad = cpuFract;
    return 0;
}
