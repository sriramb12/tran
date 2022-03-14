#ifndef email_msg_h
#define email_msg_h

#define MM_INTERNAL_MSG \
"Dear Customer,\n\
\n\
In reply to your Service Request SR %s:\n\
NXP Support has placed a file in the NXP TransWeb system for you.\n\
\n\
    File name:     %s\n\
    Description:   %s\n\
    File size:     %s\n\
    Personal secure keyword:  %s\n\
    Expires:       %s\n\
                     %s\n\
\n\
You can retrieve the data using either Transcend or\n\
TransWeb.  Transcend has a command line interface, and\n\
TransWeb has a web interface.  \n\n\
URL to down load Transcend http://transcend.freescale.net\n\
URL to access TransWeb: https://nww.transweb.nxp.com\n\
URL to access keyword: https://nww.transweb.nxp.com/index.cgi?go=KEYWORD&KEYWORD=%s\n\
\n\
You must be registered and login with the e-mail address this message was addressed to.\n\
\n\
\n\
Should you need to contact us with regard to this message, please see \n\
the notes below.\n\
\n\
Best Regards,\n\
NXP Support Team\n\n"

#define MM_EXTERNAL_MSG \
"Dear Customer,\n\
\n\
In reply to your Service Request SR %s:\n\
NXP Support has placed a file in the NXP TransWeb system for you.\n\
\n\
    File name:     %s\n\
    Description:   %s\n\
    File size:     %s\n\
    Personal secure keyword:  %s\n\
    File keyword:  %s\n\
    Expires:       %s\n\
                     %s\n\
\n\
You can retrieve the data using NXP TransWeb.\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\
URL to access keyword: https://www.nxp.com/transweb?%s\n\
\n\
Important notes:\n\
 - You must have both the personal secure keyword and file keyword to access the data.\n\
 - Both keywords are present in this E-mail  message.\n\
 - You must be registered and login with the e-mail address this message was addressed to.\n\
\n\
\n\
Should you need to contact us with regard to this message, please see the notes below.\n\
\n\
Best Regards,\n\
NXP Support Team\n\n"

#define MM_FILE_MSG \
"Hello,\n\
\n\
You have transferred NXP Support related data to one or more customers via the Transweb system.\n\
\n\
To maintain proper security, the external customer must enter both \n\
the personal secure and file keywords sent to him automatically via \n\
TransWeb to access/retrieve the data.  \n\
\n\
The message sent to the external customer(s) is given below.\n\
The personal secure keyword(s) sent to the customer(s) are given at the end of this e-mail message.\n\
If the customer(s) do not receive the e-mail message, you can send a copy of the message given below.\n\
Pay attention to the “From” address you are using. If you prefer to send as “NXP Support”, you need to\n\
- have the rights registered to use the CoreID “support1” for sending e-mails (request here: http://compass.freescale.net/go/support1) \n\
- enter “support1” in the FROM field of the e-mail\n\
\n\
The Message for your customer is below the line:\n\
-----------------------------------------------------------------------\n\
\n\
Dear Customer,\n\
\n\
In reply to your Service Request SR %s (see details below):\n\
NXP Support has placed a file in the NXP TransWeb repository for you.\n\
\n\
 - File name:  %s\n\
    Description:   %s\n\
File size:  %s\n\
File keyword: %s\n\
Expires:  %s\n\
          %s\n\
\n\
You can retrieve the data using NXP TransWeb.\n\
URL to access TransWeb: https://www.nxp.com/transweb\n\
URL to access keyword: https://www.nxp.com/transweb?%s\n\
\n\
Important notes:\n\
 - You must have both the personal secure keyword and file keyword to access the data.\n\
 - Both keywords are present in this e-mail message.\n\
 - You must be registered and login with the e-mail address the secure keyword was sent to.\n\
\n\
Should you need to contact us with regard to this message, please see the notes below.\n\
\n\
Best Regards,\n\
NXP Support Team\n\n"

#define MM_CONTACT_INFO \
"To contact us with regard to this message, please update your Service Request SR# %s at this URL:\n\
http://www.nxp.com/webapp/servicerequest.srviewdetails.framework?serviceRequestNumber=%s \n\
\n\
Alternatively you may direct your browser to http://www.nxp.com/support and follow the \"Technical Support\" link.\n\
Your updated request will then be automatically directed to the assigned engineer for prompt attention. \n\
\n\
Using our web self-service area, you may:\n\
  -  add information or comments to your SR\n\
  -  check on the status of your SR\n\
  -  re-open a SR in \"closed\" status\n\
  -  submit a different question as a new SR \n\
\n\
NOTE: If you are not familiar with this system follow the \"Sign up now\" link and use the e-mail address used in the TO field of this message as your e-mail address.\n\
Please check out our Frequently Asked Questions prior to submitting your request to the Technical Helpline. \n\
If you ever experience any problems with our web self-service application, we apologize in advance and would ask that you please telephone us at one of the following locations:\n\
\n\
Americas  1-800-521-6274  8AM-6PM Central Time \n\
\n\
Europe      9AM-5PM Central European Time\n\
  English  +44 1296 380 456\n\
  German  +49 89 92103 559\n\
  French  +33 1 69 35 48 48 \n\
\n\
Asia \n\
  Asia Pacific   +800 2666 8080   9AM-6PM Hong Kong time\n\
  China mainland  800 990 8188   9AM-6PM Hong Kong time\n\
  Hongkong        2666 8080      9AM-6PM Hong Kong time\n\
  Noida           000 800 852 1155  9AM-6PM Hong Kong time\n\
  Japan           0120 191 014   8AM-5PM Tokyo time\n\
\n\
For your convenience we also have an e-mail address (support@nxp.com) which may be used to provide further information regarding an open service request. Please include the SR number in the subject line to prevent any delays in processing this information. \n\
If you need to open a new service request please visit our Technical Support web site listed above.  Using the e-mail address to generate a new service request will cause a delay in our response time.\n\
\n\
Thank you again for your interest in NXP Semiconductor products.\n\
\n\
Best Regards,\n\
The Technical Support Team\n\
NXP Semiconductor\n\
\n\
\n\
The information contained in this e-mail is classified as:\n\
[x] General business information \n\
[ ] NXP Semiconductor internal use only \n\
[ ] NXP Semiconductor confidential proprietary\n\n"

#define MM_REG_MSG \
"\n\nBefore you can retrieve the data,  you must set up your\n\
External LDAP  account.  In order to upersonal se the secure keyword\n\
provided above,  you must set up your  account  using\n\
'%s' as your e-mail address.  If you\n\
have a different e-mail address and prefer to use it,  let\n\
the NXP customer who sent you the personal secure keyword know\n\
the e-mail address you will be using so he can send you a\n\
different personal secure keyword.\n\n\
URL for External LDAP registration:\n\n\
    https://www.nxp.com/webapp-signup/register\n\n\n"

#define SUPPORT_MSG \
	    "File '%s' retrieved.\n\n  This file contains a list of customers in the requested e-mail group."

#endif
