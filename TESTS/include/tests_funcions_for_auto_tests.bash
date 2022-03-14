
COMMON_TESTS="\
RET_CODE_EQUAL
RET_CODE_ZERO
RET_CODE_NOT_ZERO 
COMPARE_FILES 
COMPARE_STRINGS 
FILE_EXISTS 
FILE_DOES_NOT_EXIST
"

OPERATION_TESTS="\
TEST_PUT_FILE
TEST_GET_FILE
TEST_PUT_DIR
TEST_GET_DIR
KEYWORD_EXISTS_IN_SERVER
KEYWORD_DONT_EXIST_IN_SERVER
TRANSFER_IS_SERIAL
TRANSFER_IS_PARALLEL
"


MYSQL_USER="rxfh80"
MYSQL_PASSWORD="DR2dr33m"

keyword_exists_in_server()
{
   ## defaul is to pass due to there is no way to check that in production
   status=0
   if [ $is_to_start_server -eq 1 ]
   then
      if [ "$FILE_KEYWORD" = "" ]
      then
          status=1
      else
          x=`ls  -l ${TEST_SERVER_PATH}/global_data/${FILE_KEYWORD} ${TEST_SERVER_PATH}/data_files?/${FILE_KEYWORD}*`
          status=$?
      fi
   fi
   return $status
}


keyword_dont_exist_in_server()
{
    ## defaul is to pass due to there is no way to check that in production
    status=0
    if [ $is_to_start_server -eq 1 ]
    then
       if [ "$FILE_KEYWORD" = "" ]
      then
          status=1
      else
#          debug "keyword_dont_exist_in_server(): ${TEST_SERVER_PATH}/global_data/${FILE_KEYWORD}"
          x=`ls  -l ${TEST_SERVER_PATH}/global_data/${FILE_KEYWORD} `
          global_status=$?
#          debug "keyword_dont_exist_in_server(): global_status=$global_status"
#          debug "keyword_dont_exist_in_server(): ${TEST_SERVER_PATH}/data_files?/${FILE_KEYWORD}"
          x=`ls  -l ${TEST_SERVER_PATH}/data_files?/${FILE_KEYWORD}* `
          data_status=$?
#          debug "keyword_dont_exist_in_server(): data_status=$data_status"
          if [ $global_status -eq 0 -o $data_status -eq 0 ]
          then
              status=1
          fi
      fi
    fi
    return $status
}

check_md5sum()
{
   echo "checking md5sum .. "
   GET_MD5SUM=""
   if [ $is_remote -eq 0 ]
   then
      GET_MD5SUM=`md5sum $FILE_ORIG_GET  | awk '{print $1}'`
   else
      cmd="ssh ${ROW_SSH} \"cd $SSH_HOME_GET_PATH && md5sum $FILE_ORIG_GET | awk '{print \\\$1}'\""
      debug "check_md5sum(): cmd=$cmd"
      GET_MD5SUM=`eval $cmd`
      if [ $? -ne 0 ]
      then
          error "ERROR on md5sum commmand"
      fi
   fi
   if [ "$MD5SUM_ORIG" != "$GET_MD5SUM" ]
   then
      error "GET failed: md5sum does not match original=$MD5SUM_ORIG  from get=$GET_MD5SUM  exiting"
   fi
   printf "GET on row %02d OK md5sum $GET_MD5SUM matches\n" $row_number
   return 0
}


check_transfer_serial()
{
     status=1
     out=ok
     debug "check_transfer_serial(): \$ROW_CMD_IS_WEB=$ROW_CMD_IS_WEB"
     if [ $ROW_CMD_IS_WEB -eq 0 ] ## when web is used there is no client output
     then
          out=`grep -i serial   $RESULTS_OUT_DIR/transfertype_out.txt`
     fi
     sql=`grep -i serial   $RESULTS_OUT_DIR/transfertype_sql.txt`
     if [ "$out" != "" -a "$sql" != "" ]
     then
         status=0
     fi
     return $status
}


