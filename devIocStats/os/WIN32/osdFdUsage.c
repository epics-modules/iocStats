/*************************************************************************\
 * Copyright (c) 2012 Paul-Scherrer Institut(PSI)
* Copyright (c) 2009 Helmholtz-Zentrum Berlin fuer Materialien und Energie.
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdFdUsage.c - File descriptor usage: default implementation = do nothing */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *   2012-03-16 Helge Brands (PSI) 
 *  2009-05-20 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#if defined(__MINGW32__) || defined(__MINGW64__)
#define WINVER 0x0501
#endif

#include <devIocStats.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int devIocStatsInitFDUsage (void) { return 0; }
int devIocStatsGetFDUsage (fdInfo *pval) {
    DWORD handlecount;
    
    GetProcessHandleCount(GetCurrentProcess(),&handlecount);
    pval->max=16384;
    pval->used=(int)handlecount;
    
    return 0;
}
