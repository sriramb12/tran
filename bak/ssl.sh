#!/bin/sh
 rm CAfile.pem
 for i in * ; do
     openssl x509 -in $i -text >> CAfile.pem
 done
