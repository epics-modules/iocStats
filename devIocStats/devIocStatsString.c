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

/* devIocStatsString.c - String Device Support Routines for IOC statistics - based on */
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
 *
 */

/*
	--------------------------------------------------------------------
	Note that the valid values for the parm field of the link
	information are:

	stringin (DTYP = "IOC stats"):

		Some values displayed by the string records are
	 	longer than the 40 char string record length, so multiple 
		records must be used to display them.

		The supported stringin devices are all static; except for
                up_time, the record only needs to be processed once, for
                which PINI is convenient.

		startup_script_[12]	-path of startup script (2 records)
					Default - uses STARTUP and ST_CMD
					environment variables.
		bootline_[1-6]		-CPU bootline (6 records)
 		bsp_rev			-CPU board support package revision
		kernel_ver		-OS kernel build version
		epics_ver		-EPICS base version
                engineer                -IOC Engineer
					Default - uses ENGINEER env var.
                location                -IOC Location
					Default - uses LOCATION env var.
                hostname                -IOC Host Name from gethostname
                pwd[1-2]                -IOC Current Working Directory
                                        (2 records) from getcwd
                up_time                 -IOC Up Time

	stringin (DTYP = "IOC env var"):

                <environment variable>

	stringin (DTYP = "IOC epics var"):

                <EPICS environment variable from envDefs.h>
*/

#include <string.h>
#include <stdlib.h>

#include <epicsStdio.h>
#include <epicsVersion.h>
#include <epicsTime.h>
#include <epicsFindSymbol.h>

#include <dbAccess.h>
#include <devSup.h>
#include <stringinRecord.h>
#include <recGbl.h>
#include <envDefs.h>
#include <epicsExport.h>

#include "devIocStats.h"

#define MAX_NAME_SIZE (MAX_STRING_SIZE-1)

struct sStats
{
	long      number;
	DEVSUPFUN report;
	DEVSUPFUN init;
	DEVSUPFUN init_record;
	DEVSUPFUN get_ioint_info;
	DEVSUPFUN read_stringin;
};
typedef struct sStats sStats;

struct pvtArea
{
	int index;
	int type;
};
typedef struct pvtArea pvtArea;

typedef void (*statGetStrFunc)(char*);

struct validGetStrParms
{
	char* name;
	statGetStrFunc func;
	int type;
};
typedef struct validGetStrParms validGetStrParms;

static long stringin_init(int pass);
static long stringin_init_record(stringinRecord*);
static long stringin_read(stringinRecord*);
static long envvar_init_record(stringinRecord*);
static long envvar_read(stringinRecord*);
static long epics_init_record(stringinRecord*);
static long epics_read(stringinRecord*);

static void statsSScript1(char *);
static void statsSScript2(char *);
static void statsBootline1(char *);
static void statsBootline2(char *);
static void statsBootline3(char *);
static void statsBootline4(char *);
static void statsBootline5(char *);
static void statsBootline6(char *);
static void statsBSPRev(char *);
static void statsKernelVer(char *);
static void statsEPICSVer(char *);
static void statsEngineer(char *);
static void statsLocation(char *);
static void statsUpTime(char *);
static void statsHostName(char *);
static void statsPwd1(char *);
static void statsPwd2(char *);

static int devIocStatsGetEngineer (char **pval);
static int devIocStatsGetLocation (char **pval);

static validGetStrParms statsGetStrParms[]={
	{ "startup_script_1",		statsSScript1,		STATIC_TYPE },
	{ "startup_script_2",		statsSScript2, 		STATIC_TYPE },
	{ "bootline_1",			statsBootline1,		STATIC_TYPE },
	{ "bootline_2",			statsBootline2,		STATIC_TYPE },
	{ "bootline_3",			statsBootline3,		STATIC_TYPE },
	{ "bootline_4",			statsBootline4,		STATIC_TYPE },
	{ "bootline_5",			statsBootline5,		STATIC_TYPE },
	{ "bootline_6",			statsBootline6,		STATIC_TYPE },
	{ "bsp_rev",			statsBSPRev, 		STATIC_TYPE },
	{ "kernel_ver",			statsKernelVer,		STATIC_TYPE },
	{ "epics_ver",			statsEPICSVer,		STATIC_TYPE },
	{ "engineer",			statsEngineer,		STATIC_TYPE },
	{ "location",			statsLocation,		STATIC_TYPE },
	{ "up_time",			statsUpTime,		STATIC_TYPE },
        { "hostname",			statsHostName,		STATIC_TYPE },
        { "pwd1",			statsPwd1,		STATIC_TYPE },
        { "pwd2",			statsPwd2,		STATIC_TYPE },
	{ NULL,NULL,0 }
};

