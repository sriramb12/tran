#!/bin/sh

ver=$(grep -o '[0-9]\+' /etc/redhat-release | head -1)
OS=RH${ver}
HERE=$(pwd)
LOG="$HERE/logBuild_libssh2.log"
true > "$LOG"
TMPDIR=tmp_build_$$
ret=0
LIBSSH2V=$1
SSL_BASE=$2

if [ "$LIBSSH2V" = "" ]
then
    LIBSSH2V=1.9.0
fi

CUSTOM_SSH2=$HERE/lib/$OS/run/pkg/OSS-libssh2-/$LIBSSH2V

run_cmd (){
  CMD=$1
  echo "### CMD: ${CMD}" | tee -a "$LOG"
  $CMD 2>&1 | tee -a "$LOG"
  return $?
}

if [ ! -f "$CUSTOM_SSH2/lib/libssh2.a" ]
then
  if [ "$OS" = "RH6" ]
  then
    CUSTOM_SSH2=$HERE/lib/$OS/run/pkg/OSS-libssh2-/$LIBSSH2V
    CUSTOM_OPENSSL=$HERE/lib/$OS/run/pkg/OSS-openssl-/1.0.2u_with_DES
    echo "CUSTOM_OPENSSL ${CUSTOM_OPENSSL}"

    mkdir $TMPDIR
    cd "$TMPDIR" || exit 1
    gunzip -c  ../ssl_and_certificates/libssh2_$LIBSSH2V.tgz  | tar -xf -
    mkdir build
    cd build || exit 1

    CMAKE="cmake -DCMAKE_INSTALL_PREFIX=$CUSTOM_SSH2
          -DOPENSSL_INCLUDE_DIR=$CUSTOM_OPENSSL/include
          -DOPENSSL_LIBRARIES=$CUSTOM_OPENSSL/lib/libssl.so.1.0.0  $CUSTOM_OPENSSL/lib/libcrypto.so.1.0.0
            ../libssh2"

    run_cmd "$CMAKE" && run_cmd "make VERBOSE=1" && run_cmd "make install" && \
    run_cmd "cp ${CUSTOM_SSH2}/lib64/libssh2* ${CUSTOM_SSH2}/lib"
    ret=$?
  fi
  if [ "$OS" = "RH7" ]
  then
    ## Building libssh2 with autotools
    mkdir $TMPDIR
    cd "$TMPDIR" || exit 1
    if [ -f ../ssl_and_certificates/libssh2-$LIBSSH2V.tar.gz ]
    then
      tar xzvf ../ssl_and_certificates/libssh2-$LIBSSH2V.tar.gz
      mkdir build
      cd build || exit 1
      if [ -z "$SSL_BASE" ]
      then
        CONFIG="../libssh2-$LIBSSH2V/configure --prefix=$CUSTOM_SSH2 --disable-examples-build"
      else
        CONFIG="../libssh2-$LIBSSH2V/configure --prefix=$CUSTOM_SSH2 --disable-examples-build --with-libssl-prefix=$SSL_BASE"
      fi
      run_cmd "$CONFIG" && run_cmd "make VERBOSE=1" && run_cmd "make install"
      ret=$?
    fi
  fi
  cd "$HERE" || exit 1
  if [ "$ret" -eq 0 ]
  then
    rm -rf $TMPDIR
  fi
fi
exit $ret
