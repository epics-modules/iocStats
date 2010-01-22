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
 */

/* osdMemUsage.c - vxWorks implementation */

#include <private/memPartLibP.h>
/* #include <memLib.h> */

#include <devIocStats.h>

static memInfo nope = { 0, 0, 0, 0, 0, 0 };

int devIocStatsInitMemUsage (void) { return OK; }

int devIocStatsGetMemUsage (memInfo *pmi) {

#if _WRS_VXWORKS_MAJOR >= 6
    return memPartInfoGet(memSysPartId, pmi);
#else
    /* Added by LTH because memPartInfoGet() has a bug when "walking" the list */
    FAST PART_ID partId = memSysPartId;
    BLOCK_HDR *  pHdr;
    DL_NODE *    pNode;
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
            pmi->numBytesFree += 2 * pHdr->nWords;
            if(2 * pHdr->nWords > pmi->maxBlockSizeFree) 
                pmi->maxBlockSizeFree = 2 * pHdr->nWords;
        }
    }
    pmi->numBytesAlloc = 2 * partId->curWordsAllocated;
    pmi->numBlocksAlloc = partId->curBlocksAllocated;
    semGive (&partId->sem);

    pmi->numBytesTotal = sysPhysMemTop();
    return (OK);
#endif
}
