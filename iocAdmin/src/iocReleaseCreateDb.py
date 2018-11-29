#!/usr/bin/python2

import sys
import os
import subprocess
import optparse
from pkgNamesToMacroNames import *
from version_utils import *

__all__ = ['export_db_file', 'process_options']


def export_db_file( module_versions, path=None):
    """
    Use the contents of a dictionary of module versions to create a database
    of module release stringin PVs. The database
    is written to stdout if path is not provided or is None.
    """

    out_file = sys.stdout
    idx = 0
    idxMax = 30

    if path:
        try:
            out_file = open(path, 'w')
        except IOError, e:
            sys.stderr.write('Could not open "%s": %s\n' % (path, e.strerror))
            return None

    # Create a sorted list of versions
    sorted_module_versions = [(key, module_versions[key]) for key in sorted(module_versions.keys())]

    print >> out_file, '#=============================================================================='
    print >> out_file, '#'
    print >> out_file, '# Abs:  LCLS read-only stringin records for Modules specified in configure/RELEASE'
    print >> out_file, '#'
    print >> out_file, '# Name: iocRelease.db'
    print >> out_file, '#'
    print >> out_file, '# Note: generated automatically by $IOCADMIN/bin/$EPICS_HOST_ARCH/iocReleaseCreateDb.py'
    print >> out_file, '#'
    print >> out_file, '#=============================================================================='
    for [key, module_version] in sorted_module_versions:
        """
        strip off the _MODULE_VERSION from key for PV NAME
        """
        x = key.replace("_MODULE_VERSION","",1)
        if idx >= idxMax:
            break
        print >> out_file, 'record(stringin, "$(IOC):RELEASE%02d") {' % idx
        print >> out_file, '  field(DESC, "%s")' % x
        print >> out_file, '  field(PINI, "YES")' 
        print >> out_file, '  field(VAL, "%s")' % module_version
        print >> out_file, '  #field(ASG, "some read only grp")' 
        print >> out_file, '}'
        idx = idx + 1

    while idx < idxMax:
        print >> out_file, 'record(stringin, "$(IOC):RELEASE%02d") {' % idx
        print >> out_file, '  field(DESC, "Not Applicable")'
        print >> out_file, '  field(PINI, "YES")' 
        print >> out_file, '  field(VAL, "Not Applicable")'
        print >> out_file, '  #field(ASG, "some read only grp")' 
        print >> out_file, '}'
        idx = idx + 1

    if out_file != sys.stdout:
        out_file.close()

def process_options(argv):
    """
    Return parsed command-line options found in the list of
    arguments, `argv`, or ``sys.argv[2:]`` if `argv` is `None`.
    """

    if argv is None:
        argv = sys.argv[1:]

    #    usage = 'Usage: %prog RELEASE_FILE [options]'
    usage = 'Usage: %prog RELEASE_FILE RELEASE_SITE_FILE [options]'
    version = '%prog 0.2'
    parser = optparse.OptionParser(usage=usage, version=version)

    parser.add_option('-v', '--verbose', action='store_true', dest='verbose', help='print verbose output')
    parser.add_option("-e", "--db_file", action="store", type="string", dest="db_file", metavar="FILE", help="module database file path")

    parser.set_defaults(verbose=False,
                        db_file=None)

    (options, args) = parser.parse_args(argv)

    if len(args) != 2:
        parser.error("incorrect number of arguments")

    options.release_file_path = os.path.normcase(args[0])
    options.release_site_file_path = os.path.normcase(args[1])

    return options

def main(argv=None):
    options = process_options(argv)

    # get the IOC dependents
    topDir = os.path.abspath( os.path.dirname(options.release_site_file_path) )
    dependents = getEpicsPkgDependents( topDir )

    # export the iocRelease.db file
    export_db_file( dependents, options.db_file)

    return 0


if __name__ == '__main__':
    status = main()
    sys.exit(status)
