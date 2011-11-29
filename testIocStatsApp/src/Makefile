TOP=../..

include $(TOP)/configure/CONFIG

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

DBD += testIocStats.dbd

testIocStats_DBD += base.dbd
testIocStats_DBD += devIocStats.dbd
testIocStats_DBD += test.dbd

#=============================

PROD_IOC_solaris += testIocStats
PROD_IOC_Linux += testIocStats
PROD_IOC_vxWorks += testIocStats
PROD_IOC_RTEMS += testIocStats
PROD_IOC_Darwin += testIocStats
PROD_IOC_cygwin32 += testIocStats
# WIN32 does not build.
#PROD_IOC_WIN32 += testIocStats
# Do these build?
#PROD_IOC_AIX += testIocStats
#PROD_IOC_iOS += testIocStats
#PROD_IOC_freebsd += testIocStats

testIocStats_SRCS += testIocStats_registerRecordDeviceDriver.cpp
testIocStats_SRCS += testIocStatsMain.cpp
testIocStats_SRCS += testSuspension.st
testIocStats_SRCS += testCpuUse.st

testIocStats_LIBS += devIocStats
testIocStats_LIBS += seq
testIocStats_LIBS += pv
testIocStats_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

