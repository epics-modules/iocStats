/*************************************************************************\
* Copyright (c) 2012 Paul-Scherrer Institut(PSI)
* Copyright (c) 2009 Helmholtz-Zentrum Berlin fuer Materialien und Energie.
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdBootInfo.c - Boot info strings: default implementation = do nothing */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2012-03-16 Helge Brands (PSI) 
 *  2009-05-20 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#include <windows.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#include <devIocStats.h>

static char *notimpl = "<not implemented>";

int devIocStatsInitBootInfo (void) { return 0; }

int devIocStatsGetStartupScript (char **pval)
{
    *pval = GetCommandLine() ;
    
    return 0;
}

int devIocStatsGetBootLine (char **pval)
{
    *pval =notimpl ;
    return -1;
}

