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

/*
 *  Author: Eric Norum (LBNL)
 *
 *  Modification History
 *  2010-07-09 Eric Norum (LBNL)
 *     Minor modificaction to Ralph Lange's posix version
 *
 */

#include <string.h>
#include <stdlib.h>

#include <taskwd.h>
#include <epicsThread.h>

#define sysBspRev()     "<not implemented>"
#define kernelVersion() "<not implemented>"
#define sysBootLine     "<not implemented>"
#define FDTABLE_INUSE(i) (0)
#define MAX_FILES 0
#define CLUSTSIZES 2

#include <unistd.h>
#include <sys/reboot.h>
