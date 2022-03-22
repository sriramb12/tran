LDAP_PATH=/proj/dmisw/mdsdev/work/b35201/MDS_repository/transcend/m2server/ldap/solaris/ldapcsdk-5.1.7
LDAP_LIB_PATH=$LDAP_PATH/lib

LIBS="$LDAP_LIB_PATH/libssldap50.so \
 $LDAP_LIB_PATH/libssl3.so \
 $LDAP_LIB_PATH/libplc4.so \
 $LDAP_LIB_PATH/libnss3.so \
 $LDAP_LIB_PATH/libnspr4.so \
 $LDAP_LIB_PATH/libplds4.so \
 $LDAP_LIB_PATH/libsoftokn3.so \
 $LDAP_LIB_PATH/libprldap50.so \
 $LDAP_LIB_PATH/libldap50.so  \
 $LDAP_LIB_PATH/liblber50.a \
 -lnsl -lsocket -lm -lz
 "


#gcc -g -o test_ldap ldapfsl.c -DSOLARIS -DSTANDALONE -DUSING_MOZILLA_LDAP -I$LDAP_PATH/include $LIBS


LDAP_PATH=/proj/dmisw/pipadev/work/b35201/git/openldap-2.4.44
LDAP_LIB_PATH=$LDAP_PATH/lib


LIBS="$LDAP_LIB_PATH/liblunicode.a \
 $LDAP_LIB_PATH/libldap.a \
 $LDAP_LIB_PATH/liblber.a \
 $LDAP_LIB_PATH/libldap_r.a \
 $LDAP_LIB_PATH/liblutil.a \
 $LDAP_LIB_PATH/librewrite.a\
 -lnsl -lsocket -lm -lz
 "


LIBS="$LDAP_LIB_PATH/liblunicode.a \
 $LDAP_LIB_PATH/libldap.a \
 $LDAP_LIB_PATH/liblber.a \
 /_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/lib/libssl.a \
 /_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/lib/libcrypto.a \
 -lnsl -lsocket -lm -lz -lresolv \
 "

# LDAP_PATH=/_TOOLS_/dist/OSS-openldap-/2.3.9/sparc-sun-solaris2.8
# LDAP_LIB_PATH=$LDAP_PATH/lib
# LIBS=" $LDAP_LIB_PATH/libldap.a \
#  $LDAP_LIB_PATH/liblber.a \
#  /_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/lib/libssl.a \
#  /_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/lib/libcrypto.a \
#  -lnsl -lsocket -lm -lz -lresolv \
#  "

gcc -g -o test_ldap ldapfsl.c -DDEBUG -DSOLARIS -DSTANDALONE -DUSING_OPEN_LDAP -I$LDAP_PATH/include $LIBS