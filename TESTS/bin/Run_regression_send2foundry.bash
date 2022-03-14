#!/bin/bash

## May 2019, Carlos Jose Mazieri



exe_path=`dirname $0`
. ${exe_path}/../include/runTestPlan.bash

runTestPlan  ## just to set variables

cd $rtp_TESTS_DIR

if [ $? -eq 0 ]
then
  $rtp_SOURCE_TESTS_BIN/test_plan_runner.bash \
    -c $rtp_TEST_PLAN_DIR/send_2_foundry_dev.csv \
    -r $rtp_RELEASE_DIR \
    -f $rtp_ORIG_DATA_DIR/GDB.gds.gz \
    -e $rtp_CURRENT_USER_EMAIL
else
  echo "The 'client/server tests structure' does not exist, exiting ..."
  exit 1
fi
