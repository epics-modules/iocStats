#!/usr/bin/env python

import sys
import os
import subprocess
import optparse


__all__ = ['export_db_file', 'module_versions', 'process_options']


def export_db_file(module_versions, path=None):
    """
    Use the contents of a dictionary of module versions to create a database
    of module release stringin PVs. The database  
    is written to stdout if path is not provided or is None.
    """

    out_file = sys.stdout
    idx = 0
    idxMax = 20
    
    if path:
        try:
            out_file = open(path, 'w')
        except IOError, e:
            sys.stderr.write('Could not open "%s": %s\n' % (path, e.strerror))
            return None

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
        if idx >= idxMax: break
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
    

def module_versions(release_path, site_path):
    """
    Return a dictionary containing module names and versions.
    """
    
    # first grab EPICS_BASE_VER from RELEASE_SITE file, if it's there
    siteBaseVer = "Nada"
    openSiteFile = 1

    try:
        site_file = open(site_path, 'r')
    except IOError, e:
        #sys.stderr.write('Could not open "%s": %s\n' % (site_path, e.strerror))
        openSiteFile = 0

    if openSiteFile:
        for line in site_file:
            # Remove comments
            line = line.partition('#')[0]
        
            # Turn 'a = b' into a key/value pair and remove leading and trailing whitespace
            (key, sep, value) = line.partition('=')
            key = key.strip()
            value = value.strip()
        
            # save EPICS_BASE_VER, if it's in there
            if key.startswith('EPICS_BASE_VER'):
                siteBaseVer = value
                break
            
        site_file.close()
        
    # now get all the modules
    try:
        release_file = open(release_path, 'r')
    except IOError, e:
        sys.stderr.write('Could not open "%s": %s\n' % (release_path, e.strerror))
        return None

    release_file_dict = {}

    for line in release_file:
        # Remove comments
        line = line.partition('#')[0]
        
        # Turn 'a = b' into a key/value pair and remove leading and trailing whitespace
        (key, sep, value) = line.partition('=')
        key = key.strip()
        value = value.strip()
        
        # Add the key/value pair to the dictionary if the key ends with _MODULE_VERSION
        if key.endswith('_MODULE_VERSION'):
            # if BASE_MODULE_VERSION is set to EPICS_BASE_VER macro from RELEASE_SITE,
            # capture it here  
            if key == "BASE_MODULE_VERSION" and value == "$(EPICS_BASE_VER)":  
                if siteBaseVer != "Nada": 
                    release_file_dict[key] = siteBaseVer
                else:
                    # don't set BASE at all
                    pass
            else:
                release_file_dict[key] = value
    
    release_file.close()

        
    return release_file_dict


def process_options(argv):
    """
    Return parsed command-line options found in the list of
    arguments, `argv`, or ``sys.argv[2:]`` if `argv` is `None`.
    """

    if argv is None:
        argv = sys.argv[1:]

    #    usage = 'Usage: %prog RELEASE_FILE [options]'    
    usage = 'Usage: %prog RELEASE_FILE RELEASE_SITE_FILE [options]'    
    version = '%prog 0.1'
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
    versions = module_versions(options.release_file_path, options.release_site_file_path)
    export_db_file(versions, options.db_file)

    return 0
    

if __name__ == '__main__':
    status = main()
    sys.exit(status)
