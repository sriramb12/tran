#!/bin/bash

## March 2018, Carlos Jose Mazieri
##
#   Perform transcend/icom tests based on a CSV file test plan

#  CSV file
#    1. must be separated by commas or semicolons
#    2. columns (most of them are comments only and they are not used:
#       1="status"            -> it must be empty in order of the test from the current row be performed
#       2="Comment"           -> not used
#       3="ssh user@host"     -> indicates the the test row will be formed on another machine using ssh (password less is required)
#       4="Operation"         -> it must be "Put",  "Get" or "Other"  otherwise the current test row is not performed
#       5="server"            -> base server name, the lastest version whatever <name>.debug will be used from <release-dir> (see help function)
#                                It can be empty or "production" telling that no server will be started allwoing clients to run agains production serves
#       6="Client"            -> client name for the variable <client> in command line (column 8) when build type (column 7) is "dev"
#                                  the latest version <name>.debug.localhost is used, otherwise name is used
#       7="build: RH5/dev"    -> 'dev' specifies to use the latest client from development otherwise client name will be used if specified in the command line (colmn 8)
#       8="command line"      -> specifies the command to be performed any redirection:  '<' is allowed
#                                variables:  <client> from column 6,
#                                            <file>   from command line -f,
#                                            <file_size>   the size of the file from command line -f,
#                                            <email>  from command line
#                                            <server_path> the path from where server is being started
#                                            <keyword> last keyword printed from last PUT operation, it can be either "Secure keyword" if it exists or "File Keyword"
#                                           <file_keyword> the "File Keyword" from last PUT operation, "Secure keyword" if exists will not be used
#                                           <icomrcpath>  the path to be used as -icomrc <icomrcpath>
#                                           <extern_email> an email to be used in extern icom commands
#                                examples:
#                                            <client> -p <file> <email>   < ~/carlos.mazieri.txt
#                                            <client> -g <keyword>
#                                            <client> -p <file> <email>
#                                            <client> -g <file_keyword>
#       9="scenario"          -> not used
#       10="requer approval"  -> not used
#       11=".icomrc:" yes/no" -> not used
#       12="Client output"    -> not used
#
#  Test area:
#       It must be performed in:
#                                 <test dir> /
#                                              <server>/
#                                                        <db_admin>
#                                              <client>

# Output:  nn is the row number in CSV file
#                                 <test dir> /
#                                              <results> /
#                                                         <csv-name> /
#                                                                    <PUT_nn> /
#                                                                    <GET_nn> /


help()
{
    echo "$0 <-r release-dir> <-c csv-file>  [-f common-file-to-put] [-e email ] [-p port] [-client name] [-clientbuild <dev/prod>]"
    echo "   -r      release-dir  <mandatory>  the directory where to find development binaries"
    echo "   -c      csv-file     <mandatory>  the name o the CSV test plan separated by commas or semicolons"
    echo "   -f      file         <mandatory>  a file or directory to be used in variable <file> in PUT commands"
    echo "   -d      out-dir      <optional>   saves the output into out-dir, it can be like results/dir"
    echo "   -e      email        <optional>   an email to be used as variable <email> "
    echo "   -p      port         <optional>   a single digit to specify server listen port default: 3"
    echo "   -client name         <optional>   ignore the client column of the spreadsheet and uses this name as a global 'client' for all rows"
    echo "   -clientbuild mode    <optional>   ignore 'Build: dev/prod' from the spreadsheet and uses 'mode' for all rows"
    echo "   -company             <optional>   sets the internal variable <company>, it must include the minus sign such as:  -company -rtc"
    echo "   -server name         <optional>   ignore server column of the spreadsheet and uses this as global 'server' for all rows"
    echo "   -icomrc path         <optional>   a .icomrc file present in path, it will place this value into <icomrcpath> variable"
    echo "   -eemail email        <optional>   an external email outsude the company to be used in the icom external, fills the variable <extern_email>"
    exit 1
}

SERVER_PORT="-3"
SERVER_LOG=trace_debug.log
CLIENT_LOG=trace_debug.log

FILE_ORIG=""
FILE_ORIG_GET=""
MD5SUM_ORIG=""
FILE_TRANSFERRED=""
CSV_FILE=""
TEST_PLAN_NAME=""

