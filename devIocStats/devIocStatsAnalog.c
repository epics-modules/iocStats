/*************************************************************************\
* Copyright (c) 2009-2010 Helmholtz-Zentrum Berlin
*     fuer Materialien und Energie GmbH.
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

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
 *	03-05-08	CAL	Add minMBuf
 *
 * Modifications for LCLS/SPEAR at SLAC:
 * ----------------
 *  08-09-29    Stephanie Allison - moved os-specific parts to
 *              os/<os>/devIocStatsOSD.h and devIocStatsOSD.c.
 *              Split into devIocStatsAnalog, devIocStatsString,
 *              devIocStatTest.
 *  2009-05-15  Ralph Lange (HZB/BESSY)
 *              Restructured OSD parts
 *  2010-07-14  Ralph Lange (HZB/BESSY)
 *              Added CPU Utilization (IOC load), number of CPUs
 *  2010-08-12  Stephanie Allison (SLAC):
 *              Added RAM workspace support developed by
 *              Charlie Xu.
 *  2015-04-27  Stephanie Allison (SLAC):
 *              Added process ID and parent process ID.
 *              Perform statistics in a task separate from the low priority
 *              callback task.
 */

/*
	--------------------------------------------------------------------
	Note that the valid values for the parm field of the link
	information are:

	ai (DTYP="IOC stats"):
		free_bytes	 - number of bytes in IOC not allocated
		total_bytes      - number of bytes physically installed
		allocated_bytes  - number of bytes allocated
		max_free	 - size of largest free block
		free_blocks	 - number of blocks in IOC not allocated
		allocated_blocks - number of blocks allocated
                sys_cpuload	 - estimated percent CPU load on the system
                ioc_cpuload      - estimated percent CPU utilization by this IOC
                no_of_cpus       - number of CPU cores on the system
              ( cpu		 - same as ioc_cpuload [for compatibility] )
                suspended_tasks	 - number of suspended tasks
		fd		 - number of file descriptors currently in use
		max_fd		 - max number of file descriptors
		ca_clients	 - number of current CA clients
		ca_connections	 - number of current CA connections
                min_data_mbuf    - minimum percent free data   MBUFs
                min_sys_mbuf	 - minimum percent free system MBUFs
                data_mbuf	 - number of data   MBUFs
                sys_mbuf	 - number of system MBUFs
                inp_errs	 - number of IF input  errors
                out_errs	 - number of IF output errors
		records	         - number of records
		proc_id	         - process ID
		parent_proc_id	 - parent process ID
                workspace_alloc_bytes - number of RAM workspace allocated bytes
                workspace_free_bytes  - number of RAM workspace free bytes
                workspace_total_bytes - number of RAM workspace total bytes

	ai (DTYP="IOC stats clusts"):
                clust_info <pool> <index> <type> where:
		   pool		 - 0 (data) or 1 (system)
		   index         - index into cluster array
		   type		 - 0=size, 1=clusters, 2=free, 3=usage


	ao:
		memoryScanRate	 - max rate at which new memory stats can be read
		fdScanRate	 - max rate at which file descriptors can be counted
		cpuScanRate	 - max rate at which cpu load can be calculated
		caConnScanRate	 - max rate at which CA connections can be calculated

	* scan rates are all in seconds

	default rates:
		10 - memory scan rate
		20 - cpu scan rate
		10 - fd scan rate
		15 - CA scan rate
*/

#include <string.h>
#include <time.h>

#include <epicsThread.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsVersion.h>

#include <rsrv.h>
#include <dbAccess.h>
#include <dbStaticLib.h>
#include <dbScan.h>
#include <devSup.h>
#include <menuConvert.h>
#include <aiRecord.h>
#include <aoRecord.h>
#include <recGbl.h>
#include <epicsExport.h>
#include <initHooks.h>
#include <callback.h>

#include "devIocStats.h"

#define BASE_HAS_QUEUE_STATUS (EPICS_VERSION_INT == VERSION_INT(3, 16, 2, 0)) || (EPICS_VERSION_INT >= VERSION_INT(7, 0, 2, 0))

struct aStats
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_write;
	DEVSUPFUN	special_linconv;
};
typedef struct aStats aStats;


