
SSL=/run/pkg/OSS-openssl-/1.0.2u/bin/openssl

# -rw-r--r-- 1 carlos carlos 763057451 Dec 18  2018 /opt/transcend/NXP/TESTS/data/gzip_700MB.tar.gz
# 6519a59e0e398e55a836208916d443eb  /opt/transcend/NXP/TESTS/data/gzip_700MB.tar.gz
FILE=/opt/transcend/NXP/TESTS/data/gzip_700MB.tar.gz

#ls -l ~/Downloads/DOMENICO_LOSURDO_A_REVOLUCAO_RUSSA_E_A_R.pdf
#-rw-rw-r-- 1 carlos carlos 4230323 Feb 14 10:21 /home/carlos/Downloads/DOMENICO_LOSURDO_A_REVOLUCAO_RUSSA_E_A_R.pdf
# 76329fe47ad1e54c3ad3424ad4cf7abc  /home/carlos/Downloads/DOMENICO_LOSURDO_A_REVOLUCAO_RUSSA_E_A_R.pdf
#FILE=/home/carlos/Downloads/DOMENICO_LOSURDO_A_REVOLUCAO_RUSSA_E_A_R.pdf

CIPHERS="\
aes-192-cfb
aes-256-cfb
bf-cfb
aes-128-cfb
cast5-cfb
rc2-cfb
"

# --
# The aes-128-cfb is supposed tobe the faster
# --

# The following ciphers take too long
# aes-128-cfb1
# aes-256-cfb1



key=""
iv=""

setkey() # $1=key 0/1
{
   if [ $1 -eq 0 ]
   then
      key=01020304050607080910
   else
        if [ $1 -eq 1 ]
        then
           key=11121314151617181920
        else
           key=`date +"%Y%H%M%S%s"`
        fi
   fi
}

setiv() #$2=iv 0/1
{
   if [ $1 -eq 0 ]
   then
      iv=010203040506
   else
       if [ $1 -eq 1 ]
       then
          iv=070809101112
       else
          iv=`date +"%Y%H%M%S%d"`
       fi
   fi
}

encrypt() # $1=key 0/1  $2=iv 0/1 $3=cipher
{
   setkey $1
   setiv  $2
   name_enc="enc_key_${1}_IV_${2}_cipher_${3}.enc"
   name_dec="dec_key_${1}_IV_${2}_cipher_${3}.dec"
   
   CMD="$SSL $3  -e -K $key -iv $iv -in $FILE -out $name_enc"
   echo $CMD
   time_start=`date +"%s"`
   $CMD       
   CMD="$SSL $3  -d -K $key -iv $iv -in $name_enc -out $name_dec"
   echo $CMD
   $CMD
   
   time_end=`date +"%s"`
   ls -l $name_enc
   md5sum $name_enc  
   md5sum $name_dec
   duration=`expr $time_end - $time_start`
   echo "$3 duration=$duration start=$time_start end=$time_end"
   echo
   rm $name_enc $name_dec
   sync
}

generate_distinc_key()
{
   for ci in $CIPHERS
   do
        echo "------------------  $ci ---------------------------"
        key_var=2       
        iv_var=0
        while [ $iv_var -lt 2 ]
        do        
            encrypt $key_var $iv_var $ci
            iv_var=`expr $iv_var + 1`
        done
        echo 
  done
}


generate_distinc_pair()
{
   for ci in $CIPHERS
   do
        echo "------------------  $ci ---------------------------"
        key_var=2       
        iv_var=2
        encrypt $key_var $iv_var $ci
        sleep 2
        echo 
  done
}



mix_keys_and_ivs()
{
   for ci in $CIPHERS
do
    echo "------------------  $ci ---------------------------"
    key_var=0
    while [ $key_var -lt 2 ]
    do
        iv_var=0
        while [ $iv_var -lt 2 ]
        do        
            encrypt $key_var $iv_var $ci
            iv_var=`expr $iv_var + 1`
        done
        key_var=`expr $key_var + 1`
    done
    echo 
done
}

ls -l $FILE
md5sum $FILE
echo
sync

generate_distinc_pair