sStats devStringinStats  ={5,NULL,stringin_init,stringin_init_record,NULL,stringin_read};
sStats devStringinEnvVar ={5,NULL,NULL,envvar_init_record,  NULL,envvar_read  };
sStats devStringinEpics  ={5,NULL,NULL,epics_init_record,   NULL,epics_read   };
epicsExportAddress(dset,devStringinStats);
epicsExportAddress(dset,devStringinEnvVar);
epicsExportAddress(dset,devStringinEpics);

static char *notavail = "<not available>";
static char *empty    = "";
static char *script = 0;
static int scriptlen = 0;
static epicsTimeStamp starttime;

/* ---------------------------------------------------------------------- */

static long stringin_init(int pass)
{
    if (pass) return 0;

    epicsTimeGetCurrent(&starttime);

    devIocStatsInitBootInfo();
    devIocStatsInitSystemInfo();
    devIocStatsInitHostInfo();

    return 0;
}

static long stringin_init_record(stringinRecord* pr)
{
	int		i;
	char	*parm;
	pvtArea	*pvt = NULL;
	if(pr->inp.type!=INST_IO)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devStringinStats (init_record) Illegal INP field");
		return S_db_badField;
	}
	parm = pr->inp.value.instio.string;
	for(i=0;statsGetStrParms[i].name && pvt==NULL;i++)
	{
		if(strcmp(parm,statsGetStrParms[i].name)==0)
		{
			pvt=(pvtArea*)malloc(sizeof(pvtArea));
			pvt->index=i;
			pvt->type=statsGetStrParms[i].type;
		}
	}
	if(pvt==NULL)
	{
		recGblRecordError(S_db_badField,(void*)pr, 
		   "devStringinStats (init_record) Illegal INP parm field");
		return S_db_badField;
	}

	pr->dpvt=pvt;
	return 0;	/* success */
}

static long envvar_init_record(stringinRecord* pr)
{
	if(pr->inp.type!=INST_IO)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devStringinEnvVar (init_record) Illegal INP field");
		return S_db_badField;
	}
	pr->dpvt = pr->inp.value.instio.string;
	if(pr->dpvt==NULL)
	{
		recGblRecordError(S_db_badField,(void*)pr, 
		   "devStringinEnvVar (init_record) Illegal INP parm field");
		return S_db_badField;
	}
	return 0;	/* success */
}

static long epics_init_record(stringinRecord* pr)
{
        long status;
        const ENV_PARAM **ppParam = env_param_list;
  
	status = envvar_init_record(pr);
	if (status) return status;

        /* Find a match with one of the EPICS env vars */
        while (*ppParam) {
          if (strcmp(pr->dpvt, (*ppParam)->name) == 0) {
            pr->dpvt = (void *)*ppParam;
            return 0;
          }
          ppParam++;
        }
        pr->dpvt = 0;
        recGblRecordError(S_db_badField,(void*)pr,
                "devStringinEnvVar (init_record) Illegal INP parm field");
        return S_db_badField;
}

static long stringin_read(stringinRecord* pr)
{
	pvtArea* pvt=(pvtArea*)pr->dpvt;

	if (!pvt) return S_dev_badInpType;

	statsGetStrParms[pvt->index].func(pr->val);
	pr->udf=0;
	return(0);	/* success */
}

static long envvar_read(stringinRecord* pr)
{
	char **envvar = &notavail;
        char *buf;

	if (!pr->dpvt) return S_dev_badInpType;
        
        if ( (buf=getenv((char *)pr->dpvt)) ) envvar = &buf;
        strncpy(pr->val, *envvar, MAX_NAME_SIZE);
        pr->val[MAX_NAME_SIZE]=0; 
	pr->udf=0;
	return(0);	/* success */
}

static long epics_read(stringinRecord* pr)
{
        if (pr->dpvt) {
          pr->udf=0;
          if (!envGetConfigParam((ENV_PARAM *)pr->dpvt,
                                 MAX_STRING_SIZE, pr->val))
            strcpy(pr->val, "");
        }
	return(0);	/* success */
}

/* -------------------------------------------------------------------- */

typedef int getStringFunc (char **dest);

static void getStringPart(char *d, size_t offset, getStringFunc func)
{
    char *str = "";

    func(&str);

    if (strlen(str) <= offset)
        d[0] = 0;
    else {
        strncpy(d, str+offset, MAX_NAME_SIZE);
        d[MAX_NAME_SIZE] = 0;
    }
}

static void statsSScript1(char *d)  { getStringPart(d,               0, devIocStatsGetStartupScript); }
static void statsSScript2(char *d)  { getStringPart(d,   MAX_NAME_SIZE, devIocStatsGetStartupScript); }