struct pvtArea
{
	int index;
	int type;
};
typedef struct pvtArea pvtArea;

struct pvtClustArea
{
	int pool;
	int size;
	int elem;
};
typedef struct pvtClustArea pvtClustArea;

typedef void (*statGetFunc)(double*);

struct validGetParms
{
	char* name;
	statGetFunc func;
	int type;
};
typedef struct validGetParms validGetParms;

struct scanInfo
{
	IOSCANPVT ioscan;
	epicsTimerId  wd;
	volatile int total;			/* total users connected */
	volatile int on;			/* watch dog on? */
	double rate_sec;	/* seconds */
};
typedef struct scanInfo scanInfo;

static long ai_init(int pass);
static long ai_init_record(aiRecord*);
static long ai_read(aiRecord*);
static long ai_ioint_info(int cmd,aiRecord* pr,IOSCANPVT* iopvt);

static long ai_clusts_init(int pass);
static long ai_clusts_init_record(aiRecord *);
static long ai_clusts_read(aiRecord *);

static long ao_init_record(aoRecord* pr);
static long ao_write(aoRecord*);

static void statsFreeBytes(double*);
static void statsFreeBlocks(double*);
static void statsAllocBytes(double*);
static void statsAllocBlocks(double*);
static void statsMaxFree(double*);
static void statsTotalBytes(double*);
static void statsWSFreeBytes(double*);
static void statsWSAllocBytes(double*);
static void statsWSTotalBytes(double*);
static void statsCpuUsage(double*);
static void statsCpuUtilization(double*);
static void statsNoOfCpus(double*);
static void statsSuspendedTasks(double*);
static void statsFdUsage(double*);
static void statsFdMax(double*);
static void statsCAConnects(double*);
static void statsCAClients(double*);
static void statsMinDataMBuf(double*);
static void statsMinSysMBuf(double*);
static void statsDataMBuf(double*);
static void statsSysMBuf(double*);
static void statsIFIErrs(double *);
static void statsIFOErrs(double *);
static void statsRecords(double *);
static void statsPID(double *);
static void statsPPID(double *);
static void statsScanOnceQHiWtrMrk(double*);
static void statsScanOnceQUsed(double*);
static void statsScanOnceQSize(double*);
static void statsScanOnceQOverruns(double*);
static void statsCbQSize(double*);
static void statsCbLowQHiWtrMrk(double*);
static void statsCbLowQUsed(double*);
static void statsCbLowQOverruns(double*);
static void statsCbMediumQHiWtrMrk(double*);
static void statsCbMediumQUsed(double*);
static void statsCbMediumQOverruns(double*);
static void statsCbHighQHiWtrMrk(double*);
static void statsCbHighQUsed(double*);
static void statsCbHighQOverruns(double*);

struct {
	char *name;
	double scan_rate;
} parmTypes[] = {
	{ "memory_scan_rate",	10.0 },
	{ "cpu_scan_rate",	20.0 },
	{ "fd_scan_rate",	10.0 },
	{ "ca_scan_rate", 	15.0 },
    { "queue_scan_rate",	1.0 },
	{ NULL,			0.0  },
};

