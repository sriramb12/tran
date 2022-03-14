#!/bin/bash

## January 2019, Carlos Jose Mazieri



exe_path=`dirname $0`
. ${exe_path}/../include/runTestPlan.bash
. ${exe_path}/../include/runTestPlan_cmdline.bash

runTestPlan  ## just to set variables

LOCAL_PLAN="icom_production.csv"     # Test Plan to be used
ORIG_DATA_DIR=$rtp_ORIG_DATA_DIR     # directory where input files are stored
LOCAL_MAIL=$rtp_CURRENT_USER_EMAIL   # current user email
rtp_ICOMRC_PATH="-icomrc $rtp_ORIG_DATA_DIR"


Test_Producion_Servers_x_Development_Clients()
{
    runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt  producion  plan  dev
    runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz  producion  plan  dev
}

Test_Producion_Servers_x_Production_Clients()
{
    runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt  producion  plan  prod
    runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz  producion  plan  prod
}

Test_Development_Servers_x_Development_Clients()
{
    runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt  free.icomd  plan  dev
    runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz  free.icomd  plan  dev
}


Test_Producion_Servers_x_Development_Clients
Test_Producion_Servers_x_Production_Clients
Test_Development_Servers_x_Development_Clients
