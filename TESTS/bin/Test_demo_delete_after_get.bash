#!/bin/bash

## January 2019, Carlos Jose Mazieri

exe_path=`dirname $0`
cd $exe_path
cd ..
SOURCE_TEST_DIR=`pwd`
cd ..
TRANS_REPOSITORY_ROOT=`pwd`

cd ../TESTS  ## it must already exist and have server, client and results directories

if [ $? -eq 0 ]
then
  $SOURCE_TEST_DIR/bin/test_plan_runner.bash \
    -c $SOURCE_TEST_DIR/test_plan/demo_delete_after_get.csv \
    -f $SOURCE_TEST_DIR/data/flat_036Bytes.txt \
    -r $TRANS_REPOSITORY_ROOT/RELEASE_DIR \
    -company -etc
else
  echo "The 'client/server tests structure' does not exist, exiting ..."
  exit 1
fi