TEST_ROOT_PATH=`pwd`                           ## inside TESTS directory
TEST_SERVER_PATH="$TEST_ROOT_PATH/server"
TEST_CLIENT_PATH="$TEST_ROOT_PATH/client"
RESULTS_ROOT=$TEST_ROOT_PATH/results
RESULTS_OUT_BASE_DIR=""
RESULTS_OUT_DIR=""
RESULTS_OUT_LAST_PUT_ROW=""
DEV_BINARIES_PATH=""          ## from the server and client directories
LAST_KEYWORD=""
FILE_KEYWORD=""
SECURE_KEYWORD=""
HOME_PUT_PATH="$HOME/trans_auto_put"
SSH_HOME_PUT_PATH="~/trans_auto_put"
SSH_HOME_GET_PATH="~/trans_auto_get"
HOME_GET_PATH="$HOME/trans_auto_get"
##parameters
ROW_STATUS=""
ROW_COMMENT=""
ROW_SSH=""
ROW_OPERATION=""
ROW_SERVER_NAME=""
ROW_CLIENT=""
ROW_CLIENT_BUILD=""
ROW_CMDLINE=""
ROW_EXTRA_TEST=""
ROW_MAIN_TEST=""
ROW_CMD_IS_WEB=0
EMAIL=""

LINE=""

FILE_SIZE=0

is_put=0
is_get=0
is_other=0
is_remote=0
is_client_dev=0
is_using_icomrc=0
is_to_start_server=0
row_number=0


SSH_HOSTS=""

GLOBAL_CLIENT=""
GLOBAL_CLIENT_BUILD=""
GLOBAL_OUTPUT_DIR=""
GLOBAL_COMPANY=""
GLOBAL_SERVER_NAME=""
GLOBAL_ICOMRC_PATH=""
GLOBAL_EXTERN_EMAIL=""

REALPATH=`which realpath | grep -v '/pkg/fs-foundation' 2>/dev/null`

fullpath()
{
  if [ "$REALPATH" != "" ]
  then
      RET=`realpath $1`
  else
     fullpath_file=`basename $1`
     fullpath_cwd=`pwd`
     fullpath_dir=`dirname $1`
     cd $fullpath_dir
     RET="`pwd`/$fullpath_file"
     cd $fullpath_cwd
  fi
  echo $RET
}




exe_path=`dirname $0`
exe_path=`fullpath $exe_path`

. ${exe_path}/../include/tests_funcions_for_auto_tests.bash


clear_client_files()
{
   debug "clear_client_files() pwd=`pwd` CLIENT_LOG=$CLIENT_LOG"
   if [ -f "$CLIENT_LOG" ]
   then
       /bin/mv $CLIENT_LOG $RESULTS_OUT_DIR/client_log.txt
   fi
   DIR=`dirname $CLIENT_LOG`
   /bin/rm -f $DIR/trans.log $DIR/send2maskshop_log
}

debug()
{
   echo $*
}


print_date()
{
    echo [`date +"%Y-%m-%d %H:%M:%S"`]
}

create_status_file()
{
   /bin/rm -f $RESULTS_OUT_BASE_DIR/000_status_*
   echo > "$RESULTS_OUT_BASE_DIR/000_status_${1}"
}

error()
{
   echo $*
   create_status_file "FAILED"
   exit 1
}

