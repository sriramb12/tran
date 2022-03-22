#ifndef transapi_msg_h
#define transapi_msg_h

/* InterCom messages */
/* Message put in $HOME/trans.log file */
#define LOG_CWD_MSG "# InterCom puts logged in current working directory."
#define TRANS_FORMAT_ERR "Error -101:  Invalid input format entered.\n\n"
#ifdef TRANSCEND
/* Transapi banner */
#define TRANSAPI_BANNER \
"\n            Welcome to version %s of %s\n\
               (Transfer/Send Data)\n\
          Copyright 1994-2020 NXP Semiconductor N.V.\n\n\
This software is the property of NXP.  Authorized persons\n\
may use NXP software only for approved purposes.  Misuse\n\
or misappropriation of such resources is prohibited.  By using\n\
this service you agree that all items and data to be sent/received\n\
by %s are in compliance with existing NXP export \n\
regulations and licenses.\n\n\
NXP reserves the right to audit, access and inspect electronic\n\
communications and data created, stored, or transmitted in accordance\n\
with applicable law. \n\n\
BY USING THIS SERVICE YOU ARE ACKNOWLEDGING AND CONSENTING TO THE \n\
PROVISIONS OF THIS NOTICE AND APPLICABLE NXP INTERNAL POLICIES.\n\
IF YOU ARE NOT AN AUTHORIZED USER, PLEASE DISCONTINUE THE USE OF THIS\n\
SERVICE NOW. \n\n\
This product includes cryptographic software developed by the OpenSSL Project\n\
for use in the OpenSSL Toolkit. (http://www.openssl.org/)\n\n"

#define TRANS_FORMAT_MSG \
"To put a file:             Type '%s -p <file>'\n\
To get a file:             Type '%s -g <keyword>'\n\
To delete a file:          Type '%s -d <keyword>'\n\
To see file info:          Type '%s -i <keyword>'\n\
To see file history:       Type '%s -h <keyword>'\n\
To see advanced commands:  Type '%s -a'\n\
To view put/get options:   Type '%s -A'\n\
To display command help:   Type '%s -H -<command>'\n\n"

/* message for advanced commands */
#define ADV_MSG \
"Explanation of Parameters used below:\n\n\
    <email>   - One or more email addresses i.e. transbug@nxp.com\n\
    <keyword> - 9 character keyword returned from previous command.\n\n\
Optional arguments are enclosed in brackets.\n\n\
View defined Transcend sites:             '%s -x'\n\
View defined Transcend repositories:      '%s -X'\n\
Display Transcend server status:          '%s -S'\n\
Display detailed help on a command:       '%s -H -<command>'\n\
Obtain user's registration status:        '%s -o [-<site>]'\n\
Perform auto registration:                '%s -R <core ID> <password>'\n\
View program version  & other settings:   '%s -v'\n\
Retrieve new releases or documentation:   '%s -r [<file>]'\n\
Extend file expiration 1-3 extra days:    '%s -e <keyword> <days>'\n\
Display original and stored file length:  '%s -l <keyword>'\n\
Allow only the owner to delete the data:  '%s -D <keyword>'\n\
Allow deletion by any user with keyword:  '%s -E <keyword>'\n\
Lock data to prevent data retrieval:      '%s -L <keyword>'\n\
Unlock data and allow data retrieval:     '%s -U <keyword>'\n\
Mail multiple recipients secure keyword:  '%s -m <keyword> <email>'\n\
Delete data for all user keywords:        '%s -d all'\n\
View list of secure keywords received:    '%s -V [-<site>]  [all]'\n\
Query list of secure keywords received:   '%s -Q [-<site>]  [all]'\n\
Query long list of secure keywords:       '%s -Ql [-<site>]'\n\
Query list of file keywords transferred:  '%s -q [-<site>] | [all]'\n\
Query long list of file keywords:         '%s -ql [-<site>]'\n\
Set Email notification for all files:     '%s -s all [-<site>]'\n\
Cancel Email notification for all files:  '%s -c all [-<site>]'\n\
Set Email notification for one file:      '%s -s <keyword>'\n\
Cancel Email notification for one file:   '%s -c <keyword>'\n\
Retrieve Benoit file list for data:       '%s -b <keyword>'\n\
Display Benoit file list for data:        '%s -B <keyword>'\n\
Just retrieve associated file name:       '%s -J <keyword>'\n\
Ping dropbox machine:                     '%s -w'\n\
Silent ping of dropbox machine:           '%s -W'\n\
View name and friendly e-mail address :   '%s -C [-<site>] <Core ID>'\n\
View LDAP/Transcend registration entry:   '%s -u [-<site>] <Core ID>'\n\
View LDAP directory user information:     '%s -1 [-<site>] <Core ID>'\n\
Just request external customer approval:  '%s -j <customer e-mail>'\n\
Yes, perform external customer approval:  '%s -y <customer e-mail>'\n\
Release unused shared memory:             '%s -M'\n\
View iCAP classification command options: '%s -I'\n\n"

