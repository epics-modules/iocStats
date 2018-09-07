TOP = .
include $(TOP)/configure/CONFIG

DIRS += configure
DIRS += devIocStats
DIRS += iocAdmin
DIRS += op
ifeq '$(MAKE_TEST_IOC_APP)' 'YES'
DIRS += testIocStatsApp
testIocStatsApp_DEPEND_DIRS = devIocStats
DIRS += iocBoot
# For LCLS
#DIRS += testIocAdminApp
endif
include $(TOP)/configure/RULES_TOP
