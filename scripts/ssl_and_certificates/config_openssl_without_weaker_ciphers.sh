## January 2020, Carlos Mazieri
##
##  download a SSL version such as https://www.openssl.org/source/old/1.0.2/openssl-1.0.2u.tar.gz
##  then extract it and configure with a commnad like:

./config no-des -no-seed -no-rc4 -no-idea no-camellia shared --prefix=/run/pkg/OSS-openssl-/1.0.2u

make depend && make && make test && sudo make install

## Use make install INSTALL_PREFIX=<trans-source>/lib/Linux   to install on another path


## after that 

#/run/pkg/OSS-openssl-/1.0.0m/bin/openssl ciphers | grep -i DES
