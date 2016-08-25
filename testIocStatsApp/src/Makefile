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

ifdef SNCSEQ
	PROD_IOC += testIocStats

	testIocStats_SRCS += testIocStats_registerRecordDeviceDriver.cpp
	testIocStats_SRCS += testIocStatsMain.cpp
	testIocStats_SRCS += testSuspension.st
	testIocStats_SRCS += testCpuUse.st

	testIocStats_LIBS += devIocStats
	testIocStats_LIBS += seq
	testIocStats_LIBS += pv
	testIocStats_LIBS += $(EPICS_BASE_IOC_LIBS)
endif

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

