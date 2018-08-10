#!/bin/sh
set -e -x

cat << EOF > configure/RELEASE
EPICS_BASE=$HOME/epics-base
EOF

# use default selection for MSI
sed -i -e '/MSI/d' configure/CONFIG_SITE

git clone --depth 10 --branch $BASE https://github.com/epics-base/epics-base.git $HOME/epics-base

EPICS_HOST_ARCH=`sh $HOME/epics-base/startup/EpicsHostArch`

case "$STATIC" in
static)
    cat << EOF >> "$HOME/epics-base/configure/CONFIG_SITE"
SHARED_LIBRARIES=NO
STATIC_BUILD=YES
EOF
    ;;
*) ;;
esac

case "$CMPLR" in
clang)
  echo "Host compiler is clang"
  cat << EOF >> $HOME/epics-base/configure/os/CONFIG_SITE.Common.$EPICS_HOST_ARCH
GNU         = NO
CMPLR_CLASS = clang
CC          = clang
CCC         = clang++
EOF
  ;;
*) echo "Host compiler is default";;
esac

# requires wine and g++-mingw-w64-i686
if [ "$WINE" = "32" ]
then
  echo "Cross mingw32"
  sed -i -e '/CMPLR_PREFIX/d' $HOME/epics-base/configure/os/CONFIG_SITE.linux-x86.win32-x86-mingw
  cat << EOF >> $HOME/epics-base/configure/os/CONFIG_SITE.linux-x86.win32-x86-mingw
CMPLR_PREFIX=i686-w64-mingw32-
EOF
  cat << EOF >> $HOME/epics-base/configure/CONFIG_SITE
CROSS_COMPILER_TARGET_ARCHS+=win32-x86-mingw
EOF
fi

# set RTEMS to eg. "4.9" or "4.10"
if [ -n "$RTEMS" ]
then
  echo "Cross RTEMS${RTEMS} for pc386"
  install -d /home/travis/.cache
  curl -L "https://github.com/mdavidsaver/rsb/releases/download/travis-20160306-2/rtems${RTEMS}-i386-trusty-20190306-2.tar.gz" \
  | tar -C /home/travis/.cache -xj

  sed -i -e '/^RTEMS_VERSION/d' -e '/^RTEMS_BASE/d' $HOME/epics-base/configure/os/CONFIG_SITE.Common.RTEMS
  cat << EOF >> $HOME/epics-base/configure/os/CONFIG_SITE.Common.RTEMS
RTEMS_VERSION=$RTEMS
RTEMS_BASE=/home/travis/.cache/rtems${RTEMS}-i386
EOF
  cat << EOF >> $HOME/epics-base/configure/CONFIG_SITE
CROSS_COMPILER_TARGET_ARCHS+=RTEMS-pc386
EOF

fi

make -C "$HOME/epics-base" -j2

# get MSI for 3.14
case "$BASE" in
3.14*)
    echo "Build MSI"
    install -d "$HOME/msi/extensions/src"
    
    curl -L https://epics.anl.gov/download/extensions/extensionsTop_20120904.tar.gz | tar -C "$HOME/msi" -xvz

    curl -L https://epics.anl.gov/download/extensions/msi1-7.tar.gz | tar -C "$HOME/msi/extensions/src" -xvz

    mv "$HOME/msi/extensions/src/msi1-7" "$HOME/msi/extensions/src/msi"

    cat << EOF > "$HOME/msi/extensions/configure/RELEASE"
EPICS_BASE=$HOME/epics-base
EPICS_EXTENSIONS=\$(TOP)
EOF

    make -C "$HOME/msi/extensions"

    cp "$HOME/msi/extensions/bin/$EPICS_HOST_ARCH/msi" "$HOME/epics-base/bin/$EPICS_HOST_ARCH/"

    echo 'MSI:=$(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/msi' >> "$HOME/epics-base/configure/CONFIG_SITE"

    cat <<EOF >> configure/CONFIG_SITE
MSI = \$(EPICS_BASE)/bin/\$(EPICS_HOST_ARCH)/msi
EOF

  ;;
*) echo "Use MSI from Base"
  ;;
esac