check_transfer_parallel()
{
     status=1
     out=ok
     debug "check_transfer_parallel(): \$ROW_CMD_IS_WEB=$ROW_CMD_IS_WEB"
     if [ $ROW_CMD_IS_WEB -eq 0 ] ## when web is used there is no client output
     then
          out=`grep -i parallel   $RESULTS_OUT_DIR/transfertype_out.txt`
     fi
     sql=`grep -i parallel   $RESULTS_OUT_DIR/transfertype_sql.txt`
     if [ "$out" != "" -a "$sql" != "" ]
     then
         status=0
     fi
     return $status
}

create_and_run_sql_tests()
{
    status=1
    keyword=""
    if [ "$FILE_KEYWORD" != "" ]
    then
        keyword=`echo $FILE_KEYWORD | tr -d \'`
        FILE_KEYWORD=$keyword
    fi
    if [ "$keyword" != "" -a "$LAST_KEYWORD" != "" ]
    then
        keyword=`echo $LAST_KEYWORD | tr -d \'`
        LAST_KEYWORD=$keyword
    fi
    if [ "$keyword" != "" ]
    then
       if [ "$FILE_KEYWORD" = "" ]
       then
           FILE_KEYWORD=$keyword
       fi
       echo "perform database tests ...";

       Q1="from data_transfer.dt_keyword_mapping where upload_date  >= current_date() and (file_keyword  = '$FILE_KEYWORD'   or  secure_keyword = '$keyword')  \G"

       Q2="from data_transfer.dt_transactions where  startdate  >= current_date() and (file_keyword  = '$FILE_KEYWORD'   or  secure_keyword = '$keyword')  \G"
       Q3="from data_transfer.dt_transfertimes where starttime  >= current_date() and file_keyword  = '$FILE_KEYWORD'  \G"

       echo 'select * ' $Q1 >  $RESULTS_OUT_DIR/mysql_dt_keyword_mapping.sql
       echo 'select * ' $Q1 >  $RESULTS_OUT_DIR/mysql_dt_keyword_mapping.log
       echo                >>  $RESULTS_OUT_DIR/mysql_dt_keyword_mapping.log

       echo 'select * ' $Q2 >  $RESULTS_OUT_DIR/mysql_dt_transactions.sql
       echo 'select * ' $Q2 >  $RESULTS_OUT_DIR/mysql_dt_transactions.log
       echo                >>  $RESULTS_OUT_DIR/mysql_dt_transactions.log

       echo 'select * ' $Q3 >  $RESULTS_OUT_DIR/mysql_dt_transfertimes.sql
       echo 'select * ' $Q3 >  $RESULTS_OUT_DIR/mysql_dt_transfertimes.log
       echo                >>  $RESULTS_OUT_DIR/mysql_dt_transfertimes.log

       echo "waiting to perform MySQL queries .."
       sleep 3
       echo 'select * ' $Q1
       echo 'select * ' $Q1 | mysql -h dtmsdb.nxp.com -u $MYSQL_USER -p${MYSQL_PASSWORD} | tee -a  $RESULTS_OUT_DIR/mysql_dt_keyword_mapping.log

       echo 'select * ' $Q2
       echo 'select * ' $Q2 | mysql -h dtmsdb.nxp.com -u $MYSQL_USER -p${MYSQL_PASSWORD} | tee -a  $RESULTS_OUT_DIR/mysql_dt_transactions.log
      
       echo 'select * ' $Q3
       echo 'select * ' $Q3 | mysql -h dtmsdb.nxp.com -u $MYSQL_USER -p${MYSQL_PASSWORD} | tee -a  $RESULTS_OUT_DIR/mysql_dt_transfertimes.log

       check_transfer=`/bin/grep -i transfertype $RESULTS_OUT_DIR/mysql_dt_transfertimes.log | /bin/grep -i $ROW_OPERATION | tail -1`
       if [ "$check_transfer" = "" ]
       then
           echo "ERROR: Please check database queries for keyword $keyword"
           error "test failed, exiting ..."
       fi
       echo "Database looks ok for keyword $keyword"
       echo $check_transfer > $RESULTS_OUT_DIR/transfertype_sql.txt
       status=0
    else
       echo "Keyword not found, cannot perform database tests ..."
    fi
    return $status
}



