/usr/bin/gpg --delete-secret-keys  testTransFTP
/usr/bin/gpg --delete-key          testTransFTP

cat >key.par <<EOF
     %echo Generating a basic OpenPGP key with no-ask-passphrase
     %no-ask-passphrase
     Key-Type: DSA
     Key-Length: 1024
     Subkey-Type: ELG-E
     Subkey-Length: 1024
     Name-Real: testTransFTP
     Name-Comment: (transcend and transftp keys used in tests)
     Name-Email: transbug@nxp.com
     Expire-Date: 0
     # Do a commit here, so that we can later print "done" :-)
     %commit
     %echo done
EOF
/usr/bin/gpg --batch --gen-key key.par

/usr/bin/gpg  --sign-key  testTransFTP
/usr/bin/gpg --export  -a testTransFTP                > testTransFTP_public.asc
/usr/bin/gpg --export-secret-keys -a testTransFTP     > testTransFTP_private.asc

HexLongkey=`gpg --keyid-format LONG -k testTransFTP | tr '/' ' '  | head -1 | awk '{print $3}'`
echo "trusted-key $HexLongkey" >>   ~/.gnupg/gpg.conf


#
# Secret key is available.
#
# pub  2048R/ED7465E0  created: 2018-01-17  expires: never       usage: SC
#                      trust: ultimate      validity: ultimate
# sub  2048R/0B5A485A  created: 2018-01-17  expires: never       usage: E
# [ultimate] (1). testTransFTP (transcend and transftp keys used in tests) <transbug@nxp.com>
