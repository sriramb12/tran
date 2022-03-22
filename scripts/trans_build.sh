#!/bin/bash

## Feb 2018 by Carlos Mazieri <carlos.mazieri@nxp.com>

# Build wrapper make it easy automation

MODULES="\
build-intercom-external-client
build-intercom-external-server
build-intercom-internal-client
build-intercom-internal-server
build-transcend-client
build-transcend-server
build-transftp-server
build-transweb-client-icom-lib
"

OBSOLETE="\
build-foundry-clients
build-intercom-intermediate-server
build-transweb-client
build-transweb-server
"

OS=`uname`

TRANS_DEBUG=N
export TRANS_DEBUG

SPECIF_MODULES=""
ALL_MODULES=0
GROUP_MODULES=""


EXTERNAL_TRANSCEND_ENV_CFLAGS=""
export EXTERNAL_TRANSCEND_ENV_CFLAGS

INTERNAL_ENV_CFLAGS=""
GIT_SHA=""
CLEAR_LOG=0
BUILD_CONTROL_FILE=.trans_build.txt
RELEASE_MODE=0
DEBUG_SET=0
SEARCH_FUNCTION=""
generated_binaries=""
TRACE_LOG_PATH=""
SSL_MIN=1.0.2

MAKE="make -w"
MAXDEPTH="-maxdepth 1"
NEWER="-cnewer"
if [ "$OS" != "Linux" ]
then
        MAXDEPTH=""
        NEWER="-newer"
        if [ "$OS" = "SunOS" ]
        then
          MAKE=/_TOOLS_/dist/gnu-make-3.81/sparc64-sun-solaris2.8/bin/make
        fi
fi

get_binaries()
{
     if [ "$OS" != "Linux" ]
     then
        $MAKE clean > /dev/null
     fi
     generated_files=`/usr/bin/find .  $MAXDEPTH $NEWER $BUILD_CONTROL_FILE | grep -v zlib`
     generated_binaries=""
     for binary in $generated_files
     do
          is_binary=`/usr/bin/file $binary |  grep -i executable`
          if [ "$is_binary" != "" ]
          then
          generated_binaries="$binary $generated_binaries"
          fi
     done
}

