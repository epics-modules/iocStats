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
DB += siteEnvVars.db
DB += epicsEnvVars.db
DB += iocAdminScanMon.db
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

#
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE

$(COMMON_DIR)/iocAdminRTEMS.db:   $(COMMON_DIR)/iocAdminScanMon.db $(COMMON_DIR)/epicsEnvVars.db $(COMMON_DIR)/siteEnvVars.db

$(COMMON_DIR)/iocAdminSoft.db:    $(COMMON_DIR)/iocAdminScanMon.db $(COMMON_DIR)/epicsEnvVars.db $(COMMON_DIR)/siteEnvVars.db

$(COMMON_DIR)/iocAdminVxWorks.db: $(COMMON_DIR)/iocAdminScanMon.db $(COMMON_DIR)/epicsEnvVars.db $(COMMON_DIR)/siteEnvVars.db

$(COMMON_DIR)/siteEnvVars.substitutions: $(EPICS_BASE)/configure/CONFIG_SITE_ENV
	@echo Expanding siteEnvVars.substitutions from CONFIG_SITE_ENV....
	@perl ../genSiteEnvVars.pl < $< > $@
