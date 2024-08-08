## Build Status: [![build](https://github.com/epics-modules/iocStats/actions/workflows/ci-build.yml/badge.svg)](https://github.com/epics-modules/iocStats/actions/workflows/ci-build.yml) [![pre-commit](https://github.com/epics-modules/iocStats/actions/workflows/pre-commit.yml/badge.svg)](https://github.com/epics-modules/iocStats/actions/workflows/pre-commit.yml)


# iocStats - EPICS IOC Status and Control

## Detailed description and usage
See:
https://www.slac.stanford.edu/grp/ssrl/spear/epics/site/devIocStats/

## Download release tars
See:
https://github.com/epics-modules/iocStats/releases

## How to clone the GIT repository

```
$ git clone --recursive git@github.com:epics-modules/iocStats
```

## Files and Directories under iocStats

Name                 | Comment
-----                | -----
README.md            | this file
README_devIocStats   | devIocStats information
README_iocAdmin      | LCLS iocAdmin usage
RELEASE_NOTES        | release notes
configure            | build configuration files
devIocStats          | IOC statistics device support
op                   | MEDM and CSS/BOY display files
iocAdmin             | LCLS-unique EDM database, displays, and config files, includes general time records
iocBoot              | test startup files
testIocAdminApp      | LCLS-unique app for testing devIocStats and iocAdmin
testIocStatsApp      | Application for testing devIocStats only
