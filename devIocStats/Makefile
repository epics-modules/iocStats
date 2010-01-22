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

# OSI parts
SRCS += devIocStatsAnalog.c
SRCS += devIocStatsString.c
SRCS += devIocStatsTest.c

# OSD parts
SRCS += osdCpuUsage.c
SRCS += osdFdUsage.c
SRCS += osdMemUsage.c
SRCS += osdClustInfo.c
SRCS += osdSuspTasks.c
SRCS += osdIFErrors.c
SRCS += osdBootInfo.c
SRCS += osdSystemInfo.c
SRCS += osdHostInfo.c

LIBS = $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

