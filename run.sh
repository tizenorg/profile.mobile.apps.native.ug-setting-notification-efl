#!/bin/bash

usage()
{
echo "
usage: sh run.sh [-h] [-b] [-i] [-r] [-t] [-d] [-A ARCH]
                 [--help] [--build] [--install] [--run] [--test] [--debug] [--arch ARCH]

optional arguments:
  -h, --help             show this help message and exit

action:
  -b  --build           build project
  -i, --install         install to device or emulator
  -r, --run             run application. Don't use with -t option
  -t, --test            builds unit-tests as well, ignored without -b option
  -d, --debug           install debuginfo and debugsource packages
  -l, --local		local incremental build(with --noinit --incremental kwys)

build configuration options:
  -A ARCH, --arch ARCH   build target arch. Default - armv7l
  -b 'Keys', --build 'Keys'  build project with additional gbs build keys

  examples:
  'run.sh -b -t -i -A armv7l' will build application and try to install it to target-device, unit-tests will be also built
  'run.sh -i -A armv7l[i586|aarch64]' will install latest build for tizen-2.4 device [emulator|tizen-3.0 device]
"
}

##-------------- check options --------------##

if [ -z "$1" ]; then usage; exit 0; fi

BUILDOPTION=false
BUILDKEYS=""
INSTALLOPTION=false
RUNOPTION=false
DEBUGOPTION=false
TESTOPTION=false
PLATFORM=armv7l
LOCALBUILD=false

SHORTOPTS="hA:b::irdtl"
LONGOPTS="arch:,build::,install,run,debug,test,help,local"
SCRIPTNAME=`basename $0`

ARGS=$(getopt -q --options "$SHORTOPTS" --longoptions "$LONGOPTS" --name $SCRIPTNAME -- "$@")

while true; do
   case $1 in
      -h|--help)
         usage
         exit 0
         ;;
      -A|--arch)
         val=`echo $2 | sed -e "s/-//g"`
         if echo "$SHORTOPTS$LONGOPTS" | grep -q "$val"; then
           echo "Wrong ARCH"
           usage
           exit 0;
         fi
         PLATFORM=$2
         shift
         ;;
      -b|--build)
         BUILDOPTION=true
         key=`echo $2 | sed -e "s/-//g"`
         if ! echo "$SHORTOPTS$LONGOPTS" | grep -q "$key"; then
           BUILDKEYS=$2
           shift
         fi
         ;;
      -i|--install)
         INSTALLOPTION=true
         ;;
      -r|--run)
         RUNOPTION=true
         ;;
      -d|--debug)
         DEBUGOPTION=true
         ;;
      -t|--test)
         TESTOPTION=true
         ;;
      -l|--local)
         LOCALBUILD=true
         ;;
      --)
         break
         ;;
      *)
         if [ ! -z "$1" ]; then usage; exit 0; fi
         break
         ;;
   esac
   shift
done

echo "BUILDOPTION=$BUILDOPTION"
echo "BUILDKEYS=$BUILDKEYS"
echo "INSTALLOPTION=$INSTALLOPTION"
echo "RUNOPTION=$RUNOPTION"
echo "DEBUGOPTION=$DEBUGOPTION"
echo "TESTOPTION=$TESTOPTION"
echo "PLATFORM=$PLATFORM"

##------------- project config -------------##

PREFIX="org.tizen"
INSTALLDIR=/usr/apps
SDB=~/tizen-sdk/tools/sdb
TEMPDIR=/home/rpms # for keeping rpm packages on device
GBSROOT=~/GBS-ROOT

##------------------ spec ------------------##

spec_file=`find -name *.spec`

# org.tizen.message
APPNAME=`cat "$spec_file" | grep ^Name    | awk '{print $2}'`
# 0.8.52
VERSION=`cat "$spec_file" | grep ^Version | awk '{print $2}'`
# 1
RELEASE=`cat "$spec_file" | grep ^Release | awk '{print $2}'`
# message
BINNAME=`echo $APPNAME | sed "s/$PREFIX\.//"`

RPMSPATH=$GBSROOT/local/BUILD-ROOTS/scratch.$PLATFORM.0/home/abuild/rpmbuild/RPMS/$PLATFORM

DEBUGSOURCEPKGNAME=$APPNAME-debugsource-$VERSION-$RELEASE.$PLATFORM
DEBUGINFOPKGNAME=$APPNAME-debuginfo-$VERSION-$RELEASE.$PLATFORM

##-----------------------------------------##

hasPrefix()
{
  if echo "$1" | grep -q "$PREFIX"; then
    return 0;
  else
    return 1;
  fi
}

##--------------- sdb shell ---------------##

SdbShell()
{
  ShowMessage "$1" yellow
  $SDB shell su -c "$1"
}

##--------------- color echo --------------##

# arguments:
# $1 - message text
# $2 - text color (red, green, blue, yellow). Default value - blue

ShowMessage()
{
    case "$2" in
    "green")
      color="32m"
      ;;
    "red")
      color="31m"
      ;;
    "blue")
      color="34m"
      ;;
    "yellow")
      color="33m"
      ;;
    *)
      color="34m"
      ;;
    esac

  echo "\033[1;"$color$1"\033[0m"
}

##----------- check connection ------------##

checkConnection()
{
    sdbstate=$($SDB get-state)
    if [ $sdbstate = "unknown" ]
      then
        ShowMessage "Connection error. Make sure that only one device or emulator is connected." red
        exit 0
    fi
}

##---------------- build ------------------##