static validGetParms statsGetParms[]={
	{ "free_bytes",			statsFreeBytes,		MEMORY_TYPE },
	{ "free_blocks",		statsFreeBlocks,	MEMORY_TYPE },
	{ "max_free",		   	statsMaxFree,		MEMORY_TYPE },
	{ "allocated_bytes",		statsAllocBytes,	MEMORY_TYPE },
	{ "allocated_blocks",		statsAllocBlocks,	MEMORY_TYPE },
        { "total_bytes",		statsTotalBytes,	MEMORY_TYPE },
        { "workspace_alloc_bytes",	statsWSAllocBytes,	MEMORY_TYPE },
        { "workspace_free_bytes",	statsWSFreeBytes,	MEMORY_TYPE },
        { "workspace_total_bytes",	statsWSTotalBytes,	MEMORY_TYPE },
        { "sys_cpuload",		statsCpuUsage,		LOAD_TYPE },
        { "ioc_cpuload",		statsCpuUtilization,	LOAD_TYPE },
        { "cpu",			statsCpuUtilization,    LOAD_TYPE },
        { "no_of_cpus",			statsNoOfCpus,		LOAD_TYPE },
        { "suspended_tasks",		statsSuspendedTasks,	LOAD_TYPE },
	{ "fd",				statsFdUsage,		FD_TYPE },
        { "maxfd",			statsFdMax,	        FD_TYPE },
	{ "ca_clients",			statsCAClients,		CA_TYPE },
	{ "ca_connections",		statsCAConnects,	CA_TYPE },
	{ "min_data_mbuf",		statsMinDataMBuf,	MEMORY_TYPE },
	{ "min_sys_mbuf",		statsMinSysMBuf,	MEMORY_TYPE },
	{ "data_mbuf",			statsDataMBuf,		MEMORY_TYPE },
	{ "sys_mbuf",			statsSysMBuf,		MEMORY_TYPE },
	{ "inp_errs",			statsIFIErrs,		MEMORY_TYPE },
	{ "out_errs",			statsIFOErrs,		MEMORY_TYPE },
	{ "records",			statsRecords,           STATIC_TYPE },
	{ "proc_id",			statsPID,               STATIC_TYPE },
	{ "parent_proc_id",		statsPPID,              STATIC_TYPE },
	{ "scanOnceQueueHiWtrMrk",	statsScanOnceQHiWtrMrk,	QUEUE_TYPE },
	{ "scanOnceQueueUsed",		statsScanOnceQUsed,	QUEUE_TYPE },
	{ "scanOnceQueueSize",		statsScanOnceQSize,	STATIC_TYPE },
	{ "scanOnceQueueOverruns",	statsScanOnceQOverruns,	QUEUE_TYPE },
	{ "cbQueueSize",		statsCbQSize,		STATIC_TYPE },
	{ "cbLowQueueHiWtrMrk",		statsCbLowQHiWtrMrk,	QUEUE_TYPE },
	{ "cbLowQueueUsed",		statsCbLowQUsed,	QUEUE_TYPE },
	{ "cbLowQueueOverruns",		statsCbLowQOverruns,	QUEUE_TYPE },
	{ "cbMediumQueueHiWtrMrk",	statsCbMediumQHiWtrMrk,	QUEUE_TYPE },
	{ "cbMediumQueueUsed",		statsCbMediumQUsed,	QUEUE_TYPE },
	{ "cbMediumQueueOverruns",	statsCbMediumQOverruns,	QUEUE_TYPE },
	{ "cbHighQueueHiWtrMrk",	statsCbHighQHiWtrMrk,	QUEUE_TYPE },
	{ "cbHighQueueUsed",		statsCbHighQUsed,	QUEUE_TYPE },
	{ "cbHighQueueOverruns",	statsCbHighQOverruns,	QUEUE_TYPE },
	{ NULL,NULL,0 }
};

aStats devAiStats={ 6,NULL,ai_init,ai_init_record,ai_ioint_info,ai_read,NULL };
epicsExportAddress(dset,devAiStats);
aStats devAoStats={ 6,NULL,NULL,ao_init_record,NULL,ao_write,NULL };
epicsExportAddress(dset,devAoStats);
aStats devAiClusts = {6,NULL,ai_clusts_init,ai_clusts_init_record,NULL,ai_clusts_read,NULL };
epicsExportAddress(dset,devAiClusts);

static memInfo meminfo = {0.0,0.0,0.0,0.0,0.0,0.0};
static memInfo workspaceinfo = {0.0,0.0,0.0,0.0,0.0,0.0};
#if BASE_HAS_QUEUE_STATUS
static scanOnceQueueStats scanOnceQStatus;
static callbackQueueStats callbackQStatus;
#endif
static int queueDataInitialized;
static scanInfo scan[TOTAL_TYPES] = {{0}};
static fdInfo fdusage = {0,0};
static loadInfo loadinfo = {1,0.,0.};
static int susptasknumber = 0;
static int recordnumber = 0;
static clustInfo clustinfo[2] = {{{0}},{{0}}};
static int mbufnumber[2] = {0,0};
static ifErrInfo iferrors = {0,0};
static unsigned cainfo_clients = 0;
static unsigned cainfo_connex  = 0;
static epicsTimerQueueId timerQ = 0;
static epicsMutexId scan_mutex;
static int caServInitialized = 0;

