TOP=..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# For DTYP="IOC stats"
DBD += devIocStats.dbd
# For DTYP="VX stats"
DBD += devVxStats.dbd

INC += devIocStatsOSD.h

#=============================

LIBRARY_IOC_solaris = devIocStats
LIBRARY_IOC_Linux = devIocStats
LIBRARY_IOC_vxWorks = devIocStats
LIBRARY_IOC_RTEMS = devIocStats
LIBRARY_IOC_Darwin = devIocStats
LIBRARY_IOC_cygwin32 = devIocStats
# WIN32 does not build.
#LIBRARY_IOC_WIN32 = devIocStats
# Do these build?
#LIBRARY_IOC_AIX = devIocStats
#LIBRARY_IOC_iOS = devIocStats
#LIBRARY_IOC_freebsd = devIocStats

devIocStats_LIBS = $(EPICS_BASE_IOC_LIBS)
devIocStats_SYS_LIBS_solaris = kstat

# OSI parts
SRCS += devIocStatsAnalog.c
SRCS += devIocStatsString.c
SRCS += devIocStatsSub.c
SRCS += devIocStatsTest.c

# OSD parts
SRCS += osdCpuUsage.c
SRCS += osdCpuUtilization.c
SRCS += osdFdUsage.c
SRCS += osdMemUsage.c
SRCS += osdWorkspaceUsage.c
SRCS += osdClustInfo.c
SRCS += osdSuspTasks.c
SRCS += osdIFErrors.c
SRCS += osdBootInfo.c
SRCS += osdSystemInfo.c
SRCS += osdHostInfo.c

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

