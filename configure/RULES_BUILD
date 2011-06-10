# RULES_BUILD
#
#	iocAdmin rules
#

ifeq ($(findstring Host,$(VALID_BUILDS)),Host)
ifdef IOCADMIN

INSTALL_IOCRELEASE_DBS=$(IOCRELEASE_DB:%=$(INSTALL_DB)/%)

#--------------------------------------------------
# Rules

build : $(IOCRELEASE_DB)
buildInstall : $(INSTALL_IOCRELEASE_DBS)

# Create database for module version PVs
$(IOCRELEASE_DB)   : $(TOP)/configure/RELEASE $(TOP)/RELEASE_SITE
	$(RM) $@
	$(IOCADMIN)/bin/$(EPICS_HOST_ARCH)/iocReleaseCreateDb.py $^ >$@

$(INSTALL_DB)/%: %
	@echo "Installing db file $@"
	@$(INSTALL) -d -m 644 $< $(@D)

endif
endif