#define PUT_GET_MSG \
"Explanation of parameters used below:\n\
\n\
    <dir> - Directory name, may include absolute or relative path.\n\
    <email>...  - Multiple email addresses i.e. b36283, b36283, etc.\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - File with list of file names separated by carriage returns.\n\
    <keyword> - 9 character keyword returned from previous Transcend command.\n\
    <site> - Server sites:  aus | mun | eind | blr | idc | dtms.\n\
    <icap> - iCAP classification:  gbi | pubi | fiuo | fcp.\n\
\n\
Optional arguments are enclosed in brackets.\n\
\n\
Delete data after the current retrieval:  '%s -G  <keyword>'\n\
Retrieve data as a compressed file:       '%s -gz <keyword>'\n\
Retrieve data using parallel transfer:    '%s -gl <keyword>'\n\
Retrieve data using serial transfer:      '%s -gs <keyword>'\n\
Retrieve in blocks with retry option:     '%s -gr <keyword>'\n\
Retrieve forcing unbundling with tar:     '%s -gt <keyword>'\n\
Put directory forcing bundling with tar:  '%s -pt <file>'\n\
Put data using serial transfer:           '%s -ps <file>'\n\
Put data using parallel transfer:         '%s -pl <file>'\n\
Put data using the retry option:          '%s -pr <file>'\n\
Put data as an uncompressed file:         '%s -pu <file>'\n\
Put data on an alternate site:            '%s -p <- site> <file> '\n\
Put data and assign iCAP classification:  '%s -p -<site> -<icap> <file> '\n\
Put data  & send email to multiple users: '%s -p [-<site>] <file> <email>'\n\
Put directory and follow symbolic links:  '%s -t [-<site>] <dir> [<email>]'\n\
Put directory & do not follow sym links:  '%s -p [-<site>] <dir> [<email>]'\n\
Put multiple files with one transaction:  '%s -f [-<site>] <file....'\n\
Package & put files into directory:       '%s -k [-<site>] <dir> <file>...'\n\
Put files listed in named file:           '%s -n [-<site>] <file list>'\n\
Zip & put listed files into directory:    '%s -z [-<site>] <dir> <file list>'\n\
Delete data after the first retrieval:     Use -F, -K, -N, -P, -T or -Z .\n\n"
#define TRANS_RELEASE_MSG \
"To obtain Transcend documentation:   Type '%s -r trans.doc'\n\
To obtain Transcend release notes:   Type '%s -r trans.rel'\n\
To retrieve an ASCII man page:       Type '%s -r trans.a'\n\
To retrieve a troff man page:        Type '%s -r trans.1'\n\
To retrieve the Linux 64 version:    Type '%s -r linux64trans'\n\
To retrieve the Linux 32 version:    Type '%s -r linux32trans'\n\
To retrieve the RH5 version:         Type '%s -r linux5trans'\n\
To retrieve the RH4 version:         Type '%s -r linux5trans'\n\
To retrieve the Solaris 10 version:  Type '%s -r solaris10trans'\n\
To retrieve the Solaris 8 version:   Type '%s -r solaris8trans'\n\
To retrieve the MacOS X version:     Type '%s -r macosXtrans_trans'\n\
To retrieve the Fedora version:      Type '%s -r fedora_trans'\n\
For a sample .transrc file:          Type '%s -r .transrc'\n\
To retrieve the Windows GUI:         Type '%s -r Transcend.exe'\n\
For Windows command line version:    Type '%s -r Transcend.tar'\n\
After retrieving a Unix release:     Rename the release '%s'\n\n"
#endif

