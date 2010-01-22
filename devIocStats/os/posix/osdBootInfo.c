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

/* osdBootInfo.c - Boot info strings: posix implementation = get from environment or symbol */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2009-05-20 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#include <devIocStats.h>
#include <epicsFindSymbol.h>

static char *notavail = "<not available>";
static char *empty    = "";
static char *script = 0;
static int scriptlen = 0;

int devIocStatsInitBootInfo (void) { return 0; }

int devIocStatsGetStartupScript (char **pval)
{
    char *spbuf;
    char *stbuf;
    char *sp = notavail;
    char *st = empty;
    int plen, len;

    /* Get values from environment or global variable */
    if ((spbuf = getenv(STARTUP)) || (spbuf = epicsFindSymbol("startup"))) sp = spbuf;
    if ((stbuf = getenv(ST_CMD )) || (stbuf = epicsFindSymbol("st_cmd" ))) st = stbuf;

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

int devIocStatsGetBootLine (char **pval)
{
    *pval = sysBootLine;
    return 0;
}

int devIocStatsGetEngineer (char **pval)
{
    char *spbuf;
    char *sp = notavail;

    /* Get value from environment or global variable */
    if ((spbuf = getenv(ENGINEER)) || (spbuf = epicsFindSymbol("engineer"))) sp = spbuf;
    *pval = sp;
    if (sp == notavail) return -1;
    return 0;
}
int devIocStatsGetLocation (char **pval)
{
    char *spbuf;
    char *sp = notavail;

    /* Get value from environment or global variable */
    if ((spbuf = getenv(LOCATION)) || (spbuf = epicsFindSymbol("location"))) sp = spbuf;
    *pval = sp;
    if (sp == notavail) return -1;
    return 0;
}
