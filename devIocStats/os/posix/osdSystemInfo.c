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

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-20 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#include <sys/utsname.h>
#include <devIocStats.h>

static char *notavail = "<not available>";
static struct utsname uts;
static char* versionstring;

int devIocStatsInitSystemInfo (void) {
    uname(&uts);
    versionstring = calloc(strlen(uts.sysname)+strlen(uts.release)+strlen(uts.machine)+3, 1);
    strcat(versionstring, uts.sysname);
    strcat(versionstring, " ");
    strcat(versionstring, uts.release);
    strcat(versionstring, " ");
    strcat(versionstring, uts.machine);
    return 0;
}

int devIocStatsGetBSPVersion (char **pval)
{
    *pval = notavail;
    return -1;
}

int devIocStatsGetKernelVersion (char **pval)
{
    *pval = versionstring;
    return 0;
}