/* ---------------------------------------------------------------------- */

/* 
 * Run timer task just below the low priority callback task and higher
 * than the default for the channel access tasks.  Also, set okToShare to 0
 * so that the task is dedicated to devIocStats.
 */
static void timerQCreate(void*unused)
{
	timerQ = epicsTimerQueueAllocate(0, epicsThreadPriorityScanLow - 2);
}

static epicsTimerId
wdogCreate(void (*fn)(int), long arg)
{
	static epicsThreadOnceId inited = EPICS_THREAD_ONCE_INIT;

	/* lazy init of timer queue */
	if ( EPICS_THREAD_ONCE_INIT == inited )
		epicsThreadOnce( &inited, timerQCreate, 0);

	return epicsTimerQueueCreateTimer(timerQ, (void (*)(void*))fn, (void*)arg);
}

static void notifyOnCaServInit(initHookState state)
{
    if (state == initHookAfterCaServerInit) {
        caServInitialized = 1;
    }
}

static void getQueueData() {
    #if BASE_HAS_QUEUE_STATUS
        scanOnceQueueStatus(0, &scanOnceQStatus);
        callbackQueueStatus(0, &callbackQStatus);
    #endif
    queueDataInitialized = 1;
}

static void scan_time(int type)
{
    switch(type) {
      case MEMORY_TYPE:
      {
	memInfo   meminfo_local = {0.0,0.0,0.0,0.0,0.0,0.0};
	memInfo   workspaceinfo_local = {0.0,0.0,0.0,0.0,0.0,0.0};
	int       mbufnumber_local[2] = {0,0};
	ifErrInfo iferrors_local = {0,0};
        devIocStatsGetMemUsage(&meminfo_local);
        devIocStatsGetWorkspaceUsage(&workspaceinfo_local);
	devIocStatsGetClusterUsage(SYS_POOL, &mbufnumber_local[SYS_POOL]);
	devIocStatsGetClusterUsage(DATA_POOL, &mbufnumber_local[DATA_POOL]);
	devIocStatsGetIFErrors(&iferrors_local);
        epicsMutexLock(scan_mutex);
	meminfo               = meminfo_local;
	workspaceinfo         = workspaceinfo_local;
	mbufnumber[SYS_POOL]  = mbufnumber_local[SYS_POOL];
	mbufnumber[DATA_POOL] = mbufnumber_local[DATA_POOL];
	iferrors              = iferrors_local;
	devIocStatsGetClusterInfo(SYS_POOL, &clustinfo[SYS_POOL]);
	devIocStatsGetClusterInfo(DATA_POOL, &clustinfo[DATA_POOL]);
        epicsMutexUnlock(scan_mutex);
	break;
      }
      case LOAD_TYPE:
      {
	loadInfo loadinfo_local = {1,0.,0.};
	int      susptasknumber_local = 0;
        devIocStatsGetCpuUsage(&loadinfo_local);
        devIocStatsGetCpuUtilization(&loadinfo_local);
        devIocStatsGetSuspTasks(&susptasknumber_local);
        epicsMutexLock(scan_mutex);
	loadinfo       = loadinfo_local;
	susptasknumber = susptasknumber_local;
        epicsMutexUnlock(scan_mutex);
	break;
      }
      case FD_TYPE:
      {
	fdInfo   fdusage_local = {0,0};
        devIocStatsGetFDUsage(&fdusage_local);
        epicsMutexLock(scan_mutex);
	fdusage = fdusage_local;
        epicsMutexUnlock(scan_mutex);
	break;
      }
      case CA_TYPE:
      {
          unsigned cainfo_clients_local = 0;
          unsigned cainfo_connex_local  = 0;

          /* Guard to ensure that the caServ is initialized */
          if (!caServInitialized) {
              break;
          }

          casStatsFetch(&cainfo_connex_local, &cainfo_clients_local);
          epicsMutexLock(scan_mutex);
          cainfo_clients = cainfo_clients_local;
          cainfo_connex  = cainfo_connex_local;
          epicsMutexUnlock(scan_mutex);
          break;
      }
      case QUEUE_TYPE:
      {
          epicsMutexLock(scan_mutex);
          getQueueData();
          epicsMutexUnlock(scan_mutex);
          break;
      }
      default:
        break;
    }
    scanIoRequest(scan[type].ioscan);
    if(scan[type].on)
		epicsTimerStartDelay(scan[type].wd, scan[type].rate_sec);
}