make_module() #1 = module  $2 = group (optional)
{
    module=$1
    group=$2
    if [ "$GIT_SHA" = "" ]
    then
      GIT_SHA=$(./mk/cfg/git_build_info.sh)
      export GIT_SHA
      ## save this to tell the build has started at this time
      date > $BUILD_CONTROL_FILE
    fi

    if [ "$group" != "" ]
    then
       is_in_group=`echo $module |  grep $group`
       if [ "$is_in_group" = "" ]
       then
           return
       fi
    fi
    BUILD_MODULE=$module
    export BUILD_MODULE
    LOG="build_log/${module}.txt"
    mkdir -p build_log/
    /bin/rm -f $LOG
    $MAKE clean > /dev/null
    /bin/rm -f *.a
    echo "make build-zlib"
    make build-zlib  > $LOG 2>&1
    echo "make $module"
    $MAKE -w $module    >> $LOG 2>&1
    if [ $? -eq 0 ]
    then
       generated_libs=`/usr/bin/find .  $MAXDEPTH $NEWER $BUILD_CONTROL_FILE -name "*.a" | tr '\n' ' ' `
       echo "warnings `grep -c warning $LOG`, LIBS=$generated_libs"
       if [ "$SEARCH_FUNCTION" != "" ]
       then
            get_binaries
             /usr/bin/nm -A $generated_libs perl_intercom_lib/*.so $generated_binaries 2> /dev/null | grep  $SEARCH_FUNCTION
       fi
       if [ $CLEAR_LOG -eq 1 ]
       then
            /bin/rm -f $LOG
       fi
    else
        echo
        echo "*** ERROR "
        echo " build $module has failed see  $LOG"
        echo
        grep error $LOG | egrep -v -e "^gcc" | egrep -v -e "^g++"
        /bin/rm -f $BUILD_CONTROL_FILE
        exit 1
    fi
}

help()
{
  echo $0 "  [-d] [-c] [-r] [-s] [-l] [-h name-or-address] [-p port] [-n mumber]"
  echo "     [[-ac|-as|-a] | [module1 module2 ...]]"
  echo "        -d          Enable DEBUG and Trace -> generates trace_debug.log"
  echo "        -t <dir>    Saves trace_debug.log in another directory than the current"
  echo "        -c          Clear build log if build works"
  echo "        -r          Release mode: move generated binaries to RELEASE_DIR directory"
  echo "        -s          Server is a single process no fork is allowed implies DEBUG mode"
  echo "        -m          servers do NOT send emails, email content are saved into files 'transcend_email_test_<counter>.txt'"
  echo "        -ba         Bypass Authentication in LDAP"
  echo "        -l          Clients are forced to connect to localhost"
  echo "        -h <host>   Clients are forced to connect to host (or address)"
  echo "        -p <port>   Clients are forced to connect using a specific port"
  echo "        -n <number> Number of times that Clients are forced to connect to localhost (-l),"
  echo "                  host (-h <host>) and/or port(-p <port>): default: all the time"
  echo "        -NSSL       Force using SSL version 1.0.2u which can disable SSLv_2 and SSL_v3"
  echo "        -f <func>   Search for a specific string (function) in the generated libraries,"
  echo "                  shown close to warnings counter"
  echo "        -ac         Builds clients only"
  echo "        -as         Builds servers only"
  echo "        -a          Builds all modules, instead of -a modules one or modules can specified"
  echo

  for m in $MODULES
  do
      obs=`echo $OBSOLETE | grep $m`
      if [ "$obs" != "" ]
      then
           obs="[OBSOLETE]"
      fi
      echo "$m $obs"
  done
  exit 0
}

setDebugMode()
{
   if [ $DEBUG_SET -eq 0 ]
   then
       DEBUG_SET=1
       TRANS_DEBUG=YES
       export TRANS_DEBUG
       INTERNAL_ENV_CFLAGS="-DAVOID_DBG_WAIT $INTERNAL_ENV_CFLAGS"
   fi
}


while [ "$1" != "" ]
do
     case $1 in
        -f)   shift
              SEARCH_FUNCTION=$1
              ;;
        -r)   RELEASE_MODE=1
              ;;
        -c)   CLEAR_LOG=1
              ;;
        -h)   shift
              INTERNAL_ENV_CFLAGS="-DLOCAL_TEST_SERVER=\\\"$1\\\" $INTERNAL_ENV_CFLAGS"
              ;;
        -l)   INTERNAL_ENV_CFLAGS="-DLOCAL_TEST $INTERNAL_ENV_CFLAGS"
              ;;
        -s)   INTERNAL_ENV_CFLAGS="-DDEVEL_USING_SINGLE_PROCESS -Dfork=FAKE_fork $INTERNAL_ENV_CFLAGS"
              setDebugMode
              ;;
        -m)   INTERNAL_ENV_CFLAGS="-DDO_NOT_SEND_EMAIL $INTERNAL_ENV_CFLAGS"
              ;;
        -ba)  INTERNAL_ENV_CFLAGS="-DLDAP_BYPASS_AUTHENTICATION $INTERNAL_ENV_CFLAGS"
              ;;
        -n)   shift
              LOCAL_TEST_COUNTER=$1
              INTERNAL_ENV_CFLAGS="-DLOCAL_TEST_COUNTER=$LOCAL_TEST_COUNTER $INTERNAL_ENV_CFLAGS"
              ;;
        -p)   shift
              LOCAL_TEST_PORT=$1
              INTERNAL_ENV_CFLAGS="-DLOCAL_TEST_PORT=$LOCAL_TEST_PORT $INTERNAL_ENV_CFLAGS"
              ;;
        -a)   ALL_MODULES=1;;
        -as)  GROUP_MODULES="server";;
        -ac)  GROUP_MODULES="client";;
        -d)   setDebugMode
              ;;
        -t)   shift
              INTERNAL_ENV_CFLAGS="-DTRACE_LOG_PATH=\\\"$1\\\" $INTERNAL_ENV_CFLAGS"
              ;;
         *)   for module in $MODULES
              do
                    if [ "$module" = "$1" ]
                    then
                        SPECIF_MODULES="$SPECIF_MODULES $1"
                    fi
              done
              ;;
     esac
     shift
done


if [ "$INTERNAL_ENV_CFLAGS" != "" -a "$INTERNAL_ENV_CFLAGS" != " " ]
then
      EXTERNAL_TRANSCEND_ENV_CFLAGS="$INTERNAL_ENV_CFLAGS"
      export EXTERNAL_TRANSCEND_ENV_CFLAGS
fi


value_of_EXTERNAL_TRANSCEND_ENV_CFLAGS=`env | grep EXTERNAL_TRANSCEND_ENV_CFLAGS`
echo "** Using compiler options from environment variable minimal SSL = $SSL_MIN $value_of_EXTERNAL_TRANSCEND_ENV_CFLAGS"
echo


if [ "$SPECIF_MODULES" != "" ]
then
    for module in $SPECIF_MODULES
    do
        make_module $module
    done
else
    if [ $ALL_MODULES -eq 1  -o "$GROUP_MODULES" != "" ]
    then
        for module in $MODULES
        do
            make_module $module $GROUP_MODULES
        done
    else
        help
    fi
fi




echo "build logs"
ls -lrt $LOG
get_binaries

echo ""
echo "Binaries files:"

if [ $RELEASE_MODE -eq 1 -a "$generated_binaries" != "" ]
then
    /bin/mkdir -p RELEASE_DIR 2> /dev/null
    if [ "$TRANS_DEBUG" !=  "YES" ]
    then
       strip $generated_binaries
    fi
    /bin/mv $generated_binaries RELEASE_DIR
else
    /bin/ls -rtl bin
fi

/bin/rm -f $BUILD_CONTROL_FILE

## it is suppose to create the following binaries:
# foundry_send
# free.icomd
# icom
# ldap_info
# ldap_login
# ssl.icomd
# ssl.intd
# ssl.tftpd/bin
# ssl.transcend
# trans
# transweb.bin
# transweb.server

