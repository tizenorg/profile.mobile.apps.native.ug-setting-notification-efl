#!/bin/bash

usage()
{
echo "
usage: sh run.sh [-h] [-b] [-i] [-q] [-r] [-t] [-d] [-A ARCH]
                 [--help] [--build] [--install] [--quick-install] [--run] [--test] [--debug] [--arch ARCH]

optional arguments:
  -h, --help             show this help message and exit

action:
  -b  --build           build project
  -i, --install         install to device or emulator
  -q, --quick-install	quick installation(skips pkg_initdb). DO NOT use it when with manifest changed. Ignored in tizen-2.4 build
  -r, --run             run application. Don't use with -t option
  -n, --notest          skips build of unit-tests(tests are included into build-procedure by default), ignored without -b option
  -d, --debug           install debuginfo and debugsource packages
  -l, --local		local incremental build(with --noinit --incremental kwys)

build configuration options:
  -A ARCH, --arch ARCH   build target arch. Default - armv7l
  -b 'Keys', --build 'Keys'  build project with additional gbs build keys
  !You must specify profiles profile.tizen_2.4 and profile.tizen_3.0 in .gbs.conf to use VERSION options.
  !You may omit version and default profile will be picked. Notice, that target version is still tizen_3.0 in this case.
  -v VERSION, --version VERSION build project for target version. Default build on tizen_3.0

  examples:
  'run.sh -b -i -A armv7l' will build application and try to install it to target-device, unit-tests will be also built
  'run.sh -i -A armv7l[i586|aarch64]' will install latest build for tizen-2.4 device [emulator|tizen-3.0 device]
  'run.sh -b -A armv7l -v tizen_2.4[tizen_3.0]' will build for tizen-2.4[tizen-3.0] on armv7l arch
"
}

##-------------- check options --------------##

if [ -z "$1" ]; then usage; exit 0; fi

BUILDOPTION=false
BUILDKEYS=""
INSTALLOPTION=false
RUNOPTION=false
DEBUGOPTION=false
TESTOPTION=true
PLATFORM=armv7l
LOCALBUILD=false
TIZENVERSION="default"
QUICKINSTALL=false

##------------- project config -------------##
PREFIX="org.tizen"
INSTALLDIR=/usr/apps
HOMEDIR="$(getent passwd $USER | awk -F ':' '{print $6}')"
SDB=$HOMEDIR/tizen-sdk-3.0/tools/sdb
TEMPDIR=/home/rpms # for keeping rpm packages on device
GBSROOT=$HOMEDIR/GBS-ROOT
RPMSPATH=$GBSROOT/local/BUILD-ROOTS/scratch.$PLATFORM.0/home/abuild/rpmbuild/RPMS/$PLATFORM
##------------- project config -------------##

SHORTOPTS="hA:b::irdnlvq"
LONGOPTS="arch:,build::,install,run,debug,notest,help,local,version,quick-install:"
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
      -v|--version)
         val=`echo $2 | sed -e "s/-//g"`
         if echo "$SHORTOPTS$LONGOPTS" | grep -q "$val"; then
           echo "Wrong VERSION"
           usage
           exit 0;
         fi
         TIZENVERSION=$2
         if [ $TIZENVERSION != "default" ] && [ $TIZENVERSION != "tizen_3.0" ];
	 then
	    GBSROOT="$HOMEDIR/GBS-ROOT-profile.$TIZENVERSION"
	 fi
	 RPMSPATH=$GBSROOT/local/BUILD-ROOTS/scratch.$PLATFORM.0/home/abuild/rpmbuild/RPMS/$PLATFORM
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
      -n|--notest)
         TESTOPTION=false
         ;;
      -q|--quick-install)
	 QUICKINSTALL=true
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
echo "TIZENVERSION=$TIZENVERSION"
echo "QUICKINSTALL=$QUICKINSTALL"

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
  ShowMessage "$1" -c yellow
  $SDB shell su -c "$1"
}

##--------------- color echo --------------##

# arguments:
# $1 : message text
# -c|--color : text color (red, green, blue, yellow). Default value - blue
# -n|--noti : show GUI notification
# -s|--secondary : secondary message text
# example: ShowMessage "Message to show" -c red --noti -s "Some additional text"

ShowMessage()
{
    show_noti=false
    color_val=""
    message_sec=""
    message=""

    while [[ $# > 0 ]]; do
      key="$1"

      case $key in
        -c|--color)
        color_val="$2"
        shift # past argument
        ;;
        -s|--secondary)
        message_sec="$2"
        shift # past argument
        ;;
        -n|--noti)
        show_noti=true
        ;;
        *)
        message="$1"
        ;;
      esac
      shift # past argument or value
    done

    case "$color_val" in
    "green")
      color="32m"
      noti_icon="-i $PWD/edje/images/icon_emotion_sunglasses.png"
      ;;
    "red")
      color="31m"
      noti_icon="-i $PWD/edje/images/icon_emotion_angry.png"
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

    echo -e "\033[1;"$color$message $message_sec"\033[0m"
    if [ $show_noti = "true" ]; then
      notify-send -u critical $noti_icon "$message" "$message_sec"
    fi
}

##----------- check connection ------------##

checkConnection()
{
    sdbstate=$($SDB get-state)
    if [ $sdbstate = "unknown" ]
      then
        ShowMessage "Connection error." -c red --noti -s "Make sure that only one device or emulator is connected."
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

    if [ $TIZENVERSION != "default" ];
    then
      BUILDKEYS+=" -P profile.$TIZENVERSION"
    fi

    ShowMessage "gbs -v -d build -B $GBSROOT -A $PLATFORM --include-all --keep-packs $BUILDKEYS"
    gbs -v -d build -B $GBSROOT -A $PLATFORM --include-all --keep-packs $BUILDKEYS 2>&1 | tee $gbsoutput

    if cat "$gbsoutput" | grep -q "gbs:info: Done"; then
      ShowMessage "Build successfull!" -s "In glory of Tizen!" -c green --noti
      rm -f $gbsoutput;
    else
      comp_error=$(cat "$gbsoutput" | grep "error:"| sed '/Bad exit status/,//d; s/\[.*\/\(.*\) error:/\\nerror in \1/;')
      ShowMessage "Build failed!" -s "$comp_error" -c red --noti
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
      if [ $TIZENVERSION = "tizen_2.4" ];
      then
        $SDB install $RPMSPATH/$current_package
      else
        # uninstall with rpm
        SdbShell "rpm -ivh --force --nodeps $TEMPDIR/$current_package"
      fi
    done

    if [ $DEBUGOPTION = "true" ]
      then
        ShowMessage "Installing the package $DEBUGINFOPKGNAME.rpm ..."
        SdbShell "rpm -i $TEMPDIR/$DEBUGINFOPKGNAME.rpm"
        ShowMessage "Installing the package $DEBUGSOURCEPKGNAME.rpm ..."
        SdbShell "rpm -i $TEMPDIR/$DEBUGSOURCEPKGNAME.rpm"
    fi

    if [ $TIZENVERSION != "tizen_2.4" ] && [ $QUICKINSTALL = "false" ];
    then
      SdbShell "pkg_initdb"
      ShowMessage "pkg_initdb finished" -c green --noti
    fi
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

  if [ $TIZENVERSION != "tizen_2.4" ];
  then
    $SDB root on
    #uninstall
    push
  fi

  install

  if [ $TIZENVERSION != "tizen_2.4" ];
  then
    $SDB root off
  fi
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
