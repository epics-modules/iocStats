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
 *
 *              The RTEMS/BSD stack has only one pool of mbufs
 *              and only uses two sizes: MSIZE (128b) for 'ordinary'
 *              mbufs, and MCLBYTES (2048b) for 'mbuf clusters'.
 *                 Therefore, the 'data' pool is empty. However,
 *              the calculation of MinDataMBuf always shows usage
 *              info of 100% free (but 100% of 0 is still 0).
 *
 */
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include <bsp.h>
#include <rtems/libcsupport.h>
#include <rtems/libio_.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_var.h>

/* osdCpuUsage.c walks _Objects_Information_table[]/Objects_Information
 * directly; older RTEMS pulled these in transitively via rtems.h, current
 * RTEMS no longer does, so include the Score header explicitly. */
#include <rtems/score/objectimpl.h>

/* osdCpuUsage.c reads Thread_Control::cpu_time_used (a Timestamp_Control)
 * via the portable _Timestamp_Get_*() accessors rather than assuming its
 * representation (struct timespec vs. int64_t sbintime_t is a per-
 * architecture/config choice -- that's the whole point of the wrapper). */
#include <rtems/score/timestampimpl.h>

/* osdWorkspaceUsage.c uses _Workspace_Area directly; same transitive-
 * include story as objectimpl.h above. */
#include <rtems/score/wkspace.h>

/* osdSuspTasks.c walks _RTEMS_tasks_Information.Objects directly; this is
 * the Classic API task object information, declared here rather than
 * anywhere Score-level. */
#include <rtems/rtems/tasksdata.h>

#undef malloc
#undef free

#if (__RTEMS_MAJOR__ > 4) || (__RTEMS_MAJOR__ == 4 && __RTEMS_MINOR__ > 7)
#define RTEMS_PROTECTED_HEAP
#include <rtems/score/protectedheap.h>
#else
#include <rtems/score/apimutex.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "epicsVersion.h"

#define RTEMS_VERSION_INT                                                      \
  VERSION_INT(__RTEMS_MAJOR__, __RTEMS_MINOR__, __RTEMS_REVISION__, 0)

#define sysBootLine rtems_bsdnet_bootp_cmdline
/* Override default STARTUP environment variable to use INIT */
#undef STARTUP
#define STARTUP "INIT"
#define CLUSTSIZES 2 /* only regular mbufs and clusters */

#ifdef RTEMS_BSP_PGM_EXEC_AFTER /* only defined on uC5282 */
#define reboot(x) bsp_reset(0)
#elif (defined(__PPC__) && RTEMS_VERSION_INT > VERSION_INT(4, 9, 0, 0))
#define reboot(x) bsp_reset()
#elif (defined(__i386__) && RTEMS_VERSION_INT >= VERSION_INT(4, 10, 0, 0))
#define reboot(x)                                                              \
  {                                                                            \
    void bsp_reset();                                                          \
    bsp_reset();                                                               \
  }
#elif RTEMS_VERSION_INT >= VERSION_INT(6, 0, 0, 0)
/* rtemsReboot() no longer exists; current RTEMS's bsp_reset() takes a
 * (source, code) pair instead (see <bsp/bootcard.h>). */
#include <bsp/bootcard.h>
#include <rtems/score/interr.h>
#define reboot(x) bsp_reset(RTEMS_FATAL_SOURCE_APPLICATION, (x))
#else
#define reboot(x) rtemsReboot()
#endif
