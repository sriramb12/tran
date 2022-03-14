#!/bin/bask

### Carlos Mazieri Feb 202
###
##  Script to generate next package according to the Operating System



OS=""
CYGWIN_OS="windows-cygwin-64"
SHORT_TYPE=""
TYPE=
DIR_OUT=""
DIR_BIN=RELEASE_DIR
TRANS_VERSION=""
ICOM_VERSION=""
VERSION=""
PKG_NAME=""
TEMPLATE=""
TEMPLATE_TYPE="generic"
TERMINATION="tar.gz"
EXE=""

help()
{
    echo "$0 -t <trans|icom> [-os <rhel5|rhel6|rhel7|ubuntu|macos|cygwin|solaris8>] [-version <next-version>]"
    echo "       generate package accorgin to the Operating System"
    echo "       if -os not provided try to guess current OS."
    echo "       if -version is not provided get from current version"
    exit 0
}


setLinuxOS()
{
    OS=linux
    if [ -f "/etc/redhat-release" ]
    then
       ver=`grep -o '[0-9]\+' /etc/redhat-release | head -1`
       OS="rhel${ver}"
    else
         if [ -f "/etc/os-release" ]
         then
              OSES="ubuntu fedora debian"
              for probe_os in $OSES
              do
                  if [ "`grep -i $probe_os /etc/os-release`" != "" ]
                  then
                      OS=$probe_os
                      break
                  fi
              done
         fi
    fi
}


setOS()
{
    case `uname` in
      SunOS)  OS="solaris8"
               ;;
      Darwin) OS="macos"
              ;;
      Linux)  setLinuxOS
              ;;
      *CYGWIN*)  OS=$CYGWIN_OS
    esac
}


catch_digits()
{
   STR=$1
   digits=`echo $STR | tr -d '.' | tr -d ' '`
   echo $digits
}

setVersion()
{
    vv=`grep -i "TRANSAPI_VERSION_NUMBER" generic.def | head -1 | cut -f2 -d=`
    echo $vv
    TRANS_VERSION=`catch_digits $vv`
    vv=`grep -i "CURRENT_ICOM_VERSION_NUMBER" generic.def | head -1 | cut -f2 -d=`
     echo $vv
    ICOM_VERSION=`catch_digits $vv`
    if [ "$SHORT_TYPE" = "trans" ]
    then
       VERSION=$TRANS_VERSION
    else
       VERSION=$ICOM_VERSION
    fi
}


generate_package()
{
   HERE=`pwd`
   TMP_DIR="_packaging_tmp_$$"
   mkdir $TMP_DIR
   cd $TMP_DIR
   if [ "$TERMINATION" = "tar.gz" ]
   then
       gunzip ../scripts/packaging/templates/$TEMPLATE -c  | tar xvf -
   else
       unzip ../scripts/packaging/templates/$TEMPLATE
   fi
   release_tr_dir=../TR_release_trans_${TRANS_VERSION}_icom_${ICOM_VERSION}
   TRANS_OUT="trans${EXE}"
   ICOM_OUT="icom${EXE}"
   if [ "$OS" = "$CYGWIN_OS" ]
   then
       TRANS_OUT="$TYPE/trans${EXE}"
       ICOM_OUT="$TYPE/icom${EXE}"
   fi

   if [ "$SHORT_TYPE" = "trans" ]
   then
      mkdir -p $release_tr_dir/$OS
      cp "../$DIR_BIN/icom.int" $ICOM_OUT
      cp "../$DIR_BIN/trans${EXE}"    $TRANS_OUT
      cp  $ICOM_OUT  $TRANS_OUT       $release_tr_dir/$OS
   else
      cp "../$DIR_BIN/icom.ext" $ICOM_OUT
   fi
   ## generate package
   if [ "$TERMINATION" = "tar.gz" ]
   then
      tar -cvf - *  | gzip > ../$PKG_NAME
   else
      zip -r ../$PKG_NAME *
   fi
   cd $HERE
   rm -rf $TMP_DIR
}


copy_package()
{
    if [ -d "$DIR_OUT" -a -f "$PKG_NAME" ]
    then
        copy=1
        if [ -f "$DIR_OUT/$PKG_NAME" ]
        then
           echo "$DIR_OUT/$PKG_NAME already exists, overwrite it (Y/n)? "
           read opt
           if [ "$opt" = "n" -o "$opt" = "N" ]
           then
               copy=0
           fi
        fi
        if [ $copy -eq 1 ]
        then
          echo "copying $PKG_NAME as $DIR_OUT/$PKG_NAME "
          cp -f  $PKG_NAME $DIR_OUT/$PKG_NAME
          if [ $? -eq 0 ]
          then
             rm -f $PKG_NAME
          fi
        fi
    fi
    if [ -f "$PKG_NAME" ]
    then
      echo "$PKG_NAME not copied ..."
    fi
}


check_build()
{
    if [ "$1" = "trans" ]
    then
       TYPE="transcend"
       if [ ! -x "$DIR_BIN/trans${EXE}" -o ! -x "$DIR_BIN/icom.int" ]
       then
          echo "Binaries 'trans' and 'icon.int' must be present in $DIR_BIN, see 'trans_build.sh'"
          exit 1
       fi
    else
        if [ "$1" = "icom" ]
        then
           TYPE="intercom"
           if [ ! -x "$DIR_BIN/icom.ext" ]
           then
               echo "Binary 'icon.ext' must be present in $DIR_BIN, see 'trans_build.sh'"
               exit 1
           fi
        else
           help
        fi
    fi
}



## MAIN --

if [ "$1" != "-t" ]
then
   help
fi

SHORT_TYPE=$2

## get all options if present
while [ "$3" ]
do
    if [ "$3" = "-os" ]
    then
         shift
         OS=$3
         if [ "$OS" = 'cygwin' ]
         then
            OS=$CYGWIN_OS
         fi
    else
          if [ "$3" = "-version" ]
          then
             shift
             VERSION=$3
          fi
    fi
    shift
done


if [ "$OS" = "" ]
then
   setOS
fi

if [ "$VERSION" = "" ]
then
    setVersion
fi


if [ "$OS" = "$CYGWIN_OS" ]
then
  TERMINATION="zip"
  TEMPLATE_TYPE="cygwin"
  EXE=".exe"
fi


check_build "$SHORT_TYPE"

if [ "$SHORT_TYPE" = "trans" ]
then
   PKG_NAME="${TYPE}-${VERSION}-${OS}.${TERMINATION}"
else
    PKG_NAME="${TYPE}-${VERSION}-ext-${OS}.${TERMINATION}"
fi
TEMPLATE="${TYPE}.${TEMPLATE_TYPE}.${TERMINATION}"
DIR_OUT="/proj/dtms_web/${TYPE}/htdocs/live/download"

echo "OS=$OS  VERSION=$VERSION TYPE=$TYPE SHORT_TYPE=$SHORT_TYPE TEMPLATE=$TEMPLATE PKG_NAME=$PKG_NAME"

generate_package
copy_package
