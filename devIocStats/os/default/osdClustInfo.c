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

/* osdClustInfo.c - Memory buffer cluster usage: default implementation = do nothing */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-19 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#include <devIocStats.h>

int devIocStatsInitClusterInfo (void) { return 0; }
int devIocStatsGetClusterInfo (int pool, clustInfo *pval) { return -1; }
int devIocStatsGetClusterUsage (int pool, int *pval) { return -1; }
