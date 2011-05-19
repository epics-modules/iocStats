TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this
#
DB += ioc.db
DB += iocRTOS.db
DB += iocVxWorksOnly.db
DB += iocRTEMSOnly.db
DB += iocEnvVar.db
DB += iocCluster.db
DB += iocScanMon.db
DB += iocScanMonSum.db
DB += iocGeneralTime.db
DB += access.db
DB += iocAdminVxWorks.db
DB += iocAdminSoft.db
DB += iocAdminRTEMS.db
DB += iocAdminScanMon.db

#
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE

