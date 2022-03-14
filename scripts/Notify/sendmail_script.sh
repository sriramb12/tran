#!/bin/bash

rm --verbose email.log error.log

for i in `awk -F";" '{if ($1) print $1}' db_final.txt`; do
    echo "Send to $i"
    /usr/lib/sendmail -f FAMEDC@nxp.com -F "Transcend Team" $i < email_msg.txt
    sleep 1
    RESULT=$?
    if [ $RESULT -eq 0 ]
    then
        echo "Email to $i sent" >> email.log
    else
        echo RESULT = ${RESULT}
        echo "$i" >> error.log
    fi
    sleep 1
done
