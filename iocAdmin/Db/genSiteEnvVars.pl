print("file iocEnvVar.template\n");
print("{\n");
print("pattern\n");
print("{ ENVNAME, ENVVAR, ENVTYPE }\n");
while(<>) {
    $m = s/^EPICS_([A-Z_]*).*/{${1}, EPICS_${1}, epics}/;
    print $_ if $m;
}
print("}\n");
