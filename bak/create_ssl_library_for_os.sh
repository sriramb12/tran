#!/bin/sh

## January 2020, Carlos Mazieri
##
PREFIX=$1
KEEP_DES_CIPHER=$2
HERE=$(pwd)
LOG="$HERE/logBuild_openssl.log"
true > "$LOG"

#default is to remove DES Cipher
DES_OPTION=no-des

# But sometimes is necessary to have. to compile Curl Library for example
if [ "$KEEP_DES_CIPHER" = "keep-des" ]
then
   DES_OPTION=""
fi

if [ "$PREFIX" = "" ]
then
    echo "Need a prefix to install SSL library"
    exit 1
fi

TMPDIR=tmp_build_$$
mkdir $TMPDIR
cd "$TMPDIR" || exit 1
gunzip -c  ../ssl_and_certificates/openssl-1.0.2u.tar.gz | tar -xf -
cd openssl-1.0.2u || exit 1

run_cmd (){
  CMD=$1
  echo "### CMD: ${CMD}" | tee -a "$LOG"
  $CMD 2>&1 | tee -a "$LOG"
  return $?
}

run_cmd "./config  $DES_OPTION -no-seed -no-rc4 -no-idea no-camellia shared --prefix=$PREFIX" && \
run_cmd "make depend" && run_cmd "make" && run_cmd "make install"
ret=$?
cd "$HERE" || exit 1
if [ $ret -eq 0 ]
then
  rm -rf $TMPDIR
fi
exit $ret