set_parameters_from_line()
{
   line=`echo $LINE | sed  's/; /;/' | sed  's/ ;/;/'`
   line=`echo $line | sed  's/, /,/' | sed  's/ ,/,/'`

   ROW_OPERATION=`/bin/echo -e $line | awk -F "[\t;,]" '{print $4}'`
   ROW_STATUS=`echo $line | awk -F "[\t;,]" '{print $1}'`



   if [ "$ROW_OPERATION" = "" -o "$ROW_STATUS" != "" ]
   then
 #      debug "set_parameters_from_line(): returning 1 [ROW_OPERATION=$ROW_OPERATION]  [$ROW_STATUS=$ROW_STATUS]"
       return 1
   fi

   ROW_CMD_IS_WEB=0

   ROW_OPERATION=`echo $ROW_OPERATION | tr "[a-z]" "[A-Z]"`

   ROW_SSH=`echo $line | awk -F "[\t;,]" '{print $3}'`
   ROW_SERVER_NAME=`echo $line | awk -F "[\t;,]" '{print $5}'`
   ROW_CLIENT=`echo $line | awk -F "[\t;,]" '{print $6}'`
   ROW_CLIENT_BUILD=`echo $line | awk -F "[\t;,]" '{print $7}'`
   ROW_CMDLINE=`echo $line | awk -F "[\t;,]" '{print $8}'`
   ROW_MAIN_TEST=`echo $line | awk -F "[\t;,]" '{print $9}'`
   ROW_EXTRA_TEST=`echo $line | awk -F "[\t;,]" '{print $10}'`

   echo
   echo "---------------------------------------------"
   debug "row_number=$row_number"
   debug "ROW_OPERATION=$ROW_OPERATION"
   debug "ROW_SSH=$ROW_SSH"
   debug "ROW_SERVER_NAME=$ROW_SERVER_NAME"
   debug "ROW_CLIENT=$ROW_CLIENT"
   debug "ROW_CLIENT_BUILD=$ROW_CLIENT_BUILD"
   debug "ROW_CMDLINE=$ROW_CMDLINE"
   debug "ROW_MAIN_TEST=$ROW_MAIN_TEST"
   debug "ROW_EXTRA_TEST=$ROW_EXTRA_TEST"

   if [ "$GLOBAL_CLIENT" != "" -a "$GLOBAL_CLIENT" != "plan" ]
   then
       ROW_CLIENT=$GLOBAL_CLIENT
       debug "using global client ROW_CLIENT=$ROW_CLIENT"
   fi

   if [ "$GLOBAL_SERVER_NAME" != "" -a "$GLOBAL_SERVER_NAME" != "plan" ]
   then
      ROW_SERVER_NAME=$GLOBAL_SERVER_NAME
      debug "using global server ROW_SERVER_NAME=$ROW_SERVER_NAME"
   fi

   if [ "$GLOBAL_CLIENT_BUILD" != "" -a "$GLOBAL_CLIENT_BUILD" != "plan" ]
   then
        ROW_CLIENT_BUILD=$GLOBAL_CLIENT_BUILD
        debug "using global client build ROW_CLIENT_BUILD=$ROW_CLIENT_BUILD"
   fi

   if [ "$ROW_OPERATION" = "Put"  -o "$ROW_OPERATION" = "put" -o "$ROW_OPERATION" = "PUT" ]
   then
         is_put=1
   else
         is_put=0   ## is get
   fi

   if [ "$ROW_OPERATION" = "Get"  -o "$ROW_OPERATION" = "get" -o "$ROW_OPERATION" = "GET" ]
   then
         is_get=1
   else
         is_get=0   ## is get
   fi

   if [ "$ROW_OPERATION" = "Other"  -o "$ROW_OPERATION" = "other" -o "$ROW_OPERATION" = "OTHER" ]
   then
         is_other=1
   else
         is_other=0   ## is get
   fi

   if [ "$ROW_SSH" != "" ]
   then
          is_remote=1
          if [ "`echo $SSH_HOSTS | grep $ROW_SSH | grep -v grep`" = "" ]
          then
               echo "validating password less for $ROW_SSH ..."
               #ssh -o PasswordAuthentication=no  -o BatchMode=yes "$ROW_SSH" "exit"
               if [ $? -ne 0 ]
               then
                    error  "first it is necessary to set password less for $ROW_SSH"
               else
                    SSH_HOSTS="$ROW_SSH  $SSH_HOSTS"
               fi
          fi
   else
          is_remote=0
   fi

   if [ "$ROW_CLIENT_BUILD" != "dev" ]
   then
      is_client_dev=0
   else
      is_client_dev=1
   fi

#    ICOMRC=`echo $ROW_ICOMRC | grep -i yes`
#    if [ "$ICOMRC" != "" ]
#    then
#       is_using_icomrc=1
#    else
#       is_using_icomrc=0
#    fi

   is_to_start_server=0
   if [ "$ROW_SERVER_NAME" != "" ]
   then
       server_prod=`echo $ROW_SERVER_NAME | tr "[a-z]" "[A-Z]"`
       if [ "$server_prod" != "PRODUCTION" ]
       then
            is_to_start_server=1
       else
            is_to_start_server=0
       fi
   fi
   return 0 ## OK
}


