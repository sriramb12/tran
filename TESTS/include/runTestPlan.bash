
## Sept 2018, Carlos Jose Mazieri
##
## This file must be included in other high level scripts
##
## The main function is runTestPlan() which executes a Test Plan spreadsheet using other parameters
##
## High level script using this MUST be  in the same directory because the File Tree is used to perform tests
##
## File Structure:
#     root/
#                TESTS/        TESTS/server      TESTS/client
#
#                source/       source/TESTS      source/TESTS/bin/                   source/TESTS/test_plan
#                                                source/TESTS/bin/runTestPlan.bash



rtp_RM="rm -f"
#rtp_RM="ls -l"

rtp_DEB=""


rtp_VAR_OK=0

rtp_ROOT_DIR=""
rtp_SOURCE_DIR=""

rtp_TESTS_DIR=""
rtp_SERVER_DIR=""
rtp_CLIENT_DIR=""
rtp_RESULT_DIR=""

rtp_SOURCE_TESTS=""
rtp_SOURCE_TESTS_BIN=""
rtp_TEST_PLAN_DIR=""
rtp_RELEASE_DIR=""
rtp_ORIG_DATA_DIR=""
rtp_ORIG_DIR=""
rtp_ORIG_FILE=""

rtp_TEST_PLAN=""
rtp_INTERNAL_EMAIL=""
rtp_FILE=""
rtp_SERVER=""
rtp_CLIENT=""
rtp_CLIENT_TYPE=""
rtp_COMPANY=""

rtp_COMPANY_NAME=""
rtp_COMPANY_OPION=""
rtp_SERVER_OPTION=""
rtp_CLIENT_OPTION=""
rtp_CLIENT_TYPE_OPTION=""
rtp_EXTERNAL_EMAIL="transbugexternal@gmail.com"
rtp_CURRENT_USER_EMAIL=""
rtp_ICOMRC_PATH=""

rtp_counter_passed=0
rtp_counter_failed=0

rpt_check_and_create_dir()
{
    dir=$1
    if [ ! -d "$dir" ]
    then
         mkdir -p $dir 2> /dev/null
         if [ $? -ne 0 ]
         then
            echo "Could create directory $dir"
            exit 1
         fi
    fi
}


rtp_priv_setVars()
{
  dir=`dirname $0`
  cd $dir
  rtp_SOURCE_TESTS_BIN=`pwd`
  cd ..
  rtp_SOURCE_TESTS=`pwd`
  rtp_TEST_PLAN_DIR=$rtp_SOURCE_TESTS/test_plan
  rtp_ORIG_DATA_DIR=$rtp_SOURCE_TESTS/data
  cd ..
  rtp_SOURCE_DIR=`pwd`
  rtp_RELEASE_DIR=$rtp_SOURCE_DIR/RELEASE_DIR
  cd ..
  rtp_ROOT_DIR=`pwd`
  rtp_TESTS_DIR=$rtp_ROOT_DIR/TESTS
  rtp_SERVER_DIR=$rtp_TESTS_DIR/server
  rtp_CLIENT_DIR=$rtp_TESTS_DIR/client
  rtp_RESULT_DIR=$rtp_TESTS_DIR/results
  rpt_check_and_create_dir  $rtp_CLIENT_DIR
  rpt_check_and_create_dir  $rtp_RESULT_DIR
  if [ ! -d "$rtp_SERVER_DIR" ]
  then
        echo
        echo "Creating Test Environment directory structure ..."
        rpt_check_and_create_dir $rtp_SERVER_DIR
        `gpg --list-keys testTransFTP 1> /dev/null 2> /dev/null`
        if [ $? -ne 0 ]
        then
             cd $rtp_SOURCE_TESTS/gpg_key/
             sh import_and_set_gpg_testTransFTP_key.sh
        fi
        cd $rtp_TESTS_DIR
        tar -xzf $rtp_SOURCE_TESTS/install/server.tgz
        echo 
        read -p "Test Environment directory structure created. Press Enter to start testing .." nothing
  fi
  rtp_CURRENT_USER_EMAIL=`rtp_getCurrentUserEmail`
}


