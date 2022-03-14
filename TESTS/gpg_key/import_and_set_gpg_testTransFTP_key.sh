#!/bin/sh
#
# gpg: keyring `/home/nxa13339/.gnupg/secring.gpg' created
# gpg: key CD54D7DE: public key "testTransFTP (to be used in tests) <transbug@nxp.com>" imported
# gpg: Total number processed: 1
# gpg:               imported: 1  (RSA: 1)

/usr/bin/gpg --import  testTransFTP_public.asc
/usr/bin/gpg --import  testTransFTP_private.asc
#/usr/bin/gpg --edit-key  testTransFTP

HexLongkey=`gpg --keyid-format LONG -k testTransFTP | tr '/' ' '  | head -1 | awk '{print $3}'`
echo "trusted-key $HexLongkey" >>   ~/.gnupg/gpg.conf
