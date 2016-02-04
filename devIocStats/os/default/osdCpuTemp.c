/*************************************************************************\
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdCpuTemp.c - CPU Temperature: default implementation = do nothing */

/*
 *  Author: Jeong Han Lee (ESS)
 *
 *  Modification History
 *
 */

#include <devIocStats.h>

int devIocStatsInitCpuTemp (void) { return 0; }
int devIocStatsGetCpuTemp (tempInfo *pval) { return -1; }