/* -------------------------------------------------------------------- */

static long ai_clusts_init(int pass)
{
    if (pass) return 0;
    devIocStatsInitClusterInfo();
    return 0;
}

static long ai_init(int pass)
{
    long i;

    if (pass) return 0;

    initHookRegister(&notifyOnCaServInit);

    /* Create timers */
    for (i = 0; i < TOTAL_TYPES; i++) {
        scanIoInit(&scan[i].ioscan);
        scan[i].wd = wdogCreate(scan_time, i);
        scan[i].total = 0;
        scan[i].on = 0;
        scan[i].rate_sec = parmTypes[i].scan_rate;
    }

    /* Init OSD stuff */
    scan_mutex = epicsMutexMustCreate();
    devIocStatsInitCpuUsage();
    devIocStatsInitCpuUtilization(&loadinfo);
    devIocStatsInitFDUsage();
    devIocStatsInitMemUsage();
    devIocStatsInitWorkspaceUsage();
    devIocStatsInitSuspTasks();
    devIocStatsInitIFErrors();
    /* Get initial values of a few things that don't change much */
    devIocStatsGetClusterInfo(SYS_POOL, &clustinfo[SYS_POOL]);
    devIocStatsGetClusterInfo(DATA_POOL, &clustinfo[DATA_POOL]);
    devIocStatsGetClusterUsage(SYS_POOL, &mbufnumber[SYS_POOL]);
    devIocStatsGetClusterUsage(DATA_POOL, &mbufnumber[DATA_POOL]);
    devIocStatsGetCpuUtilization(&loadinfo);
    devIocStatsGetIFErrors(&iferrors);
    devIocStatsGetFDUsage(&fdusage);

    /* Count EPICS records */
    if (pdbbase) {
        DBENTRY dbentry;
        long	  status;
        dbInitEntry(pdbbase,&dbentry);
        status = dbFirstRecordType(&dbentry);
        while (!status) {
            recordnumber += dbGetNRecords(&dbentry);
            status = dbNextRecordType(&dbentry);
        }
        dbFinishEntry(&dbentry);
    }
    return 0;
}

static long ai_clusts_init_record(aiRecord *pr)
{
	int elem = 0, size = 0, pool = 0,  parms = 0;
	char	*parm;
	pvtClustArea	*pvt = NULL;

	if(pr->inp.type!=INST_IO)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devAiClusts (init_record) Illegal INP field");
		return S_db_badField;
	}
	parm = pr->inp.value.instio.string;
	parms = sscanf(parm,"clust_info %d %d %d", &pool, &size, &elem);
	if ((parms == 3) &&
            (pool >= 0) && (pool < 2) &&
            (size >= 0) &&
            (elem >= 0) && (elem < 4))
	{
		pvt=(pvtClustArea*)malloc(sizeof(pvtClustArea));
		if (pvt)
		{
	   		pvt->pool = pool;
	   		pvt->size = size;
	   		pvt->elem = elem;
		}
	}
	if (pvt == NULL)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devAiClusts (init_record) Illegal INP parm field");
		return S_db_badField;
	}
	/* Make sure record processing routine does not perform any conversion*/
	pr->linr=menuConvertNO_CONVERSION;
	pr->dpvt=pvt;
	return 0;
}

static long ai_init_record(aiRecord* pr)
{
	int		i;
	char	*parm;
	pvtArea	*pvt = NULL;

	if(pr->inp.type!=INST_IO)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devAiStats (init_record) Illegal INP field");
		return S_db_badField;
	}
	parm = pr->inp.value.instio.string;
	for(i=0;statsGetParms[i].name && pvt==NULL;i++)
	{
		if(strcmp(parm,statsGetParms[i].name)==0)
		{
			pvt=(pvtArea*)malloc(sizeof(pvtArea));
			pvt->index=i;
			pvt->type=statsGetParms[i].type;
		}
	}
	
	if(pvt==NULL)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devAiStats (init_record) Illegal INP parm field");
		return S_db_badField;
	}

	/* Make sure record processing routine does not perform any conversion*/
	pr->linr=menuConvertNO_CONVERSION;
	pr->dpvt=pvt;
	return 0;
}

