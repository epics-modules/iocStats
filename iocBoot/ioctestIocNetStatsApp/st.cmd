#!../../bin/linux-x86_64/testIocNetStats

## You may have to change testIocNetStats to something else
## everywhere it appears in this file

< envPaths
epicsEnvSet("ENGINEER", "md23")
epicsEnvSet("LOCATION", "Home")
epicsEnvSet("WIKI", "TBD")
epicsEnvSet("IOCNAME", "netTest")


## Register all support components
dbLoadDatabase("../../dbd/testIocNetStats.dbd",0,0)
testIocNetStats_registerRecordDeviceDriver(pdbbase) 

## Load record instances
#dbLoadRecords("../../db/testIocNetStats.db","user=mirek")
dbLoadRecords("../../db/ioc_stats.db","IOCNAME=IOCTEST,IOC=IOC")
dbLoadRecords("../../db/ioc_NetStats.db","IOCNAME=IOCTEST,INP_IF_NAME=eth0,IF_NAME=LAN1")
dbLoadRecords("../../db/ioc_NetStats.db","IOCNAME=IOCTEST,INP_IF_NAME=lo,IF_NAME=LO")
#dbLoadRecords("../../db/ioc_NetStats.db","IOCNAME=IOCTEST,INP_IF_NAME=wlan0,IF_NAME=WLAN1")

iocInit()

## Start any sequence programs
#seq snctestIocNetStats,"user=mirek"