static void statsBootline1(char *d) { getStringPart(d,               0, devIocStatsGetBootLine); }
static void statsBootline2(char *d) { getStringPart(d,   MAX_NAME_SIZE, devIocStatsGetBootLine); }
static void statsBootline3(char *d) { getStringPart(d, 2*MAX_NAME_SIZE, devIocStatsGetBootLine); }
static void statsBootline4(char *d) { getStringPart(d, 3*MAX_NAME_SIZE, devIocStatsGetBootLine); }
static void statsBootline5(char *d) { getStringPart(d, 4*MAX_NAME_SIZE, devIocStatsGetBootLine); }
static void statsBootline6(char *d) { getStringPart(d, 5*MAX_NAME_SIZE, devIocStatsGetBootLine); }

static void statsEngineer(char *d)  { getStringPart(d,               0, devIocStatsGetEngineer); }
static void statsLocation(char *d)  { getStringPart(d,               0, devIocStatsGetLocation); }

static void statsBSPRev(char *d)    { getStringPart(d,               0, devIocStatsGetBSPVersion); }
static void statsKernelVer(char *d) { getStringPart(d,               0, devIocStatsGetKernelVersion); }

static void statsPwd1(char *d)      { getStringPart(d,               0, devIocStatsGetPwd); }
static void statsPwd2(char *d)      { getStringPart(d,   MAX_NAME_SIZE, devIocStatsGetPwd); }

static void statsHostName (char *d) { getStringPart(d,               0, devIocStatsGetHostname); }

static void statsEPICSVer(char *d)
{ strncpy(d,  epicsReleaseVersion, MAX_NAME_SIZE); d[MAX_NAME_SIZE]=0; }

static void statsUpTime(char *d)
{
    epicsTimeStamp now;
    unsigned long time_diff;
    unsigned long secs, mins, hours, count;
    char timest[40];

    epicsTimeGetCurrent(&now);
    time_diff = (unsigned long) epicsTimeDiffInSeconds(&now, &starttime);
    secs = time_diff % 60;
    time_diff /= 60;
    mins = time_diff % 60;
    time_diff /= 60;
    hours = time_diff % 24;
    time_diff /= 24;
    count = 0;
    if (time_diff > 0)
    {
        if (time_diff == 1) count = sprintf(timest, "1 day, ");
        else count = sprintf(timest, "%lu days, ", time_diff);
    }
    sprintf(&timest[count], "%02lu:%02lu:%02lu", hours, mins, secs);
    strncpy(d, timest, 40);
    return;
}
static int devIocStatsGetEngineer (char **pval)
{
    char *spbuf;
    char **sppbuf;
    char *sp = notavail;

    /* Get value from environment or global variable */
    if      ((spbuf  = getenv(ENGINEER)))            sp = spbuf;
    else if ((sppbuf = epicsFindSymbol("engineer"))) sp = *sppbuf;
    else if ((spbuf = getenv("LOGNAME")))            sp = spbuf;
    else if ((spbuf = getenv("USER")))               sp = spbuf;
    *pval = sp;
    if (sp == notavail) return -1;
    return 0;
}
static int devIocStatsGetLocation (char **pval)
{
    char *spbuf;
    char **sppbuf;
    char *sp = notavail;

    /* Get value from environment or global variable */
    if      ((spbuf  = getenv(LOCATION)))            sp = spbuf;
    else if ((sppbuf = epicsFindSymbol("location"))) sp = *sppbuf;
    *pval = sp;
    if (sp == notavail) return -1;
    return 0;
}
int devIocStatsGetStartupScriptDefault (char **pval)
{
    char *spbuf;
    char **sppbuf;
    char *stbuf;
    char **sttbuf;
    char *sp = notavail;
    char *st = empty;
    int plen, len;

    /* Get values from environment or global variable */
    if      ((spbuf  = getenv(STARTUP)))                sp = spbuf;
    else if ((sppbuf = epicsFindSymbol("startup")))     sp = *sppbuf;
    else if ((spbuf  = getenv("IOCSH_STARTUP_SCRIPT"))) sp = spbuf;
    if      ((stbuf  = getenv(ST_CMD )))                st = stbuf;
    else if ((sttbuf = epicsFindSymbol("st_cmd")))      st = *sttbuf;

    /* Concatenate with a '/' inbetween */
    plen = strlen(sp);
    len = plen + strlen (st) + 2;
    if (len > scriptlen) { /* we need more space */
        script = realloc(script, len);
        if (script) scriptlen = len; else scriptlen = 0;
    }
    strcpy(script, sp);
    if (sp == notavail) script[plen] = '\0';
    else if (strlen(st) > 0) script[plen] = '/';
    strcpy(script+plen+1, st);
    *pval = script;
    if (sp == notavail) return -1;
    return 0;
}
