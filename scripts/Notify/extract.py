#!/run/pkg/TWW-python-/2.7.2/bin/python
# -*- coding: utf-8 -*-

from __future__ import print_function
import os, sys, ldap
from pprint import pprint


## GLOBALS ##
HEADER      = '\033[95m'
OKBLUE      = '\033[94m'
OKGREEN     = '\033[92m'
WARNING     = '\033[93m'
FAIL        = '\033[91m'
ENDC        = '\033[0m'
BOLD        = '\033[1m'
UNDERLINE   = '\033[4m'

NXP_LDAP_SERVER = "ldap://amec-dc.nxp.com:389"

NXP_SERVICE_DN  = "CN=srv_rdapps01,OU=SRV Accounts,OU=Accounts,OU=Service Delivery,DC=wbi,DC=nxp,DC=com"
NXP_SERVICE_PWD = "bU]Q_M!f_)M2WPWq"

NXP_SEARCH_BASE = "OU=NXP,DC=wbi,DC=nxp,DC=com"
NXP_ATTRIBUTES  = [
    'cn', 'distinguishedName', 'employeeID', 'employeeType', 'extensionAttribute12',
    'mail', 'extensionAttribute2', 'name', 'displayName', 'userPrincipalName', 'telephoneNumber'
]
## END GLOBALS ##


def search_dir(conn, coreid):
    query = "(|(cn={0})(extensionAttribute12={1}))".format(coreid, coreid)
    try:
        r = conn.search_s(NXP_SEARCH_BASE, ldap.SCOPE_SUBTREE, query, NXP_ATTRIBUTES)
    except:
        return None
    try:
        return r[0][1]['mail'][0]
    except:
        # makes a second attempt by email
        query = "(mail={arg}@nxp.com)".format(arg=coreid)
        try:
            r = conn.search_s(NXP_SEARCH_BASE, ldap.SCOPE_SUBTREE, query, NXP_ATTRIBUTES)
        except:
            return None
        try:
            return r[0][1]['mail'][0]
        except:
            return None


## MAIN ##
if __name__ == '__main__':
    ldap.set_option(ldap.OPT_X_TLS_REQUIRE_CERT, ldap.OPT_X_TLS_NEVER) # ignore certificate errors
    conn = ldap.initialize(NXP_LDAP_SERVER)
    conn.simple_bind_s(NXP_SERVICE_DN, NXP_SERVICE_PWD)

    Dict = {}
    F = open('db_dump.txt').readlines()

    for line in F:
        # ignore email.com and dummy.com address (they are fake)
        if 'email' in line or 'dummy' in line:
            continue
        # remove any trash char, examining only those bigger then 6 chars
        l = line.strip('\n').strip('\r').strip('"').strip('<').strip('>').strip('&').strip('#').strip('(').strip(')')
        if len(l) >= 6:
            print(OKBLUE, "going to examine", ENDC, BOLD, l, ENDC)
            if not '@' in l:
                # not a email, query it directly
                email = search_dir(conn, l)
            else:
                # maybe FSL or NXP email, split coreid from the rest
                if 'freescale' in l or 'nxp' in l:
                    coreid = l.split('@')[0]
                    # only if bigger then 6 chars
                    if len(coreid) >= 6:
                        email = search_dir(conn, coreid)
                    else:
                        email = None
                # assuming its a customer and his email is fine
                else:
                    #email = l
                    print(OKGREEN, "external customer", email, ENDC)
            if email:
                print(OKGREEN, email, ENDC)
            else:
                print(WARNING, "cannot find", l, ENDC)
            if email and email in Dict:
                Dict[email] += 1
            elif email:
                Dict[email] = 1
            else:
                print(FAIL, l, "ignored", ENDC)

    # save final emails
    final = open('db_final.txt', 'w')
    for k,v in Dict.iteritems():
        final.write("{0};{1}\n".format(k, v))
    final.close()
