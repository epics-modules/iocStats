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

/* osdMemUsage.c - Memory usage info: default implementation = do nothing */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2015-10-22 Keenan Lang (APS)
 *  2012-03-16 Helge Brands (PSI)
 *  2009-05-13 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#if defined(__MINGW32__) || defined(__MINGW64__)
#define WINVER 0x0501
#endif

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <psapi.h>
#pragma comment (lib, "Psapi.lib")


#include <devIocStats.h>
#define DIV 1024.0
int devIocStatsInitMemUsage (void) { return 0; }
int devIocStatsGetMemUsage (memInfo *pval) {
    MEMORYSTATUSEX statex;
    PROCESS_MEMORY_COUNTERS_EX procmem;

    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx(&statex);

    GetProcessMemoryInfo(GetCurrentProcess(),(PPROCESS_MEMORY_COUNTERS)&procmem,sizeof(procmem));

    pval->numBytesFree =(double)statex.ullAvailPhys ;
    pval->numBytesTotal =(double)statex.ullTotalPhys ;
    pval->numBytesAlloc =(double) procmem.PrivateUsage ;
    return 0;
 }