#ifdef INTERCOM
#ifdef NXP
/* messages for internal InterCom */
#define TRANSAPI_BANNER \
"\n            Welcome to version %s of %s\n\
               (Inter-Company Data Transfer)\n\
                    Internal release\n\
                For use by NXP employees\n\
          Copyright 1994-2020 NXP Semiconductor N.V.\n\n\
This software is the property of NXP.  Authorized persons\n\
may use NXP software only for approved purposes.  Misuse\n\
or misappropriation of such resources is prohibited.  By using\n\
this service you agree that all items and data to be sent/received\n\
by %s are in compliance with existing NXP export \n\
regulations and licenses.\n\n\
NXP reserves the right to audit, access and inspect electronic\n\
communications and data created, stored, or transmitted in accordance\n\
with applicable law. \n\n\
BY USING THIS SERVICE YOU ARE ACKNOWLEDGING AND CONSENTING TO THE \n\
PROVISIONS OF THIS NOTICE AND APPLICABLE NXP INTERNAL POLICIES.\n\
IF YOU ARE NOT AN AUTHORIZED USER, PLEASE DISCONTINUE THE USE OF THIS\n\
SERVICE NOW. \n\n\
This product includes cryptographic software developed by the OpenSSL Project\n\
for use in the OpenSSL Toolkit. (http://www.openssl.org/)\n\n"

#define TRANS_FORMAT_MSG \
"To put a file:              Type '%s -p -<company> <file>'\n\
For company abbreviations:  Type '%s -x'\n\
To get a file:              Type '%s -g <keyword>'\n\
To delete a file:           Type '%s -d <keyword>'\n\
To see file info:           Type '%s -i <keyword>'\n\
To see file history:        Type '%s -h <keyword>'\n\
To see advanced commands:   Type '%s -a'\n\
To view put/get options:    Type '%s -A'\n\
To display command help:    Type '%s -H -<command>'\n\n"
#define ADV_MSG \
"Explanation of Parameters used below:\n\n\
    <email>   - One or more email addresses i.e. transbug@nxp.com\n\
    <keyword> - 9 character keyword returned from previous command.\n\n\
Optional arguments are enclosed in brackets.\n\n\
View defined InterCom sites:              '%s -x'\n\
Viewdefined InterCom repositories:        '%s -X'\n\
Display InterCom server status:           '%s -S'\n\
Display detailed help on a command:       '%s -H -<command>'\n\
Obtain user's registration status:        '%s -o [-<site>]'\n\
View program version  & other settings:   '%s -v'\n\
Retrieve new releases or documentation:   '%s -r [<file>]'\n\
Extend file expiration 1-3 extra days:    '%s -e <keyword> <days>'\n\
Display original and stored file length:  '%s -l <keyword>'\n\
Allow only the owner to delete the data:  '%s -D <keyword>'\n\
Allow deletion by any user with keyword:  '%s -E <keyword>'\n\
Lock data to prevent data retrieval:      '%s -L <keyword>'\n\
Unlock data and allow data retrieval:     '%s -U <keyword>'\n\
Mail multiple recipients secure keyword:  '%s -m <keyword> <email>'\n\
Delete data for all user keywords:        '%s -d all'\n\
Query list of secure keywords received:   '%s -Q'\n\
Query long list of secure keywords:       '%s -Ql'\n\
Query list of file keywords transferred:  '%s -q'\n\
Query long list of file keywords:         '%s -ql'\n\
Set Email notification for all files:     '%s -s all'\n\
Cancel Email notification for all files:  '%s -c all'\n\
Set Email notification for one file:      '%s -s <keyword>'\n\
Cancel Email notification for one file:   '%s -c <keyword>'\n\
Retrieve Benoit file list for data:       '%s -b <keyword>'\n\
Display Benoit file list for data:        '%s -B <keyword>'\n\
Just retrieve associated file name:       '%s -J <keyword>'\n\
Ping dropbox machine:                     '%s -w'\n\
Silent ping of dropbox machine:           '%s -W'\n\
View LDAP/InterCom registration entry:    '%s -u [-<site>] <Core ID>'\n\
View LDAP directory user information:     '%s -1 [-<site>] <Core ID>'\n\
Just request external customer approval:  '%s -j <customer e-mail>'\n\
Yes, perform external customer approval:  '%s -y <customer e-mail>'\n\
Release unused shared memory:             '%s -M'\n\
View iCAP classification command options: '%s -I'\n\n"