static long ao_init_record(aoRecord* pr)
{
	int		type;
	char	*parm;
	pvtArea	*pvt = NULL;

	if(pr->out.type!=INST_IO)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devAiStats (init_record) Illegal OUT field");
		return S_db_badField;
	}
	parm = pr->out.value.instio.string;
	for(type=0; type<TOTAL_TYPES; type++)
	{
		if(parmTypes[type].name && strcmp(parm,parmTypes[type].name)==0)
		{
			pvt=(pvtArea*)malloc(sizeof(pvtArea));
			pvt->index=type;
			pvt->type=type;
		}
	}
	if(pvt==NULL)
	{
		recGblRecordError(S_db_badField,(void*)pr,
			"devAiStats (init_record) Illegal INP parm field");
		return S_db_badField;
	}

	/* Initialize value with default if not set in db */
	if (!pr->val)
		pr->val=parmTypes[pvt->type].scan_rate;

	/* Make sure record processing routine does not perform any conversion*/
	pr->linr=menuConvertNO_CONVERSION;
	pr->dpvt=pvt;
	return 2;
}

static long ai_ioint_info(int cmd,aiRecord* pr,IOSCANPVT* iopvt)
{
	pvtArea* pvt=(pvtArea*)pr->dpvt;

	if (!pvt) return S_dev_badInpType;

	if(cmd==0) /* added */
	{
		if(scan[pvt->type].total++ == 0)
		{
			/* start a watchdog */
			epicsTimerStartDelay(scan[pvt->type].wd, scan[pvt->type].rate_sec);
			scan[pvt->type].on=1;
		}
	}
	else /* deleted */
	{
		if(--scan[pvt->type].total == 0)
			scan[pvt->type].on=0; /* stop the watchdog */
	}

	*iopvt=scan[pvt->type].ioscan;
	return 0;
}

static long ao_write(aoRecord* pr)
{
	pvtArea	*pvt=(pvtArea*)pr->dpvt;
	int type;

	if (!pvt) return S_dev_badInpType;

	type=pvt->type;
        
        if (pr->val > 0.0)
          scan[type].rate_sec = pr->val;
        else
          pr->val = scan[type].rate_sec;
        pr->udf=0;
	return 0;
}

/* Cluster info read - returning value from global array */
static long ai_clusts_read(aiRecord* prec)
{
    pvtClustArea* pvt=(pvtClustArea*)prec->dpvt;

    if (!pvt) return S_dev_badInpType;

    if (pvt->size < CLUSTSIZES) {
        epicsMutexLock(scan_mutex);
        prec->val = clustinfo[pvt->pool][pvt->size][pvt->elem];
        epicsMutexUnlock(scan_mutex);
    } else {
        prec->val = 0;
    }
    prec->udf = 0;
    return 2; /* don't convert */
}

/* Generic read - calling function from table */
static long ai_read(aiRecord* pr)
{
    double val;
    pvtArea* pvt=(pvtArea*)pr->dpvt;

    if (!pvt) return S_dev_badInpType;

    epicsMutexLock(scan_mutex);
    statsGetParms[pvt->index].func(&val);
    epicsMutexUnlock(scan_mutex);
    pr->val = val;
    pr->udf = 0;
    return 2; /* don't convert */
}

/* -------------------------------------------------------------------- */


static double minMBuf(int pool)
{
    int i = 0;
    double lowest = 1.0, comp;

    while ((clustinfo[pool][i][0] != 0) && (i < CLUSTSIZES))
    {
        if (clustinfo[pool][i][1] != 0) {
            comp = ((double)clustinfo[pool][i][2]) / clustinfo[pool][i][1];
            if (comp < lowest) lowest = comp;
        }
        i++;
    }
    return (lowest * 100);
}

