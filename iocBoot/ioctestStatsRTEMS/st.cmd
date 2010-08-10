# ioctestStatsRTEMS startup file
chdir("/boot/u/qb/saa/spear/iocStats/")
ld("bin/RTEMS-beatnik/testIocStats.obj")
#
# do OS-specific startup here
#
epicsEnvSet("ENGINEER","<engineer>")
epicsEnvSet("LOCATION","<location>")
## Register all support components
dbLoadDatabase("dbd/testIocStats.dbd",0,0)
testIocStats_registerRecordDeviceDriver(pdbbase)

## Load all record instances (VxWorks)
#dbLoadRecords("db/iocAdminVxWorks.db","IOC=IOCTEST")
## or load only those records for RTEMS IOCs
dbLoadRecords("db/iocAdminRTEMS.db","IOC=IOCTEST")
## or load only those records for Soft IOCs
#dbLoadRecords("db/iocAdminSoft.db","IOC=IOCTEST")
## optionally load the SCAN monitoring records
#dbLoadRecords("db/iocAdminScanMon.db","IOC=IOCTEST")
## optionally load RELEASE records
#dbLoadRecords("db/testIocAdminRelease.db","IOC=IOCTEST")

iocInit()

#seq(&testSuspension)
#seq(&testCpuUse)


