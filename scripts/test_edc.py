#!/run/pkg/OSS-anaconda-/2.0.0/bin/python
# -*- coding: utf-8 -*-

## This is a test. I designed it when I was porting EDC daemon to use
## the NXP Central Directory. Note I do 2 search, in the WBI-ID and in the field
## containing CoreID. This way, we can survive the period where employers can
## came from the old VDIs and the new (looged in with his WBI-ID).


from __future__ import print_function

import sys
from pprint import pprint
from ldap3 import Server, Connection, ALL, ALL_ATTRIBUTES, ALL_OPERATIONAL_ATTRIBUTES


NXP_LDAP_SERVER = 'ldaps://inva008.eu-rdc02.nxp.com'
NXP_SERVICE_DN  = 'cn=trans_SA,ou=ServiceAccount,ou=services,o=NXP'
NXP_SERVICE_PWD = 'Start2016!1Q'
NXP_SEARCH_BASE = 'ou=Active,ou=Identities,ou=Internal,o=NXP'
NXP_ATTRIBUTES  = [
    'sn', 'telephoneNumber', 'nxpSite', 'nxpPersonType', 'nxpExternalID',
    'nxpCountryCode', 'nxpIdentityType', 'mail', 'manager', 'managerWorkforceID',
    'mobile', 'cn', 'co', 'description', 'fullName', 'givenName', 'nxpBirthName',
    'city', 'manager', 'managerWorkforceID', 'nxpPersonType', 'telephoneNumber'
]


server = Server(NXP_LDAP_SERVER, port=636, use_ssl=True)
conn   = Connection(server, NXP_SERVICE_DN , NXP_SERVICE_PWD, auto_bind=True)


IDS = ['(cn={})'.format('nxa13334'), '(nxpExternalID={})'.format('ra5672'), '(|(cn={0})(nxpExternalID={1}))'.format('nxa13334', 'b36283')]

for i in IDS:
    print(10*"-", "-> query =", i)
    r = conn.search(search_base=NXP_SEARCH_BASE, search_filter=i, attributes=NXP_ATTRIBUTES)
    #r = conn.search(search_base=NXP_SEARCH_BASE, search_filter=mail, attributes=[ALL_ATTRIBUTES, ALL_OPERATIONAL_ATTRIBUTES])
    if r:
        for r in conn.response:
            for k,v in r['attributes'].iteritems():
                print(k+':', v[0])
    else:
        print("error querying LDAP, return {}".format(r))
        sys.exit(666)

