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

/* osdWorkspaceUsage.c - Workspace usage info: default implementation = do nothing */

/*
 *  Author: Chengcheng Xu (SLAC)
 *
 */

#include <devIocStats.h>

int devIocStatsInitWorkspaceUsage (void) { return 0; }
int devIocStatsGetWorkspaceUsage (memInfo *pval) { return -1; }
