/* devIocStatsTest.c - Device Support Test Routines for IOC statistics - copied from */
/* devVXStats.c - Device Support Routines for vxWorks statistics */
/*
 *	Author: Jim Kowalkowski
 *	Date:  2/1/96
 *
 *	Experimental Physics and Industrial Control System (EPICS)
 *
 *	Copyright 1991, the Regents of the University of California,
 *	and the University of Chicago Board of Governors.
 *
 *	This software was produced under  U.S. Government contracts:
 *	(W-7405-ENG-36) at the Los Alamos National Laboratory,
 *	and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *	Initial development by:
 *		The Controls and Automation Group (AT-8)
 *		Ground Test Accelerator
 *		Accelerator Technology Division
 *		Los Alamos National Laboratory
 *
 *	Co-developed with
 *		The Controls and Computing Group
 *		Accelerator Systems Division
 *		Advanced Photon Source
 *		Argonne National Laboratory
 */

#include "epicsStdio.h"
#include "epicsThread.h"
#include "epicsTime.h"

/* ----------test routines----------------- */

/* Some sample loads on MVME167:
*	
*	-> sp jbk_artificial_load, 100000000, 10000, 1
*		Load average: 69%
*	-> sp jbk_artificial_load, 100000000, 100000, 1
*		Load average: 95%
*	-> sp jbk_artificial_load, 100000000, 25000, 1
*		Load average: 88%
*/
int jbk_artificial_load(unsigned long iter,unsigned long often,unsigned long tick_delay)
{
	volatile unsigned long i;
	double   delay = (double)tick_delay * epicsThreadSleepQuantum();

	if(iter==0)
	{
		printf("Usage: jbk_artificial_load(num_iterations,iter_betwn_delays,tick_delay)\n");
		return 0;
	}

	for(i=0;i<iter;i++)
	{
		if(i%often==0)
			epicsThreadSleep(delay);
	}

	return 0;
}

/*
 * Busyloop to be run from a task; this can be used to
 * test the CPU-usage estimate.
 */

double vxStats_busyloop_period=1.0;
volatile int vxStats_busyloop_run = 1;

void
vxStats_busyloop(unsigned busyperc)
{
epicsTimeStamp then, now;
double         fac = vxStats_busyloop_period/(double)100.0;

	if ( busyperc > 100 )
		busyperc = 100;

	while ( vxStats_busyloop_run ) {
		epicsTimeGetCurrent(&then);
		do {
			epicsTimeGetCurrent(&now);
		} while ( epicsTimeDiffInSeconds(&now,&then) < (double)busyperc*fac );
		
		epicsThreadSleep((double)(100-busyperc)*fac);
	}
}

