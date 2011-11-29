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

PROD_IOC_solaris += testIocAdmin
PROD_IOC_Linux += testIocAdmin
PROD_IOC_vxWorks += testIocAdmin
PROD_IOC_RTEMS += testIocAdmin
PROD_IOC_Darwin += testIocAdmin
PROD_IOC_cygwin32 += testIocAdmin
# WIN32 does not build.
#PROD_IOC_WIN32 += testIocAdmin
# Do these build?
#PROD_IOC_AIX += testIocAdmin
#PROD_IOC_iOS += testIocAdmin
#PROD_IOC_freebsd += testIocAdmin

testIocAdmin_SRCS += testIocAdmin_registerRecordDeviceDriver.cpp
testIocAdmin_SRCS += testIocAdminMain.cpp

testIocAdmin_LIBS += devIocStats
testIocAdmin_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

