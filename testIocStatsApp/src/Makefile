TOP=../..

include $(TOP)/configure/CONFIG
IOCADMIN=$(TOP)

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

DBD += testIocStats.dbd

testIocStats_DBD += base.dbd
testIocStats_DBD += devIocStats.dbd
testIocStats_DBD += test.dbd

#=============================

PROD_IOC += testIocStats

testIocStats_SRCS += testIocStats_registerRecordDeviceDriver.cpp
testIocStats_SRCS += testIocStatsMain.cpp
testIocStats_SRCS += testSuspension.st
testIocStats_SRCS += testCpuUse.st

testIocStats_LIBS += devIocStats
testIocStats_LIBS += seq
testIocStats_LIBS += pv
testIocStats_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

IOCRELEASE_DB += testIocStatsRelease.db

#=============================

PROD_HOST += ioc_stats
ioc_stats_SRCS += ioc_stats.c
ioc_stats_SRCS += BSlib.c
ioc_stats_LIBS += $(EPICS_BASE_HOST_LIBS)

PROD_HOST += rdbls
rdbls_SRCS += rdbls.c
rdbls_SRCS += BSlib.c
rdbls_LIBS += $(EPICS_BASE_HOST_LIBS) 

PROD_HOST += rdbapplist
rdbapplist_SRCS += rdbapplist.c
rdbapplist_SRCS += BSlib.c
rdbapplist_LIBS += $(EPICS_BASE_HOST_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

