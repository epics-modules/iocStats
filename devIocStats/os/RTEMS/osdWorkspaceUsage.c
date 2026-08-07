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

/* osdWorkspaceUsage.c - RTEMS implementation */

/*=============================================================================

 Name: osdWorkspaceUsage.c
 devIocStatsInitWorkspaceUsage      - Initialization when ai_init is called
 devIocStatsGetWorkspaceUsage     - Getting the information for RTEMS RAM
 Workspace

 Abs:  This function acquires the status of the RTEM RAM Workspace, the amount
 used, free, and the total size of the workspace

 Auth: 11-Aug-2010, Chengcheng Xu (SLAC)
       Changed to be similar to osdMemUsage.c by Stephanie Allison.

 -----------------------------------------------------------------------------*/
#include <devIocStats.h>

int devIocStatsInitWorkspaceUsage(void) { return 0; }

int devIocStatsGetWorkspaceUsage(memInfo *pval) {
  Heap_Information_block info;
#ifdef RTEMS_PROTECTED_HEAP
  _Protected_heap_Get_information(&_Workspace_Area, &info);
#else  /* RTEMS_PROTECTED_HEAP */
  _RTEMS_Lock_allocator(); /*Lock allocator to ensure accuracy */
  _Heap_Get_information(
      &_Workspace_Area,
      &info); /*_Heap_Get_information is part of the RTEMS API */
  _RTEMS_Unlock_allocator();
#endif /* RTEMS_PROTECTED_HEAP */
#if (__RTEMS_MAJOR__ > 4) || (__RTEMS_MAJOR__ == 4 && __RTEMS_MINOR__ > 9)
  /* rtems_configuration_get_work_space_size() (the public accessor for
   * the global `Configuration` object, which a support library can't
   * reach directly -- it's only declared when the application itself
   * includes <rtems/confdefs.h>) reports the compile-time *configured*
   * workspace size. Hardware-verified on a real BBB (RTEMS_INIT=new)
   * that this does NOT match the actual runtime-negotiated workspace
   * region: Free.total+Used.total came back ~227MB while this reported
   * ~624KB. Derive the total from the same heap-info call that already
   * produced Free/Used instead -- it's the only value guaranteed
   * self-consistent with them, regardless of how this port sizes the
   * workspace at boot. */
  pval->numBytesTotal = info.Free.total + info.Used.total;
#else
  pval->numBytesTotal = _Configuration_Table->work_space_size;
#endif
  pval->numBytesFree = info.Free.total;
  pval->numBytesAlloc = info.Used.total;
  return 0;
}