#define PUT_GET_MSG \
"Explanation of parameters used below:\n\n\
    <email> - Multiple email addresses i.e. transbug@nxp.com\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - File with list of file names separated by carriage returns.\n\
    <keyword> - 9 character keyword returned from previous command.\n\
    <company> - Three letter company abbreviation.\n\
    <icap> - iCAP classification.\n\n\
Optional arguments are enclosed in brackets.\n\n\
Delete data after the current retrieval:  '%s -G  <keyword>'\n\
Retrieve data as a compressed file:       '%s -gz <keyword>'\n\
Retrieve data using parallel transfer:    '%s -gl <keyword>'\n\
Retrieve data using serial transfer:      '%s -gs <keyword>'\n\
Retrieve in blocks with retry option:     '%s -gr <keyword>'\n\
Retrieve forcing unbundling with tar:     '%s -gt <keyword>'\n\
Put forcing bundling with tar:            '%s -pt <company> <file>'\n\
Put data using serial transfer:           '%s -ps <company> <file>'\n\
Put data using parallel transfer:         '%s -pl <company> <file>'\n\
Put data using the retry option:          '%s -pr <company> <file>'\n\
Put data as an uncompressed file:         '%s -pu <company> <file>'\n\
Put data and assign iCAP classification:  '%s -p -<company> -<icap> <file> '\n\
Put data  & send email to multiple users: '%s -p -<company> <file> <email>'\n\
Put directory and follow symbolic links:  '%s -t -<company> <dir> [<email>]'\n\
Put directory & do not follow sym links:  '%s -p -<company> <dir> [<email>]'\n\
Put multiple files with one transaction:  '%s -f -<company> <file>...'\n\
Package & put files into directory:       '%s -k -<company> <dir> <file>...'\n\
Put files listed in named file:           '%s -n -<company> <file list> [<email>]'\n\
Zip & put listed files into directory:    '%s -z -<company> <dir> <file list> [<email>]'\n\
Send duplicate files to the mask shop:    '%s -O -<company> <file>'\n\
Resend encrypted data to the foundry:     '%s -6 <keyword>'\n\
Delete data after the first retrieval:     Use -F, -K, -N, -P, -T or -Z .\n\n"

