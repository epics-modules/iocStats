#!/usr/bin/env perl
#
# Usage:  genSiteEnvVars.pl CONFIG_SITE_ENV [ ... ]
#
# Reads an EPICS CONFIG_SITE_ENV file and outputs
# a substitutions file to generate appropriate EPICS
# records for those variables via iocEnvVar.template
#
use strict;
use warnings;

print <<__END__;
file "iocEnvVar.template" {
  pattern { ENVNAME, ENVVAR, ENVTYPE }
__END__

my @epics_vars;
while(<>) {
    my $m = s/^EPICS_([A-Z_0-9]*).*\n/${1}/;
    push(@epics_vars, $_) if $m;
}

my %varcount;
my @unique_vars = grep { not $varcount{$_}++ } @epics_vars;

foreach (@unique_vars) {
    print "{ $_, EPICS_$_, epics }\n"
}
print("}\n");
