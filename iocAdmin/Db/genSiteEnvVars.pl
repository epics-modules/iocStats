#!/usr/bin/env perl
#
# Reads an EPICS CONFIG_SITE_ENV file and outputs
# a substitutions file to generate appropriate EPICS
# records for those variables via iocEnvVar.template
#
# Will drop environment variables deemed too long as they
# may produce invalid PV names. This is configured in
# CONFIG_SITE.
#
use strict;
use warnings;

use File::Basename;
use Getopt::Std;

my $tool = basename($0);

our ($opt_h, $opt_l);

sub HELP_MESSAGE {
    print STDERR "Usage: $tool [-h] [-l max_length] <input files>\n";
    exit 2;
}

HELP_MESSAGE() if !getopts("hl:") || $opt_h;

my $max_length;
if ($opt_l) {
    $max_length = $opt_l;
} else {
    $max_length = 15;
}

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
    if (length($_) > $max_length) {
        print STDERR "Warning: Variable $_ is potentially too long for a ";
        print STDERR "PV name given a typical prefix. Skipping.\n";
        next
    }
    my $desc = "EPICS_$_";
    if (length($desc) > 40) {
        $desc = substr($desc, 0, 40);
        print STDERR "Warning: Truncated description for $_ to fit 40 characters: $desc\n";
    }
    print "{ $_, EPICS_$_, $desc, epics }\n";
}
print("}\n");
