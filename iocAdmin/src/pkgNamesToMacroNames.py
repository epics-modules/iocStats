'''This file facilitates translating EPICS package names, typically modules, to
a list of macro names used for that package at SLAC. 1:N
It also translates a macro name to the name of the package it's used for. 1:1
'''
import os
import string

_macroNameToPkgName  = {}
_pkgNameToMacroNames = {}

# These macroNames are never mapped to pkgNames
_macroNameToPkgName[ 'ALARM_CONFIGS_TOP' ] = None
_macroNameToPkgName[ 'BASE_SITE_TOP' ] = None
_macroNameToPkgName[ 'CONFIG' ] = None
_macroNameToPkgName[ 'CONFIG_SITE_TOP' ] = None
_macroNameToPkgName[ 'EPICS_BASE_VER' ] = None
_macroNameToPkgName[ 'EPICS_EXTENSIONS' ] = None
_macroNameToPkgName[ 'EPICS_MODULES' ] = None
_macroNameToPkgName[ 'EPICS_SITE_TOP' ] = None
_macroNameToPkgName[ 'EVR_MODULE' ] = None
_macroNameToPkgName[ 'IOC_SITE_TOP' ] = None
_macroNameToPkgName[ 'LINUX_KERNEL_MODULES' ] = None
_macroNameToPkgName[ 'MAKE_TEST_IOC_APP' ] = None
_macroNameToPkgName[ 'MY_MODULES' ] = None
_macroNameToPkgName[ 'PACKAGE_SITE_TOP' ] = None
_macroNameToPkgName[ 'PSPKG_ROOT' ] = None
_macroNameToPkgName[ 'RULES' ] = None
_macroNameToPkgName[ 'TEMPLATE_TOP' ] = None
_macroNameToPkgName[ 'TOOLS_SITE_TOP' ] = None
_macroNameToPkgName[ 'TOP' ] = None

def macroNameToPkgName( macroName ):
    if macroName in _macroNameToPkgName:
        pkgName = _macroNameToPkgName[ macroName ]
    else:
        pkgName = macroName.lower()
    return pkgName

def pkgNameGetMacroNames( pkgName ):
    if pkgName in _pkgNameToMacroNames:
        macroNames = _pkgNameToMacroNames[ pkgName ]
        if not pkgName.upper() in macroNames:
            macroNames.append( pkgName.upper() )
    else:
        macroNames = [ pkgName.upper() ]
    return macroNames

def pkgNameAddMacroName( pkgName, macroName ):
    if not pkgName in _pkgNameToMacroNames:
        _pkgNameToMacroNames[pkgName] = [ macroName ]
    else:
        macroNames = _pkgNameToMacroNames[pkgName]
        if not macroName in macroNames:
            macroNames.append( macroName )
            _pkgNameToMacroNames[pkgName] = macroNames

    if not macroName in _macroNameToPkgName:
        _macroNameToPkgName[macroName]	= pkgName
    else:
        if _macroNameToPkgName[macroName] != pkgName:
            if _macroNameToPkgName[macroName] is None:
                print("pkgNameAddMacroName Error: Pkg %s Macro %s is not a valid pkgName" % \
                        ( pkgName, macroName ))
            else:
                print("pkgNameAddMacroName Error: Pkg %s Macro %s already mapped to %s" % \
                        ( pkgName, macroName, _macroNameToPkgName[macroName] ))

