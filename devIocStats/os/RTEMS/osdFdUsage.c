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
 * Modifications at LBNL:
 * -----------------
 * 	97-11-21	SRJ	Added reports of max free mem block,
 *				Channel Access connections and CA clients.
 *				Repaired "artificial load" function.
 *	98-01-28	SRJ	Changes per M. Kraimer's devVXStats of 97-11-19:
 *				explicitly reports file descriptors used;
 *				uses Kraimer's method for CPU load average;
 *				some code simplification and name changes.
 *
 * Modifications for SNS at ORNL:
 * -----------------
 *	03-01-29	CAL 	Add stringin device support.
 *	03-05-08	CAL	Add minMBuf
 *
 * Modifications for LCLS/SPEAR at SLAC:
 * ----------------
 *  2008-09-29  Stephanie Allison - moved os-specific parts to
 *              os/<os>/devIocStatsOSD.h and devIocStatsOSD.c.  Added reboot.
 *              Split into devIocStatsAnalog, devIocStatsString,
 *              devIocStatTest.
 *  2009-05-15  Ralph Lange (HZB/BESSY)
 *              Restructured OSD parts
 *
 */

#include <devIocStats.h>

#if __RTEMS_MAJOR__ >= 6
#include <rtems/libio_.h>
#endif /* _RTEMS_MAJOR__ >= 6 */

int devIocStatsInitFDUsage(void) { return 0; }

int devIocStatsGetFDUsage(fdInfo *pval) {
#if __RTEMS_MAJOR__ >= 6
  pval->used = rtems_libio_count_open_iops();
#else
  int i, tot;

  for (tot = 0, i = 0; i < rtems_libio_number_iops; i++) {
    if (rtems_libio_iops[i].flags & LIBIO_FLAGS_OPEN)
      tot++;
  }
  pval->used = tot;
#endif /* _RTEMS_MAJOR__ >= 6 */
  pval->max = rtems_libio_number_iops;
  return 0;
}
