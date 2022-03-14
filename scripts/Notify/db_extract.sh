#!/bin/bash

rm --verbose db_dump.txt db_final.txt

## For Intercom (all the users) and external TransWeb
#mysql -h dtms01-tx30.am.freescale.net -u ra5063 -pfreescale -se "SELECT DISTINCT to_email FROM data_transfer.dt_keyword_mapping WHERE file_keyword like '%e' AND to_email not like '%,%'" > db_dump.txt

## For Transcend and TransWeb (all the users)
mysql -h dtms01-tx30.am.freescale.net -u ra5063 -pfreescale -se "SELECT DISTINCT requester_email FROM data_transfer.dt_transactions WHERE file_keyword REGEXP '^[A-Za-z0-9]+$' and file_keyword not like '%e' AND LENGTH(file_keyword) = 9" > db_dump.txt

sleep 1

## For Intercom (all the users) and TransWeb (all the users)
#mysql -h dtms01-tx30.am.freescale.net -u ra5063 -pfreescale -se "SELECT DISTINCT from_email FROM data_transfer.dt_keyword_mapping WHERE file_keyword like '%e' AND to_email not like '%,%'" >> db_dump.txt

sort -u -o db_dump.txt db_dump.txt
