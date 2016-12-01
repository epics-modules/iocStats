/*************************************************************************\
* EPICS BASE Versions 3.14.15
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdSysZoneTemp.c - Temperature of the themal zone 0 : default implementation = do nothing */

/*
 *  Author: Jeong Han Lee (ESS)
 *
 *  Modification History
 *
 */

#include "devIocStats.h"

int devIocStatsInitSysZoneTemp (void) { return 0; }
int devIocStatsGetSysZoneTemp (tempInfo *pval) { return -1; }
