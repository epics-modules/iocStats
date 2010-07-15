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

/* osdCpuUtilization.c - CPU Utilization: default implementation = do nothing */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *
 */

#include <devIocStats.h>

int devIocStatsInitCpuUtilization (loadInfo *pval) {
    pval->noOfCpus = NO_OF_CPUS;
    return 0;
}

int devIocStatsGetCpuUtilization (loadInfo *pval) { return -1; }