get_keywords() #$1=client output file
{
   status=1
   output=$1
   LAST_KEYWORD=""
   FILE_KEYWORD=""
   SECURE_KEYWORD=""
   if [ -e "$output" ]
   then
       LAST_KEYWORD=`/bin/grep 'Sending personal secure keyword' $output | tail -1 | awk '{print $5}'`
       SECURE_KEYWORD=$LAST_KEYWORD
       FILE_KEYWORD=`/bin/grep 'File keyword is' $output | awk '{print $NF}'`
       status=0
   fi
   UPPER=""
   LOWER=""
   NUMBER=""
   if [ "$LAST_KEYWORD" = "" -a "$FILE_KEYWORD" = "" ]
   then
      status=1
   else
      LOWER=`echo  $FILE_KEYWORD | egrep "[a-z]"`
      NUMBER=`echo $FILE_KEYWORD | egrep "[0-9]"`
   fi
   if [ "$SECURE_KEYWORD" != "" ]
   then
       UPPER=`echo $SECURE_KEYWORD | egrep "[A-Z]"`
   fi
   ## check keyword format
   if [ $status -eq 0 ]
   then
       if [ "$LOWER" = "" -o "$NUMBER" = "" ]
       then
          echo  "bad file keyword [$FILE_KEYWORD] LOWER=[$LOWER]  NUMBER=[$NUMBER]"
          status=1
       fi
       if [ "$SECURE_KEYWORD" != "" -a "$UPPER" = "" ]
       then
          echo  "bad secure keyword [$SECURE_KEYWORD]"
          status=1
       fi
       if [ $status -eq 1 ]
       then
            echo "ERROR: keyword bad format"
       fi
   fi
   return $status
}


set_output_dir()
{
    row_operation=$1
    row_number=$2
    outdir=""
    if [ "$GLOBAL_OUTPUT_DIR" = "" ]
    then
        outdir=`printf "%s/%s/%03d_%s" $RESULTS_ROOT $TEST_PLAN_NAME $row_number $row_operation`
    else
        outdir=`printf "%s/%s/%03d_%s" $RESULTS_ROOT $GLOBAL_OUTPUT_DIR $row_number $row_operation`
    fi
    echo $outdir
}


copy_original_file()
{
   CMD=""
   if [ $is_remote -eq 0 ]
   then
        FILE_TRANSFERRED="$FILE_ORIG"
#        uncomment the code below to copy origem to home
#        it is not necessary
#        FILE_TRANSFERRED="$HOME_PUT_PATH/$FILE_ORIG_GET"
#        if [ ! -d "$SSH_HOME_PUT_PATH" -o ! -e "$FILE_TRANSFERRED" ]
#        then
#           CMD="/bin/mkdir -p $HOME_PUT_PATH"
#           debug $CMD
#           eval $CMD
#           CMD="/bin/cp $FILE_ORIG $HOME_PUT_PATH"
#           debug $CMD
#           eval $CMD
#        fi
   else
       FILE_TRANSFERRED="$SSH_HOME_PUT_PATH/$FILE_ORIG_GET"
       dir_exists="/bin/ls $FILE_TRANSFERRED"
       create_dir="/bin/mkdir -p $SSH_HOME_PUT_PATH"
       CMD="ssh ${ROW_SSH} \"$dir_exists || $create_dir\""
       debug "copy_original_file(): cmd=$CMD"
       exists_remote=`eval $CMD`
       if [ "$exists_remote" = "" ]
       then
          CMD="scp  -r ${FILE_ORIG} ${ROW_SSH}:$SSH_HOME_PUT_PATH"
       else
          CMD=""
       fi
   fi
   if [ "$CMD" != "" ]
   then
      debug "copy_original_file(): cmd=$CMD"
      eval $CMD

      if [ $? -ne 0 ]
      then
          error "Error on copying source file"
      fi
   fi
}


kill_servers()
{
   if [ "$1" != "" ]
   then
      cur_server=`basename $1`
      ALL_SERVERS="$cur_server $ALL_SERVERS"
   fi
   if [ "$ALL_SERVERS" != "" ]
   then
      killall  $ALL_SERVERS 2> /dev/null
   fi
}


start_server()
{
   rm -f ${DEV_BINARIES_PATH}/trans.log ${DEV_BINARIES_PATH}/icom.log
   last_server_version=`ls -rt ${DEV_BINARIES_PATH}/${ROW_SERVER_NAME}* | tail -1`
   if [ $? -ne 0 -o "$last_server_version" = "" ]
   then
       error "Server ${ROW_SERVER_NAME} not found "
   fi
   kill_servers $last_server_version
   change_dir_server
   /bin/rm -f $SERVER_LOG
   debug "start_server(): `print_date` $last_server_version $SERVER_PORT"
   echo "`print_date` $last_server_version $SERVER_PORT" > $RESULTS_OUT_DIR/server_cmd.txt
   $last_server_version $SERVER_PORT
   sleep 3
   server_proc=`ps -ef | grep $last_server_version | grep -v grep`
   echo "`print_date` $server_proc"
}


