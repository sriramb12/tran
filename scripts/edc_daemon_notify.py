#!/run/pkg/OSS-anaconda-/2.0.0/bin/python
# -*- coding: utf-8 -*-

from __future__ import print_function

import os
import sys
import fcntl
import errno
import hashlib
import smtplib
import subprocess
import mysql.connector
from pprint import pprint
from email.mime.text import MIMEText
from ldap3 import Server, Connection, ALL, ALL_ATTRIBUTES, ALL_OPERATIONAL_ATTRIBUTES


## GLOBALS ##
CNX     = mysql.connector.connect(user='ra5063', password='freescale', host='dtms01-tx30', database='edc')
REC     = ('uid', 'transtype', 'name', 'value', 'justification', 'status', 'term', 'coreid', 'mngid', 'startdate', 'enddate')
results = {'coreid': '', 'name': '', 'email': '', 'role': '', 'mngid': '', 'mng_name': '', 'mng_email': ''}
FAMEDC  = "FAMEDC@nxp.com"
ADMIN   = 'transbug@nxp.com'
FLOCK   = '/tmp/.edc_lock'

NXP_LDAP_SERVER = 'ldaps://inva008.eu-rdc02.nxp.com'
NXP_SERVICE_DN  = 'cn=trans_SA,ou=ServiceAccount,ou=services,o=NXP'
NXP_SERVICE_PWD = 'Start2016!1Q'
NXP_SEARCH_BASE = 'ou=Active,ou=Identities,ou=Internal,o=NXP'
NXP_ATTRIBUTES  = ['sn', 'telephoneNumber', 'nxpSite', 'nxpPersonType', 'nxpExternalID', 'nxpCountryCode',
		'nxpIdentityType', 'mail', 'manager', 'managerWorkforceID', 'mobile', 'cn', 'co', 'description',
		'fullName', 'givenName', 'nxpBirthName', 'city', 'manager', 'managerWorkforceID', 'nxpPersonType', 'telephoneNumber'
	]
## END GLOBALS ##


def request_hash(req):
    "This is create a MD5Sum formed from the concatening of strings"
    m = hashlib.md5()
    m.update(str(req['uid']))
    m.update(req['name'])
    m.update(req['value'])
    m.update(req['justification'])
    m.update(req['coreid'])
    m.update(req['mngid'])
    return m.hexdigest()


def ldap_conn():
    # connecting
    server = Server(NXP_LDAP_SERVER, port=636, use_ssl=True)
    conn   = Connection(server, NXP_SERVICE_DN , NXP_SERVICE_PWD, auto_bind=True)
    #print("DEBUG", conn)
    conn.start_tls()
    return conn


def query_supervisor(conn, supervisor):
    uid  = supervisor.split(',')[0].split('=')[1]
    mail = '(cn={})'.format(uid)
    r = conn.search(search_base=NXP_SEARCH_BASE, search_filter=mail, attributes=NXP_ATTRIBUTES)
    if r:
        for r in conn.response:
            #print("DEBUG: Query manager", r['attributes'])
            if r['attributes']['nxpPersonType'][0] == 'Employee':
                #print("DEBUG: its an Employee", r['attributes']['mail'][0])
                results['mngid']      = r['attributes']['cn'][0]
                results['mng_email']  = r['attributes']['mail'][0]
                results['mng_name']   = r['attributes']['fullName'][0]
    else:
        print(r)
        sys.exit(666)


def query_user(conn, arg):
    mail = '(cn={})'.format(arg)
    r = conn.search(search_base=NXP_SEARCH_BASE, search_filter=mail, attributes=NXP_ATTRIBUTES)
    if r:
        for r in conn.response:
            #print("DEBUG: First attempt", r['attributes'])
            if r['attributes']['nxpPersonType'][0] == 'Employee':
                #print("DEBUG: its an Employee", r['attributes']['mail'][0])
                results['coreid'] = r['attributes']['cn'][0]
                results['email']  = r['attributes']['mail'][0]
                results['name']   = r['attributes']['fullName'][0]
                results['role']   = r['attributes']['nxpPersonType'][0]
                ## will query supervisor separetely
                query_supervisor(conn, r['attributes']['manager'][0])
    else:
        ## Needs a second attempt because the coreid maybe is the old
        r = None; mail = None
        mail = '(nxpExternalID={})'.format(arg)
        r = conn.search(search_base=NXP_SEARCH_BASE, search_filter=mail, attributes=NXP_ATTRIBUTES)
        if r:
            for r in conn.response:
                #print("DEBUG: Second attempt", r['attributes'])
                if r['attributes']['nxpPersonType'][0] == 'Employee':
                   #print("DEBUG: its an Employee", r['attributes']['mail'][0])
                   results['coreid'] = r['attributes']['cn'][0]
                   results['email']  = r['attributes']['mail'][0]
                   results['name']   = r['attributes']['fullName'][0]
                   results['role']   = r['attributes']['nxpPersonType'][0]
                   ## will query supervisor separetely
                   query_supervisor(conn, r['attributes']['manager'][0])
        else:
            print("error querying LDAP, return {}".format(r))
            sys.exit(666)


