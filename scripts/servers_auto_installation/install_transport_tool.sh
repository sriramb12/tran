#!/bin/sh

PROC_NAME="ssl.transcend|ssl.tfptd|ssl.icomd|free.icomd"
do_kill=1

#                          site       link-name       final-file-name         where-to-install     owner-group     argv     remove-shm
#                          ---------  -----------     ------------------      -----------------    -------------   -----    ----------
# testing purposes  [-n ]  phx        ssl.transcend   ssl.transcend_rev_10     /opt/transcend/NXP   db_admin.group  3         0/1


## example:  
#            [client] just sends the binaries to the server putting information in the message
#                     ------------------------------------------------------------------------
#                echo "TRANSPORT_TOOL_RELEASE: phx ssl.transcend ssl.transcend_rev_10 /opt/transcend/TESTS  db_admin.group  3  1"  | trans -web db_admin -p -adam ssl.transcend db_admin@nxp.com
#                echo "TRANSPORT_TOOL_RELEASE: phx ssl.icomd     ssl.icomd_rev_10     /opt/transcend/TESTS  db_admin.group  4  0"  | trans -web db_admin -p -adam ssl.icomd     db_admin@nxp.com
#
#            [server]  on the contrab a call like this will try to install
#                     ----------------------------------------------------
#                sh -vx install_transport_tool.sh  adam  /opt/transcend/NXP/RELEASE_DIR/trans.localhost    ssl.transcend & 
#                sh -vx install_transport_tool.sh  adam  /opt/transcend/NXP/RELEASE_DIR/icom.int.localhost ssl.icomd     &

if [ "$1" = "-n" ]
then
  do_kill=0
  shift
fi

SITE=$1
TRANS_CMD="$2 -web db_admin"
BINARY=$3

BINARY_REV=""
SERVER_PATH=""
OWNER_AND_GROUP=""
ARGUMENT=""
REMOVE_SHM=1  # optional remove shm 
RELEASE_SHM=0

KEYWORD=""
MSG_INSTALL=""
DAMEON_PID=0
RUNNING_PROCESS=0
SHM_CONTROL_FILE=/tmp/TRANSPORT_TOOL_RELEASE.txt

START_TIME_IN_SECONDS=`date +"%s"`
HOURS_TO_TRY=14
TIME_TO_TRY_IN_SECONDS=`expr $HOURS_TO_TRY \* 3600`
END_TIME_IN_SECONDS=`expr $START_TIME_IN_SECONDS + $TIME_TO_TRY_IN_SECONDS`
WAIT_TIME=300


## Test settings
#---------------
WAIT_TIME=5
#END_TIME_IN_SECONDS=`expr $START_TIME_IN_SECONDS + 30`

LSOF=/usr/sbin/lsof
if [ ! -x "$LSOF" ]
then
   LSOF=/usr/bin/lsof
fi

if [ ! -x "$LSOF" ]
then
   LSOF=/bin/lsof
fi


debug()
{
   echo [`date +"%Y-%m-%d %H:%M:%S"`] $*
}


get_db_admin_keywords()
{
   KEYWORDS=`$TRANS_CMD -q -${SITE} | grep "^+" | awk '{print $2}'`
   for KEY in $KEYWORDS
   do
       TMP=/tmp/$KEY.txt 
       rm -f $TMP
       $TRANS_CMD -i $KEY > $TMP
       if [ $? -ne 0 ]
       then
            return 0
       fi
       file_matches=`grep $BINARY $TMP | grep -i File: | awk '{print $2}'`
       is_to_install=`grep TRANSPORT_TOOL_RELEASE: $TMP | grep -i Description: | cut -f3 -d:` 
       rm -f $TMP
       if [ "$file_matches" != ""  -a  "$is_to_install" != "" ]       
       then
          MSG_INSTALL="TRANSPORT_TOOL_RELEASE: $is_to_install"
          debug $MSG_INSTALL
          
          msg_site=`echo $is_to_install |  awk '{print $1}'`
          if [ "$SITE" = "$msg_site" ]
          then
              ## awk '{print $3} is $BINARY name wich is already in place
              BINARY_REV=`echo $is_to_install |  awk '{print $3}'`
              SERVER_PATH=`echo $is_to_install |  awk '{print $4}'`
              OWNER_AND_GROUP=`echo $is_to_install |  awk '{print $5}'`
              ARGUMENT=`echo $is_to_install |  awk '{print $6}'`
              REMOVE_SHM=`echo $is_to_install |  awk '{print $7}'`
              KEYWORD=$KEY
              return 0
          fi
       fi
   done
   return 1
}



