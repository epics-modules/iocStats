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

/* The kernel-internal `struct ifnet` linked list (walked via if_next) is
 * gone under libbsd -- ifnet is now a CK_STAILQ and if_ierrors/if_oerrors
 * are behind the if_get_counter KPI, neither meant for use outside the
 * kernel proper. getifaddrs()/struct if_data is the portable, userspace-
 * visible way to get the same per-interface error counts: each interface
 * contributes one AF_LINK entry whose ifa_data is its struct if_data. */
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/socket.h>

int devIocStatsInitIFErrors(void) { return 0; }

int devIocStatsGetIFErrors(ifErrInfo *pval) {
  struct ifaddrs *addrs, *ifa;

  pval->ierrors = 0;
  pval->oerrors = 0;

  if (getifaddrs(&addrs) < 0) {
    return -1;
  }

  for (ifa = addrs; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_LINK &&
        ifa->ifa_data != NULL) {
      struct if_data *ifd = (struct if_data *)ifa->ifa_data;
      pval->ierrors += ifd->ifi_ierrors;
      pval->oerrors += ifd->ifi_oerrors;
    }
  }

  freeifaddrs(addrs);
  return 0;
}