#define TRANS_RELEASE_MSG \
"To obtain InterCom documentation:   Type '%s -r trans.doc'\n\
To obtain InterCom release notes:   Type '%s -r trans.rel'\n\
To retrieve an ASCII man page:       Type '%s -r trans.a'\n\
To retrieve a troff man page:        Type '%s -r trans.1'\n\
To retrieve the Linux 64 version:    Type '%s -r linux64trans'\n\
To retrieve the Linux 32 version:    Type '%s -r linux32trans'\n\
To retrieve the RH5 version:         Type '%s -r linux5trans'\n\
To retrieve the RH4 version:         Type '%s -r linux5trans'\n\
To retrieve the Solaris 10 version:  Type '%s -r solaris10trans'\n\
To retrieve the Solaris 8 version:   Type '%s -r solaris8trans'\n\
To retrieve the MacOS X version:     Type '%s -r macosXtrans_trans'\n\
To retrieve the Fedora version:      Type '%s -r fedora_trans'\n\
For a sample .transrc file:          Type '%s -r .transrc'\n\
To retrieve the Windows GUI:         Type '%s -r InterCom.exe'\n\
For Windows command line version:    Type '%s -r InterCom.tar'\n\
After retrieving a Unix release:     Rename the release '%s'\n\n"
#endif
/* Messages for external InterCom */
#ifdef EXTERNAL
#define TRANSAPI_BANNER \
"\n            Welcome to version %s of %s\n\
               (Inter-Company Data Transfer)\n\
                    External release\n\
                 For use by external partners\n\
          Copyright 1994-2020 NXP Semiconductor N.V.\n\n\
This software is the property of NXP.  Authorized persons\n\
may use NXP software only for approved purposes.  Misuse\n\
or misappropriation of such resources is prohibited.  By using\n\
this service you agree that all items and data to be sent/received\n\
by %s are in compliance with existing NXP export \n\
regulations and licenses.\n\n\
NXP reserves the right to audit, access and inspect electronic\n\
communications and data created, stored, or transmitted in accordance\n\
with applicable law. \n\n\
BY USING THIS SERVICE YOU ARE ACKNOWLEDGING AND CONSENTING TO THE \n\
PROVISIONS OF THIS NOTICE AND APPLICABLE NXP INTERNAL POLICIES.\n\
IF YOU ARE NOT AN AUTHORIZED USER, PLEASE DISCONTINUE THE USE OF THIS\n\
SERVICE NOW. \n\n\
This product includes cryptographic software developed by the OpenSSL Project\n\
for use in the OpenSSL Toolkit. (http://www.openssl.org/)\n\n"

#define TRANS_FORMAT_MSG \
"To put a file:             Type '%s -p <file>'\n\
To get a file:             Type '%s -g <keyword>'\n\
To delete a file:          Type '%s -d <keyword>'\n\
To see file info:          Type '%s -i <keyword>'\n\
To see file history:       Type '%s -h <keyword>'\n\
To see advanced commands:  Type '%s -a'\n\
To view put/get options:   Type '%s -A'\n\
To display command help:   Type '%s -H -<command>'\n\n"

/* message for advanced commands */
#define ADV_MSG \
"Explanation of Parameters used below:\n\n\
    <email>   - One or more email addresses i.e. transbug@nxp.com\n\
    <keyword> - 9 character keyword returned from previous command.\n\n\
Optional arguments are enclosed in brackets.\n\n\
Display InterCom server status:           '%s -S'\n\
Display detailed help on a command:       '%s -H -<command>'\n\
View NXP registration site URL:           '%s -R'\n\
View program version  & other settings:   '%s -v'\n\
Retrieve new releases or documentation:   '%s -r [<file>]'\n\
Extend file expiration 1-3 extra days:    '%s -e <keyword> <days>'\n\
Display original and stored file length:  '%s -l <keyword>'\n\
Allow only the owner to delete the data:  '%s -D <keyword>'\n\
Allow deletion by any user with keyword:  '%s -E <keyword>'\n\
Lock data to prevent data retrieval:      '%s -L <keyword>'\n\
Unlock data and allow data retrieval:     '%s -U <keyword>'\n\
Mail multiple recipients secure keyword:  '%s -m <keyword> <email>'\n\
Delete data for all user keywords:        '%s -d all'\n\
Query list of secure keywords received:   '%s -Q'\n\
Query long list of secure keywords:       '%s -Ql'\n\
Query list of file keywords transferred:  '%s -q'\n\
Query long list of file keywords:         '%s -ql'\n\
Set Email notification for all files:     '%s -s all'\n\
Cancel Email notification for all files:  '%s -c all'\n\
Set Email notification for one file:      '%s -s <keyword>'\n\
Cancel Email notification for one file:   '%s -c <keyword>'\n\
Retrieve Benoit file list for data:       '%s -b <keyword>'\n\
Display Benoit file list for data:        '%s -B <keyword>'\n\
Just retrieve associated file name:       '%s -J <keyword>'\n\
Ping dropbox machine:                     '%s -w'\n\
Silent ping of dropbox machine:           '%s -W'\n\
Release unused shared memory:             '%s -M'\n\
View iCAP classification command options: '%s -I'\n\
Generate '.icomrc' in the home directory: '%s -rc [account-email]'\n\n"

