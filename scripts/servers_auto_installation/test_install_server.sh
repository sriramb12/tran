#!/bin/sh

DIR=`dirname $0`
cd $DIR/../../
SVN_ROOT=`pwd`


sh $DIR/install_transport_tool.sh adam $SVN_ROOT/RELEASE_DIR/trans.localhost ssl.transcend &
sh $DIR/install_transport_tool.sh adam $SVN_ROOT/RELEASE_DIR/icom.int.localhost_6924 ssl.icomd &


