#!/bin/sh

DIR=`dirname $0`
cd $DIR/../../
SVN_ROOT=`pwd`

cd ..
ROOT=`pwd`
SERVER2=$ROOT/TESTS/server2

mkdir -p $SERVER2 2> /dev/null
rm -f $SERVER2/ssl.transcend_rev_10   $SERVER2/ssl.icomd 

cd /tmp
rm -f ssl.icomd ssl.transcend

echo "!#/bin/sh"           > ssl.icomd
echo "echo testing ..."    > ssl.icomd
chmod 755      ssl.icomd
cp ssl.icomd  ssl.transcend

echo "TRANSPORT_TOOL_RELEASE: adam ssl.transcend ssl.transcend_rev_10 $SERVER2 $USER   3  1"  | $SVN_ROOT/RELEASE_DIR/trans.localhost -web db_admin -p -adam ssl.transcend db_admin@nxp.com
echo "TRANSPORT_TOOL_RELEASE: adam ssl.icomd     ssl.icomd_rev_10     $SERVER2 $USER   4  0"  | $SVN_ROOT/RELEASE_DIR/trans.localhost -web db_admin -p -adam ssl.icomd     db_admin@nxp.com
