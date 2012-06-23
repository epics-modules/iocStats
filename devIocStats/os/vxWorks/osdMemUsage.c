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
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-15 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 *  2011-03-25 E.Bjorklund (LANSCE)
 *     Translate between the devIocStats memeory info structure
 *     and its vxWorks 6 counterpart.
 *
 */

/* osdMemUsage.c - vxWorks implementation */

#include <private/memPartLibP.h>
/* #include <memLib.h> */

#include <devIocStats.h>

static memInfo nope = { 0, 0, 0, 0, 0, 0 };

int devIocStatsInitMemUsage (void) { return OK; }

int devIocStatsGetMemUsage (memInfo *pmi) {

/*==============================================================================
 * For VxWorks 6 and above, use memPartGetInfo to retrieve the data
 */
#if _WRS_VXWORKS_MAJOR >= 6

    *pmi = nope; /* Clear out the memory info structure */
    MEM_PART_STATS vxPmi;       /* vxWorks memory partition info structure   */

    int status = memPartInfoGet (memSysPartId, &vxPmi);
    if (0 == status) {
        pmi->numBytesTotal    = (double)vxPmi.numBytesFree + 
	                        (double)vxPmi.numBytesAlloc;
        pmi->numBytesFree     = (double)vxPmi.numBytesFree;
        pmi->numBytesAlloc    = (double)vxPmi.numBytesAlloc;
        pmi->numBlocksFree    = (double)vxPmi.numBlocksFree;
        pmi->numBlocksAlloc   = (double)vxPmi.numBlocksAlloc;
        pmi->maxBlockSizeFree = (double)vxPmi.maxBlockSizeFree;
    }/* end if memPartInfoGet succeeded*/

    return status;

/*==============================================================================
 * For VxWorks 5, walk the list manually
 */
#else
    /* Added by LTH because memPartInfoGet() has a bug when "walking" the list */
    FAST PART_ID partId = memSysPartId;
    BLOCK_HDR *  pHdr;
    DL_NODE *    pNode;
    double nbf;
    *pmi = nope;
    if (ID_IS_SHARED (partId))  /* partition is shared? */
    {
        /* shared partitions not supported yet */
        return (ERROR);
    }
    /* partition is local */
    if (OBJ_VERIFY (partId, memPartClassId) != OK)
        return (ERROR);
    /* take and keep semaphore until done */
    semTake (&partId->sem, WAIT_FOREVER);
    for (pNode = DLL_FIRST (&partId->freeList); pNode != NULL; pNode = DLL_NEXT (pNode))
    {
        pHdr = NODE_TO_HDR (pNode);
        {
            pmi->numBlocksFree ++ ;
	    nbf = 2.0 * (double)pHdr->nWords;
            pmi->numBytesFree += nbf;
            if(nbf > pmi->maxBlockSizeFree) 
	        pmi->maxBlockSizeFree = nbf;
        }
    }
    pmi->numBytesAlloc = 2.0 * (double)partId->curWordsAllocated;
    pmi->numBlocksAlloc = (double)partId->curBlocksAllocated;
    semGive (&partId->sem);

    pmi->numBytesTotal = (double)((unsigned long)sysPhysMemTop());
    return (OK);
#endif
}
