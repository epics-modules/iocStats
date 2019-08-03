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

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-25 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *  2010-07-14  Ralph Lange (HZB/BESSY)
 *     Added CPU Utilization (IOC load), number of CPUs
 *  2016-02-25  Jeong Han Lee (ESS)
 *     Replaced _SC_NPROCESSORS_CONF with _SC_NPROCESSORS_ONLN
 */

#ifndef devIocStatsOSD_H
#define devIocStatsOSD_H

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <epicsExit.h>

#ifdef SYSBOOTLINE_NEEDED
static char *sysBootLine = "<not implemented>";
#endif
#define FDTABLE_INUSE(i) (0)
#define MAX_FILES 0
#define CLUSTSIZES 2
#define NO_OF_CPUS sysconf(_SC_NPROCESSORS_ONLN)
#define TICKS_PER_SEC sysconf(_SC_CLK_TCK)

#if EPICS_VERSION_INT>=VERSION_INT(3,15,1,0)
#  define reboot(x) epicsExitLater(0)
#else
#  define reboot(x) epicsExit(0)
#endif

#endif /* devIocStatsOSD_H */