static void statsFreeBytes(double* val)
{
    *val = meminfo.numBytesFree;
}
static void statsFreeBlocks(double* val)
{
    *val = meminfo.numBlocksFree;
}
static void statsAllocBytes(double* val)
{
    *val = meminfo.numBytesAlloc;
}
static void statsAllocBlocks(double* val)
{
    *val = meminfo.numBlocksAlloc;
}
static void statsMaxFree(double* val)
{
    *val = meminfo.maxBlockSizeFree;
}
static void statsTotalBytes(double* val)
{
    *val = meminfo.numBytesTotal;
}
static void statsWSAllocBytes(double* val)
{
    *val = workspaceinfo.numBytesAlloc;
}
static void statsWSFreeBytes(double* val)
{
    *val = workspaceinfo.numBytesFree;
}
static void statsWSTotalBytes(double* val)
{
    *val = workspaceinfo.numBytesTotal;
}
static void statsCpuUsage(double* val)
{
    *val = loadinfo.cpuLoad;
}
static void statsCpuUtilization(double* val)
{
    *val = loadinfo.iocLoad;
}
static void statsNoOfCpus(double* val)
{
    *val = (double)loadinfo.noOfCpus;
}
static void statsSuspendedTasks(double *val)
{
    *val = (double)susptasknumber;
}
static void statsFdUsage(double* val)
{
    *val = (double)fdusage.used;
}
static void statsFdMax(double* val)
{
    *val = (double)fdusage.max;
}
static void statsCAClients(double* val)
{
    *val = (double)cainfo_clients;
}
static void statsCAConnects(double* val)
{
    *val = (double)cainfo_connex;
}
static void statsMinSysMBuf(double* val)
{
    *val = minMBuf(SYS_POOL);
}
static void statsMinDataMBuf(double* val)
{
    *val = minMBuf(DATA_POOL);
}
static void statsSysMBuf(double* val)
{
    *val = (double)mbufnumber[SYS_POOL];
}
static void statsDataMBuf(double* val)
{
    *val = (double)mbufnumber[DATA_POOL];
}
static void statsIFIErrs(double* val)
{
    *val = (double)iferrors.ierrors;
}
static void statsIFOErrs(double* val)
{
    *val = (double)iferrors.oerrors;
}
static void statsRecords(double *val)
{
    *val = (double)recordnumber;
}
static void statsPID(double *val)
{
    *val = 0;
    devIocStatsGetPID(val);
}
static void statsPPID(double *val)
{
    *val = 0;
    devIocStatsGetPPID(val);
}

static void statsScanOnceQHiWtrMrk(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = scanOnceQStatus.maxUsed;
#else
    *val = 0;
#endif
}
static void statsScanOnceQUsed(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = scanOnceQStatus.numUsed;
#else
    *val = 0;
#endif
}
static void statsScanOnceQSize(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = scanOnceQStatus.size;
#else
    *val = 0;
#endif
}
static void statsScanOnceQOverruns(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = scanOnceQStatus.numOverflow;
#else
    *val = 0;
#endif
}

static void statsCbQSize(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.size;
#else
    *val = 0;
#endif
}

static void statsCbLowQHiWtrMrk(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.maxUsed[priorityLow];
#else
    *val = 0;
#endif
}
static void statsCbLowQUsed(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.numUsed[priorityLow];
#else
    *val = 0;
#endif
}
static void statsCbLowQOverruns(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.numOverflow[priorityLow];
#else
    *val = 0;
#endif
}

static void statsCbMediumQHiWtrMrk(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.maxUsed[priorityMedium];
#else
    *val = 0;
#endif
}
static void statsCbMediumQUsed(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.numUsed[priorityMedium];
#else
    *val = 0;
#endif
}
static void statsCbMediumQOverruns(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.numOverflow[priorityMedium];
#else
    *val = 0;
#endif
}

static void statsCbHighQHiWtrMrk(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.maxUsed[priorityHigh];
#else
    *val = 0;
#endif
}
static void statsCbHighQUsed(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.numUsed[priorityHigh];
#else
    *val = 0;
#endif
}
static void statsCbHighQOverruns(double *val)
{
#if BASE_HAS_QUEUE_STATUS
    if(!queueDataInitialized) getQueueData();
    *val = callbackQStatus.numOverflow[priorityHigh];
#else
    *val = 0;
#endif
}