private_test_put()
{
   status=1
   ret_code=$1
   if [ $ret_code -eq 0 ]
   then
      get_keywords $RESULTS_OUT_DIR/client_output.txt
      if [ $? -eq 0 ]
      then
          create_and_run_sql_tests
          status=$?
      fi
   fi
   return $status
}


private_test_get_file()
{
   status=1
   ret_code=$1
   if [ $ret_code -eq 0 ]
   then
      check_md5sum
      if [ $? -eq 0 ]
      then
          create_and_run_sql_tests
          status=$?
      fi
   fi
   return $status
}


private_do_test() # return:  0=OK   1=error, #$0 = $?  $1 = TEST_TYPE
{
   status=1
   ret_code=$1
   shift
   test_type="$*"
   case "$test_type" in
       RET_CODE_ZERO)        if [ $ret_code -eq 0 ]
                             then
                               status=0
                             fi
                             ;;
       RET_CODE_NOT_ZERO)    if [ $ret_code -ne 0 ]
                             then
                               status=0
                             fi
                             ;;
       RET_CODE_EQUAL*)      value=`echo  $test_type | awk '{print $2}'`
                             if [ $ret_code -eq $value ]
                             then
                               status=0
                             fi
                             ;;
      COMPARE_STRINGS*)      if [ "$2" != "" -a "$3" != "" ]
                             then
                                   if [ "$2" = "$3" ]
                                   then
                                        status=0
                                   fi
                             fi
                             ;;
      COMPARE_FILES*)        if [ "$2" != "" -a "$3" != "" ]
                             then
                                   diff "$2" "$3"
                                   if [ $? -eq 0 ]
                                   then
                                        status=0
                                   fi
                             fi
                             ;;
       TEST_PUT_FILE)        private_test_put $ret_code
                             status=$?
                             ;;
                             
       TEST_GET_FILE)        private_test_get_file $ret_code
                             status=$?
                             ;;
                             
  KEYWORD_EXISTS_IN_SERVER)  keyword_exists_in_server
                             status=$?
                             ;;
                             
  KEYWORD_DONT_EXIST_IN_SERVER)                              
                             keyword_dont_exist_in_server
                             status=$?
                             ;;
                             
   TRANSFER_IS_SERIAL)       check_transfer_serial
                             status=$?
                             ;;
                             
   TRANSFER_IS_PARALLEL)     check_transfer_parallel
                             status=$?
                             ;;
                             
      *)
                             ;;
   esac
   return $status
}

## doTest(),  return 0=OK   1=error
doTest()  #$0 = $?  $1 = $ROW_MAIN_TEST
{
   ret_code=$1
   shift
   test_type="$*"
   debug "doTest(): pwd=`pwd`"
   if [ "$test_type" = "" ]
   then
       if [ $is_put -eq 1 ]
       then
          test_type="TEST_PUT_FILE"          
       else
          if [ $is_get -eq 1 ]
          then
             test_type="TEST_GET_FILE"
          fi
       fi
   fi
   debug "doTest(): ret_code  [ $ret_code ]"
   debug "doTest(): test_type [ $test_type ]"
   if [ "$test_type" = "" ]
   then
      echo 
      error  "*ERROR **,  test type not specified"
   fi
   private_do_test $ret_code $test_type
   return $?
}


testRow()  # return 0=OK   1=error,  #$0 = $?
{
  status=1
  ROW_RET_CODE=$1
  doTest $ROW_RET_CODE $ROW_MAIN_TEST
  status=$?
#  debug "testRow(): status=$status ROW_EXTRA_TEST=$ROW_EXTRA_TEST"
  if [ $status -eq 0 -a "$ROW_EXTRA_TEST" != "" ]
  then
     doTest $ROW_RET_CODE $ROW_EXTRA_TEST
     status=$?
  fi
  return $status
}