stop_server()
{
    cur_dir=`pwd`
    change_dir_server
    debug "stop_server(): `print_date` intances=$intances"
    intances=`/bin/ps | grep ${ROW_SERVER_NAME} | grep -v grep | wc -l`
    counter=0
    WAIT_SERVER=7
    while [ $intances -gt 1 -a $counter -lt 10 ]
    do
           echo "Waiting sever to stop ($WAIT_SERVER seconds) (counter $counter)  ..."
           sleep $WAIT_SERVER
           intances=`/bin/ps | grep ${ROW_SERVER_NAME} | grep -v grep | wc -l`
           counter=$((counter+1))
    done
    kill_servers
    if [ -f "$SERVER_LOG" ]
    then
         TMP=/tmp/ret_$$.txt
         grep -i "insert " $SERVER_LOG     > $TMP
         grep -i "update " $SERVER_LOG     >> $TMP
         grep -i "select " $SERVER_LOG     >> $TMP
         sort $TMP > $RESULTS_OUT_DIR/queries_log.txt

         grep -i manager $SERVER_LOG    > $TMP
         grep request.c  $SERVER_LOG |  grep "gu."   >> $TMP
         grep customer_type $SERVER_LOG   >> $TMP
         grep ldapfsl_ $SERVER_LOG   >> $TMP
         sort $TMP > $RESULTS_OUT_DIR/small_server_log.txt

         /bin/rm -f $TMP
         /bin/mv $SERVER_LOG $RESULTS_OUT_DIR/server_log.txt
    fi
    cd $cur_dir
}


client_cmd()
{
   rm -f ${DEV_BINARIES_PATH}/trans.log ${DEV_BINARIES_PATH}/icom.log
   cmd=""
   ROW_CMD_IS_WEB=0
   if [ $is_remote -eq 1 ]
   then
      cmd="ssh ${ROW_SSH} \""
      if [ $is_put -eq 0 ]
      then
         cmd="$cmd /bin/mkdir -p $SSH_HOME_GET_PATH && cd $SSH_HOME_GET_PATH && /bin/rm -fr $FILE_ORIG_GET && "
      else
         cmd="$cmd cd $SSH_HOME_PUT_PATH && "
      fi
   else
       if [ $is_put -eq 0 ]
       then
            cmd="$cmd /bin/rm -fr $FILE_ORIG_GET && "
       else
            DIR_ORIG=`dirname $FILE_ORIG`
            if [ "$DIR_ORIG" != "" -a "$DIR_ORIG" != "." ]
            then
                cmd="$cmd cd $DIR_ORIG && "
            fi
       fi
   fi
   for str in $ROW_CMDLINE
   do
      case $str in
        "<extern_email>")   cmd="$cmd $GLOBAL_EXTERN_EMAIL"
                           ;;
        "<icomrcpath>")    cmd="$cmd $GLOBAL_ICOMRC_PATH"
                           ;;
        "<company>")       cmd="$cmd $GLOBAL_COMPANY"
                           ;;
        "<file_size>")     cmd="$cmd $FILE_SIZE"
                           ;;
        "<server_path>*")  word=`echo $str | tr -d "<server_path>"`
                           cmd="$cmd $word"
                           ;;
        "<client>")  if [ $is_client_dev -eq 0 ]
                     then
                        cmd="$cmd $ROW_CLIENT"
                     else
                        client=`ls -rt ${DEV_BINARIES_PATH}/${ROW_CLIENT}* | tail -1`
                        if [ "$client" = "" ]
                        then
                           error "Could not find the development client in ${DEV_BINARIES_PATH}/${ROW_CLIENT}"
                        fi
                        cmd="$cmd $client"
                     fi
                     ;;
        "<file>")    cmd="$cmd $FILE_ORIG_GET";;
        "<email>")   cmd="$cmd $EMAIL";;
        "<file_keyword>") cmd="$cmd $FILE_KEYWORD";;
        "<keyword>")
                     if [ "$LAST_KEYWORD" != "" ]
                     then
                         cmd="$cmd $LAST_KEYWORD"
                     else
                         cmd="$cmd $FILE_KEYWORD"
                     fi
                     ;;
        "-web")      ROW_CMD_IS_WEB=1
                     cmd="$cmd $str";;

           *)        cmd="$cmd $str";;
      esac
   done
   if [ $is_remote -eq 1 ]
   then
      cmd="$cmd\""
   fi
   echo "$cmd" > $RESULTS_OUT_DIR/client_cmd.txt
   echo "$cmd"
   return $ROW_CMD_IS_WEB ## for some reason the setting on -web case does not work, that is why we return a value here
}