save_shm_control_file()
{  
    rm -f $SHM_CONTROL_FILE
    ## the long sleep below get sync among more then once install process
    sleep 20 
    echo $$ >> $SHM_CONTROL_FILE
    sleep 5
}


release_shm_control_file()
{
   if [ -s "$SHM_CONTROL_FILE" -a $RELEASE_SHM -eq 0 ]
   then
       tmp_save_control_file=${SHM_CONTROL_FILE}_${$}
       cp $SHM_CONTROL_FILE ${SHM_CONTROL_FILE}_${$}
       grep -v $$ $tmp_save_control_file > $SHM_CONTROL_FILE
       RELEASE_SHM=1
   fi
}


is_shm_control_file_busy()
{
    other=`grep -v $$ $SHM_CONTROL_FILE`
    if [ "$other" != "" ]
    then
       return 1 # busy, there is another install process
    fi
    return 0    
}


check_binary_to_install()
{
        if [ "$BINARY" = "" -o "$BINARY_REV" = "" -o "$SERVER_PATH" = "" -o "$ARGUMENT" = "" ]
        then
            debug "$0: Missing parameters: "
            echo "  \$BINARY=$BINARY"
            echo "  \$BINARY_REV=$BINARY_REV"
            echo "  \$SERVER_PATH=$SERVER_PATH"
            return 1
        fi

        FULL_BINARY_REV="$SERVER_PATH/$BINARY_REV"
        if [ ! -x "$FULL_BINARY_REV" ]
        then
            debug "$0: binary file $FULL_BINARY_REV not found"
            return 1
        fi
}


kill_daemon()
{    
    if [ $DAMEON_PID -ne 0 ]
    then
        debug "killing the daemon if \$DAMEON_PID=$DAMEON_PID exists"
        if [ $do_kill -eq 1 ]
        then
            kill $DAMEON_PID
            if [ $? -ne 0 ]
            then
                echo "$0: could not kill the process $DAMEON_PID"
                return 1
            fi
            DAMEON_PID=0
        fi
    fi
    return 0
}


kill_not_working_processes()
{
    RUNNING_PROCESS=0
    DAMEON_PID=0
    procs=`ps aux | grep -v grep | egrep  "$BINARY" | awk '{print $2}'`
    # for each PID in PIDs array
    for pid in $procs
    do
        ppid=`ps -o ppid $pid | tail -1 | tr -d ' '`
        PROC_NAME=`cat /proc/$pid/cmdline 2> /dev/null| egrep /${BINARY}`
        debug "\$PROC_NAME=$PROC_NAME PS: `ps -f -p $pid | tail -1`"
        if [ "$PROC_NAME"  = "" ]
        then
            continue
        fi
        if [ "$ppid" != "1" ]
        then
            RUNNING_PROCESS=`expr $RUNNING_PROCESS + 1`
            #  if the server does not have a TCP connection it is because it has been cancelled
            ACTIVE=`$LSOF -p $pid | egrep ' IPv[46] ' | grep ESTABLISHED | grep -i -v Mysql | grep -v grep`
            if [ "$ACTIVE" = "" ]
            then  
                ps -f -p $pid | tail -1
                if [ $do_kill -eq 1 ]
                then
                    kill $pid
                    if [ $? -eq 0 ]
                    then
                         RUNNING_PROCESS=`expr $RUNNING_PROCESS - 1`
                    fi
                else
                    debug "[test] kill $pid"
                fi
            fi
        else
            DAMEON_PID=$pid
        fi
    done
}

#-----------------------------------------------
## returns  0=OK new daemon is not Running 
#           1=still running 
#-----------------------------------------------
wait_until_no_more_active_transfers()
{
     now=`date +"%s"` 
     ok=1
     debug "waiting transfers to finish (if there is at least one) ..."
     while [ $now -lt $END_TIME_IN_SECONDS ]
     do
         debug "\$now=$now trying up to \$END_TIME_IN_SECONDS=$END_TIME_IN_SECONDS"
         ok=1
         kill_not_working_processes
         debug "\$RUNNING_PROCESS=$RUNNING_PROCESS \$DAMEON_PID=$DAMEON_PID"
         ## kill the damon first and let others to finish
         kill_daemon
         if [ $? -ne 0 ]
         then
            return 1
         fi
         ## when there is no processes,  sleep some seconds and check it again
         if [ $RUNNING_PROCESS -eq 0 ]
         then
            sleep 3
            kill_not_working_processes
         fi
         if [ $RUNNING_PROCESS -eq 0 ]
         then
             ok=0             
             break
         fi
         sleep $WAIT_TIME  ## wait  minutes
         now=`date +"%s"` 
     done
     debug "\$RUNNING_PROCESS=$RUNNING_PROCESS \$ok=$ok"
     if [ $ok -ne 0 ]
     then
        debug "TIME expired, could not perform the installation ....."
        debug "Please try it again ..."
        return 1
     fi
     return 0
}


