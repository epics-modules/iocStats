/* devIocStats.h -  Device Support Include for IOC statistics - based on */
/* devVXStats.c - Device Support Routines for vxWorks statistics */
/*
 *	Author: Jim Kowalkowski
 *	Date:  2/1/96
 *
 *	Experimental Physics and Industrial Control System (EPICS)
 *
 *	Copyright 1991, the Regents of the University of California,
 *	and the University of Chicago Board of Governors.
 *
 *	This software was produced under  U.S. Government contracts:
 *	(W-7405-ENG-36) at the Los Alamos National Laboratory,
 *	and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *	Initial development by:
 *		The Controls and Automation Group (AT-8)
 *		Ground Test Accelerator
 *		Accelerator Technology Division
 *		Los Alamos National Laboratory
 *
 *	Co-developed with
 *		The Controls and Computing Group
 *		Accelerator Systems Division
 *		Advanced Photon Source
 *		Argonne National Laboratory
 *
 */

/* Cluster info pool types */
#define DATA_POOL 0
#define SYS_POOL  1

/* devIocStats value types (different update rates) */
#define MEMORY_TYPE	0
#define LOAD_TYPE	1
#define FD_TYPE		2
#define CA_TYPE		3
#define STATIC_TYPE	4
#define TOTAL_TYPES	5

/* Names of environment variables (may be redefined in OSD include) */
#define STARTUP  "STARTUP"
#define ST_CMD   "ST_CMD"
#define ENGINEER "ENGINEER"
#define LOCATION "LOCATION"

#include "devIocStatsOSD.h"

typedef int clustInfo[CLUSTSIZES][4];

typedef struct {
    unsigned long numBytesTotal;
    unsigned long numBytesFree;
    unsigned long numBytesAlloc;
    unsigned long numBlocksFree;
    unsigned long numBlocksAlloc;
    unsigned long maxBlockSizeFree;
} memInfo;

typedef struct {
    int used;
    int max;
} fdInfo;

typedef struct {
    int ierrors;
    int oerrors;
} ifErrInfo;

/* Functions (API) for OSD layer */
/* All funcs return 0 (OK) / -1 (ERROR) */

/* CPU Usage */
extern int devIocStatsInitCpuUsage (void);
extern int devIocStatsGetCpuUsage (double *pval);

/* FD Usage */
extern int devIocStatsInitFDUsage (void);
extern int devIocStatsGetFDUsage (fdInfo *pval);

/* Memory Usage */
extern int devIocStatsInitMemUsage (void);
extern int devIocStatsGetMemUsage (memInfo *pval);

/* Suspended Tasks */
extern int devIocStatsInitSuspTasks (void);
extern int devIocStatsGetSuspTasks (int *pval);

/* Cluster Info */
extern int devIocStatsInitClusterInfo (void);
extern int devIocStatsGetClusterInfo (int pool, clustInfo *pval);
extern int devIocStatsGetClusterUsage (int pool, int *pval);

/* Network Interface Errors */
extern int devIocStatsInitIFErrors (void);
extern int devIocStatsGetIFErrors (ifErrInfo *pval);

/* Boot Info */
extern int devIocStatsInitBootInfo (void);
extern int devIocStatsGetStartupScript (char **pval);
extern int devIocStatsGetBootLine (char **pval);
extern int devIocStatsGetEngineer (char **pval);
extern int devIocStatsGetLocation (char **pval);

/* System Info */
extern int devIocStatsInitSystemInfo (void);
extern int devIocStatsGetBSPVersion (char **pval);
extern int devIocStatsGetKernelVersion (char **pval);

/* Host Info */
extern int devIocStatsInitHostInfo (void);
extern int devIocStatsGetPwd (char **pval);
extern int devIocStatsGetHostname (char **pval);