#define PUT_GET_MSG \
"Explanation of parameters used below:\n\
\n\
    <dir> - Directory name, may include absolute or relative path.\n\
    <email>...  - Multiple email addresses i.e. b36283, b36283, etc.\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - File with list of file names separated by carriage returns.\n\
    <keyword> - 9 character keyword returned from previous Transcend command.\n\
    <icap> - iCAP classification:  gbi | pubi | fiuo | fcp.\n\
\n\
Optional arguments are enclosed in brackets.\n\
\n\
Delete data after the current retrieval:  '%s -G  <keyword>'\n\
Retrieve data as a compressed file:       '%s -gz <keyword>'\n\
Retrieve data using parallel transfer:    '%s -gl <keyword>'\n\
Retrieve data using serial transfer:      '%s -gs <keyword>'\n\
Retrieve in blocks with retry option:     '%s -gr <keyword>'\n\
Retrieve forcing unbundling with tar:     '%s -gt <keyword>'\n\
Put directory forcing bundling with tar:  '%s -pt <file>'\n\
Put data using serial transfer:           '%s -ps <file>'\n\
Put data using parallel transfer:         '%s -pl <file>'\n\
Put data using the retry option:          '%s -pr <file>'\n\
Put data as an uncompressed file:         '%s -pu <file>'\n\
Put data and assign iCAP classification:  '%s -p -<icap> <file> '\n\
Put data  & send email to multiple users: '%s -p <file> <email>'\n\
Put directory and follow symbolic links:  '%s -t <dir> [<email>]'\n\
Put directory & do not follow sym links:  '%s -p <dir> [<email>]'\n\
Put multiple files with one transaction:  '%s -f <file....'\n\
Package & put files into directory:       '%s -k <dir> <file>...'\n\
Put files listed in named file:           '%s -n <file list>'\n\
Zip & put listed files into directory:    '%s -z <dir> <file list>'\n\
Delete data after the first retrieval:     Use -F, -K, -N, -P, -T or -Z .\n\n"
#define TRANS_RELEASE_MSG \
"To obtain InterCom documentation:    Type '%s -r trans.doc'\n\
To obtain InterCom release notes:    Type '%s -r trans.rel'\n\
To retrieve an ASCII man page:       Type '%s -r trans.a'\n\
To retrieve a troff man page:        Type '%s -r trans.1'\n\
To retrieve the Linux 64 version:    Type '%s -r linux64trans'\n\
To retrieve the Linux 32 version:    Type '%s -r linux32trans'\n\
To retrieve the RH5 version:         Type '%s -r linux5trans'\n\
To retrieve the RH4 version:         Type '%s -r linux5trans'\n\
To retrieve the Solaris 10 version:  Type '%s -r solaris10trans'\n\
To retrieve the Solaris 8 version:   Type '%s -r solaris8trans'\n\
To retrieve the MacOS X version:     Type '%s -r macosXtrans_trans'\n\
To retrieve the Fedora version:      Type '%s -r fedora_trans'\n\
For a sample .icomrc file:           Type '%s -r .transrc'\n\
To retrieve the Windows GUI:         Type '%s -r Transcend.exe'\n\
For Windows command line version:    Type '%s -r Transcend.tar'\n\
After retrieving a Unix release:     Rename the release '%s'\n\n"

#endif /* for External Intercom */
#endif/* For Intercom */

#define ADMIN_MSG \
"Manual registration:    '%s -M <login> <subnet> <email1> <email2>'\n\
Registration reset:     '%s -M  <login> <subnet> reset\n\
Registration deletion:  '%s -M  <login> <subnet> delete\n\
Ping drop box machine:  '%s -w\n\
Silent ping to dropbox: '%s -W'\n\n"
#define CUST_TRANS_FORMAT_MSG \
"Error -101:  Invalid input format entered.\n\n"
#define CUST_MSG \
"To add users to Mica Support group:     Type '%s -4a Userid1 UserITo d2 ...'\n\
To delete users from Mica Support:      Type '%s -4d UserID1 UserID2...'\n\
To view users in Mica Support group:    Type '%s -4v users'\n\n"

#endif /* Define for transapi_msg_h */
