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

/* osdMemUsage.c - Memory usage info: Linux implementation = use /proc/self/statm */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-06-08 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <devIocStats.h>

static unsigned long pagesize;

int devIocStatsInitMemUsage (void) {
    pagesize = sysconf(_SC_PAGESIZE); 
    return 0;
}

int devIocStatsGetMemUsage (memInfo *pval)
{
    static char statmfile[] = "/proc/self/statm";
    static char memfile[]   = "/proc/meminfo";
    unsigned long size, resident, value, total = 0, memfree = 0;
    char title[32] = "";
    char units[32] = "";
    int ret = 0;
    int found = 0;
    FILE *fp;

    fp = fopen(statmfile, "r");
    if (fp) {
        fscanf(fp, "%lu %lu", &size, &resident);
        fclose(fp);
    }

    fp = fopen(memfile, "r");
    if (fp) {
        while (ret != EOF && found < 4) {
            ret = fscanf(fp, "%31s %lu %s\n", title, &value, units);
            if (strcmp(title, "MemTotal:") == 0) {
                total = value * 1024;
                found++;
            } else if (strcmp(title, "MemFree:") == 0 ||
                       strcmp(title, "Buffers:") == 0 ||
                       strcmp(title, "Cached:") == 0) {
                memfree += value * 1024;
                found++;
            }
        }
        fclose(fp);
    }

    pval->numBytesAlloc = (double)resident * (double)pagesize;
    pval->numBytesFree  = (double)memfree;
    pval->numBytesTotal = (double)total;

    return 0;
}
