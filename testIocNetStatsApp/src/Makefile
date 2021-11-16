TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = testIocNetStats
# testIocNetStats.dbd will be created and installed
DBD += testIocNetStats.dbd

# testIocNetStats.dbd will be made up from these files:
testIocNetStats_DBD += base.dbd
testIocNetStats_DBD += devIocStats.dbd

# Include dbd files from all support applications:
#testIocNetStats_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#testIocNetStats_LIBS += xxx

# testIocNetStats_registerRecordDeviceDriver.cpp derives from testIocNetStats.dbd
testIocNetStats_SRCS += testIocNetStats_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
testIocNetStats_SRCS_DEFAULT += testIocNetStatsMain.cpp
testIocNetStats_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#testIocNetStats_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary
ifneq ($(SNCSEQ),)
    # Build sncExample into testIocNetStatsSupport
    testIocNetStats_LIBS += devIocStatsSeq
    sncExample_SNCFLAGS += +r
    testIocNetStats_DBD += sncExample.dbd
    # A .stt sequence program is *not* pre-processed:
    testIocNetStatsSupport_SRCS += sncExample.stt
    testIocNetStatsSupport_LIBS += seq pv
    testIocNetStats_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    # A .st sequence program *is* pre-processed:
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
else
    testIocNetStats_LIBS += devIocStats
endif

# Finally link to the EPICS Base libraries
testIocNetStats_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

