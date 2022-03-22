#!/bin/sh

SRC=../src

LDAP_PATH=`dirname $0`

if [ "$LDAP_PATH" = "." ]
then
   LDAP_PATH=`pwd`
fi

LDAP_LIB_PATH=$LDAP_PATH/lib


LIBS="$LDAP_LIB_PATH/liblunicode.a \
 $LDAP_LIB_PATH/libldap.a \
 $LDAP_LIB_PATH/liblber.a \
 /_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/lib/libssl.a \
 /_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/lib/libcrypto.a \
 -lnsl -lsocket -lm -lz -lresolv \
 "


gcc -g -o test_solaris_ldap $SRC/ldapfsl.c -DDEBUG -DSOLARIS -DSTANDALONE -DUSING_OPEN_LDAP -I$SRC -I$LDAP_PATH/include $LIBS