# Populate macro names for packages
# Most of these are all just simple mappings to uppercase
pkgNameAddMacroName( 'acqiris',				'ACQIRIS' )
pkgNameAddMacroName( 'ADCore',				'ADCORE' )
pkgNameAddMacroName( 'ADCrayDl',			'ADCRAYDL' )
pkgNameAddMacroName( 'ADCSimDetector',		'ADCSIMDETECTOR' )
pkgNameAddMacroName( 'ADEdtPdv',			'ADEDTPDV' )
pkgNameAddMacroName( 'ADPgpEdt',			'ADPGPEDT' )
pkgNameAddMacroName( 'ADPointGrey',			'ADPOINTGREY' )
pkgNameAddMacroName( 'ADProsilica',			'ADPROSILICA' )
pkgNameAddMacroName( 'ADS',					'ADS' )
pkgNameAddMacroName( 'ADSimDetector',		'ADSIMDETECTOR' )
pkgNameAddMacroName( 'ADStream',			'ADSTREAM' )
pkgNameAddMacroName( 'ADSupport',			'ADSUPPORT' )
pkgNameAddMacroName( 'ADUtil',				'ADUTIL' )
pkgNameAddMacroName( 'ADVisar',				'ADVISAR' )
pkgNameAddMacroName( 'agilent53210a',		'AGILENT53210A' )
pkgNameAddMacroName( 'agilent53220A',		'AGILENT53220A' )
pkgNameAddMacroName( 'anc350',				'ANC350' )
pkgNameAddMacroName( 'apci1710',			'APCI1710' )
pkgNameAddMacroName( 'aravisGigE',			'ARAVISGIGE' )
pkgNameAddMacroName( 'areaDetectorSupp',	'AREADETECTORSUPP' )
pkgNameAddMacroName( 'asynDribble',			'ASYNDRIBBLE' )
pkgNameAddMacroName( 'asynGenicam',			'ASYNGENICAM' )
pkgNameAddMacroName( 'asyn',				'ASYN' )
pkgNameAddMacroName( 'autosave',			'AUTOSAVE' )
pkgNameAddMacroName( 'average',				'AVERAGE' )
pkgNameAddMacroName( 'axis',				'AXIS' )
pkgNameAddMacroName( 'bacnet',				'BACNET' )
pkgNameAddMacroName( 'BergozBCM-RF-asyn',	'BERGOZBCM-RF-ASYN' )
pkgNameAddMacroName( 'bkhAsyn',				'BKHASYN' )
pkgNameAddMacroName( 'bldClient',			'BLDCLIENT' )
pkgNameAddMacroName( 'boost-wrapper',		'BOOST-WRAPPER' )
pkgNameAddMacroName( 'bpmDigitizer',		'BPMDIGITIZER' )
pkgNameAddMacroName( 'BsaCore',				'BSACORE' )
pkgNameAddMacroName( 'bsaDriver',			'BSADRIVER' )
pkgNameAddMacroName( 'busy',				'BUSY' )
pkgNameAddMacroName( 'Bx9000_MBT',			'BX9000_MBT' )
pkgNameAddMacroName( 'caenADCV965',			'CAENADCV965' )
pkgNameAddMacroName( 'calc',				'CALC' )
pkgNameAddMacroName( 'caPutLog',			'CAPUTLOG' )
pkgNameAddMacroName( 'cexpsh',				'CEXPSH' )
pkgNameAddMacroName( 'cpswAsynPortDriver',	'CPSWASYNPORTDRIVER' )
pkgNameAddMacroName( 'cpsw-wrapper',		'CPSW-WRAPPER' )
pkgNameAddMacroName( 'crossbarControl',		'CROSSBARCONTROL' )
pkgNameAddMacroName( 'devBusMapped',		'DEVBUSMAPPED' )
pkgNameAddMacroName( 'devGenVar',			'DEVGENVAR' )
pkgNameAddMacroName( 'devlib2',				'DEVLIB2' )
pkgNameAddMacroName( 'diagTimer',			'DIAGTIMER' )
pkgNameAddMacroName( 'drvPciMcor',			'DRVPCIMCOR' )
pkgNameAddMacroName( 'drvUioPciGen',		'DRVUIOPCIGEN' )
pkgNameAddMacroName( 'EDT_CL',				'EDT_CL' )
pkgNameAddMacroName( 'epm2000',				'EPM2000' )
pkgNameAddMacroName( 'ethercat',			'ETHERCAT' )
pkgNameAddMacroName( 'ether_ip',			'ETHER_IP' )
pkgNameAddMacroName( 'etherPSC',			'ETHERPSC' )
pkgNameAddMacroName( 'ev2_driver',			'EV2_DRIVER' )
pkgNameAddMacroName( 'event2',				'EVENT2' )
pkgNameAddMacroName( 'event',				'EVENT' )
pkgNameAddMacroName( 'evrClient',			'EVRCLIENT' )
pkgNameAddMacroName( 'exampleCPP',			'EXAMPLECPP' )
pkgNameAddMacroName( 'fcom',				'FCOM' )
pkgNameAddMacroName( 'fcomUtil',			'FCOMUTIL' )
pkgNameAddMacroName( 'ffmpegServer',		'FFMPEGSERVER' )
pkgNameAddMacroName( 'genPolySub',			'GENPOLYSUB' )
pkgNameAddMacroName( 'genSub',				'GENSUB' )
pkgNameAddMacroName( 'gtr',					'GTR' )
pkgNameAddMacroName( 'highlandLVDTV550',	'HIGHLANDLVDTV550' )
pkgNameAddMacroName( 'history',				'HISTORY' )
pkgNameAddMacroName( 'Hp53181A',			'HP53181A' )
pkgNameAddMacroName( 'hpsTpr-wrapper',		'HPSTPR-WRAPPER' )
pkgNameAddMacroName( 'hytec8413',			'HYTEC8413' )
pkgNameAddMacroName( 'icdTemplates',		'ICDTEMPLATES' )
pkgNameAddMacroName( 'InternalData',		'INTERNALDATA' )
pkgNameAddMacroName( 'iocAdmin',			'IOCADMIN' )
pkgNameAddMacroName( 'iocStats',			'IOCSTATS' )
pkgNameAddMacroName( 'ip231',				'IP231' )
pkgNameAddMacroName( 'ip330-asyn',			'IP330-ASYN' )
pkgNameAddMacroName( 'ip330',				'IP330' )
pkgNameAddMacroName( 'ip440-asyn',			'IP440-ASYN' )
pkgNameAddMacroName( 'ip440',				'IP440' )
pkgNameAddMacroName( 'ip445-asyn',			'IP445-ASYN' )
pkgNameAddMacroName( 'ip445',				'IP445' )
pkgNameAddMacroName( 'ip470',				'IP470' )
pkgNameAddMacroName( 'ipac',				'IPAC' )
pkgNameAddMacroName( 'ipimb',				'IPIMB' )
pkgNameAddMacroName( 'ipmiComm',			'IPMICOMM' )
pkgNameAddMacroName( 'JitterCleaner',		'JITTERCLEANER' )
pkgNameAddMacroName( 'Keithley6482',		'KEITHLEY6482' )
pkgNameAddMacroName( 'Keithley6487',		'KEITHLEY6487' )
pkgNameAddMacroName( 'l2MpsAsyn',			'L2MPSASYN' )
pkgNameAddMacroName( 'laserLocking',		'LASERLOCKING' )
pkgNameAddMacroName( 'lcls2-timing-bsa-wrapper', 'LCLS2-TIMING-BSA-WRAPPER' )
pkgNameAddMacroName( 'LeCroy_ENET',			'LECROY_ENET' )
pkgNameAddMacroName( 'LLRFLibs',			'LLRFLIBS' )
pkgNameAddMacroName( 'MCoreUtils',			'MCOREUTILS' )
pkgNameAddMacroName( 'miscUtils',			'MISCUTILS' )
pkgNameAddMacroName( 'mksu',				'MKSU' )
pkgNameAddMacroName( 'MMC',					'MMC' )
pkgNameAddMacroName( 'modbus',				'MODBUS' )
pkgNameAddMacroName( 'ModBusTCPClnt',		'MODBUSTCPCLNT' )
pkgNameAddMacroName( 'motor',				'MOTOR' )
pkgNameAddMacroName( 'mrfioc2',				'MRFIOC2' )
pkgNameAddMacroName( 'NDDriverStdArrays',	'NDDRIVERSTDARRAYS' )
pkgNameAddMacroName( 'normativeTypesCPP',	'NORMATIVETYPESCPP' )
pkgNameAddMacroName( 'nullhttpd',			'NULLHTTPD' )
pkgNameAddMacroName( 'oxigrafMO2i',			'OXIGRAFMO2I' )
pkgNameAddMacroName( 'p4p',					'P4P' )
pkgNameAddMacroName( 'pau',					'PAU' )
pkgNameAddMacroName( 'pcds_motion',			'PCDS_MOTION' )
pkgNameAddMacroName( 'perfMeasure',			'PERFMEASURE' )
pkgNameAddMacroName( 'pgp',					'PGP' )
pkgNameAddMacroName( 'plcAdmin',			'PLCADMIN' )
pkgNameAddMacroName( 'pva2pva',				'PVA2PVA' )
pkgNameAddMacroName( 'pvAccessCPP',			'PVACCESSCPP' )
pkgNameAddMacroName( 'pvaClientCPP',		'PVACLIENTCPP' )
pkgNameAddMacroName( 'pvaClientTestCPP',	'PVACLIENTTESTCPP' )
pkgNameAddMacroName( 'pvaPy',				'PVAPY' )
pkgNameAddMacroName( 'pvaSrv',				'PVASRV' )
pkgNameAddMacroName( 'pvCommonCPP',			'PVCOMMONCPP' )
pkgNameAddMacroName( 'pvDatabaseCPP',		'PVDATABASECPP' )
pkgNameAddMacroName( 'pvDataCPP',			'PVDATACPP' )
pkgNameAddMacroName( 'pvExampleTestCPP',	'PVEXAMPLETESTCPP' )
pkgNameAddMacroName( 'QF2preMonitor',		'QF2PREMONITOR' )
pkgNameAddMacroName( 'quadEM',				'QUADEM' )
pkgNameAddMacroName( 'RFControlFirmware',	'RFCONTROLFIRMWARE' )
pkgNameAddMacroName( 'RFControl',			'RFCONTROL' )
pkgNameAddMacroName( 'rtemsutils',			'RTEMSUTILS' )
pkgNameAddMacroName( 's7plc',				'S7PLC' )
pkgNameAddMacroName( 'seq',					'SEQ' )
pkgNameAddMacroName( 'sis8300',				'SIS8300' )
pkgNameAddMacroName( 'snmp',				'SNMP' )
pkgNameAddMacroName( 'spline',				'SPLINE' )
pkgNameAddMacroName( 'sr620',				'SR620' )
pkgNameAddMacroName( 'sscan',				'SSCAN' )
pkgNameAddMacroName( 'sSubRecord',			'SSUBRECORD' )
pkgNameAddMacroName( 'std',					'STD' )
pkgNameAddMacroName( 'streamdevice',		'STREAMDEVICE' )
pkgNameAddMacroName( 'synapps-support',		'SYNAPPS-SUPPORT' )
pkgNameAddMacroName( 'tds3000',				'TDS3000' )
pkgNameAddMacroName( 'teste',				'TESTE' )
pkgNameAddMacroName( 'timeStampFifo',		'TIMESTAMPFIFO' )
pkgNameAddMacroName( 'timesync',			'TIMESYNC' )
pkgNameAddMacroName( 'timingApi',			'TIMINGAPI' )
pkgNameAddMacroName( 'tpr',					'TPR' )
pkgNameAddMacroName( 'tprPattern',			'TPRPATTERN' )
pkgNameAddMacroName( 'tprTrigger',			'TPRTRIGGER' )
pkgNameAddMacroName( 'TRCore',				'TRCORE' )
pkgNameAddMacroName( 'TRGeneralStandards',	'TRGENERALSTANDARDS' )
pkgNameAddMacroName( 'twincat-ads',			'TWINCAT-ADS' )
pkgNameAddMacroName( 'udpComm',				'UDPCOMM' )
pkgNameAddMacroName( 'usb_sn',				'USB_SN' )
pkgNameAddMacroName( 'vacuum',				'VACUUM' )
pkgNameAddMacroName( 'VHSx0x',				'VHSX0X' )
pkgNameAddMacroName( 'vmeCardRecord',		'VMECARDRECORD' )
pkgNameAddMacroName( 'vsam',				'VSAM' )
pkgNameAddMacroName( 'xipIo',				'XIPIO' )
pkgNameAddMacroName( 'xps8',				'XPS8' )
pkgNameAddMacroName( 'yaml-cpp-wrapper',	'YAML-CPP-WRAPPER' )
pkgNameAddMacroName( 'yamlDownloader',		'YAMLDOWNLOADER' )
pkgNameAddMacroName( 'yamlLoader',			'YAMLLOADER' )
pkgNameAddMacroName( 'ycpswasyn',			'YCPSWASYN' )

