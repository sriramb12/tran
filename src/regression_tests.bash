#!/bin/bash


TESTES[0]="./test_ldap -E -c nxa13339"
TESTES[1]="./test_ldap -E -c b35201"
TESTES[2]="./test_ldap -E -m carlos.mazieri@nxp.com"
TESTES[3]="./test_ldap -E -c carlos.mazieri@gmail.com"
TESTES[4]="./test_ldap -E -m carlos.mazieri@gmail.com"

TESTES[5]="./test_ldap -f -c nxa13339"
TESTES[6]="./test_ldap -f -c b35201"
TESTES[7]="./test_ldap -f -m carlos.mazieri@nxp.com"


./compile_test_ldap_linux.sh
status=$?

tests=${#TESTES[@]}
counter=0

while [ $status -eq 0 -a $counter -lt $tests ]
do
    echo ${TESTES[$counter]}
    CMD="${TESTES[$counter]} "
    $CMD
    status=$?
    counter=`expr $counter +  1`
done