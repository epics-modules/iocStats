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

/* osdClustInfo.c - RTEMS implementation */

/* extracted from */
/* devIocStatsAnalog.c - Analog Device Support Routines for IOC statistics - based on */
/* devVXStats.c - Device Support Routines for vxWorks statistics */
/*
 *	Author: Jim Kowalkowski
 *	Date:  2/1/96
 *
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
 *
 *	03-05-08	CAL	Add minMBuf
 *
 * Modifications for LCLS/SPEAR at SLAC:
 * ----------------
 *  08-08-26    Till Straumann, ported to RTEMS.
 *              The RTEMS/BSD stack has only one pool of mbufs
 *              and only uses two sizes: MSIZE (128b) for 'ordinary'
 *              mbufs, and MCLBYTES (2048b) for 'mbuf clusters'.
 *              Therefore, the 'data' pool is empty. However,
 *              the calculation of MinDataMBuf always shows usage
 *              info of 100% free (but 100% of 0 is still 0).
 *
 *  08-09-29    Stephanie Allison - moved os-specific parts to
 *              os/<os>/devIocStatsOSD.h and devIocStatsOSD.c.  Added reboot.
 *              Split into devIocStatsAnalog, devIocStatsString,
 *              devIocStatTest.
 *
 *  2009-05-19  Ralph Lange (HZB/BESSY)
 *              restructured OSD parts
 */

#include <devIocStats.h>

/* This would otherwise need _KERNEL to be defined... */
extern struct mbstat mbstat;

int devIocStatsInitClusterInfo (void) { return 0; }

int devIocStatsGetClusterInfo (int pool, clustInfo *pval)
{
    if (pool == DATA_POOL) return -1;

    (*pval)[0][0] = MSIZE;
    (*pval)[0][1] = mbstat.m_mbufs;
    (*pval)[0][2] = mbstat.m_mtypes[MT_FREE];
    (*pval)[0][3] = (*pval)[0][1] - (*pval)[0][2];

    (*pval)[1][0] = MCLBYTES;
    (*pval)[1][1] = mbstat.m_clusters;
    (*pval)[1][2] = mbstat.m_clfree;
    (*pval)[1][3] = (*pval)[1][1] - (*pval)[1][2];

    return 0;
}

int devIocStatsGetClusterUsage (int pool, int *pval)
{
    if ( pool == DATA_POOL ) return -1;

    *pval = mbstat.m_mbufs;

    return 0;
}
