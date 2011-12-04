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

/* devIocStatsSub.c - Subroutine Routines for IOC statistics */

#include <string.h>

#include <epicsThread.h>
#include <epicsExport.h>
#include <epicsPrint.h>

#include <dbScan.h>
#include <subRecord.h>
#include <registryFunction.h>

#include "devIocStats.h"

/*====================================================

  Name: rebootProc

  Rem:  This function resets the network
        devices and transfers control to
        boot ROMs.

        If any input A through F is greater
        than zero, the reboot is not allowed;
	these are "inhibits."  Unless input L
	is equal to one, the reboot is not allowed;
	this is an "enable."  The intention is to
	feed a BO record with a one-shot timing of
	a few seconds to it, which has to be set
	within a small window before requesting the
	reboot.
	
        Input G is the bitmask for the reboot
        input argument.  The possible bits are
        defined in sysLib.h.  If input G is
        0 (default), the reboot will be normal
        with countdown.  If the BOOT_CLEAR bit
        is set, the memory will be cleared first.

        A taskDelay is needed before the reboot
        to allow the reboot message to be logged.

  Side: Memory is cleared if BOOT_CLEAR is set.
        A reboot is initiated.
        A message is sent to the error log.

  Ret: long
           OK - Successful operation (Always)

=======================================================*/
static long rebootProc(struct subRecord *psub)
{
  if ((psub->a < 0.5) && (psub->b < 0.5) &&
      (psub->c < 0.5) && (psub->d < 0.5) &&
      (psub->e < 0.5) && (psub->f < 0.5) &&
      (psub->l > 0.5)) {
     epicsPrintf("IOC reboot started\n");
     epicsThreadSleep(1.0);
     reboot((int)(psub->g + 0.1));
  }
  return(0);
}
/*====================================================

  Name: scanMonInit

  Rem:  scanMon initialization.  Allocate space for
        timestamp in DPVT.  Set E to -1 to force
        initialization by scanMon.
        
  Side: None.

  Ret: long
           OK - Successful operation (Always)

=======================================================*/
static long scanMonInit(struct subRecord *psub)
{
  psub->dpvt = malloc(sizeof(epicsTimeStamp));
  psub->e = -1.0;
  if (!psub->dpvt) return -1;
  return (0);
}

/*====================================================

  Name: scanMon

  Rem:  This function calculates the time between record
        updates.  Instantiate one of these records per SCAN
        period to monitor if a SCAN task is getting too far
        behind in its updates (ie, due to a higher priority
        task).
        
        Inputs:
        INPA = mode (0 = relative, 1 = absolute)
        INPB = minor tolerance (%)
        INPC = major tolerance (%)
        When SCAN is not periodic (ie, Passive, Event, I/O Intr):
        INPD = expected update period (sec)

        Input/Outputs:
        INPE = update period (sec)
        DPVT = timestamp

        Outputs:
        VAL = update time (sec) for mode = 1
              update time - expected update time (sec) for mode = 0
        
  Side: None.

  Ret: long
           OK - Successful operation (Always)

=======================================================*/
static long scanMon(struct subRecord *psub)
{
  double         scan_period = scanPeriod(psub->scan);
  epicsTimeStamp curr_time;
  long           status = 0;

  if (!psub->dpvt) return -1;

  epicsTimeGetCurrent(&curr_time);

  /* Check SCAN period.  If the record is not periodic,
     use D input as the expected period. */
  if (scan_period <= 0.0) {
    if (psub->d < 0.0) scan_period = 0.0;
    else               scan_period = psub->d;
  }
  /* Initialize when SCAN period changes. */
  if (scan_period != psub->e) {
    psub->e   = scan_period;
    status    = -1;
  /* Otherwise, calculate update time. */
  } else {
    psub->val = epicsTimeDiffInSeconds(&curr_time,
                                       (epicsTimeStamp *)psub->dpvt);
    if (psub->a < 0.5) psub->val -= scan_period;
  }
  /* Save time stamp for next time around */ 
  *((epicsTimeStamp *)psub->dpvt) = curr_time;
  
  /* Recalculate alarm limits. These really only need to be
     recalculated whenever scan_period, A, B changes or whenever
     HIHI, LOLO, HIGH, LOW get externally updated
     (which is not allowed)
     so it's just easier recalculating them every time. */
  psub->hihi = scan_period * (psub->a + psub->c/100.);
  psub->high = scan_period * (psub->a + psub->b/100.);
  if (psub->a < 0.5) {
    psub->lolo = scan_period * (psub->a - psub->c/100.);
    psub->low  = scan_period * (psub->a - psub->b/100.);
  } else {
    psub->lolo = -scan_period;
    psub->low  = -scan_period;
  }
  return(status);
}

epicsRegisterFunction(rebootProc);
epicsRegisterFunction(scanMonInit);
epicsRegisterFunction(scanMon);
