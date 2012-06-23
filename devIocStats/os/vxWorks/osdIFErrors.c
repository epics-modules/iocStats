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

/* osdIFErrors.c - Network interface errors: vxWorks implementation */

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
 *  2009-05-20  Ralph Lange (HZB/BESSY)
 *              Restructured OSD parts
 *
 */

#include <devIocStats.h>

#if _WRS_VXWORKS_MAJOR >= 6
#ifndef VIRTUAL_STACK
IMPORT struct ifnethead ifnet_head;
#endif
#endif

int devIocStatsInitIFErrors (void) { return 0; }

int devIocStatsGetIFErrors (ifErrInfo *pval)
{

    /* add all interfaces' errors */
#if _WRS_VXWORKS_MAJOR >= 6
#if 0
    /* FIXME: There is no implementation for vxWorks 6!! */
    TAILQ_FOREACH(ifp, (&ifnet_head), if_link)
#else
    return -1;
#endif
#else
    struct ifnet *ifp;
    for (ifp = ifnet; ifp != NULL; ifp = ifp->if_next) {
#ifdef END_MIB_2233 /* This is defined in end.h if 2233 drivers are supported on the OS */
        /* This block localizes the 2233 variables so we wont get unused variable warnings */
        IP_DRV_CTRL *   pDrvCtrl = NULL;
        END_OBJ *       pEnd=NULL;

        /* Dig into the structure */
        pDrvCtrl = (IP_DRV_CTRL *)ifp->pCookie;

        /* Get a pointer to the driver's data if there is one */
        if (pDrvCtrl) pEnd = PCOOKIE_TO_ENDOBJ(pDrvCtrl->pIpCookie);

        /* If we have an end driver and it is END_MIB_2233 then we look in the m2Data structure for the errors. */
        if (pEnd && (pEnd->flags & END_MIB_2233)) {
            M2_DATA	 *pIfMib = &pEnd->pMib2Tbl->m2Data;

            pval->ierrors += pIfMib->mibIfTbl.ifInErrors;
            pval->oerrors += pIfMib->mibIfTbl.ifOutErrors;
        } else {
            /* We get the errors the old way */
            pval->ierrors += ifp->if_ierrors;
            pval->oerrors += ifp->if_oerrors;
        }
#else /* ifdef END_MIB_2233 */
        pval->ierrors += ifp->if_ierrors;
        pval->oerrors += ifp->if_oerrors;
#endif /* ifdef END_MIB_2233 */
    }
#endif
    return 0;
}
