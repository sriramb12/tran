#!/run/pkg/OSS-anaconda-/2.0.0/bin/python
# -*- coding: utf-8 -*-

from __future__ import print_function

import os
import re
import sys
import fcntl
import email
import errno
import imaplib
import mysql.connector
from email.parser import HeaderParser
from ldap3 import Server, Connection, ALL, ALL_ATTRIBUTES, ALL_OPERATIONAL_ATTRIBUTES


## GLOBALS ##
Outlook  = 'outlook.office365.com'
User     = "FAMEDC@nxp.com"
Password = "Today@2016"
CNX      = mysql.connector.connect(user='ra5063', password='freescale', host='dtms01-tx30', database='edc')
REC      = ('name', 'value', 'coreid', 'mngid', 'emailkey', 'startdate', 'enddate')
results  = {'coreid': '', 'name': '', 'email': '', 'role': '', 'mngid': '', 'mng_name': '', 'mng_email': ''}
FLOCK    = '/tmp/.edc_lock'

NXP_LDAP_SERVER = 'ldaps://inva008.eu-rdc02.nxp.com'
NXP_SERVICE_DN  = 'cn=trans_SA,ou=ServiceAccount,ou=services,o=NXP'
NXP_SERVICE_PWD = 'Start2016!1Q'
NXP_SEARCH_BASE = 'ou=Active,ou=Identities,ou=Internal,o=NXP'
NXP_ATTRIBUTES  = ['sn', 'telephoneNumber', 'nxpSite', 'nxpPersonType', 'nxpExternalID',
		'nxpCountryCode', 'nxpIdentityType', 'mail', 'manager', 'managerWorkforceID', 
		'mobile', 'cn', 'co', 'description', 'fullName', 'givenName', 'nxpBirthName', 
		'city', 'manager', 'managerWorkforceID', 'nxpPersonType', 'telephoneNumber'
		]
## END GLOBALS ##


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


def request_load(uid, hexDigest):
    cursor = CNX.cursor()
    query = """SELECT DISTINCT A.name, A.value, A.coreid, A.mngid, B.emailkey, A.startdate, DATE_ADD(NOW(), INTERVAL 1 YEAR) AS enddate
                FROM `requests` A, `proceedings` B
                WHERE A.uid='{uid}' AND B.reqid=A.uid AND B.emailkey='{key}'""".format(uid=uid, key=hexDigest)

    cursor.execute(query)
    request = cursor.fetchall()

    ## Nothing to be dequeued, leaving
    if len(request) < 1: return False

    ## Initializing LDAP connection
    conn = ldap_conn()

    ## with Distinct, only the first result matter
    rec = request[0]

    tmp = dict(zip(REC, rec))
    query_user(conn, tmp['coreid'])
    ## Getting user and manager extra info from LDAP
    tmp['emp_name'], tmp['email']     = results['name'], results['email']
    tmp['mng_name'], tmp['mng_email'] = results['mng_name'], results['mng_email']

    cursor.close()
    return tmp


def enqueue(uid, req):
    cursor = CNX.cursor()
    ## Firtly dequeue the current uid, updating manager's coreid
    sql = """UPDATE requests
             SET queue=1, mngid='{}', status='approved', enddate='{}'
             WHERE uid='{}'""".format(req["mngid"], req['enddate'], str(uid))

    cursor.execute(sql)
    ## Now save the proceedings for the approval reply
    CNX.commit()
    cursor.close()


def email_map(got, wannabe):
    safe_dict = {
	's.adhikari@freescale.com': 'sanjay.kumar@nxp.com',
	'ming.lo@freescale.com': 'mh.lo@nxp.com',
	'tomas@freescale.com': 'tomas.bostrom@nxp.com',
	'trevorsmith@freescale.com': 'trevor.smith@nxp.com',
	'jaime.hueso@freescale.com': 'jaime.zavala@nxp.com',
	'tomas@freescale.com': 'tomas.bostrom@nxp.com',
	'Jean-paul.Davi@freescale.com': 'jean.davi@nxp.com'
	}
    got, wannabe = got.lower(), wannabe.lower()
    if got not in safe_dict:
        return False
    else:
        if safe_dict[got] == wannabe:
            print("replier='{replier}' found in local cache as \
                    safe_dict[{got}]={wannabe}".format(replier=got, got=got, wannabe=wannabe))
            return True
        else:
            return False


def approval_validate(got, wannabe):
    print("replier='{got}', while database says {wannabe}".format(got=got, wannabe=wannabe))
    g, w = got.split('@')[0], wannabe.split('@')[0]
    if g.lower() == w.lower():
        return True
    else:
        map_result = email_map(got, wannabe)
        if map_result:
            return True
        else:
            return False


def MAIN():
    #print("Connecting to {} ...".format(Outlook))
    conn = imaplib.IMAP4_SSL(Outlook)
    conn.login(User, Password)
    conn.select('INBOX')

    (retcode, messages) = conn.search(None, '(UNSEEN)')

    if retcode == 'OK':
        #print("IMAP connection established.")
        for num in messages[0].split(' '):
            try:
                typ, data = conn.fetch(num,'(RFC822)')
            except:
                #print("cannot fetch UNSEEN msgs as RFC822")
                sys.exit(666)

            print(70*"-")
            msg = email.message_from_string(data[0][1])

            ## Processing subject early
            subject = msg['subject'].replace("\n", "").replace("\r", "")

            ## Ignoring Auto reply messages (this just works for MS Exchange)
            auto_reply = msg['X-Auto-Response-Suppress']
            if auto_reply:
                print("Message '{}' is an auto reply, ignoring".format(subject))
                continue

            print("Subject: '{}'".format(subject))

            replier = re.search("<(.*)>", msg['from'], re.IGNORECASE).group(1)
            digest  = re.search("HexDigest: `(.*)`", data[0][1], re.IGNORECASE).group(1)
            ## Checking UID match
            try:
                uid = re.search("\(uid: (.*)\)", subject, re.IGNORECASE).group(1)
            except:
                print("Cannot match UID regex for '{}'".format(subject))
                sys.exit(666)

            if replier and digest and uid:
                ## mark current message as read
                print("replier='{}', digest='{}', uid='{}'".format(replier, digest, uid))
                request = request_load(uid, digest)
                if request:
                    if approval_validate(replier, request['mng_email']):
                        print("Valid reply.\nApproved from {} to {}".format(request['startdate'], request['enddate']))
                        enqueue(uid, request)
                        typ, data = conn.store(num, '+FLAGS', '\Seen')
                    else:
                        print("\nNot valid reply!")
                else:
                    print("\nNot valid reply!")
            else:
                print("\ncannot identify the replier, hexDigest or the UID for the message '{}', ignoring".format(num))
                continue


####### MAIN SCRIPT #######
if __name__ == '__main__':
    lock_file = open(FLOCK, 'w')
    ## Will lock previous execution
    try:
        fcntl.lockf(lock_file, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except (OSError, IOError) as e:
        if e.errno == errno.EAGAIN:
            #if os.path.isfile(FLOCK):
                #print("cannot lock {} because its already locked ({})".format(FLOCK, e))
            #else:
                #print("cannot lock {} for the daemon execcution ({})".format(FLOCK, e))
            sys.exit(666)
        raise

    ## Calling MAIN()
    MAIN()

    ## finishing
    CNX.close()
    os.unlink(FLOCK)
    sys.exit(0)
