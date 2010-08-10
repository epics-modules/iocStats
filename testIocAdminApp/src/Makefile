TOP=../..

include $(TOP)/configure/CONFIG

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
DBD += testIocAdmin.dbd

testIocAdmin_DBD += base.dbd
testIocAdmin_DBD += devIocStats.dbd
IOCADMIN=$(TOP)
IOCRELEASE_DB += testIocAdminRelease.db

#=============================

PROD_IOC += testIocAdmin

testIocAdmin_SRCS += testIocAdmin_registerRecordDeviceDriver.cpp
testIocAdmin_SRCS += testIocAdminMain.cpp

testIocAdmin_LIBS += devIocStats
testIocAdmin_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