rtp_priv_remove_server_data()
{
   $rtp_RM  $rtp_SERVER_DIR/data_files?/*  2> /dev/null
}


rtp_priv_remove_client_destination_file()
{
   $rtp_RM $rtp_CLIENT_DIR/$rtp_ORIG_FILE          2> /dev/null
   $rtp_RM $rtp_CLIENT_DIR/trace_debug.log         2> /dev/null
   $rtp_RM $rtp_CLIENT_DIR/trans.log               2> /dev/null
   $rtp_RM $rtp_CLIENT_DIR/icom.log                2> /dev/null
   $rtp_RM $rtp_ORIG_DIR_DIR/trace_debug.log       2> /dev/null
   $rtp_RM $rtp_ORIG_DIR_DIR/trans.log             2> /dev/null
   $rtp_RM $rtp_ORIG_DIR_DIR/icom.log              2> /dev/null
}


rtp_priv_remove_data()
{
  rtp_priv_remove_server_data
  rtp_priv_remove_client_destination_file
}


rtp_getCurrentUserEmail()
{
    email="transbug@nxp.com"
    if [ -d "$rtp_TESTS_DIR" ]
    then
       ldapsearc_cmd="$rtp_TESTS_DIR/_ldapsearch_"
       ldapsearc_source_dir="$rtp_SOURCE_DIR/src"
       if [ ! -x "$ldapsearc_cmd" -a -d "$ldapsearc_source_dir" ]
       then
           gcc -DSTANDALONE -DBEAUTY_OUTPUT_ONLY -I$ldapsearc_source_dir -o $ldapsearc_cmd $ldapsearc_source_dir/ldapfsl.c -L/usr/lib64 -lldap -lssl -lcrypto
       fi
       if [ -x "$ldapsearc_cmd" ]
       then
          m=`$ldapsearc_cmd -E -c $USER | grep Email | head -1 | awk {'print $2'}`
          if [ "$m" != "" ]
          then
              email=$m
          fi
       fi
    fi
    echo $email
}


runTestPlan()
{
  rtp_TEST_PLAN=$1    ## name of the test plan without path
  rtp_INTERNAL_EMAIL=$2
  rtp_FILE=$3         ## file or dir to be used as input
  rtp_SERVER=$4       ## can be 'production/prod/plan' to use from spreadsheet, or the name to start it from development environment
  rtp_CLIENT=$5       ## 'plan' to use it from the spreadsheet, or 'trans'  or other name such as 'icom.int', or 'icom.ext'
  rtp_CLIENT_TYPE=$6  ## 'plan' to use it from spreadsheet otherwise 'dev' or 'prod'
  rtp_COMPANY=$7      # optional = etc rtc dtms or nothing

  rtp_COMPANY_NAME=""
  rtp_COMPANY_OPION=""
  rtp_SERVER_OPTION=""
  rtp_CLIENT_OPTION=""
  rtp_CLIENT_TYPE_OPTION=""

  if [ $rtp_VAR_OK -eq 0 ]
  then
      rtp_priv_setVars
      rtp_VAR_OK=1
  fi

  if [ $# -lt 6 ]
  then
     return
  fi

  rtp_ORIG_FILE=`basename $rtp_FILE`
  rtp_ORIG_DIR="`dirname $rtp_FILE`"
  if [ "$rtp_ORIG_DIR" = "" ]
  then
      rtp_ORIG_DIR="."
  fi

  rpt_cmd_line_allows_to_run
  if [ $? -ne 0 ]
  then
      #echo  "rpt_cmd_line_allows_to_run() NOT ZERO, returning ..."
      return
#   else
#       echo "rpt_cmd_line_allows_to_run() return 0 OK to run"
#       return
  fi

  cd $rtp_TESTS_DIR

  if [ "$rtp_COMPANY" != "" ]
  then
     rtp_COMPANY_NAME="${rtp_COMPANY}"
     rtp_COMPANY_OPION="-company -${rtp_COMPANY}"
  else
     rtp_COMPANY_NAME="default"
  fi

  if [ "`echo $rtp_SERVER | grep -i prod`" = "" -a "`echo $rtp_SERVER | grep -i plan`" = "" ]
  then
     rtp_SERVER_OPTION="-server $rtp_SERVER"
  fi

  if [ "$rtp_CLIENT" != "plan" ]
  then
      rtp_CLIENT_OPTION="-client $rtp_CLIENT"
  fi

  if [ "$rtp_CLIENT_TYPE" != "plan" ]
  then
     rtp_CLIENT_TYPE_OPTION="-clientbuild $rtp_CLIENT_TYPE"
  fi

  rtp_TEST_PLAN_PATHNAME=$rtp_TEST_PLAN_DIR/$rtp_TEST_PLAN
  rtp_plan=`basename $rtp_TEST_PLAN .csv`
  rtp_file_orig=`basename $rtp_FILE`
  rtp_OUT_NAME="${rtp_file_orig}__{to:${rtp_COMPANY_NAME}}__{server:${rtp_SERVER}}__{{client:${rtp_CLIENT}}_{type:${rtp_CLIENT_TYPE}}}__{test_plan:${rtp_plan}}"

  rtp_priv_remove_data
  rtp_CLIENT_DIR_RESULTS="$rtp_RESULT_DIR/${rtp_OUT_NAME}"

  if [ ! -d "$rtp_CLIENT_DIR_RESULTS" ]
  then
          mkdir -p $rtp_CLIENT_DIR_RESULTS
  fi
  $rtp_RM $rtp_CLIENT_DIR_RESULTS/000*  2> /dev/null ## clear previous 000 result indication file

  bash $rtp_DEB $rtp_SOURCE_TESTS_BIN/test_plan_runner.bash \
     -c $rtp_TEST_PLAN_PATHNAME \
     -f $rtp_FILE \
     -r $rtp_RELEASE_DIR \
     -e $rtp_INTERNAL_EMAIL \
     -eemail $rtp_EXTERNAL_EMAIL \
      $rtp_CLIENT_OPTION \
      $rtp_CLIENT_TYPE_OPTION \
      $rtp_COMPANY_OPION \
      $rtp_SERVER_OPTION \
      $rtp_ICOMRC_PATH \
     -d $rtp_OUT_NAME \
          | tee $rtp_RESULT_DIR/${rtp_OUT_NAME}/general_log.txt
   if [  ${PIPESTATUS[0]} -ne 0 ]
   then
         echo
         echo "*** LAST TEST failed ... "
         echo
         ## do not rename, leave that for next run
#         /bin/mv $rtp_CLIENT_DIR_RESULTS "$rtp_RESULT_DIR/FAILED_${rtp_OUT_NAME}"
         rtp_counter_failed=`expr $rtp_counter_failed + 1`
   else
         /bin/mv $rtp_CLIENT_DIR_RESULTS "$rtp_RESULT_DIR/PASSED_${rtp_OUT_NAME}"
         rtp_counter_passed=`expr $rtp_counter_passed + 1`
   fi
   rtp_priv_remove_data
   echo
   echo "-- Test Plan ${rtp_plan} tests summary, see output in $rtp_RESULT_DIR"
   echo "   Passed: $rtp_counter_passed"
   echo "   Failed: $rtp_counter_failed"
   echo
}

