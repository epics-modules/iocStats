/*************************************************************************\
* Copyright (c) 2013 Helmholtz-Zentrum Berlin fuer Materialien und Energie.
* Copyright (c) 2009 Helmholtz-Zentrum Berlin fuer Materialien und Energie.
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdCpuUsage.c - vxWorks implementation: use cpu burner */

/* extracted from */
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
 *
 *	03-05-08	CAL	Add minMBuf
 *
 * Modifications for LCLS/SPEAR at SLAC:
 * ----------------
 *  08-09-29    Stephanie Allison - moved os-specific parts to
 *              os/<os>/devIocStatsOSD.h and devIocStatsOSD.c.  Added reboot.
 *              Split into devIocStatsAnalog, devIocStatsString,
 *              devIocStatTest.
 *
 *  2009-05-15  Ralph Lange (HZB/BESSY)
 *              restructured OSD parts
 *
 * ----------------
 *  2013-04-11  Ben Franksen (HZB/BESSY)
 *              New implementation using a simple counter to burn CPU,
 *              added some tests, added variables for re-configuration
 */

#include <stdio.h>
#include <wdLib.h>
#include <sysLib.h>

#include "epicsAssert.h"
#include "epicsMutex.h"
#include "epicsThread.h"

#include "devIocStats.h"

/*
 * Implementation Notes
 * 
 * Incrementing a counter is the most straight-forward way to burn cpu, just take
 * care that the counter variable is declared volatile. This should tell even
 * modern highly optimizing compilers not to optimize away the memory access.
 * 
 * We use a vxWorks watchdog timer to start the counter and to stop it after a
 * given amount of clock ticks so we can run the counter for a precisely defined
 * period of time.
 * 
 * Initial calibration runs exactly the same procedure as the measurement, but
 * saves the final value of the counter in a global variable (nBurnNoContention),
 * assuming that at the time we calibrate there is no contention for the CPU yet.
 * 
 * 
 * The actual measurement is done in a background task that periodically sleeps,
 * burns cpu, and then calculates the load from the value of the counter by
 * comparing it to the value of nBurnNoContention.
 * Sleep and burn periods of the measurement task can be re-configured by
 * setting global variables (cpuBurnTicks, cpuSleepTicks), both are initialized so
 * that they correspond to 5 seconds each.
 * 
 * The devIocStatsGetCpuUsage routine just returns the last value computed by the
 * background task.
 */

/*
 * global variables
 */
static unsigned nBurnNoContention;  /* counts per clock tick w/o cpu contention */
static epicsMutexId mutex;          /* protect usage against concurrent access */
static double usage;                /* current cpu load measurement result */

/* these are public so can be re-configured at runtime */
int cpuBurnTicks;                   /* number of clock ticks to burn */
int cpuSleepTicks;                  /* number of clock ticks to sleep */
int cpuBurnCalibrateTicks = 30;     /* number of ticks to burn when calibrating */
int cpuBurnCalibrateTests = 0;      /* number of calibration tests to run */

/*
 *  These variables are strictly reserved for communication between
 *  cpuBurn, startCounter, and stopCounter. Do not use elsewhere.
 */
static WDOG_ID wd;
static volatile unsigned burn;      /* run burn loop while this is true */
static volatile unsigned counter;   /* gets incremented in a loop to burn cpu */
static unsigned start;              /* value of counter on start tick */
static int nTicks;                  /* number of clock ticks to burn */

/* called from clock ISR */
static int stopCounter()
{
    burn = 0;
    return 0;
}

/* called from clock ISR */
static int startCounter()
{
    wdStart(wd, nTicks, stopCounter, 0);
    start = counter;
    return 0;
}

/* Warning: not re-entrant */
static unsigned cpuBurn(int numTicks)
{
    burn = TRUE;
    nTicks = numTicks;
    /* wait for the next tick to have a precise start time */
    wdStart(wd, 1, startCounter, 0);
    while (burn) {
        counter++;
    }
    return counter - start;
}

/*
 * Determine the variance of the results of cpuBurn.
 * To run this routine at startup before calibration,
 * set cpuBurnCalibrateTests to a positive number (e.g. 100)
 * somewhere in the startup file before the iocInit call.
 */
void cpuBurnCalibrateTest(int numTicks, int numTests)
{
    int i;
    unsigned minv = UINT_MAX, maxv = 0, sumv = 0;

    for (i = 0; i < numTests; i++) {
        unsigned v = cpuBurn(numTicks);

        sumv += v;
        if (v < minv)
            minv = v;
        if (v > maxv)
            maxv = v;
    }
    if (numTests > 0) {
        printf("cpuBurnMeasureTest: min=%u max=%u avg=%u rel_diff=%f\n",
            minv, maxv, sumv / numTests, (maxv - minv) / (double)minv);
        printf("  counts per %d ticks\n", numTicks);
    }
}

/*
 * Note by default this task keeps "real" cpu load at a minimum of 50% --
 * do not use with multicore ;-)
 *
 * Set cpuSleepTicks and cpuBurnTicks to re-configure how much cpu gets
 * burned and how often.
 */
static void cpuUsageTask(void *parm)
{
    while (TRUE) {
        unsigned nBurnRef = nBurnNoContention * cpuBurnTicks;
        unsigned nBurnNow = cpuBurn(cpuBurnTicks);

        /* if we are near zero load, this could happen due to small
           variations in timing */
        if (nBurnNow > nBurnRef) {
            nBurnNow = nBurnRef;
        }
        epicsMutexLock(mutex);
        usage = 100.0 * (double)(nBurnRef - nBurnNow) / (double)nBurnRef;
        epicsMutexUnlock(mutex);
        taskDelay(cpuSleepTicks);
    }
}

int devIocStatsGetCpuUsage(loadInfo *pinfo)
{
    epicsMutexLock(mutex);
    pinfo->cpuLoad = usage;
    epicsMutexUnlock(mutex);
    return 0;
}

int devIocStatsInitCpuUsage(void)
{
    wd = wdCreate();
    assert(wd);

    mutex = epicsMutexMustCreate();

    cpuBurnCalibrateTest(cpuBurnCalibrateTicks, cpuBurnCalibrateTests);

    /* default values: measure 5 seconds, then sleep 5 seconds */
    cpuSleepTicks = cpuBurnTicks = 5 * sysClkRateGet();

    /*
     * Calibration: determine number of counts to burn 1 clock tick,
     * then calculate the result up to cpuBurnTicks ticks.
     */
    nBurnNoContention =
        cpuBurn(cpuBurnCalibrateTicks) / cpuBurnCalibrateTicks;

    /* assume initial load is zero */
    usage = 0.0;

    epicsThreadMustCreate("cpuUsageTask", epicsThreadPriorityMin,
        epicsThreadGetStackSize(epicsThreadStackMedium), cpuUsageTask, 0);
    return 0;
}
