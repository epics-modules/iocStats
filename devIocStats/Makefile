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

LIBRARY_IOC = devIocStats

devIocStats_LIBS = $(EPICS_BASE_IOC_LIBS)
devIocStats_SYS_LIBS_solaris = kstat

# OSI parts
SRCS += devIocStatsAnalog.c
SRCS += devIocStatsString.c
SRCS += devIocStatsWaveform.c
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

OBJS_vxWorks += osdCpuUsageTest.o

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

