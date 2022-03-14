#ifndef trans_server_msg_h
#define trans_server_msg_h
/* Defines for Transcend and InterCom errors follow */

#define mwr_signoff_msg \
"Final sign off was not completed for data sent to the mask shop.\n\n\
    Mask set:  %s\n\
    MWR number:  %s\n\
    Mask shop:  %s\n\
    Sender:  %s\n"

#define no_dtms_project_msg \
"No active MWR projects are present for data sent to the mask shop.\n\n\
    Mask set:  %s\n\
    Mask shop:  %s\n\
    Sender:  %s\n"

#define wroc_msg \
"\n\nTo transcend data to our Ed Bluestein site:  Type 'trans -p -blu <file>'\n\
To transcend data to our Tempe site:  Type 'trans -p -tem <file>'\n\
To transcend data to our Munich site:  Type 'trans -p -mun <file>'\n\
To transcend data to our Sendai site:  Type 'trans -p -sen <file>'\n\n"
#define austin_msg \
"\n\nTo transcend data to our WROC site:    Type 'trans -p -wro <file>'\n\
To transcend data to our Tempe site:  Type 'trans -p -tem <file>'\n\
To transcend data to our Sendai site:  Type 'trans -p -sen <file>'\n\
To transcend data to our Munich site:  Type 'trans -p -mun <file>'\n\n"
#define munich_msg \
"\n\nTo transcend data to our WROC site:    Type 'trans -p -wro <file>'\n\
To transcend data to our Tempe site:  Type 'trans -p -tem <file>'\n\
To transcend data to our Sendai site:  Type 'trans -p -sen <file>'\n\
To transcend data to our Ed Bluestein site:  Type 'trans -p -blu <file>'\n\n"
#ifdef NXP 
#define FREE_REL_MSG \
"\n\nYou can retrieve the data using either Transcend or\n\
TransWeb.  Transcend has a command line interface, and\n\
TransWeb has a web interface.  \n\n\
You must sign in or be registered as %s to retrieve the data.\n\
URL to access data:    https://nww.transweb.nxp.com/index.cgi?go=KEYWORD&KEYWORD=%s\n\n\
URL to access TransWeb:  https://nww.transweb.nxp.com\n\n\
URL to down load Transcend:  http://transcend.freescale.net\n\n"
#endif

#define INTERNAL_REL_MSG \
"\n\nNotes:\n\n\
1. Before you can retrieve the data,  you must set up your External LDAP  account.  \n\n\
   URL for External registration: https://www.nxp.com/webapp-signup/register\n\n\
2. You must set up your External LDAP account using '%s' as your e-mail address to retrieve this data.\n\n\
3. If “Pending authorization” appears above, then data cannot be retrieved until the NXP manager approves.\n\n\
4. If you have multiple email addresses, you must repeat steps 1 and 2 for each address you want to use.\n\n\
5. Personal Secure keywords only work for the email address sent to.  They cannot be forwarded to others.\n\n\n\
After you have set up your LDAP account, you can retrieve the data using TransWeb or InterCom.\n\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n\
For questions or problems, contact the Transcend Team:\n\n\
    E-mail:  transbug@nxp.com\n\n"
#define tmp_msg \
"\n\nYou can retrieve the data using TransWeb or Intercom 5.1.\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n\
Important note:  You must have both the personal secure keyword and\n\
file keyword to access the data with TransWeb.  You must \n\
have the personal secure keyword to access the data using InterCom.\n\
Both keywords are present in this E-mail  message.\n\n"
#define EXTERNAL_REL_MSG \
"\n\nYou can retrieve the data using either InterCom or\n\
TransWeb.  InterCom has a command line interface, and\n\
TransWeb has a web interface.  \n\
You must sign in or be registered as %s to retrieve the data.\n\n\
URL to access data:    https://nww.transweb.nxp.com/index.cgi?go=KEYWORD&KEYWORD=%s\n\n\
URL to access TransWeb: https://nww.transweb.nxp.com\n\n\
URL to down load InterCom http://intercom.freescale.net\n\n"
#define TRANSWEB_REG_MSG \
"\n\nBefore you can retrieve the data,  you must set up your\n\
External LDAP  account.  In order to use the secure keyword\n\
provided above,  you must set up your  account  using\n\
'%s' as your e-mail address.  If you\n\
have a different e-mail address and prefer to use it,  let\n\
the NXP customer who sent you the personal secure keyword know\n\
the e-mail address you will be using so he can send you a\n\
different personal secure keyword.\n\n\
URL for External LDAP registration:\n\n\
    https://www.nxp.com/webapp-signup/register\n\n\
After you have set up your LDAP account, you can retrieve the data using TransWeb.\n\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n"
#define TRANSWEB_REL_MSG \
"\n\nYou can retrieve the data using TransWeb.\n\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n"
#define TRANS_FTP_REL_MSG \
"\n\nYou can retrieve the data using InterCom.\n\
URL to down load InterCom http://intercom.freescale.net\n\n"
#define INTERCOM_ONLY_MSG \
"\n\nYou can retrieve the data using InterCom (Inter-Company Data Transfer).  \n\
If needed, your NXP contact can send you a copy of Intercom.\n\n\
Notes:\n\n\
1. Before you can retrieve the data,  you must set up your External LDAP  account.  \n\n\
   URL for External LDAP registration: https://www.nxp.com/webapp-signup/register\n\n\
2. You must set up your External LDAP account using '%s' as your e-mail address to retrieve this data.\n\n\
3. If “Pending authorization” appears above, then data cannot be retrieved until the NXP manager approves.\n\n\
4. If you have multiple email addresses, you must repeat steps 1 and 2 for each address you want to use.\n\n\
5. Personal Secure keywords only work for the email address sent to.  They cannot be forwarded to others.\n\n\n\
After you have set up your LDAP account, you can retrieve the data using InterCom.\n\n\
If you have problems accessing/retrieving the data, contact the Transcend Team:\n\n\
 Email:  transbug@nxp.com\n\n"