run_row_cmd()
{
   start_date=0
   end_date=0
   cmd="$*"
   status=0
   /bin/rm -f $CLIENT_LOG
   debug "run_row_cmd(): `print_date` pwd=`pwd` is_web=$ROW_CMD_IS_WEB \$CLIENT_LOG=$CLIENT_LOG"
  
    debug "run_row_cmd(): eval $cmd |& tee  $RESULTS_OUT_DIR/client_output.txt"
    start_date=`date +"%s"`
    eval $cmd |& tee  $RESULTS_OUT_DIR/client_output.txt
    status=${PIPESTATUS[0]}
    # debug "\${PIPESTATUS[0]}=${PIPESTATUS[0]} \${PIPESTATUS[1]}=${PIPESTATUS[0]}"
    end_date=`date +"%s"`
   
   /bin/rm -f trans.log
   clear_client_files
   grep -i "Transfer Method:"  $RESULTS_OUT_DIR/client_output.txt > $RESULTS_OUT_DIR/transfertype_out.txt 2>/dev/null
   
   if [ $is_to_start_server -eq 1 ]
   then
      stop_server
   fi
   testRow $status
   ret=$?
  
   if [ $is_put -eq 1 -o $is_get -eq 1 ]
   then
      transfer_method=`cat $RESULTS_OUT_DIR/transfertype_sql.txt`
      transfertime=`expr $end_date - $start_date`    
      echo "$transfer_method, in $transfertime seconds, for $FILE_KEYWORD" > $RESULTS_OUT_DIR/transfertime_seconds.txt
   fi
   
   return $ret
}

put_file()
{
   cmd=`client_cmd`
   ROW_CMD_IS_WEB=$?
   DIR_ORIG=`dirname $FILE_ORIG`      
   ## as client_cmd is called between `` for same reason setting global variables inside does not work
   if [ "$DIR_ORIG" != "" -a "$DIR_ORIG" != "." ]
   then
       CLIENT_LOG="$DIR_ORIG/trace_debug.log"
   fi
   debug "put_file(): CLIENT_LOG=$CLIENT_LOG cmd=$cmd"
   run_row_cmd $cmd
   if [ $? -ne 0 ]
   then
      msg=`printf "PUT failed on row %03d, exiting ...\n" $row_number`
      error $msg
   fi
   echo $LAST_KEYWORD > $RESULTS_OUT_DIR/last_keyword.txt
   echo $FILE_KEYWORD > $RESULTS_OUT_DIR/file_keyword.txt
   printf "PUT on row %03d OK\n" $row_number
}


run_client_as_get()
{
   /bin/rm -rf $FILE_ORIG_GET
   if [ "$LAST_KEYWORD" = "" -a "$FILE_KEYWORD" = "" ]
   then
      previus_row=$((row_number-1))
      while [ "$LAST_KEYWORD" = "" -a "$FILE_KEYWORD" = "" -a $previus_row -gt 0 ]
      do
          RESULTS_OUT_LAST_PUT_ROW=`set_output_dir PUT $previus_row`
          if [ -e "$RESULTS_OUT_LAST_PUT_ROW/last_keyword.txt" ]
          then
             LAST_KEYWORD=`cat $RESULTS_OUT_LAST_PUT_ROW/last_keyword.txt`
          fi
          if [ -e "$RESULTS_OUT_LAST_PUT_ROW/file_keyword.txt" ]
          then
             FILE_KEYWORD=`cat $RESULTS_OUT_LAST_PUT_ROW/file_keyword.txt`
          fi
          previus_row=$((previus_row-1))
      done
   fi
   if [ "$LAST_KEYWORD" = "" -a "$FILE_KEYWORD" = "" ]
   then
      error "Could not get last keyword ..."
   fi
   cmd=`client_cmd`   
   ROW_CMD_IS_WEB=$?
   CLIENT_LOG=trace_debug.log
   debug "run_client_as_get(): cmd=$cmd"
   run_row_cmd $cmd
   if [ $? -ne 0 ]
   then
        msg=`printf "client failed on row %03d, exiting ...\n" $row_number`
        error $msg
   fi
   printf "Operation on row %03d OK\n" $row_number
}



