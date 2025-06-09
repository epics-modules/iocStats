#!/usr/bin/env perl
#
# Usage:  genSiteEnvVars.pl CONFIG_SITE_ENV
#
# Reads an EPICS CONFIG_SITE_ENV file and outputs
# a substitutions file to generate appropriate EPICS
# records for those variables via iocEnvVar.template
#
print <<__END__;
file "iocEnvVar.template" {
  pattern { ENVNAME, ENVVAR, ENVTYPE }
__END__

while(<>) {
    $m = s/^EPICS_([A-Z_]*).*/{${1}, EPICS_${1}, epics}/;
    print $_ if $m;
}
print("}\n");
