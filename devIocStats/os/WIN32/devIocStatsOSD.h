/* devIocStatsOSD.h - devIocStats.c Support for RTEMS - based on */
/* devVXStats.c - Device Support Routines for vxWorks statistics */
/*
 *	Author: Jim Kowalkowski
 *	Date:  2/1/96
 *
 *	Experimental Physics and Industrial Control System (EPICS)
 *
 *	Copyright 1991, the Regents of the University of California,
 *	and the University of Chicago Board of Governors.
 *
 *	This software was produced under  U.S. Government contracts:
 *	(W-7405-ENG-36) at the Los Alamos National Laboratory,
 *	and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *	Initial development by:
 *		The Controls and Automation Group (AT-8)
 *		Ground Test Accelerator
 *		Accelerator Technology Division
 *		Los Alamos National Laboratory
 *
 *	Co-developed with
 *		The Controls and Computing Group
 *		Accelerator Systems Division
 *		Advanced Photon Source
 *		Argonne National Laboratory
 *
 * Modifications for LCLS/SPEAR at SLAC:
 * ----------------
 *  08-08-26    Till Straumann, ported to RTEMS.
 *
 *              RTEMS notes:
 *                - RTEMS also uses a 'workspace' memory
 *                  area which is independent of the malloc heap.
 *                  Some system-internal data structures are
 *                  allocated from the workspace area.
 *                  So far, support for monitoring the workspace area
 *                  has not been implemented (although it would be
 *                  straightforward to do.
 *
 *              The RTEMS/BSD stack has only one pool of mbufs
 *              and only uses two sizes: MSIZE (128b) for 'ordinary'
 *              mbufs, and MCLBYTES (2048b) for 'mbuf clusters'.
 *                 Therefore, the 'data' pool is empty. However,
 *              the calculation of MinDataMBuf always shows usage
 *              info of 100% free (but 100% of 0 is still 0).
 *
 */

#ifndef devIocStatsOSD_H
#define devIocStatsOSD_H

#include <string.h>
#include <stdlib.h>

#include <epicsExit.h>

#define CLUSTSIZES 2
#if EPICS_VERSION_INT>=VERSION_INT(3,15,1,0)
#  define reboot(x) epicsExitLater(0)
#else
#  define reboot(x) epicsExit(0)
#endif

#endif /* devIocStatsOSD_H */
