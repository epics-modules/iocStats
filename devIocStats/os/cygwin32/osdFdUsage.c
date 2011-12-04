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

/* osdFdUsage.c - File descriptor usage: Linux implementation = use /proc/self/fd and getrlimit() */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-28 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#include <sys/resource.h>
#include <dirent.h>
#include <stdlib.h>

#include <devIocStats.h>

int devIocStatsInitFDUsage (void) { return 0; }

int devIocStatsGetFDUsage (fdInfo *pval)
{
    static char fddir[]="/proc/self/fd";
    DIR *pdir;
    struct dirent *pdit;
    struct rlimit lim;
    int i = 0;

    if ((pdir = opendir(fddir)) == NULL) return -1;
    while ((pdit = readdir(pdir)) != NULL) i++;
    if (closedir(pdir) == -1) return -1;
    pval->used = i - 3; /* Don't count this operation, '.' and '..' */

    if (getrlimit(RLIMIT_NOFILE, &lim)) return -1;
    pval->max = lim.rlim_cur;

    return 0;
}
