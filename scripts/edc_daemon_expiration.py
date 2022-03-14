#!/run/pkg/TWW-python-/2.7.2/bin/python
# -*- coding: utf-8 -*-

from __future__ import print_function

import os
import sys
import fcntl
import errno
import MySQLdb

from UserProfile import UserProfile


## GLOBALS ##
CNX   = MySQLdb.connect(user='ra5063', passwd='freescale', host='dtms01-tx30')
FLOCK = '/tmp/.edc_lock'
## END GLOBALS ##

def remove_request(uid):
    cursor = CNX.cursor()
    query  = "DELETE FROM edc.requests WHERE uid = {} AND status='approved' LIMIT 1".format(uid)
    cursor.execute(query)
    CNX.close()


def requests_load():
    cursor = CNX.cursor()
    query = """
        SELECT uid, value, enddate, datediff(NOW(), enddate) AS date_count, coreid, mngid
        FROM edc.requests
        WHERE transtype='email'
        AND status = 'approved'
        AND datediff(NOW(), enddate) > 0
        AND SUBSTR(value, INSTR(value, '@') + 1) NOT IN (SELECT value FROM edc.requests WHERE transtype='domain' AND status = 'approved')
        ORDER BY `date_count` DESC"""
    cursor.execute(query)
    request = cursor.fetchall()
    ## Nothing to be dequeued, leaving
    if len(request) < 1: return False
    else: return request


def MAIN():
    req  = requests_load()
    if req:
        for row in req:
            try:
                up   = UserProfile()
                info = up.query_user(row[5])
                print("\nYES", row)
                #print("   ", info)
                print(up)
            except Exception as e:
                print("\nNO", row, e)
                #remove_request(row['uid'])



####### MAIN SCRIPT #######
if __name__ == '__main__':
    lock_file = open(FLOCK, 'w')
    ## Will lock previous execution
    try:
        fcntl.lockf(lock_file, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except (OSError, IOError) as e:
        if e.errno == errno.EAGAIN:
            sys.exit(666)
        raise

    ## Calling MAIN()
    MAIN()

    ## finishing
    CNX.close()
    os.unlink(FLOCK)
    sys.exit(0)
