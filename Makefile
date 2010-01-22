TOP = .
include $(TOP)/configure/CONFIG

DIRS += configure
DIRS += devIocStats
#DIRS += iocAdminLib
#DIRS += testIocStatsApp

include $(TOP)/configure/RULES_TOP
