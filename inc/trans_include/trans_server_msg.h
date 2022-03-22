#ifndef trans_server_msg_h
#define trans_server_msg_h
/* Defines for Transcend and InterCom errors follow */

/* Used by internal InterCom server */
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
/* Used by Transcend */
#define FREE_REL_MSG \
"\n\nYou can retrieve the data using either Transcend or\n\
TransWeb.  Transcend has a command line interface, and\n\
TransWeb has a web interface.  \n\n\
You must sign in or be registered as %s to retrieve the data.\n\
URL to access data:    https://nww.transweb.nxp.com/index.cgi?go=KEYWORD&KEYWORD=%s\n\n\
URL to access TransWeb:  https://nww.transweb.nxp.com\n\n\
URL to down load Transcend:  http://nww.transcend.nxp.com\n\n"

/* Used by External TransWeb/InterCom */
#define TO_UNAPPROVED_EXTERNAL_REL_MSG \
"WARNING:          \n\n\
You have not yet been approved to retrieve data from NXP.  data cannot be retrieved until the NXP manager approves.\n\
You will receive additional e-mail notification when approval has been performed.\n\
\nNotes:\n\n\
1. Before you can retrieve the data,  you must set up yourNXP  account.  \n\n\
   URL for NXP registration: https://www.nxp.com/webapp-signup/register\n\n\
2. You must set up your NXP account using '%s' as your e-mail address to retrieve this data.\n\n\
3. If “Pending authorization” appears above, then data cannot be retrieved until the NXP manager approves.\n\n\
4. If you have multiple email addresses, you must repeat steps 1 and 2 for each address you want to use.\n\n\
5. Personal Secure keywords only work for the email address sent to.  They cannot be forwarded to others.\n\n\n\
After you have set up your NXP account, you can retrieve the data using TransWeb or InterCom.\n\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n\
For questions or problems, contact us:\n\n\
    E-mail:  transbug@nxp.com\n\n"

/* Used by External TransWeb/InterCom */
#define TO_APPROVED_EXTERNAL_REL_MSG \
"\n\nNotes:\n\n\
1. Before you can retrieve the data,  you must set up your NXP  account.  \n\n\
   URL for NXP registration: https://www.nxp.com/webapp-signup/register\n\n\
2. You must set up your NXP account using '%s' as your e-mail address to retrieve this data.\n\n\
3. If you have multiple email addresses, you must repeat steps 1 and 2 for each address you want to use.\n\n\
4. Personal Secure keywords only work for the email address sent to.  They cannot be forwarded to others.\n\n\n\
After you have set up your NXP account, you can retrieve the data using TransWeb or InterCom.\n\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n\
For questions or problems, contact us:\n\n\
    E-mail:  transbug@nxp.com\n\n"
/* Used by External TransWeb/InterCom */
#define TO_NXP_REL_MSG \
"\n\nYou can retrieve the data using either InterCom or\n\
TransWeb.  InterCom has a command line interface, and\n\
TransWeb has a web interface.  \n\
You must sign in or be registered as %s to retrieve the data.\n\n\
URL to access data:    https://nww.transweb.nxp.com/index.cgi?go=KEYWORD&KEYWORD=%s\n\n\
URL to access TransWeb: https://nww.transweb.nxp.com\n\n\
URL to down load InterCom http://nww.intercom.nxp.com\n\n"
#ifdef garbage
#define TRANSWEB_REG_MSG \
"\n\nBefore you can retrieve the data,  you must set up your\n\
NXP  account.  In order to use the secure keyword\n\
provided above,  you must set up your  account  using\n\
'%s' as your e-mail address.  If you\n\
have a different e-mail address and prefer to use it,  let\n\
the NXP customer who sent you the personal secure keyword know\n\
the e-mail address you will be using so he can send you a\n\
different personal secure keyword.\n\n\
URL for NXP registration:\n\n\
    https://www.nxp.com/webapp-signup/register\n\n\
After you have set up your NXP account, you can retrieve the data using TransWeb.\n\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n"

#define TRANSWEB_REL_MSG \
"\n\nYou can retrieve the data using TransWeb.\n\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n"
#endif /* for garbage */

