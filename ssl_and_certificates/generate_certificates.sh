#!/bin/sh
## add the hostname
SERV=awv343389.nxdi.nl-cdc01.nxp.com
## Edit varaiables as your needs
###
#OPENSSL=/proj/dmisw/mdsdev/work/b35201/MDS_repository/trans_br_nxp/lib/RH6/run/pkg/OSS-openssl-/1.0.2u/bin/openssl
OPENSSL=openssl
SUBJ="/C=US/ST=Arizona/L=Chandler/O=NXP Semiconductors/OU=DE/CN=$SERV/emailAddress=transbug@nxp.com"

RSA_BITS=4096
SHA_OPTION=-sha512
DAYS_TO_EXPIRE=3650

generate_RSA_KEY()
{
   OUT_FILE=$1
   if [ "$OUT_FILE" = "" ]
   then
      echo "ERROR generate_RSA_KEY() needs a output file parameter"
      exit 1
   fi
   $OPENSSL   genrsa -out $OUT_FILE $RSA_BITS
}


generate_CA_certificate()
{
     generate_RSA_KEY  CA.key

     #CA CSR
     $OPENSSL req -new $SHA_OPTION -key CA.key -out CA.csr -subj "$SUBJ"

     #CA certificate
     $OPENSSL x509 -signkey CA.key -in CA.csr -req -days $DAYS_TO_EXPIRE -out CA.pem
}


generate_server_CA_signed_by_CA()
{
      generate_RSA_KEY ServerCert_signedByCA.key
      $OPENSSL req -new $SHA_OPTION -key ServerCert_signedByCA.key -out ServerCert_signedByCA.csr -subj "$SUBJ"
      $OPENSSL x509 -req -in ServerCert_signedByCA.csr -CA CA.pem -CAkey CA.key -CAcreateserial -out ServerCert_signedByCA.crt -days $DAYS_TO_EXPIRE $SHA_OPTION
}

generate_CA_intermediary()
{
     generate_RSA_KEY CA_Intermediary.key

     #Intermediary CA CSR
     $OPENSSL req -new $SHA_OPTION -key CA_Intermediary.key -out CA_Intermediary.csr -subj "$SUBJ"

     #Intermediary Certificate
     $OPENSSL x509 -req -in CA_Intermediary.csr -CA CA.pem -CAkey CA.key -CAcreateserial -out CA_Intermediary.crt -days $DAYS_TO_EXPIRE $SHA_OPTION
}


generate_server_CA_signed_by_intermediary()
{
    generate_RSA_KEY ServerCert_signedByCAIntermediary.key

    #server CSR
    $OPENSSL req -new $SHA_OPTION -key ServerCert_signedByCAIntermediary.key -out ServerCert_signedByCAIntermediary.csr -subj "$SUBJ"

    # server certificate
    $OPENSSL x509 -req -in ServerCert_signedByCAIntermediary.csr -CA CA_Intermediary.crt -CAkey CA_Intermediary.key -CAcreateserial -out ServerCert_signedByCAIntermediary.crt -days $DAYS_TO_EXPIRE $SHA_OPTION

    cat ServerCert_signedByCA.key  > trans.ssl.pem
    cat ServerCert_signedByCA.crt >> trans.ssl.pem
    cp trans.ssl.pem  combined.pem  
    cp CA.pem         ca.pem
}


TMP_DIR=tmp_process_$$
mkdir $TMP_DIR
cd $TMP_DIR
generate_CA_certificate
generate_server_CA_signed_by_CA
generate_CA_intermediary
generate_server_CA_signed_by_intermediary
cp trans.ssl.pem  combined.pem ca.pem ../
cd -
rm -rf $TMP_DIR
