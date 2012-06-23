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
 *              Gathering heap statistics could be expensive;
 *              I wouldn't want to run this too often w/o
 *              knowing how it is implemented and how it could
 *              impact the running system.
 *
 *              Furthermore, vxStats reports 'free', 'used' and
 *              'total' memory.  Unfortunately, it just assumes
 *              'total = free + used' instead of the correct total
 *              number (difference could give a hint about
 *              fragmentation). However, if you know the true total
 *              amount of memory you can estimate fragmentation in
 *              your head...
 *
 *              RTEMS also uses a 'workspace' memory
 *              area which is independent of the malloc heap.
 *              Some system-internal data structures are
 *              allocated from the workspace area.
 *              Support for monitoring the workspace area 
 *              is provided in osdWorkspaceUsage.c.
 *
 *  2009-05-15  Ralph Lange (HZB/BESSY)
 *              Restructured OSD parts
 *
 */

#include <devIocStats.h>

int devIocStatsInitMemUsage (void) { return 0; }

int devIocStatsGetMemUsage (memInfo *pval)
{
#ifdef RTEMS_PROTECTED_HEAP
# if   (__RTEMS_MAJOR__ > 4) \
   || (__RTEMS_MAJOR__ == 4 && __RTEMS_MINOR__ > 9)
    extern Heap_Control *RTEMS_Malloc_Heap;
    Heap_Control *h = RTEMS_Malloc_Heap;
# else
    extern Heap_Control RTEMS_Malloc_Heap;
    Heap_Control *h = &RTEMS_Malloc_Heap;
# endif
    Heap_Information_block info;

    _Protected_heap_Get_information(h, &info);
#else /* RTEMS_PROTECTED_HEAP */
    extern rtems_id      RTEMS_Malloc_Heap;
    rtems_id h = RTEMS_Malloc_Heap;
    region_information_block info;

    rtems_region_get_information(h, &info);
    /* rtems' malloc_free_space() looks at 'largest' -- why not 'total'? */
#endif /* RTEMS_PROTECTED_HEAP */
    pval->numBytesTotal    = (double)info.Free.total + (double)info.Used.total;
    pval->numBytesFree     = (double)info.Free.total;
    pval->numBytesAlloc    = (double)info.Used.total;
    pval->maxBlockSizeFree = (double)info.Free.largest;
    return 0;
}
