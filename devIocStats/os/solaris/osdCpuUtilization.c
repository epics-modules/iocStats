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

/* osdCpuUtilization.c - CPU utilization info: Solaris implementation = read /proc/PID/psinfo */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <procfs.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <epicsTime.h>
#include <devIocStats.h>

static epicsTimeStamp oldTime;
static double oldUsage;
static double scale;
static char statfile[30];

static double usageFromProc (void) {
    int fd;
    psinfo_t pstat;

    if ((fd = open(statfile, O_RDONLY)) < 0) {
        return 0.;
    }
    if (read(fd, &pstat, sizeof(psinfo_t)) != sizeof(psinfo_t)) {
        close(fd);
        return 0.;
    }
    close(fd);
    return pstat.pr_time.tv_sec + pstat.pr_time.tv_nsec / 1e9;
}

int devIocStatsInitCpuUtilization (loadInfo *pval) {
    snprintf(statfile, sizeof(statfile), "/proc/%ld/psinfo", getpid());
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
