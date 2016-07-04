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

/* osdPwdInfo.c - PWD info strings: default implementation = do nothing */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-20 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */
#include <windows.h> 
#include <stdio.h>

#include <devIocStats.h>

static char *notimpl = "<not implemented>";
char *pwd;
char *hostname;


int devIocStatsInitHostInfo (void) { 
    pwd =(char*)malloc(sizeof(char)*MAX_PATH);
    hostname =(char*)malloc(sizeof(char)*MAX_PATH);
    return 0; 
}

int devIocStatsGetPwd (char **pval)
{
    DWORD dwRet;
    dwRet = GetCurrentDirectory(MAX_PATH, pwd);
    if (dwRet == 0){
      *pval = notimpl;
      return -1;
    }else{
      *pval = pwd;
      return 0;
    }
}

int devIocStatsGetHostname (char **pval)
{

    
    *pval = getenv("COMPUTERNAME");
    return -1;
}
