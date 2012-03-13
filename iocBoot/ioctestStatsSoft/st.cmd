#!../../bin/linux-x86_64/testIocStats
< envPaths

cd ${TOP}

#
# do OS-specific startup here
#
epicsEnvSet("ENGINEER","$(ENGINEER=engineer)")
epicsEnvSet("LOCATION","$(LOCATION=location)")
# Soft IOCs only
epicsEnvSet("STARTUP","$(TOP)")
epicsEnvSet("ST_CMD","st.cmd")
## Register all support components
dbLoadDatabase("dbd/testIocStats.dbd",0,0)
testIocStats_registerRecordDeviceDriver(pdbbase)

## Load all record instances (VxWorks)
#dbLoadRecords("db/iocAdminVxWorks.db","IOC=IOCTEST")
## or load only those records for RTEMS IOCs
#dbLoadRecords("db/iocAdminRTEMS.db","IOC=IOCTEST")
## or load only those records for Soft IOCs
dbLoadRecords("db/iocAdminSoft.db","IOC=IOCTEST")
## optionally load the SCAN monitoring records
#dbLoadRecords("db/iocAdminScanMon.db","IOC=IOCTEST")
#dbLoadRecords("db/testIocAdminRelease.db","IOC=IOCTEST")

iocInit()

#seq(&testSuspension)
#seq(&testCpuUse)


