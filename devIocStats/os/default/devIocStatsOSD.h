/*************************************************************************\
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

/* devIocStatsOSD.h - Header for OS dependent implementation part */

#include <epicsExit.h>

#ifdef SYSBOOTLINE_NEEDED
static char *sysBootLine = "<not implemented>";
#endif
#define FDTABLE_INUSE(i) (0)
#define MAX_FILES 0
#define CLUSTSIZES 2
#define NO_OF_CPUS 1
#define TICKS_PER_SEC 1
#define reboot(x) epicsExit(0)