build()
{
    ShowMessage "Building application..."

    gbsoutput="gbsoutput.log"

    if [ $LOCALBUILD = "true" ];
    then
      BUILDKEYS="$BUILDKEYS --noinit --incremental"
    fi

    if [ $TESTOPTION = "true" ];
    then
      BUILDKEYS+=" --define 'TEST%20ON'"
    fi

    ShowMessage "gbs -v -d build -B $GBSROOT -A $PLATFORM --include-all --keep-packs $BUILDKEYS"
    gbs -v -d build -B $GBSROOT -A $PLATFORM --include-all --keep-packs $BUILDKEYS 2>&1 | tee $gbsoutput

    if cat "$gbsoutput" | grep -q "gbs:info: Done"; then
      ShowMessage "Build successfull!" green
      rm -f $gbsoutput;
    else
      ShowMessage "Build failed!" red
      rm -f $gbsoutput
      exit 0;
    fi
}

##-----------------------------------------##

# figure out which packages need to be installed

initPackageList()
{
    packages_count=`cat "$spec_file" | grep -c %package`

    # if packages count more then 1
    if [ $packages_count -gt 1 ]
      then
        ShowMessage "When building the project is generated $packages_count (+ debuginfo and debugsource) RPM packages"
        # case of multipackage project
        PACKAGELIST=`cat "$spec_file" | grep %package | sed 's/-n//' |awk '{print $2}'`
      else
        ShowMessage "one package"
        # only one package into package list
        PACKAGELIST=$APPNAME
    fi
}

##------------ Uninstalling ---------------##

uninstall()
{
    ShowMessage "Stopping old application..."
    SdbShell "pkill -f $APPNAME"

    for current_package in $PACKAGELIST
    do
      ShowMessage "Uninstalling old application $current_package..."
      if hasPrefix $current_package; then
        # uninstall with pkgcmd
        SdbShell "pkgcmd -q -u -n $current_package"
      else
        # uninstall with rpm
        SdbShell "rpm -e --nodeps $current_package-*"
      fi
    done

    if [ $DEBUGOPTION = "true" ]
      then
        SdbShell "rpm -e --nodeps $APPNAME-debugsource-*"
        SdbShell "rpm -e --nodeps $APPNAME-debuginfo-*"
    fi
}

##-------------- Pushing ------------------##

push()
{
    SdbShell "mkdir -p $TEMPDIR/"

    for current_package_name in $PACKAGELIST
    do
      current_package=$current_package_name-$VERSION-$RELEASE.$PLATFORM.rpm
      ShowMessage "Pushing the package $current_package to device ..."
      $SDB push $RPMSPATH/$current_package $TEMPDIR/
    done

    if [ $DEBUGOPTION = "true" ]
      then
        $SDB push $RPMSPATH/$DEBUGSOURCEPKGNAME.rpm $TEMPDIR/
        $SDB push $RPMSPATH/$DEBUGINFOPKGNAME.rpm $TEMPDIR/
    fi
}

##------------- Installing ----------------##

install()
{
    for current_package_name in $PACKAGELIST
    do
      current_package=$current_package_name-$VERSION-$RELEASE.$PLATFORM.rpm
      ShowMessage "Installing the package $current_package ..."
      #$SDB install $RPMSPATH/$current_package
      #if hasPrefix $current_package; then
        # install with pkgcmd
        #SdbShell "pkgcmd -q -i -t rpm -p $TEMPDIR/$current_package"
      #else
        # uninstall with rpm
        SdbShell "rpm -ivh --force --nodeps $TEMPDIR/$current_package"
      #fi
    done

    if [ $DEBUGOPTION = "true" ]
      then
        ShowMessage "Installing the package $DEBUGINFOPKGNAME.rpm ..."
        SdbShell "rpm -i $TEMPDIR/$DEBUGINFOPKGNAME.rpm"
        ShowMessage "Installing the package $DEBUGSOURCEPKGNAME.rpm ..."
        SdbShell "rpm -i $TEMPDIR/$DEBUGSOURCEPKGNAME.rpm"
    fi

    SdbShell "pkg_initdb"
}

##--------------- Running ----------------##

run()
{
  if hasPrefix $APPNAME; then
    ABSOLUTEPATHTOAPP=$INSTALLDIR/$APPNAME/bin/$BINNAME
  else
    fullappname=`echo $APPNAME | sed "s/^/$PREFIX\./"`
    ABSOLUTEPATHTOAPP=$INSTALLDIR/$fullappname/bin/$BINNAME
  fi

  if  [ $1 = "tests" ]
    then
      TESTAPP=$ABSOLUTEPATHTOAPP-test
      # todo: stop old test-run
      ShowMessage "running the $TESTAPP..."
      SdbShell "$TESTAPP"
    else
      SdbShell "pkill -f $APPNAME"
      ShowMessage "running the $ABSOLUTEPATHTOAPP..."
      SdbShell "$ABSOLUTEPATHTOAPP"
  fi
}

##-----------------------------------------##

installApp()
{
  checkConnection
  initPackageList

  $SDB root on
  #uninstall
  push
  install

  $SDB root off
}

runApp()
{
  checkConnection
  run
}

runTest()
{
  checkConnection
  run tests
}

##------------------ main -----------------##

if [ $BUILDOPTION = "true" ]; then build; fi
if [ $INSTALLOPTION = "true" ]; then installApp; fi
if [ $RUNOPTION = "true" ]; then runApp; fi

##-----------------------------------------##
