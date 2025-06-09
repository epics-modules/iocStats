#!/usr/bin/env perl
print <<__END__;
file "iocEnvVar.template" {
  pattern { ENVNAME, ENVVAR, ENVTYPE }
__END__

while(<>) {
    $m = s/^EPICS_([A-Z_]*).*/{${1}, EPICS_${1}, epics}/;
    print $_ if $m;
}
print("}\n");