# Add special cases
pkgNameAddMacroName( 'areaDetector',	'AREA_DETECTOR' )
pkgNameAddMacroName( 'base',			'BASE' )
pkgNameAddMacroName( 'base',			'BASE_MODULE_VERSION' )
pkgNameAddMacroName( 'base',			'EPICS_BASE' )
pkgNameAddMacroName( 'BergozBCM-RF-asyn','BERGOZBCM_RF_ASYN' )
pkgNameAddMacroName( 'Bk9000_MBT',		'BK9000_MBT' )
pkgNameAddMacroName( 'bldClient',		'BLD_CLIENT' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'Bx9000_MBT',		'BX9000' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'Bx9000_MBT',		'BX9000_MBT' )
pkgNameAddMacroName( 'Bx9000_MBT',		'BX9000MBT' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'Camcom',			'CAMCOM' )
pkgNameAddMacroName( 'caSnooper',		'CASNOOPER' )
pkgNameAddMacroName( 'cexpsh',			'CEXP' )
pkgNameAddMacroName( 'ChannelWatcher',	'CHANNELWATCHER' )
pkgNameAddMacroName( 'diagTimer',		'DIAG_TIMER' )
pkgNameAddMacroName( 'etherPSC',		'EPSC' )
pkgNameAddMacroName( 'ethercat',		'ECASYN' )
pkgNameAddMacroName( 'fwdCliS',			'FWDCLIS' )
pkgNameAddMacroName( 'gtr',				'GTR_VERSION' )
pkgNameAddMacroName( 'ip231-asyn',		'IP231_ASYN' )
pkgNameAddMacroName( 'ip330-asyn',		'IP330_ASYN' )
pkgNameAddMacroName( 'ip440',			'XY2440' )
pkgNameAddMacroName( 'ip440-asyn',		'IP440_ASYN' )
pkgNameAddMacroName( 'ip445',			'XY2445' )
pkgNameAddMacroName( 'ip445-asyn',		'IP445_ASYN' )
pkgNameAddMacroName( 'LeCroy_ENET',		'LECROY' )
pkgNameAddMacroName( 'normativeTypesCPP','NORMATIVE' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'normativeTypesCPP','NORMATIVETYPES' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'procServ',		'PROCSERV' )
pkgNameAddMacroName( 'PSCD_Camac',		'PSCDCAMAC' )
pkgNameAddMacroName( 'pvAccessCPP',		'PVACCESS' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'pvAccessCPP',		'pvAccessCPP' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'pvaClientCPP',	'PVACLIENT' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'pvaSrv',			'PVASRV' )
pkgNameAddMacroName( 'pvCommonCPP',		'PVCOMMONCPP' )
pkgNameAddMacroName( 'pvCommonCPP',		'PVCOMMON' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'pvCommonCPP',		'pvCommonCPP' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'pvDatabaseCPP',	'PVDATABASECPP' )
pkgNameAddMacroName( 'pvDatabaseCPP',	'PVDATABASE' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'pvDataCPP',		'PVDATACPP' )
pkgNameAddMacroName( 'pvDataCPP',		'PVDATA' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'pvDataCPP',		'pvDataCPP' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'pvIOCCPP',		'PVIOC' )
pkgNameAddMacroName( 'seq',				'SNCSEQ' )
pkgNameAddMacroName( 'sscan',			'SSCAN' )
pkgNameAddMacroName( 'sSubRecord',		'SSUBRECORD' )
pkgNameAddMacroName( 'streamdevice',	'STREAMDEVICE' )
pkgNameAddMacroName( 'streamdevice',	'STREAM' )	# TODO: We should flag these non-standard macro names
pkgNameAddMacroName( 'StripTool',		'STRIPTOOL' )
pkgNameAddMacroName( 'timingApi',		'TIMING_API' )
pkgNameAddMacroName( 'VHQx0x',			'VHQX0X' )
pkgNameAddMacroName( 'VHSx0x',			'VHSX0X' )
pkgNameAddMacroName( 'VisualDCT',		'VISUALDCT' )
pkgNameAddMacroName( 'vmeCardRecord',	'VME_CARD_RECORD' )

