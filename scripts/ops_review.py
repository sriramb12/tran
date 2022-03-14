#!/run/pkg/OSS-anaconda-/2.0.0/bin/python
# -*- coding: utf-8 -*-

from __future__ import print_function

import os
import mysql.connector
from pprint import pprint
import matplotlib.pyplot as plt
from matplotlib.dates import date2num

## GLOBALS ##
CNX = mysql.connector.connect(user='ra5063', password='freescale', host='dtms01-tx30', database='data_transfer')


## FUNCTIONS ##
def total_keywords_per_month():
    """Total transfer (Put and Get operations) per month Since the introduction of MySQL"""

    cursor = CNX.cursor()
    query = "SELECT count(file_keyword) AS keywords, timestamp FROM `dt_transactions` \
        WHERE request_type in ('Put', 'Get') AND timestamp > '2015-06-14' \
        AND timestamp < '2016-07-15' \
        GROUP BY YEAR(timestamp), MONTH(timestamp)"

    cursor.execute(query)
    ret = cursor.fetchall()

    ## Nothing returned, leaving
    if len(ret) < 1: return False
    pprint(ret)
    as_list = map(list, zip(*ret))
    pprint(as_list)
    plt.plot_date(date2num(as_list[1]), as_list[0], linestyle='--', marker='o', color='b')
    plt.grid()
    plt.title('Total transfers (Put and Get operations) per month Since Jun/2015')
    plt.xlabel("Per month (date)")
    plt.ylabel("Total of keywords (count)")
    plt.show()


def total_bytes_per_month():
    """Total of bytes transferred per month Since the introduction of MySQL"""

    cursor = CNX.cursor()
    query = "SELECT CAST(round(sum(original_size)/count(original_size), 0) AS SIGNED) as size, timestamp \
             FROM `dt_transactions` WHERE request_type in ('Put', 'Get') \
             AND timestamp > '2015-06-14' AND timestamp < '2016-07-15' \
             GROUP BY YEAR(timestamp), MONTH(timestamp)"

    cursor.execute(query)
    ret = cursor.fetchall()

    ## Nothing returned, leaving
    if len(ret) < 1: return False
    #pprint(ret)
    as_list = map(list, zip(*ret))
    pprint(as_list[1])
    pprint(as_list[0])
    plt.plot_date(date2num(as_list[1]), as_list[0], linestyle='--', marker='o', color='b')
    plt.grid()
    plt.title('Total of bytes transferred (Put and Get operations) per month since April/2015')
    plt.xlabel("Per month (date)")
    plt.ylabel("Total of keywords (count)")
    plt.show()


def distinct_users_per_month():
    """Number of distinct users per month Since 01/Jan/2016"""

    cursor = CNX.cursor()
    query = """SELECT file_keyword, SUBSTRING_INDEX(requester_email,'@', 1) as coreid,
               DATE_FORMAT(timestamp, '%m') AS Month, tool, request_type
                FROM `dt_transactions`
                WHERE length(SUBSTRING_INDEX(requester_email,'@', 1)) >= 8
                 AND length(SUBSTRING_INDEX(requester_email,'@', 1)) <= 9
                 AND SUBSTRING_INDEX(requester_email,'@', 1) REGEXP '[0-9]'
                 AND timestamp >= '2016-01-01' AND length(file_keyword) < 10
                AND request_type <> "Transdirlist put"
                AND (request_type like "%Put" OR request_type like "%Get")"""

    months = {'02':[], '03':[], '04':[], '05':[], '06':[], '07':[]}
    print(query)
    cursor.execute(query)
    ret = cursor.fetchall()

    exclude = [
            'bogditu1', 'cionesc1', 'b31390-14', 'vburlac1', 'nmanesc1', 'mnistor1',
            'drbadea1', 'ccaciul1', 'aboi001c', 'scostin1', 'bhunter1', 'agancev1',
            'arailea1', 'knguyen4', 'rionesc1', 'bhamciu1', 'czamfir1', 'alerufa1',
            'salesco2']

    ## Nothing returned, leaving
    if len(ret) < 1: return False
    #pprint(ret)

    for r in ret:
        if r[1] not in exclude:
            print("NOT IN exclude", r)
            if not r[1] in months[r[2]]:
                months[r[2]].append(r[1])
    pprint(months)
    for k in iter(months.keys()):
        print(k, len(months[k]))

    dest = open('DEST.txt', 'w')
    for k,v in months.iteritems():
        dest.write('## {0} ##\n'.format(k))
        for i in v:
            dest.write("{0}\n".format(i))
    dest.close()
## cat DEST.txt | grep -v "##" | xargs -I {} sh -c "./ldap_info.py {};" > ldap_query.log

## MAIN ##
def MAIN():
    total_keywords_per_month()
    #total_bytes_per_month()
    distinct_users_per_month()
if __name__ == '__main__':
    ## Calling MAIN()
    MAIN()

