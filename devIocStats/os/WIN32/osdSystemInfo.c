/*************************************************************************\
 * Copyright (c) 2012 Paul-Scherrer Institut(PSI)
 * Copyright (c) 2009 Helmholtz-Zentrum Berlin fuer Materialien und Energie.
 * Copyright (c) 2002 The University of Chicago, as Operator of Argonne
 *     National Laboratory.
 * Copyright (c) 2002 The Regents of the University of California, as
 *     Operator of Los Alamos National Laboratory.
 * EPICS BASE Versions 3.13.7
 * and higher are distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
\*************************************************************************/

/* osdSystemInfo.c - System info strings: vxWorks implementation */

/*
 *  Author: Ralph Lange (HZB/BESSY)
 *
 *  Modification History
 *  2012-03-16 Helge Brands (PSI) 
 *  2009-05-20 Ralph Lange (HZB/BESSY)
 *     Restructured OSD parts
 *
 */

#include <windows.h>
#include <epicsStdio.h>
#include <devIocStats.h>

static char* versionstring;

int devIocStatsInitSystemInfo(void) {

    DWORD dwVersion = 0;
    DWORD dwMajorVersion = 0;
    DWORD dwMinorVersion = 0;
    DWORD dwBuild = 0;

    dwVersion = GetVersion();

    // Get the Windows version.

    dwMajorVersion = (DWORD) (LOBYTE(LOWORD(dwVersion)));
    dwMinorVersion = (DWORD) (HIBYTE(LOWORD(dwVersion)));

    // Get the build number.

    if (dwVersion < 0x80000000)
        dwBuild = (DWORD) (HIWORD(dwVersion));
    versionstring = calloc(40, 1);
    switch (dwMajorVersion) {
        case(6):
        {
            switch (dwMinorVersion) {
                case(0):
                {
                    sprintf(versionstring, "Windows Vista/server 2003 %d.%d(%d)", dwMajorVersion, dwMinorVersion, dwBuild);
                    break;
                }
                case(1):
                {
                    sprintf(versionstring, "Windows 7/Server 2008 %d.%d(%d)", dwMajorVersion, dwMinorVersion, dwBuild);
                    break;
                }
            }
            break;
        }
        case(5):
        {
            switch (dwMinorVersion) {
                case(0):
                {
                    sprintf(versionstring, "Windows 2000 %d.%d(%d)", dwMajorVersion, dwMinorVersion, dwBuild);
                    break;
                }
                case(1):
                {
                    sprintf(versionstring, "Windows XP %d.%d(%d)", dwMajorVersion, dwMinorVersion, dwBuild);
                    break;
                }
                case(2):
                {
                    sprintf(versionstring, "Windows Server 2003 %d.%d(%d)", dwMajorVersion, dwMinorVersion, dwBuild);
                    break;
                }
            }
            break;
        }
    }
    return 0;
}

int devIocStatsGetBSPVersion(char **pval) {
    *pval = 0; //sysBspRev();
    return 0;
}

int devIocStatsGetKernelVersion(char **pval) {
    *pval = versionstring;

    return 0;
}
