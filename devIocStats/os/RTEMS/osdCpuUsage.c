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

/* osdCpuUsage.c - RTEMS implementation */

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
 *  2009-05-19  Ralph Lange (HZB/BESSY)
 *              restructured OSD parts
 */

#include <devIocStats.h>

# if   (__RTEMS_MAJOR__ > 4) \
   || (__RTEMS_MAJOR__ == 4 && __RTEMS_MINOR__ > 7)
typedef char objName[13];
#define RTEMS_OBJ_GET_NAME(tc,name) rtems_object_get_name((tc)->Object.id, sizeof(name),(name))
#ifdef SSRLAPPSMISCUTILS
static struct timespec prev_uptime   = {0};
static struct timespec prev_idletime = {0};
extern int isnan();
#include <ssrlAppsMiscUtils.h>
#endif
# else
typedef char * objName;
#define RTEMS_OBJ_GET_NAME(tc,name) name = (tc)->Object.name
# endif

#if defined(RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS) || \
    (__RTEMS_MAJOR__ > 4) || \
    (__RTEMS_MAJOR__ == 4 && __RTEMS_MINOR__ > 9)
#define CPU_ELAPSED_TIME(tc) ((double)(tc)->cpu_time_used.tv_sec + ((double)tc->cpu_time_used.tv_nsec/1E9))
#else
#define CPU_ELAPSED_TIME(tc) ((double)(tc)->ticks_executed)
#endif

/*
 * The following CPU usage logic from Phil Sorensen at CHESS
 *
 * The cpu usage is done the same way as in cpukit/libmisc/cpuuse
 * from the RTEMS source.
 */

static double prev_total = 0;
static double prev_idle  = 0;

static void cpu_ticks(double *total, double *idle)
{
    Objects_Information *obj;
    Thread_Control     *tc;

    int   x, y;
    objName name;

    *total = 0;
    *idle = 0;

    for (x = 1; x <= OBJECTS_APIS_LAST; x++) {
        if (!_Objects_Information_table[x]) {
            continue;
        }

        obj = _Objects_Information_table[x][1];
        if (obj) {
            for (y = 1; y <= obj->maximum; y++) {
                tc = (Thread_Control *)obj->local_table[y];
                if (tc) {
                    *total += CPU_ELAPSED_TIME(tc);
                    RTEMS_OBJ_GET_NAME( tc,  name );
                    if (name && name[0]) {
                        if (name[0] == 'I' && name[1] == 'D' &&
                            name[2] == 'L' && name[3] == 'E') {
                            *idle = CPU_ELAPSED_TIME(tc);
                        }
                    }
                }
            }
        }
    }
}

int devIocStatsInitCpuUsage (void)
{
    cpu_ticks(&prev_total, &prev_idle);
#ifdef SSRLAPPSMISCUTILS
    miscu_cpu_load_percentage_init(&prev_uptime, &prev_idletime);
#endif
    return 0;
}

int devIocStatsGetCpuUsage (loadInfo *pval)
{
#ifdef SSRLAPPSMISCUTILS
    pval->cpuLoad = miscu_cpu_load_percentage(&prev_uptime, &prev_idletime);
    if (isnan(pval->cpuLoad)) return -1; else return 0;
#else
    double total;
    double idle;
    double delta_total;
    double delta_idle;

    cpu_ticks(&total, &idle);

    if (total >= prev_total) {
        delta_total = total - prev_total;
        delta_idle  = idle - prev_idle;
    } else {
        delta_total = total;
        delta_idle  = idle;
    }
    prev_total = total;
    prev_idle = idle;

    if (delta_idle > delta_total)
        pval->cpuLoad = 0.0;
    else
        pval->cpuLoad = 100.0 - (delta_idle * 100.0 / delta_total);
    return 0;
#endif
}