def build_email(rec):
    email = None
    if rec['status'] == 'pending':
        email = """Dear Customer {mng_name},\r

Thank you for using EDC and the Transcend family of products.\r

It has come to our attention that a file transfer is waiting for your approval. Here are the details:\r

\tRequest (type={transtype}): {name} - {value}\r
\tRequester: {emp_name} ({email})\r
\tApprover: {mng_name} ({mng_email})\r
\tBusiness Justification: {justification}\r
\tCreated in: {startdate}\r

To confirm that you would like to approve it please just send a reply of this e-mail by pressing the 'Reply' button. This confirmation serves two purposes: First, it verifies that the EDC system is able to send you an e-mail. Second, it protects you in case someone forges a request in your name.\r

*** IMPORTANT *** Replying this email will results in Approval of this request. If you need to reject the request, go to http://edc.freescale.net/requests.cgi?uid={uid}. For any further information, open a ticket using the following information:\r

\tTicket System: http://servicedesk.freescale.com\r
\tCategory: APPS_MFG_DMO\r
\tSub-Category: DAR_DATA_DELIVERY/TRANSCEND\r
\tAssignment Group: APPS_DATA_TRANSPORT/ARCHIVE\r

The Transcend Team.
HexDigest: `{digest}`
""".format(**rec)
    elif rec['status'] == 'approved':
        email = """Dear Customer {emp_name},\r

Thank you for using EDC and the Transcend family of products.\r

This is a message to let you know that the request below has been approved:\r

\tRequest (type={transtype}): {name} - {value}\r
\tRequester: {emp_name} ({email})\r
\tApprover: {mng_name} ({mng_email})\r
\tBusiness Justification: {justification}\r
\tCreated in: {startdate}\r
\tDirect Link: http://edc.freescale.net/requests.cgi?uid={uid}\r

For any further assistance, please visit http://servicedesk.freescale.com and open a ticket using:\r

\tCategory: APPS_MFG_DMO\r
\tSub-Category: DAR_DATA_DELIVERY/TRANSCEND\r
\tAssignment Group: APPS_DATA_TRANSPORT/ARCHIVE\r

The Transcend Team.
HexDigest: `{digest}`
""".format(**rec)
    return email


def dequeue():
    cursor = CNX.cursor()
    query = "SELECT uid, transtype, name, value, justification, status, term, coreid, mngid, \
            startdate, enddate FROM requests WHERE queue = 1 AND status NOT IN ('rejected', 'revoked')"

    cursor.execute(query)
    queue = cursor.fetchall()

    ## Nothing to be dequeued, leaving
    if len(queue) < 1: return False

    ## Initializing LDAP connection
    conn = ldap_conn()

    records = []
    for rec in queue:
        tmp = dict(zip(REC, rec))
        query_user(conn, tmp['coreid'])
        ## Getting user and manager extra info from LDAP
        tmp['emp_name'], tmp['email']     = results['name'], results['email']
        tmp['mng_name'], tmp['mng_email'] = results['mng_name'], results['mng_email']
        ## MD5Sum digest for the 'Proceedings' (valid email reply)
        tmp['digest'] = request_hash(tmp)
        records.append(tmp)

    cursor.close()
    return records


def mark_dequeued(req):
    cursor = CNX.cursor()
    ## 1) dequeue the current uid, updating manager's coreid according with LDAP
    sql = "UPDATE requests SET queue=0, mngid='{}' WHERE uid='{}'".format(req["mngid"], str(req["uid"]))
    cursor.execute(sql)
    ## 2) Now save the proceedings for the approval reply, store the hexdigest
    sql = "INSERT INTO proceedings (reqid, emailkey) VALUES ('{}', '{}')".format(req['uid'], req['digest'])
    cursor.execute(sql)
    # 3) Make sure data is committed to the database
    CNX.commit()
    cursor.close()


def MAIN():
    queue = dequeue()
    if not queue:
        #print("nothing in the queue, exiting ...")
        return

    for q in queue:
        print(90*"-")
        print("processing database queue entry:")
        pprint(q)

        ## Domains are for Andy approval
        if q['transtype'] == 'domain':
            q['mngid']     = 'RA5063'
            q['mng_name']  = 'Andrew Espenscheid'
            q['mng_email'] = 'andrew.espenscheid@nxp.com'

        ## will build a MIME buffer to compose the message 
        msg = MIMEText(build_email(q))

        ## Who is the notify
        msg['From'] = FAMEDC
        msg['Bcc']  = ADMIN

        ## Preparing the subject and the Notify list according witht he status
        if q['status'] == 'pending':
            msg['Subject']  = '[EDC] {value} pending for approval (uid: {uid})'.format(**q)
            nlist           = [q['mng_email'], q['email'], ADMIN, FAMEDC]
            msg['To']       = q['mng_email']
            msg['Cc']       = q['email']
        else:
            msg['Subject']  = '[EDC] {value} approved (uid: {uid})'.format(**q)
            nlist           = [q['email'], q['mng_email'], ADMIN, FAMEDC]
            msg['To']       = q['email']
            msg['Cc']       = q['mng_email']


        s = smtplib.SMTP('remotesmtp.freescale.net')
        #s.set_debuglevel(1)
        s.sendmail(FAMEDC, nlist, msg.as_string())
        s.quit()
        mark_dequeued(q)


####### MAIN SCRIPT #######
if __name__ == '__main__':
    lock_file = open(FLOCK, 'w')
    ## Will lock previous execution
    try:
        fcntl.lockf(lock_file, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except (OSError, IOError) as e:
        if e.errno == errno.EAGAIN:
            #if os.path.isfile(FLOCK):
            #    print("cannot lock {} because its already locked ({})".format(FLOCK, e))
            #else:
            #    print("cannot lock {} for the daemon execcution ({})".format(FLOCK, e))
            sys.exit(666)
        raise

    ## Calling MAIN()
    MAIN()

    ## finishing
    CNX.close()
    os.unlink(FLOCK)
    sys.exit(0)
