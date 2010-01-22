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

/* osdMemUsage.c - RTEMS implementation */

/* based on */
/* devIocStatsOSD.c - devIocStats.c Support for RTEMS - based on */
/* devVXStats.c - Device Support Routines for vxWorks statistics */
/*
 *	Author: Jim Kowalkowski
 *	Date:  2/1/96
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
 *  2009-05-15  Ralph Lange (HZB/BESSY)
 *              Restructured OSD parts
 *
 */

#include <devIocStats.h>

/* Heap implementation changed; we should use
 * malloc_free_space() which handles these changes
 * transparently but then we don't get the
 * 'bytesUsed' information.
 */
# if   (__RTEMS_MAJOR__ > 4) \
   || (__RTEMS_MAJOR__ == 4 && __RTEMS_MINOR__ > 7)
#define RTEMS_MALLOC_IS_HEAP
#include <rtems/score/protectedheap.h>
# endif

int devIocStatsInitMemUsage (void) { return 0; }

int devIocStatsGetMemUsage (memInfo *pval)
{
#ifdef RTEMS_MALLOC_IS_HEAP
    extern Heap_Control RTEMS_Malloc_Heap;
    Heap_Control *h = &RTEMS_Malloc_Heap;
    Heap_Information_block info;

    _Protected_heap_Get_information(h, &info);
#else /* RTEMS_MALLOC_IS_HEAP */
    extern rtems_id      RTEMS_Malloc_Heap;
    rtems_id h = RTEMS_Malloc_Heap;
    region_information_block info;

    rtems_region_get_information(h, &info);
    /* rtems' malloc_free_space() looks at 'largest' -- why not 'total'? */
#endif /* RTEMS_MALLOC_IS_HEAP */
    pval->numBytesTotal    = info.Free.total + info.Used.total;
    pval->numBytesFree     = info.Free.total;
    pval->numBytesAlloc    = info.Used.total;
    pval->maxBlockSizeFree = info.Free.largest;
    return 0;
}
