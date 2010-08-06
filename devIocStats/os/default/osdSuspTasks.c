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

/* osdSuspTasks.c - Number of suspended tasks: default implementation = use EPICS task watchdog */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-20 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#include <taskwd.h>
#include <epicsThread.h>
#include <devIocStats.h>

static int suspendedTasks = 0;

static void taskFault(void *user, epicsThreadId tid)
{
    suspendedTasks++;
}

int devIocStatsInitSuspTasks (void)
{
    taskwdAnyInsert(NULL, taskFault, NULL);
    return 0;
}

int devIocStatsGetSuspTasks (int *pval)
{
    *pval = suspendedTasks;
    return 0;
}
