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

/* osdSystemInfo.c - System info strings: posix implementation = use uname() */

#include <devIocStats.h>

static char *notavail = "<not available>";

int devIocStatsInitSystemInfo (void) {
    return 0;
}

int devIocStatsGetBSPVersion (char **pval)
{
    *pval = notavail;
    return -1;
}

int devIocStatsGetKernelVersion (char **pval)
{
    *pval = notavail;
    return 0;
}
