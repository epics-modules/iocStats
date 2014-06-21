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

/* devIocStatsWaveform.c - Waveform Device Support Routines for IOC statistics - based on */
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
 *	03-05-08	CAL	Add minMBuf
 *
 * Modifications for LCLS/SPEAR at SLAC:
 * ----------------
 *  08-09-29    Stephanie Allison - moved os-specific parts to
 *              os/<os>/devIocStatsOSD.h and devIocStatsOSD.c.
 *              Added logic from linuxStats from SNS.
 *              Split into devIocStatsAnalog, devIocStatsString,
 *              devIocStatTest.
 *  2009-05-20  Ralph Lange (HZB/BESSY)
 *              Restructured OSD parts
 *  2014-06-12  Stephanie Allison - added wavefrom support.
 *
 */

/*
	--------------------------------------------------------------------
	Note that the valid values for the parm field of the link
	information are:

	wavefrom (DTYP = "IOC stats"):

		The supported waveform devices are all static; the record only needs 
		to be processed once, for which PINI is convenient.

		startup_script	         -path of startup script
					Default - uses STARTUP and ST_CMD
					environment variables.
		bootline		-CPU bootline
		epics_ver		-EPICS base version
                pwd                     -IOC Current Working Directory
                                        from getcwd
*/

#include <string.h>

#include <epicsVersion.h>
#include <dbAccess.h>
#include <devSup.h>
#include <waveformRecord.h>
#include <menuFtype.h>
#include <recGbl.h>
#include <epicsExport.h>

#include "devIocStats.h"

struct wStats
{
	long      number;
	DEVSUPFUN report;
	DEVSUPFUN init;
	DEVSUPFUN init_record;
	DEVSUPFUN get_ioint_info;
	DEVSUPFUN read_waveform;
};
typedef struct wStats wStats;

struct pvtArea
{
	int index;
	int type;
};
typedef struct pvtArea pvtArea;

typedef void (*statGetWfmFunc)(char*, size_t);

struct validGetWfmParms
{
	char* name;
	statGetWfmFunc func;
	int type;
};
typedef struct validGetWfmParms validGetWfmParms;

static long waveform_init(int pass);
static long waveform_init_record(waveformRecord*);
static long waveform_read(waveformRecord*);

static void statsSScript(char *, size_t);
static void statsBootline(char *, size_t);
static void statsPwd(char *, size_t);
static void statsEPICSVer(char *, size_t);

static validGetWfmParms statsGetWfmParms[]={
	{ "startup_script",		statsSScript,		STATIC_TYPE },
	{ "bootline",			statsBootline,		STATIC_TYPE },
	{ "epics_ver",			statsEPICSVer,		STATIC_TYPE },
        { "pwd",			statsPwd,		STATIC_TYPE },
	{ NULL,NULL,0 }
};

wStats devWaveformStats  ={5,NULL,waveform_init,waveform_init_record,NULL,waveform_read};
epicsExportAddress(dset,devWaveformStats);

/* ---------------------------------------------------------------------- */

static long waveform_init(int pass)
{
    if (pass) return 0;

    devIocStatsInitBootInfo();

    return 0;
}

static long waveform_init_record(waveformRecord* pr)
{
	int		i;
	char	*parm;
	pvtArea	*pvt = NULL;
	if(pr->ftvl!=menuFtypeCHAR)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devWaveformStats (init_record) Illegal FTVL field");
		return S_db_badField;
	}
	if(pr->inp.type!=INST_IO)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devWaveformStats (init_record) Illegal INP field");
		return S_db_badField;
	}
	parm = pr->inp.value.instio.string;
	for(i=0;statsGetWfmParms[i].name && pvt==NULL;i++)
	{
		if(strcmp(parm,statsGetWfmParms[i].name)==0)
		{
			pvt=(pvtArea*)malloc(sizeof(pvtArea));
			pvt->index=i;
			pvt->type=statsGetWfmParms[i].type;
		}
	}
	if(pvt==NULL)
	{
		recGblRecordError(S_db_badField,(void*)pr, 
		   "devWaveformStats (init_record) Illegal INP parm field");
		return S_db_badField;
	}

	pr->dpvt=pvt;
	return 0;	/* success */
}


static long waveform_read(waveformRecord* pr)
{
	pvtArea* pvt=(pvtArea*)pr->dpvt;

	if (!pvt) return S_dev_badInpType;
	if (pr->nelm > 0) { 
	  statsGetWfmParms[pvt->index].func((char *)pr->bptr, pr->nelm-1);
	  pr->nord = strlen((char *)pr->bptr) + 1;
	  pr->udf=0;
	}
	return(0);	/* success */
}

/* -------------------------------------------------------------------- */

typedef int getWaveformFunc (char **dest);

static void getWaveform(char *d, size_t nelm, getWaveformFunc func)
{
    char *str = "";

    func(&str);
    memset(d, 0, nelm);
    strncpy(d, str, nelm);
    d[nelm] = 0;
}

static void statsSScript(char *d, size_t nelm)  { getWaveform(d, nelm, devIocStatsGetStartupScript); }

static void statsBootline(char *d, size_t nelm) { getWaveform(d, nelm, devIocStatsGetBootLine); }

static void statsPwd(char *d, size_t nelm)      { getWaveform(d, nelm, devIocStatsGetPwd); }

static void statsEPICSVer(char *d, size_t nelm) {  
  memset(d, 0, nelm);
  strncpy(d, epicsReleaseVersion, nelm);
  d[nelm] = 0;
}
