#!/bin/sh

tar cvzf ../trans_latest.tgz --exclude=gzip_700MB.tar.gz --exclude=/RELEASE_DIR/ --exclude-vcs  .



## Example how to use 

   #cp ../trans_latest.tgz /proj/dtms_web/transweb-beta/htdocs/repository 

   #wget --user=nxa13339 --password=xxxxxxxx http://transweb-beta.am.freescale.net/repository/trans_latest.tgz
