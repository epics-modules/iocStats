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

/* osdCpuTemp.c - Temperature of the CPU: default implementation = do nothing */

/*
 *  Author: Florian Feldbauer (RUB)
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <devIocStats.h>

int devIocStatsGetCpuTemp(int *pval) {
  FILE *pFile = fopen ( "/sys/class/thermal/thermal_zone0/temp", "r" );
  if( !pFile ) {
    fprintf( stderr, "\033[31;1m: Could not open file '/sys/class/thermal/thermal_zone0/temp': %s\033[0m\n",
             strerror( errno ) );
    return -1;
  }

  unsigned num = fscanf( pFile, "%d", pval );
  if ( 1 != num ) {
    fprintf( stderr, "\033[31;1mCould not parse value\033[0m\n" );
  }
  fclose( pFile );
  return 0;
}
