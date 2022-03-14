#!/bin/bash

## March 2018, Carlos Jose Mazieri
##

exe_path=`dirname $0`
. ${exe_path}/../include/runTestPlan.bash
. ${exe_path}/../include/runTestPlan_cmdline.bash

runTestPlan  ## just to set variables

ORIG_DATA_DIR=$rtp_ORIG_DATA_DIR      # directory where input files are stored
LOCAL_MAIL=$rtp_CURRENT_USER_EMAIL    # current user email address
LOCAL_PLAN="generic_switches.csv"     # Test Plan to be used


T_01_transcend_server_dev_trans_client_prod()
{
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.transcend trans prod tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.transcend trans prod tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.transcend trans prod tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.transcend trans prod mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.transcend trans prod mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.transcend trans prod mmz  # Maskshop
}


T_02_transcend_server_dev_trans_client_dev()
{
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.transcend trans dev
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.transcend trans dev
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.transcend trans dev
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.transcend trans dev mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.transcend trans dev mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.transcend trans dev mmz  # Maskshop
}


T_03_intercom_server_dev_trans_client_prod()
{
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.icomd trans prod tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.icomd trans prod tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.icomd trans prod tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.icomd trans prod mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.icomd trans prod mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.icomd trans prod mmz  # Maskshop
}


T_04_intercom_server_dev_trans_client_dev()
{
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.icomd trans dev
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.icomd trans dev
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.icomd trans dev
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.icomd trans dev mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.icomd trans dev mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.icomd trans dev mmz  # Maskshop
}


T_05_intercom_server_dev_icom_client_prod()
{
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.icomd icom prod tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.icomd icom prod tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.icomd icom prod mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.icomd icom prod mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.icomd icom prod mmz  # Maskshop
}


T_06_intercom_server_dev_icom_client_dev()
{
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.icomd icom.int dev tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.icomd icom.int dev tmz
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    ssl.icomd icom.int dev mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    ssl.icomd icom.int dev mmz  # Maskshop
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         ssl.icomd icom.int dev mmz  # Maskshop
}


T_07_intercom_server_prod_clients_dev()
{
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    production trans    dev etc
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    production trans    dev etc
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         production trans    dev etc
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    production icom.int dev etc
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    production icom.int dev etc
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         production icom.int dev etc
}


T_08_transcend_server_prod_clients_dev_2_default()
{
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    production trans dev
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         production trans dev
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    production trans dev
     runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         production trans dev
}


_priv_all_sites()
{
    client_type=$1
    trans_client=`ls -rt $rtp_RELEASE_DIR/trans* | grep -v trans.log`
    sites=`$trans_client -X | tail -10 | grep -v "^$" | grep -v Transweb | grep -v Send2 | grep -v '\-\-' |  awk '{print $2}'`
    for site in $sites
    do
      runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    production trans    $client_type  $site
      runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    production trans    $client_type  $site
      runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         production trans    $client_type  $site
      runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/flat_036Bytes.txt    production icom.int $client_type  $site
      runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_700MB.tar.gz    production icom.int $client_type  $site
      runTestPlan $LOCAL_PLAN $LOCAL_MAIL  $ORIG_DATA_DIR/gzip_23MB.gz         production icom.int $client_type  $site
    done
}

T_09_transcend_server_prod_clients_dev_all_sites()
{
    _priv_all_sites "dev"
}

T_10_transcend_server_prod_clients_prod_all_sites()
{
    _priv_all_sites "prod"
}

T_01_transcend_server_dev_trans_client_prod
T_02_transcend_server_dev_trans_client_dev
T_03_intercom_server_dev_trans_client_prod
T_04_intercom_server_dev_trans_client_dev
T_05_intercom_server_dev_icom_client_prod
T_06_intercom_server_dev_icom_client_dev

T_07_intercom_server_prod_clients_dev
T_08_transcend_server_prod_clients_dev_2_default
T_09_transcend_server_prod_clients_dev_all_sites
T_10_transcend_server_prod_clients_prod_all_sites
