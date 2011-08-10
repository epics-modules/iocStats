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

/* osdBootInfo.c - Boot info strings: vxWorks implementation */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-20 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#include <devIocStats.h>

#if 0
#define BOOT_DEV_LEN            20  /* max chars in device name */
#define BOOT_HOST_LEN           20  /* max chars in host name */
#define BOOT_ADDR_LEN           30  /* max chars in net addr */
#define BOOT_TARGET_ADDR_LEN    50  /* IP address + mask + lease times */
#define BOOT_ADDR_LEN           30  /* max chars in net addr */
#define BOOT_FILE_LEN           80  /* max chars in file name */
#define BOOT_USR_LEN            20  /* max chars in user name */
#define BOOT_PASSWORD_LEN       20  /* max chars in password */
#define BOOT_OTHER_LEN          80  /* max chars in "other" field */

#define BOOT_FIELD_LEN          80  /* max chars in boot field */

typedef struct                          /* BOOT_PARAMS */
{
    char bootDev [BOOT_DEV_LEN];        /* boot device code */
    char hostName [BOOT_HOST_LEN];      /* name of host */
    char targetName [BOOT_HOST_LEN];    /* name of target */
    char ead [BOOT_TARGET_ADDR_LEN];    /* ethernet internet addr */
    char bad [BOOT_TARGET_ADDR_LEN];    /* backplane internet addr */
    char had [BOOT_ADDR_LEN];           /* host internet addr */
    char gad [BOOT_ADDR_LEN];           /* gateway internet addr */
    char bootFile [BOOT_FILE_LEN];      /* name of boot file */
    char startupScript [BOOT_FILE_LEN]; /* name of startup script file */
    char usr [BOOT_USR_LEN];            /* user name */
    char passwd [BOOT_PASSWORD_LEN];    /* password */
    char other [BOOT_OTHER_LEN];        /* available for applications */
    int  procNum;                       /* processor number */
    int  flags;                         /* configuration flags */
    int  unitNum;                       /* network device unit number */
} BOOT_PARAMS;

extern STATUS bootStructToString (
    char *paramString, BOOT_PARAMS *pBootParams);
extern char * bootStringToStruct (
    char *bootString, BOOT_PARAMS *pBootParams);
#endif

static const int stcmd_offset = offsetof(BOOT_PARAMS,startupScript);
static BOOT_PARAMS bps;

int devIocStatsInitBootInfo (void) {
    bootStringToStruct(sysBootLine, &bps);
    return 0;
}

int devIocStatsGetStartupScript (char **pval)
{
    *pval = ((char*)(&bps))+stcmd_offset;
    return 0;
}

int devIocStatsGetBootLine (char **pval)
{
    *pval = sysBootLine;
    return 0;
}
