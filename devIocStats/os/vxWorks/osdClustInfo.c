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

/* osdClustInfo.c - vxWorks implementation */

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
 *  08-09-29    Stephanie Allison - moved os-specific parts to
 *              os/<os>/devIocStatsOSD.h and devIocStatsOSD.c.  Added reboot.
 *              Split into devIocStatsAnalog, devIocStatsString,
 *              devIocStatTest.
 *
 *  2009-05-19  Ralph Lange (HZB/BESSY)
 *              restructured OSD parts
 */

#include <devIocStats.h>

int devIocStatsInitClusterInfo (void) { return 0; }

int devIocStatsGetClusterInfo (int pool, clustInfo *pval)
{
    NET_POOL_ID pNetPool;
    CL_POOL_ID  pCluster;
    int i;
    int test;

#if _WRS_VXWORKS_MAJOR >= 6
    int j = 0;
#endif

    if (pool == SYS_POOL)
        pNetPool = _pNetSysPool;
    else
        pNetPool = _pNetDpool;
    if (!pNetPool) {
      return -1;
    }

    test = pNetPool->clTbl[0]->clSize; 
    for (i = 0; i < CL_TBL_SIZE; i++)
    {
        if (!(pCluster = pNetPool->clTbl[i])) break; /* Quit if no more cluster tables */

#if _WRS_VXWORKS_MAJOR >= 6
        if ((i > 0) && (pCluster->clSize == test)) continue; /* ignore duplicate entries */
        if (pCluster->clSize <= 0) break;
        test = pCluster->clSize; 
        (*pval)[j][0] = test;
        (*pval)[j][1] = pCluster->clNum;
        (*pval)[j][2] = pCluster->clNumFree;
        (*pval)[j][3] = pCluster->clUsage;
        j++;
#else
        if (i > 0) 
            if (pCluster->clSize != (2 * test)) break;
        test = pCluster->clSize; 
        (*pval)[i][0] = test;
        (*pval)[i][1] = pCluster->clNum;
        (*pval)[i][2] = pCluster->clNumFree;
        (*pval)[i][3] = pCluster->clUsage;
#endif
    }
    return 0;
}

int devIocStatsGetClusterUsage (int pool, int *pval)
{
    NET_POOL_ID pNetPool;
    int i, sum = 0;

    if (pool == SYS_POOL)
        pNetPool = _pNetSysPool;
    else
        pNetPool = _pNetDpool;
    if (!pNetPool) {
      *pval = 0;
      return -1;
    }

    for (i = 0; i < NUM_MBLK_TYPES; i++) {
#if _WRS_VXWORKS_MAJOR >= 6
        sum += pNetPool->pPoolStat->mTypes[i];
#else
        sum += pNetPool->pPoolStat->m_mtypes[i];
#endif
    }
    *pval = sum;
    return 0;
}
