#!/bin/sh -p

# Compiler directives
#
# REGRESSION_TEST_LDAP           - shows all attributes
# DEBUG                          - show debug lines
# BEAUTY_OUTPUT_ONLY             - output suitable for command line users


# LDAP_UID_FORCE_FSL_COREID      - if a xFSL coreid exists return it for all queries
# LDAP_UID_EQUAL_SEARCH_OR_LOGIN - if a xFSL coreid exists and the query specifies the xFSL coreid, return xFSL coreid
#                                  other queries: name, email, etc will return WBI ID
# if neither of LDAP_UI above    - all queries return WBI ID


gcc -DSTANDALONE -DREGRESSION_TEST_LDAP -Werror -g  -o test_ldap ldapfsl.c -L/usr/lib64 -lldap -lssl -lcrypto -llber
exit

gcc -Wall -Wextra -DBEAUTY_OUTPUT_ONLY -DSTANDALONE -DLDAP_UID_FORCE_FSL_COREID -o test_ldap ldapfsl.c -L/usr/lib64 -lldap -lssl -lcrypto -llber
