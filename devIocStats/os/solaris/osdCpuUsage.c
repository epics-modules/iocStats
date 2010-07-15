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

/* osdCpuUsage.c - CPU Usage: Solaris implementation = read from kernel stats */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *
 */

#include <stdio.h>
#include <kstat.h>

#include <epicsTime.h>
#include <devIocStats.h>

static epicsTimeStamp oldTime;
static double oldUsage;
static kstat_ctl_t *kc = NULL;

static double cpuFromKstat (void) {
    uint64_t user = 0;
    uint64_t sys = 0;
    kstat_t *ks;
    kstat_named_t *kn;
    unsigned int cpu;

    for (cpu = 0; cpu < NO_OF_CPUS; cpu++) {
        ks = kstat_lookup(kc, "cpu", cpu, "sys");
        if (kstat_read(kc, ks, 0) == -1) {
            perror("kstat_read");
            return 0.0;
        }
        if ((kn = kstat_data_lookup(ks, "cpu_ticks_kernel"))) {
            sys += kn->value.ui64;
        }
        if ((kn = kstat_data_lookup(ks, "cpu_ticks_user"))) {
            user += kn->value.ui64;
        }
    }
    return (user + sys) / (double)TICKS_PER_SEC;
}

int devIocStatsInitCpuUsage (void) {
    if ((kc = kstat_open()) == NULL) {
        perror("kstat_read");
    }
    epicsTimeGetCurrent(&oldTime);
    oldUsage = cpuFromKstat();
    return 0;
}

int devIocStatsGetCpuUsage (loadInfo *pval) {
    epicsTimeStamp curTime;
    double curUsage;
    double elapsed;
    double cpuFract;

    epicsTimeGetCurrent(&curTime);
    curUsage = cpuFromKstat();
    elapsed = epicsTimeDiffInSeconds(&curTime, &oldTime);

    cpuFract = (elapsed > 0) ? 100 * (curUsage - oldUsage) / (elapsed * NO_OF_CPUS): 0.0;

    oldTime = curTime;
    oldUsage = curUsage;

    pval->cpuLoad = cpuFract;
    return 0;
}
