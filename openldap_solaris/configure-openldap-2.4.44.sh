#!/bin/sh

##==============================================================================
#  March 2017
#
#      Carlos Mazieri
#
#  This configuration was used to build openldap-2.4.44 for Solaris 8
##==============================================================================


CC=gcc
export CC

CFLAGS=-I/_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/include
export CFLAGS

LDFLAGS=-L/_TOOLS_/dist/OSS-openssl-/1.0.1g/sparc-sun-solaris2.8/lib/
export LDFLAGS

./configure \
    --disable-slapd \
    --with-gnu-ld \
    --with-tls=openssl