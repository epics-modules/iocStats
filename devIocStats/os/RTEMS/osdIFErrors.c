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

/* osdIFErrors.c - Network interface errors: RTEMS implementation */

/*
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
 *  2008-09-29  Stephanie Allison - moved os-specific parts to
 *              os/<os>/devIocStatsOSD.h and devIocStatsOSD.c.  Added reboot.
 *              Split into devIocStatsAnalog, devIocStatsString,
 *              devIocStatTest.
 *  2009-05-20  Ralph Lange (HZB/BESSY)
 *              Restructured OSD parts
 *
 */

#include <devIocStats.h>

int devIocStatsInitIFErrors(void) { return 0; }

#if RTEMS_LIBBSD_STACK

#include <ifaddrs.h>
#include <net/if.h>

int devIocStatsGetIFErrors(ifErrInfo *pval) {
  struct ifaddrs *ifap, *ifa;

  /* add all interfaces' errors */
  pval->ierrors = 0;
  pval->oerrors = 0;

  if (getifaddrs(&ifap) != 0) {
    return -1;
  }

  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr->sa_family != AF_LINK) {
      continue;
    }
#define IFA_STAT(s) (((struct if_data *)ifa->ifa_data)->ifi_##s)
    pval->ierrors += IFA_STAT(ierrors);
    pval->oerrors += IFA_STAT(oerrors);
  }

  freeifaddrs(ifap);

  return 0;
}

#else /* RTEMS_LIBBSD_STACK */

#include <net/if_var.h>

/* This would otherwise need _KERNEL to be defined... */
extern struct ifnet *ifnet;

int devIocStatsGetIFErrors(ifErrInfo *pval) {

  struct ifnet *ifp;

  /* add all interfaces' errors */
  pval->ierrors = 0;
  pval->oerrors = 0;
  for (ifp = ifnet; ifp != NULL; ifp = ifp->if_next) {
    pval->ierrors += ifp->if_ierrors;
    pval->oerrors += ifp->if_oerrors;
  }
  return 0;
}

#endif /* RTEMS_LIBBSD_STACK */
