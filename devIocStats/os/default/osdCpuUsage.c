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

/* osdCpuUsage.c - CPU Usage: default implementation = do nothing */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-20 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *  2010-07-14 Ralph Lange (HZB/BESSY)
 *     Changed interface when adding CPU Utilization
 *
 */

#include <devIocStats.h>

int devIocStatsInitCpuUsage (void) { return 0; }
int devIocStatsGetCpuUsage (loadInfo *pval) { return -1; }