#define TRANS_FTP_REL_MSG \
"\n\nYou can retrieve the data using InterCom.\n\
URL to down load InterCom http://nww.intercom.nxp.com\n\n"
#define INTERCOM_ONLY_MSG \
"\n\nYou can retrieve the data using InterCom (Inter-Company Data Transfer).  \n\
If needed, your NXP contact can send you a copy of Intercom.\n\n\
Notes:\n\n\
1. Before you can retrieve the data,  you must set up your NXP  account.  \n\n\
   URL for NXP registration: https://www.nxp.com/webapp-signup/register\n\n\
2. You must set up your NXP account using '%s' as your e-mail address to retrieve this data.\n\n\
3. If “Pending authorization” appears above, then data cannot be retrieved until the NXP manager approves.\n\n\
4. If you have multiple email addresses, you must repeat steps 1 and 2 for each address you want to use.\n\n\
5. Personal Secure keywords only work for the email address sent to.  They cannot be forwarded to others.\n\n\n\
After you have set up your NXP account, you can retrieve the data using InterCom.\n\n\
If you have problems accessing/retrieving the data, contact us:\n\n\
 Email:  transbug@nxp.com\n\n"

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
1. Before you can retrieve the data,  you must set up your NXP  account.  \n\n\
   URL for NXP registration: https://www.nxp.com/webapp-signup/register\n\n\
2. You must set up your NXP account using '%s' as your e-mail address to retrieve this data.\n\n\
3. If “Pending authorization” appears above, then data cannot be retrieved until the NXP manager approves.\n\n\
4. If you have multiple email addresses, you must repeat steps 1 and 2 for each address you want to use.\n\n\
5. Personal Secure keywords only work for the email address sent to.  They cannot be forwarded to others.\n\n\n\
After you have set up your NXP account, you can retrieve the data using TransWeb or InterCom.\n\n\
URL to access data: https://www.nxp.com/transweb?%s\n\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\n\
For questions or problems, contact us:\n\n\
    E-mail:  transbug@nxp.com\n\n\
-----------------------------------------------------------------------\n\n"
#define TRANSWEB_UNAPPROVED_MSG \
"You have transferred data to one or more unapproved external customers using TransWeb or InterCom.  The external customers  you have transferred data to have received e-mail with the file and personal secure keywords for the data.  However, they will not be able to retrieve the data until approval has been performed.  Please have your manager perform the following steps to approve the customers:\n\
    1.  Go to http://nww.edc.nxp.com and sign in.\n\
    2.  Search for the customer.\n\
    3.  Click on the customer's name.\n\
    4.  Select Approve this Email.\n\
    5.  Click on submit.----------------------------\n\
\n\
A message indicating the customer has been approved is then displayed.\n\
\n\
----------------------------\n\
\n\
The message sent to the customer(s) is shown below:\n\
\n\
%s has placed a file in the NXP TransWeb repository for you.  \n\
\n\
     File name:  %s\n\
    Description:   %s\n\
    File size:  %s\n\
    File keyword: %s\n\
    Expires:  %s\n\n\
WARNING:          \n\n\
You have not yet been approved to retrieve data from NXP.  data cannot be retrieved until the NXP manager approves.\n\
You will receive additional e-mail notification when approval has been performed.\n\
\n\
Notes:\n\
\n\
1. Before you can retrieve the data,  you must set up your NXP account.  \n\
\n\
   URL for NXP registration: https://www.nxp.com/webapp-signup/register\n\
\n\
2. You must set up your NXP account using '%s' as your e-mail address to retrieve this data.\n\
\n\
\n\
3. If you have multiple email addresses, you must repeat steps 1 and 2 for each address you want to use.\n\
\n\
4. Personal Secure keywords only work for the email address sent to.  They cannot be forwarded to others.\n\
\n\
After you have set up your NXP account and been approved, you can retrieve the data using TransWeb or InterCom.\n\
\n\
URL to access data: https://www.nxp.com/transweb?%s\n\
\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\
\n\
For questions or problems, contact us:\n\
\n\
    E-mail:  transbug@nxp.com\n\
\n\
---------------------------------------------------------------------\n\n"

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
"You must authenticate with your NXP account to use\n\
InterCom.\n\n\
             To register for an account go to URL:\n\
https://www.nxp.com/webapp-signup/register\n\n"
#endif /* ifdef for trans_server_msg_h */
