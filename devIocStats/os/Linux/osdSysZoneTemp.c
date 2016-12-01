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

/* osdSysZoneTemp.c - Temperature of the themal zone 0 : Linux implementation = use /sys/class/thermal/thermal_zone0/temp  */

/*
 *  Author: Jeong Han Lee (ESS)
 *
 *  Modification History
 *
 */

#include <stdio.h>
#include "devIocStats.h"

int devIocStatsInitSysZoneTemp (void) { return 0; }

int devIocStatsGetSysZoneTemp (tempInfo *pval) {
  static char statfile[] = "/sys/class/thermal/thermal_zone0/temp";
  int temp = 0;
  FILE *fp;
  fp = fopen(statfile, "r");
  if (fp) {
    fscanf(fp, "%d", &temp);
    fclose(fp);
  }
  pval->sysZoneTemp = temp;
  return 0;
}
