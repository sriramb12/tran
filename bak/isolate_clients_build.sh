#!/bin/sh
# 

# Purpose: 
#  isolate clients source code and includes to separate from the servers
#
#  clients build should work on icom_extern_client_single directory
#
#  January 2021 by Carlos Jose Mazieri



/bin/rm -f build-intercom-external-client
 
sh trans_build.sh build-intercom-external-client


SOURCE_FILES=`/bin/grep "^gcc -O3" logBuild_build-intercom-external-client.txt | /usr/bin/awk '{print $NF}' | /bin/grep \.c`

HERE=`/bin/pwd`

ICOM_DIR="`/bin/pwd`/icom_extern_client_single"

/bin/mkdir -p $ICOM_DIR
/bin/rm -rf $ICOM_DIR/*  errors_isolate.txt

if [ -e  ".git_sha" ]
then
   /bin/echo "72157c9" > .git_sha
fi


/bin/cp -r zlib $ICOM_DIR

copy_file()
{
    if [ ! -e "${ICOM_DIR}/{$1}" ]
    then
       dir=`/usr/bin/dirname $1`
       file=`/usr/bin/basename $1`
       /bin/mkdir -p ${ICOM_DIR}/$dir
       /bin/echo "copying $1 to  ${ICOM_DIR}" 2>> errors_isolate.txt
       /bin/cp ${HERE}/$1 ${ICOM_DIR}/$1
    fi    
}


# The option -MM generates the list of the include files 
compile()
{
    /usr/bin/gcc -O3 -Wall -Wextra -fno-stack-protector  \
        -DTRANSAPI -DBYTESWAP -DSINGLE_CONNECT -DSINGLE_CONNECT -DEXTERNAL -DINTERCOM \
        -Ddo_tar -DNONE -DLINUX    -I./trans_include -I./global_include -I./m2include -I./zlib  \
        -I/run/pkg/OSS-openssl-/1.0.2u/include -DSSL_ENCRYPTION -D_LARGEFILE_SOURCE -U_OFF_T \
        -D_FILE_OFFSET_BITS=64 -DLDAP_DEPRECATED=1  -DTRANSAPI_VERSION=\"9.3.7\" \
        -DCURRENT_TRANS_VERSION=\"9.3.7\" -DCURRENT_ICOM_VERSION=\"7.8\" -DGIT_SHA=\"72157c9\" \
        -DBUILD_DATE=\"20210118\" -DLDAP_UID_EQUAL_SEARCH_OR_LOGIN  -MM -E $1
}


get_include_files()
{  
   for f in $SOURCE_FILES
   do
      includes=`compile $f`
    
      for i in $includes
      do
         header=`echo $i | grep '\.h' | grep -v openssl `
         if [ "$header" != "" ]
         then  
             echo "GCC: $f"
             copy_file $header
         fi
      done
   done
}


get_source_files()
{
    for f in $SOURCE_FILES
    do
         copy_file $f       
    done
}


get_source_files
copy_file "global_include/global_debug.h"
copy_file "global_include/password.h"
copy_file "ssl_and_certificates/openssl-1.0.2u.tar.gz"
get_include_files

cd $HERE

/bin/cp trans_build.sh create_ssl_library_for_os.sh git_build_info.sh *.def *.mk Makefile .git_sha $ICOM_DIR
/bin/cp .git_sha $ICOM_DIR/.git_sha_static
/bin/cp -r LICENSES $ICOM_DIR
/bin/rm -rf $ICOM_DIR/zlib
copy_file "zlib/zlib-1.2.3.tgz"

cat errors_isolate.txt
/bin/rm -f errors_isolate.txt
