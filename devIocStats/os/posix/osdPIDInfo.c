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

/* osdPIDInfo.c - Process ID info */

#include <devIocStats.h>
#include <unistd.h>

int devIocStatsGetPID(double *proc_id) {
  *proc_id = (double)getpid();
  return 0;
}

int devIocStatsGetPPID(double *proc_id) {
  *proc_id = (double)getppid();
  return 0;
}