#define TRANSNET_FILE_MSG1 \
"You have transferred data to one or more external TransNet\n\
customers.  The external customer must enter the personal secure\n\
keyword sent to him via TransNet and the file keyword\n\
present in this E-mail message to access/retrieve the data.\n\
Please send a copy of this message to the customer(s) who\n\
will be accessing the data.\n\n\
Message for Customer:\n\n\
%s has placed a file in the TransNet repository for you.\n\n\
        File name:  %s\n\
        Description: %s\n\
        File keyword: %s\n\
        File size:  %s\n\
        Expires:  %s\
         %s\n"
#define TRANSNET_FILE_MSG2 \
"You have transferred data to one or more external TransNet\n\
customers.  The external customer must enter the personal secure\n\
keyword sent to him via TransNet and the file keyword\n\
present in this E-mail message to access/retrieve the data.\n\
Please send a copy of this message to the customer(s) who\n\
will be accessing the data.\n\n\
Message for Customer:\n\n\
%s has placed a file in the TransNet repository for you.\n\n\
    File name:  %s\n\
    File keyword: %s\n\
    File size:  %s\n\
    Expires:  %s\
          %s\n"
#define TRANSWEB_FILE_MSG \
"You have transferred data to one or more customers using TransWeb or InterCom.\n\
The external customers you have transferred data to have received e-mail with \n\
the file and personal secure keywords for the data.\n\
\n\
The message sent to the customers is below the line.\n\
-----------------------------------------------------------------------\n\n\
%s has placed a file in the NXP TransWeb repository for you.\n\
\n\
    File name:  %s\n\
    Description:   %s\n\
    File size:  %s\n\
    File keyword: %s\n\
    Expires:  %s\n\
          %s\n\
\nNotes:\n\n\
1. Before you can retrieve the data,  you must set up your External LDAP  account.  \n\n\
   URL for External LDAP registration: https://www.nxp.com/webapp-signup/register\n\n\
2. You must set up your External LDAP account using '%s' as your e-mail address to retrieve this data.\n\n\
3. If “Pending authorization” appears above, then data cannot be retrieved until the NXP manager approves.\n\n\
4. If you have multiple email addresses, you must repeat steps 1 and 2 for each address you want to use.\n\n\
5. Personal Secure keywords only work for the email address sent to.  They cannot be forwarded to others.\n\n\n\
After you have set up your LDAP account, you can retrieve the data using TransWeb or InterCom.\n\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n\
For questions or problems, contact us:\n\n\
    E-mail:  transbug@nxp.com\n\n\
-----------------------------------------------------------------------\n\n"

/* Used in external_transweb.c */
#define INTERCOM_FILE_MSG \
"You have transferred data to one or more customers via the InterCom system.\n\
The external customers you have transferred data to have received e-mail with \n\
a personal secure keyword for the data.\n\
\n\
The message sent to the customers is below the line.\n\
-----------------------------------------------------------------------\n\
\n\
%s has placed a file in the NXP InterCom repository for you.\n\
\n\
    File name:  %s\n\
        Description:   %s\n\
    File size:  %s\n\
    Expires:  %s\n\
          %s\n\
\nYou can retrieve the data using InterCom (Inter-Company Data\n\
Transfer).  this software supports data transfer between\n\
NXP and our external partners.  If Intercom is not\n\
installed at your site, ask your NXP contact to send\n\
you a copy.  Intercom is available for Linux, Solaris, and\n\
Windows.\n\n\
Important notes:\n\
 - You must retrieve the data using the personal secure keyword.\n\
 - The personal secure keyword is present in this E-mail  message.\n\
 - You must be registered and login with the e-mail address the personal secure keyword was sent to.\n\
             To register for an account go to URL:\n\
https://www.nxp.com/webapp-signup/register\n\n\
For questions or problems, contact us:\n\n\
    E-mail:  transbug@nxp.com\n\n\
-----------------------------------------------------------------------\n\n"
#define REGISTRATION_URL_MSG \
"You must authenticate with your External LDAP account to use\n\ 
InterCom.\n\n\ 
             To register for an account go to URL:\n\
https://www.nxp.com/webapp-signup/register\n\n"
#endif /* ifdef for trans_server_msg_h */
