/*************************************************************************\
 * Copyright (c) 2012 Paul-Scherrer Institut(PSI)
 * Copyright (c) 2009-2010 Helmholtz-Zentrum Berlin
 *     fuer Materialien und Energie GmbH.
 * Copyright (c) 2002 The University of Chicago, as Operator of Argonne
 *     National Laboratory.
 * Copyright (c) 2002 The Regents of the University of California, as
 *     Operator of Los Alamos National Laboratory.
 * EPICS BASE Versions 3.13.7
 * and higher are distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdCpuUtilization.c - CPU Utilization: default implementation = do nothing */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2012-03-16 Helge Brands (PSI) 
 *
 */

#if defined(__MINGW32__) || defined(__MINGW64__)
#define WINVER 0x0501
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

//#include <iostream>
//#include <iomanip>
#include <winbase.h>
#include <winperf.h>
#pragma comment (lib, "Advapi32.lib")
#include <devIocStats.h>

DWORD myprocessid;
ULONGLONG Ticks,Prev_Ticks;
FILETIME Proc_Creationtime,Proc_IdleTime, Proc_KernelTime, Proc_UserTime;
FILETIME Sys_IdleTime, Sys_KernelTime, Sys_UserTime;
FILETIME Prev_Proc_IdleTime, Prev_Proc_KernelTime, Prev_Proc_UserTime;
FILETIME Prev_Sys_IdleTime, Prev_Sys_KernelTime, Prev_Sys_UserTime;

double CpuUsage, iocCpuUsage;


ULONGLONG SubtractTimes(const FILETIME ftA, const FILETIME ftB) {
    LARGE_INTEGER a, b;

    a.LowPart = ftA.dwLowDateTime;
    a.HighPart = ftA.dwHighDateTime;
    b.LowPart = ftB.dwLowDateTime;
    b.HighPart = ftB.dwHighDateTime;
    return a.QuadPart - b.QuadPart;
}

int test_Cpu(loadInfo *pval) {


    ULONGLONG Sys_KernelTime_Diff;
    ULONGLONG Sys_UserTime_Diff;
    ULONGLONG Sys_IdleTime_Diff;
    ULONGLONG Proc_KernelTime_Diff;
    ULONGLONG Proc_UserTime_Diff;
    ULONGLONG nTotalSys;
    ULONGLONG nTotalProc;
    ULONGLONG nTotalMachine;

    
    GetSystemTimes(&Sys_IdleTime, &Sys_KernelTime, &Sys_UserTime);
    GetProcessTimes(GetCurrentProcess(),&Proc_Creationtime,&Proc_IdleTime, &Proc_KernelTime, &Proc_UserTime);
 
    Sys_KernelTime_Diff = SubtractTimes(Sys_KernelTime, Prev_Sys_KernelTime);

    Sys_UserTime_Diff = SubtractTimes(Sys_UserTime, Prev_Sys_UserTime);

    Sys_IdleTime_Diff = SubtractTimes(Sys_IdleTime, Prev_Sys_IdleTime);

    Proc_KernelTime_Diff = SubtractTimes(Proc_KernelTime, Prev_Proc_KernelTime);

    Proc_UserTime_Diff = SubtractTimes(Proc_UserTime, Prev_Proc_UserTime);

   nTotalSys = Sys_KernelTime_Diff + Sys_UserTime_Diff;

    nTotalProc = Proc_UserTime_Diff + Proc_KernelTime_Diff;

    nTotalMachine = (Sys_KernelTime_Diff + Sys_UserTime_Diff)-Sys_IdleTime_Diff;
    
    if (nTotalSys > 0) {
        pval->iocLoad=((100.0 * nTotalProc) / nTotalSys);
        pval->cpuLoad = ((100.0 * nTotalMachine) /nTotalSys );
    }

    Prev_Ticks=Ticks;
    Prev_Proc_IdleTime = Proc_IdleTime;
    Prev_Proc_KernelTime = Proc_KernelTime;
    Prev_Proc_UserTime = Proc_UserTime;

    Prev_Sys_IdleTime = Sys_IdleTime;
    Prev_Sys_KernelTime = Sys_KernelTime;
    Prev_Sys_UserTime = Sys_UserTime;

    return 0;

}

int devIocStatsInitCpuUtilization(loadInfo *pval) {

 
     GetSystemTimes(&Prev_Sys_IdleTime, &Prev_Sys_KernelTime, &Prev_Sys_UserTime);
    GetProcessTimes(GetCurrentProcess(),&Proc_Creationtime,&Prev_Proc_IdleTime, &Prev_Proc_KernelTime, &Prev_Proc_UserTime);

    
    myprocessid = GetCurrentProcessId();
    
#ifdef _WIN64
    pval->noOfCpus = GetMaximumProcessorCount(ALL_PROCESSOR_GROUPS);
#else
    
    pval->noOfCpus = strtol(getenv("NUMBER_OF_PROCESSORS"),NULL,10);
#endif    
    
    return 0;
}

int devIocStatsGetCpuUtilization(loadInfo *pval) {

    test_Cpu(pval);
    
    return 0;

}
