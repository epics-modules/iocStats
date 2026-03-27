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
pattern { ENVNAME, ENVVAR, ENVDESC, ENVTYPE }
__END__

my @epics_vars;
while(<>) {
    my $m = s/^EPICS_([A-Z_0-9]*).*\n/${1}/;
    push(@epics_vars, $_) if $m;
}

my %varcount;
my @unique_vars = grep { not $varcount{$_}++ } @epics_vars;

foreach (@unique_vars) {
    my $desc = "EPICS_$_";
    if (length($desc) > 40) {
        $desc = substr($desc, 0, 40);
    }
    print STDERR "Warning: Truncated description for $_ to fit 40 characters: $desc\n" if $desc ne "EPICS_$_";
    print "{ $_, EPICS_$_, $desc, epics }\n";
}
print("}\n");
