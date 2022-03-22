#!/bin/sh

## Febuary 2020, Carlos Mazieri
## May 2021, Benjamin Alvarez
##
PREFIX=$1
LIBCURLV=$2
LIBSSH2V=$3
SSL_BASE=$4

ret=0
HERE=$(pwd)
LOG="$HERE/logBuild_curl.log"
true > "$LOG"
ver=$(grep -o '[0-9]\+' /etc/redhat-release | head -1)
OS=RH${ver}

if [ "$PREFIX" = "" ]
then
    echo "Need a prefix to install CURL library"
    exit 1
fi

# Defaults
if [ "$LIBCURLV" = "" ]
then
    LIBCURLV=7.68.0
fi
if [ "$LIBSSH2V" = "" ]
then
    LIBSSH2V=1.9.0
fi

TMPDIR=tmp_build_$$
mkdir $TMPDIR

run_cmd (){
  CMD=$1
  echo "### CMD: ${CMD}" | tee -a "$LOG"
  $CMD 2>&1 | tee -a "$LOG"
  return $?
}

if [ "$OS" = "RH6" ]
then
    # build curl library using cmake, autotools build for custom libssh2 and custom openssl seems to not work
    CMD="./create_libssh2_for_os.sh $LIBSSH2V"
    echo "$CMD  > logBuild_libssh2.log" >> "$LOG"
    $CMD
    cd "$TMPDIR" || exit 1
    gunzip -c  ../ssl_and_certificates/curl-$LIBCURLV-large-buffer.tar.gz | tar -xf -
    mkdir build
    cd build || exit 1
    SSH2_DIR=$HERE/lib/$OS/run/pkg/OSS-libssh2-/$LIBSSH2V
    SSL_DIR=$HERE/lib/$OS/OSS-openssl-/1.0.2u_with_DES
    # Tried using $SSL_BASE in here, but turns out it fails, notice
    # '/run/pkg' is missing in the $SSL_DIR path, so actually this library is not used in RH6

    CMAKE="cmake -DCMAKE_INSTALL_PREFIX=$PREFIX
        -DOPENSSL_INCLUDE_DIR=$SSL_DIR/include
        -DOPENSSL_LIBRARIES=$SSL_DIR/lib
        -DOPENSSL_ROOT_DIR=$SSL_DIR
        -DLIBSSH2_INCLUDE_DIR=$SSH2_DIR/include
        -DLIBSSH2_LIBRARY=$SSH2_DIR/lib/libssh2.a
            ../curl-$LIBCURLV"
    run_cmd "$CMAKE" && run_cmd "make VERBOSE=1" && run_cmd "make install"
    ret=$?
else
    # RH7 and others
    # build libssh2
    CUSTOM_SSH2=$HERE/lib/$OS/run/pkg/OSS-libssh2-/$LIBSSH2V
    if [ -f "$CUSTOM_SSH2"/lib/libssh2.a ]
    then
      echo "Found libssh2-$LIBSSH2V"
    else
      cd "$HERE" || exit 1
      CMD="./create_libssh2_for_os.sh $LIBSSH2V $SSL_BASE"
      echo "$CMD  > logBuild_libssh2.log" >> "$LOG"
      $CMD
    fi
    # Custom Curl build for RH7
    cd "$TMPDIR" || exit 1
    tar xzvf ../ssl_and_certificates/curl-$LIBCURLV-large-buffer.tar.gz
    mkdir build
    cd build || exit 1
    if [ -z "$SSL_BASE" ]
    then
      CONFIG="../curl-$LIBCURLV/configure --disable-ldap --disable-ldaps --with-libssh2=$CUSTOM_SSH2  --prefix=$PREFIX"
    else
      CONFIG="../curl-$LIBCURLV/configure --disable-ldap --disable-ldaps --enable-shared=no --with-libssh2=$CUSTOM_SSH2  --prefix=$PREFIX --with-ssl=$SSL_BASE"
    fi
    run_cmd "$CONFIG" && run_cmd "make VERBOSE=1" && run_cmd "make install"
    ret=$?
fi

cd "$HERE" || exit 1
if [ $ret -eq 0 ]
then
  if [ -d "$PREFIX/lib64" ] && [ ! -d "$PREFIX/lib" ]
  then
     /bin/cp -r  "$PREFIX/lib64" "$PREFIX/lib"
  fi
  rm -rf $TMPDIR
fi
exit $ret
