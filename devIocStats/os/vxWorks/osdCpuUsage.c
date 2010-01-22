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
 */

#include <epicsEvent.h>
#include <epicsTime.h>
#include <epicsThread.h>

#include <devIocStats.h>

struct cpuUsageInfo
{
    epicsEventId startSem;
    int          didNotComplete;
    double       tNoContention;
    double       tNow;
    int          nBurnNoContention;
    int          nBurnNow;
    double       usage;
};
typedef struct cpuUsageInfo cpuUsageInfo;

static cpuUsageInfo cpuUsage = {0};

#if 0
/* This is not going to burn anything with a modern gcc
 * (which pre-computes the result AND knows that
 * sqrt has no side-effects). Unless we use the result
 * the whole routine is probably optimized away...
 * T.S, 2008.
 */
static double cpuBurn()
{
	 int i;
	 double result = 0.0;

	 for(i=0;i<5; i++) result += sqrt((double)i);
	 return(result);
}
#else
static double cpuBurn(void)
{
    epicsTimeStamp then, now;
    double         diff;

    /* poll the clock for 500us */
    epicsTimeGetCurrent(&then);
    do {
        epicsTimeGetCurrent(&now);
        diff = epicsTimeDiffInSeconds(&now,&then);
    } while ( diff < 0.0005 );
    return diff;
}
#endif

static void cpuUsageTask(void *parm)
{
    while(TRUE)
    {
        int	 i;
        epicsTimeStamp tStart, tEnd;

        epicsEventWait(cpuUsage.startSem);
        cpuUsage.nBurnNow=0;
        epicsTimeGetCurrent(&tStart);
        for(i=0; i< cpuUsage.nBurnNoContention; i++)
        {
            cpuBurn();
            epicsTimeGetCurrent(&tEnd);
            cpuUsage.tNow = epicsTimeDiffInSeconds(&tEnd, &tStart);
            ++cpuUsage.nBurnNow;
        }
        cpuUsage.didNotComplete = FALSE;
    }
}

int devIocStatsGetCpuUsage(double *pval)
{
    if (cpuUsage.startSem) {
        if (cpuUsage.didNotComplete && cpuUsage.nBurnNow==0) {
            cpuUsage.usage = 100.0;
        } else {
            double temp;
            double ticksNow,nBurnNow;

            ticksNow = cpuUsage.tNow;
            nBurnNow = (double)cpuUsage.nBurnNow;
            ticksNow *= (double)cpuUsage.nBurnNoContention/nBurnNow;
            temp = ticksNow - cpuUsage.tNoContention;
            temp = 100.0 * temp/ticksNow;
            if(temp<0.0 || temp>100.0) temp=0.0;/*take care of tick overflow*/
            cpuUsage.usage = temp;
        }
        cpuUsage.didNotComplete = TRUE;
        epicsEventSignal(cpuUsage.startSem);
    } else {
        cpuUsage.usage = 0.0;
    }
    *pval = cpuUsage.usage;
    return 0;
}

int devIocStatsInitCpuUsage(void)
{
    int    nBurnNoContention = 0;
    double tToWait = SECONDS_TO_BURN;
    epicsTimeStamp tStart, tEnd;

    /* Initialize only if OS wants to spin */
    if (tToWait > 0) {
        /*wait for a tick*/
        epicsTimeGetCurrent(&tStart);
        do { 
            epicsTimeGetCurrent(&tEnd);
        } while ( epicsTimeDiffInSeconds(&tEnd, &tStart) <= 0.0 );
        epicsTimeGetCurrent(&tStart);
        while(TRUE)
        {
            cpuBurn();
            epicsTimeGetCurrent(&tEnd);
            cpuUsage.tNow = epicsTimeDiffInSeconds(&tEnd, &tStart);
            if (cpuUsage.tNow >= tToWait ) break;
            nBurnNoContention++;
        }
        cpuUsage.nBurnNoContention = nBurnNoContention;
        cpuUsage.nBurnNow          = nBurnNoContention;
        cpuUsage.startSem = epicsEventMustCreate(epicsEventFull);
        cpuUsage.tNoContention = cpuUsage.tNow;
  /*
        * FIXME: epicsThreadPriorityMin is not really the lowest
        *        priority. We could use a native call to
        *        lower our priority further but OTOH there is not
        *        much going on at these low levels...
  */
        epicsThreadCreate("cpuUsageTask",
                          epicsThreadPriorityMin,
                          epicsThreadGetStackSize(epicsThreadStackMedium),
                                  (EPICSTHREADFUNC)cpuUsageTask,
                                   0);
    } else {
        cpuUsage.startSem = 0;
    }
    return 0;
}
