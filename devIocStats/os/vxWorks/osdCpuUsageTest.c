/*************************************************************************\
* Copyright (c) 2013 Helmholtz-Zentrum Berlin fuer Materialien und Energie.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/
/*
 * To test validity of cpu load measurements, run cpuBurnLoad50 on the vxWorks
 * shell for some time (e.g. 60 seconds) and monitor the cpu load. It should
 * increase by 50%.
 * 
 * The routine cpuBurnLoad50 creates an additional load of exactly 50%
 * (average) by periodically sleeping and then burning cpu for one clock tick
 * each.
 */
#include <stdio.h>

#include <wdLib.h>
#include <semLib.h>
#include <sysLib.h>

static WDOG_ID test_wd;
static volatile unsigned test_counter;
static volatile unsigned test_burn;
static volatile unsigned test_ticks;
static SEM_ID test_sem;

static int testTick()
{
    if (test_ticks--) {
        wdStart(test_wd, 1, testTick, 0);
    }
    if (test_burn) {
        test_burn = FALSE;  /* we are busy, stop the counter */
    } else {
        semGive(test_sem);  /* we are sleeping, wake up */
        test_burn = TRUE;
    }
    return 0;
}

/* Test routine, increase avg. cpu load by exactly 50% */
int cpuBurnLoad50(int seconds)
{
    test_wd = wdCreate();
    if (!test_wd) {
        printf("Error: cannot create watchdog timer\n");
        return -1;
    }

    test_sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    if (!test_sem) {
        printf("Error: cannot create semaphore\n");
        wdDelete(test_wd);
        return -1;
    }

    test_ticks = seconds*sysClkRateGet();
    test_burn = FALSE;
    wdStart(test_wd, 1, testTick, 0);
    while (test_ticks) {
        /* sleep for one tick */
        semTake(test_sem, WAIT_FOREVER);
        /* burn cpu for one tick */
        while (test_burn) {
            test_counter++;
        }
    }

    semDelete(test_sem);
    wdDelete(test_wd);
    return 0;
}
