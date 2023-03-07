/* ioccar.c */
/*****************************************************************
                          COPYRIGHT NOTIFICATION
*****************************************************************
 
(C)  COPYRIGHT 1991 Regents of the University of California,
and the University of Chicago Board of Governors.
 
This software was developed under a United States Government license
described on the COPYRIGHT_Combined file included as part
of this distribution.
**********************************************************************/

/****************************************************************
*
*	Current Author:		Marty Kraimer
*	Date:			10APR96
*
*
* Modification Log:
* -----------------
* .01  10APR96	mrk	list db to CA links
****************************************************************/

#if defined(NO_NETWORK_STAT)

long ioccar(char *precordname,int level, int *Pcal, int *Pcalnconn, int *Pcaldconn) { return 0; };

#else

#if defined(vxWorks)
#include <vxWorks.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#if defined(vxWorks)
#include <taskLib.h>
#endif
#include "dbStaticLib.h"
#include "link.h"
/*definitions needed because of old vs new database access*/
#undef DBR_SHORT
#undef DBR_PUT_ACKT
#undef DBR_PUT_ACKS
#undef VALID_DB_REQ
#undef INVALID_DB_REQ
/*end of conflicting definitions*/
#include "cadef.h"
#include "dbDefs.h"
#include "epicsPrint.h"
#include "dbCommon.h"
#include "dbCa.h"
#include <epicsVersion.h>
#define LT_EPICSBASE(v,r,l) ((EPICS_VERSION<=(v)) && (EPICS_REVISION<=(r)) && (EPICS_MODIFICATION<(l)))
#define GT_EPICSBASE(v,r,l) ((EPICS_VERSION>=(v)) && (EPICS_REVISION>=(r)) && (EPICS_MODIFICATION>(l)))
#if LT_EPICSBASE(3,14,13)
#include "../src/db/dbCaPvt.h"
#elif GT_EPICSBASE(7,0,0)
#include "../modules/database/src/ioc/db/dbCaPvt.h"
#else
#include "../src/ioc/db/dbCaPvt.h"
#endif
extern struct dbBase *pdbbase;

long ioccar(char *precordname,int level, int *Pcal, int *Pcalnconn, int *Pcaldconn)
{
    DBENTRY		dbentry;
    DBENTRY		*pdbentry=&dbentry;
    long		status;
    dbCommon		*precord;
    dbRecordType		*pdbRecordType;
    dbFldDes		*pdbFldDes;
    DBLINK		*plink;
    int			ncalinks=0;
    int			nconnected=0;
    int			noReadAccess=0;
    int			noWriteAccess=0;
    unsigned long	nDisconnect=0;
    unsigned long	nNoWrite=0;
    caLink		*pca;
    int			j;


    dbInitEntry(pdbbase,pdbentry);
    status = dbFirstRecordType(pdbentry);
    while(!status) {
	status = dbFirstRecord(pdbentry);
	while(!status) {
	    if(!precordname
	    || (strcmp(precordname,dbGetRecordName(pdbentry)) ==0)) {
		pdbRecordType = pdbentry->precordType;
		precord = (dbCommon *)pdbentry->precnode->precord;
		for(j=0; j<pdbRecordType->no_links; j++) {
		    pdbFldDes = pdbRecordType->papFldDes[pdbRecordType->link_ind[j]];
		    plink = (DBLINK *)((char *)precord + pdbFldDes->offset);
		    if (plink->type == CA_LINK) {
			ncalinks++;
			pca = (caLink *)plink->value.pv_link.pvt;
			if(pca && (ca_field_type(pca->chid) != TYPENOTCONN)) {
			    nconnected++;
			    nDisconnect += pca->nDisconnect;
			    nNoWrite += pca->nNoWrite;
			    if(!ca_read_access(pca->chid)) noReadAccess++;
			    if(!ca_write_access(pca->chid)) noWriteAccess++;
			    if(level>1) {
				printf("    connected ");
				printf("%s",ca_host_name(pca->chid));
				if(!ca_read_access(pca->chid))
					printf(" no_read_access");
				if(!ca_write_access(pca->chid))
					printf(" no_write_access");
				printf(" %s.%s %s",
				    precord->name,
				    pdbFldDes->name,
				    plink->value.pv_link.pvname);
				if(nDisconnect)
				    printf(" nDisconnect %lu",pca->nDisconnect);
				if(nNoWrite)
				    printf(" nNoWrite %lu",pca->nNoWrite);
				printf("\n");
			    }
			} else { 
			    if(level>0) {
				printf("not_connected %s.%s %s",
				    precord->name,
				    pdbFldDes->name,
				    plink->value.pv_link.pvname);
				if(nDisconnect)
				    printf(" nDisconnect %lu",pca->nDisconnect);
				if(nNoWrite)
				    printf(" nNoWrite %lu",pca->nNoWrite);
				printf("\n");
			    }
			}
		    }
		}
		if(precordname) goto done;
	    }
	    status = dbNextRecord(pdbentry);
	}
	status = dbNextRecordType(pdbentry);
    }
done:
    *Pcal    = ncalinks;
    *Pcalnconn = (ncalinks - nconnected);
    *Pcaldconn = nDisconnect;
    dbFinishEntry(pdbentry);
    return(0);
}
#endif