change_dir_client()
{
  cd $TEST_CLIENT_PATH
  if [ $? -ne 0 ]
  then
     error "could change directory to client"
  fi
}


change_dir_server()
{
  cd $TEST_SERVER_PATH
  if [ $? -ne 0 ]
  then
     error "could change directory to server"
  fi
}


perform_test_row()
{
    RESULTS_OUT_DIR=`set_output_dir $ROW_OPERATION $row_number`
    RESULTS_OUT_BASE_DIR=`dirname $RESULTS_OUT_DIR`
    debug "/bin/rm -rf $RESULTS_OUT_DIR"
    /bin/rm -rf $RESULTS_OUT_DIR
    debug "/bin/mkdir -p $RESULTS_OUT_DIR"
    /bin/mkdir -p $RESULTS_OUT_DIR
    if [ $? -ne 0 ]
    then
        error "could not create output directory  $RESULTS_OUT_DIR"
    fi
    cd $TEST_ROOT_PATH
    if [ $is_to_start_server -eq 1 ]
    then
      start_server
    fi

    echo "$LINE" > $RESULTS_OUT_DIR/csv_row.txt
    change_dir_client
    if [ $is_put -eq 1 ]
    then
         copy_original_file
         put_file
    else
         run_client_as_get
    fi
    debug "perform_test_row() finished..."
}


start_test()
{
    row_number=0
    test_done=0
    while IFS="" read -r line || [[ -n "$line" ]]; do
      lines[$row_number]=$line
      row_number=`expr $row_number + 1`
    done < "$CSV_FILE"

    lines_counter=$row_number
    counter_local=0
    while [ $counter_local -lt $lines_counter ]
    do
       LINE=${lines[$counter_local]}
       counter_local=$((counter_local+1))
       row_number=$counter_local
       set_parameters_from_line
       if [ $? -eq 0 ]
       then
           test_done=1
           perform_test_row
       fi
    done
    if [ $test_done -eq 1 ]
    then
       create_status_file "PASSED"
    fi
}




while [ "$1" != "" ]
do
    case $1 in
    -icomrc)    shift
               GLOBAL_ICOMRC_PATH=$1
               ;;
    -eemail)   shift
               GLOBAL_EXTERN_EMAIL=$1
               ;;
    -server)   shift
               GLOBAL_SERVER_NAME=$1
               ;;
    -company)  shift
               GLOBAL_COMPANY=$1
               ;;
    -d)        shift
               GLOBAL_OUTPUT_DIR=$1
               ;;
 -clientbuild) shift
                GLOBAL_CLIENT_BUILD=$1
                 ;;
   -client)  shift
             GLOBAL_CLIENT=$1
             ;;
     -c)  shift
          CSV_FILE=$1
          TEST_PLAN=`basename $CSV_FILE`
          TEST_PLAN_NAME=`basename $TEST_PLAN .csv`
          TEST_PLAN_NAME=`basename $TEST_PLAN_NAME .CSV`
          ;;
     -r)  shift
          DEV_BINARIES_PATH=`fullpath $1`
          if [ ! -d "$DEV_BINARIES_PATH" ]
          then
               echo "release-dir does not exist or it is not a directory"
               help
          fi
          ;;
     -e)  shift
          EMAIL=$1
          ;;
     -f)  shift
          FILE_ORIG=`fullpath $1`
          if [ ! -e "$FILE_ORIG" ]
          then
               echo "file for PUT does not exist"
               help
          fi
          FILE_SIZE=`du -b "$FILE_ORIG" | tail -1 | awk '{print $1}'`
          FILE_ORIG_GET=`basename $FILE_ORIG`
          MD5SUM_ORIG=`md5sum $FILE_ORIG | awk '{print $1}'`
          ;;
     -p)  shift
          SERVER_PORT="-$1"
          ;;
    esac
    shift
done





if [ "$CSV_FILE" = "" -o "$DEV_BINARIES_PATH" = ""  -o "$FILE_ORIG" = "" ]
then
    help
fi

start_test

