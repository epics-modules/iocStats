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

/* osdSuspTasks.c - Number of suspended tasks: RTEMS implementation */

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

int devIocStatsInitSuspTasks (void) { return 0; }

int devIocStatsGetSuspTasks (int *pval)
{
    Objects_Control   *o;
    Objects_Id        id = OBJECTS_ID_INITIAL_INDEX;
    Objects_Id        nid;
    int               n = 0;
    Objects_Locations l;

    /* count all suspended (LOCAL -- cannot deal with remote ones ATM) tasks */
    while ( (o = _Objects_Get_next( &_RTEMS_tasks_Information, id, &l, &nid )) ) {
        if ( (RTEMS_ALREADY_SUSPENDED == rtems_task_is_suspended( nid )) ) {
            n++;
        }
        _Thread_Enable_dispatch();
        id = nid;
    }
    *pval = n;
    return 0;
}
