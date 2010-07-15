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

/* osdCpuUsage.c - CPU Usage: Linux implementation = use /proc/stat */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *
 */

#include <stdio.h>

#include <epicsTime.h>
#include <devIocStats.h>

static epicsTimeStamp oldTime;
static double oldUsage;

static double cpuFromProc (void) {
    static char statfile[] = "/proc/stat";
    long user = 0;
    long nice = 0;
    long sys = 0;
    FILE *fp;

    fp = fopen(statfile, "r");
    if (fp) {
        fscanf(fp, "cpu %lu %lu %lu", &user, &nice, &sys);
        fclose(fp);
    }
    return (user + nice + sys) / (double)TICKS_PER_SEC;
}

int devIocStatsInitCpuUsage (void) {
    epicsTimeGetCurrent(&oldTime);
    oldUsage = cpuFromProc();
    return 0;
}

int devIocStatsGetCpuUsage (loadInfo *pval) {
    epicsTimeStamp curTime;
    double curUsage;
    double elapsed;
    double cpuFract;

    epicsTimeGetCurrent(&curTime);
    curUsage = cpuFromProc();
    elapsed = epicsTimeDiffInSeconds(&curTime, &oldTime);

    cpuFract = (elapsed > 0) ? 100 * (curUsage - oldUsage) / (elapsed * NO_OF_CPUS): 0.0;

    oldTime = curTime;
    oldUsage = curUsage;

    pval->cpuLoad = cpuFract;
    return 0;
}