##--------------------------------------------------------------
#
kill_shm()
{
    SHARED=`ipcs -m | grep 0x | awk '{print $2}'`
    debug "kill_shm(): removing shared memories"
    for shd in $SHARED
    do
        ipcrm -m $shd
    done


    SHARED=`ipcs -q | grep 0x | awk '{print $1}'`
    for shd in $SHARED
    do
        ipcrm -Q $shd
    done


    SHARED=`ipcs -s | grep 0x | awk '{print $1}'`
    for shd in $SHARED
    do
        ipcrm -S $shd
    done
}


run_daemon()
{
   cd $SERVER_PATH
   if [ -x "$BINARY_REV" ]
   then
      debug "starting the daemon ... $BINARY"
      rm -f $BINARY
      if [ $? -ne 0 ]
      then
          debug "remove the daemon "
          return 1
      fi
      ln -sf $BINARY_REV $BINARY
      CMD="./$BINARY -${ARGUMENT}"
      echo "CWD=`pwd`"
      ls -l $BINARY
      debug echo $CMD
      $CMD
   else
      debug "Could not start the daemon "
      return 1
   fi
}


kill_current_processes_and_install_new_release()
{
    check_binary_to_install
    ret=$?
    if [ $ret -ne 0 ]
    then
       return $ret
    fi
    
    wait_until_no_more_active_transfers
    ret=$?
    
    ## if no more proccesses, then go and handle shared memory:  wait other process
    if [ $ret -eq 0 ]
    then
        ## shared memory control, if more than install process
        is_shm_control_file_busy
        is_busy=$?
        if [ $REMOVE_SHM -eq 0 -o $is_busy -eq 0 ]
        then
            release_shm_control_file
        fi
        ## if there is another process, wait it to finish 
        ## scripts that do not want to remove shared memory,
        ## wait others to do it
        while [ $is_busy -ne 0 ]
        do
            sleep 10
            is_shm_control_file_busy
            is_busy=$?
        done
        if [ $REMOVE_SHM -eq 1 ]
        then
            kill_shm
        fi
    fi
        
    release_shm_control_file
    
    #if daemon was killed or it was not running,  $DAMEON_PID will be zero, so start new daemon
    # here it does not matter:
    #       if the shared memory has been removed or not 
    #       if there still transfers going on 
    if [ $DAMEON_PID -eq 0 ]
    then
       run_daemon    
       ret=$?
    else
        debug "Daemon still running, not started new release"
    fi
   
   return $ret
}


retrive_file_and_move_to_server_path()
{
   cd $HOME
   mkdir tmp 2> /dev/null
   cd tmp
   rm -f $BINARY ${BINARY}.tmp
   if [ -x "$BINARY" ]
   then
       return 1 ## it must not exist afer rm -f
   fi
   pwd
   CMD="$TRANS_CMD -G $KEYWORD"
   debug $CMD 
   $CMD > ${BINARY}.tmp
   chmod 755 ${BINARY}.tmp
   mv ${BINARY}.tmp  $BINARY
   if [ -x "$BINARY" ]
   then
        mv $BINARY $SERVER_PATH/$BINARY_REV
        chown $OWNER_AND_GROUP $SERVER_PATH/$BINARY_REV
        cd 
        return 0
   fi
   return 1
}


## ---------------   MAIN ------------

get_db_admin_keywords

if [ $? -eq 0 ] ## got the keyword, then try to install it
then 
    debug "Installing keyword $KEYWORD ..."
    save_shm_control_file
    LOG_FILE=/tmp/${BINARY}_${$}
    echo $LOG_FILE
    echo "From: transbug@nxp.com"  > $LOG_FILE
    echo "To: transbug@nxp.com"   >> $LOG_FILE
    echo "Subject: keyword=$KEYWORD $MSG_INSTALL"  >> $LOG_FILE    
    debug "hostname: `hostname`"  >> $LOG_FILE
    
 #   exec 1>> $LOG_FILE
    retrive_file_and_move_to_server_path
    if [ $? -eq 0 ]
    then
        kill_current_processes_and_install_new_release
    fi
    release_shm_control_file
    cat $LOG_FILE | /usr/lib/sendmail -t
    sleep 1
#    rm -f $LOG_FILE
fi

