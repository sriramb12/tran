#ifndef transcend_client_msg_h
#define transcend_client_msg_h

/* InterCom messages */
/* Message put in $HOME/trans.log file */
#define LOG_CWD_MSG "# InterCom puts logged in current working directory."
/* InterCom banner */
#define TRANS_BANNER \
"\n            Welcome to version %s of InterCom\n\
               (Inter-Company Data Transfer)\n\
                    Internal release\n\
                For use by NXP employees\n\
          Copyright 1994-2020 NXP Semiconductor N.V.\n\n\
This software is the property of NXP.  Authorized persons\n\
may use NXP software only for approved purposes.  Misuse\n\
or misappropriation of such resources is prohibited.  By using\n\
this service you agree that all items and data to be sent/received\n\
by InterCom are in compliance with existing NXP export \n\
regulations and licenses.\n\n\
NXP reserves the right to audit, access and inspect electronic\n\
communications and data created, stored, or transmitted in accordance\n\
with applicable law. \n\n\
BY USING THIS SERVICE YOU ARE ACKNOWLEDGING AND CONSENTING TO THE \n\
PROVISIONS OF THIS NOTICE AND APPLICABLE NXP INTERNAL POLICIES.\n\
IF YOU ARE NOT AN AUTHORIZED USER, PLEASE DISCONTINUE THE USE OF THIS\n\
SERVICE NOW. \n\n"
#define TRANS_FORMAT_ERR "Error -101:  Invalid input format entered.\n\n\
To put a file:              Type '%s -p [-<company abbr>] <file>'\n\
For company abbreviations:  Type '%s -x'\n\
To get a file:              Type '%s -g <keyword>'\n\
To delete a file:           Type '%s -d <keyword>'\n\
To see file info:           Type '%s -i <keyword>'\n\
To see file history:        Type '%s -h <keyword>'\n\
To see advanced commands:   Type '%s -a'\n\
To view put/get options:    Type '%s -A'\n\n"
#define FORMAT_MSG \
"To put a file:              Type '%s -p [-<company abbr>] <file>'\n\
For company abbreviations:  Type '%s -x'\n\
To get a file:              Type '%s -g <keyword>'\n\
To delete a file:           Type '%s -d <keyword>'\n\
To see file info:           Type '%s -i <keyword>'\n\
To see file history:        Type '%s -h <keyword>'\n\
To see advanced commands:   Type '%s -a'\n\
To view put/get options:    Type '%s -A'\n\n"

/* message for advanced commands */
#define ADV_MSG \
"Explanation of Parameters used below:\n\n\
    <email>   - Multiple email addresses i.e. transbug@nxp.com\n\
    <keyword> - 9 character keyword returned from previous InterCom command.\n\
    <company> - Three letter company abbreviation.\n\
    <site>    - Site abbreviation\n\n\
View list of external companies:            '%s -x'\n\
View list of internal sites:                '%s -X'\n\
View iCAP classification command options:   '%s -I'\n\
Just request external customer approval:    '%s -j <customer e-mail>'\n\
Yes, perform external approval:    '%s -y <requestor> <approver> <e-mail> <reason>' [<time>]\n\
Display detailed help on a command:         '%s -H -<command>'\n\
Perform new InterCom registration:          '%s -R [-<site>] <core ID> [<password>]'\n\
View program version  & other settings:     '%s -v [-<site>]'\n\
Retrieve new releases or documentation:     '%s -r [-<site>] [-<file>]'\n\
Release shared memory not being used:       '%s -M'\n\
Ping dropbox machine:                       '%s -w [-<site>]\n\
Silent ping of dropbox machine:             '%s -W [-<site>]\n\
Resend encrypted data via TransFTP:         '%s -E <keyword>'\n\
Extend file expiration 1-3 extra days:      '%s -e <keyword> <days>'\n\
Display original and stored file length:    '%s -l <keyword>'\n\
Lock data to prevent data retrieval:        '%s -L <keyword>'\n\
Unlock data and allow data retrieval:       '%s -U <keyword>'\n\
View LDAP/InterCom registration entry:      '%s -u [-<site>] <core ID>'\n\
View LDAP directory user information:       '%s -1 [-<site>] <core ID>'\n\
Mail multiple recipients secure keyword:    '%s -m <keyword> <email>'\n\
Query new secure keywords received:         '%s -q [-<site]>'\n\
View list of secure keywords received:      '%s -V [-<site]>'\n\
Set Email notification for all files:       '%s -s [-<site>] all'\n\
Cancel Email notification for all files:    '%s -c [-<site>] all'\n\
Set Email notification for one file:        '%s -s <keyword>'\n\
Cancel Email notification for one file:     '%s -c <keyword>'\n\n"

#define CUST_FORMAT_MSG \
"Error -101:  Invalid input format entered.\n\n"
#define CUST_MSG \
"To add users to NXP Support group:     Type '%s -3a Userid1 UserITo d2 ...'\n\
To delete users from NXP Support:      Type '%s -3d UserID1 UserID2...'\n\
To view users in NXP Support group:    Type '%s -3v users'\n\n"

#define PUT_GET_MSG \
"Explanation of parameters used below:\n\n\
    <email> - Multiple email addresses i.e. transbug@nxp.com\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - File with list of file names separated by carriage returns.\n\
    <keyword> - 9 character keyword returned from previous InterCom command.\n\
    <company> - Three letter company abbreviation.\n\
    <icap> - iCAP classification.\n\n\
Delete data after the current retrieval:  '%s -G <keyword>'\n\
Retrieve data as a compressed file:       '%s -gz <keyword>'\n\
Retrieve data using parallel transfer:    '%s -gl <keyword>'\n\
Retrieve data using serial transfer:      '%s -gs <keyword>'\n\
Retrieve in blocks with retry option:     '%s -gr <keyword>'\n\
Retrieve forcing unbundling with tar:     '%s -gt <keyword>'\n\
Put forcing unbundling with tar:          '%s -pt <keyword>'\n\
Put data using serial transfer:           '%s -ps <file>'\n\
Put data using parallel transfer:         '%s -pl <file>'\n\
Put data using the retry option:          '%s -pr <file>'\n\
Put data as an uncompressed file:         '%s -pu <file>'\n\
Put data and assign iCAP classification:  '%s -p -<company> -<icap> <file> '\n\
Put data  & send email to multiple users: '%s -p -<company> <file> <email>'\n\
Tar and put entire directory tree:        '%s -t -<company> <dir> [<email>]'\n\
Put tree & do not follow symbolic links:  '%s -p -<company> <dir> [<email>]'\n\
Put multiple files with one transaction:  '%s -f -<company> <file>...'\n\
Package & put files into directory:       '%s -k -<company> <dir> <file>...'\n\
Put files listed in named file:           '%s -n -<company> <file list> [<email>]'\n\
Zip & put listed files into directory:    '%s -z -<company> <dir> <file list> [<email>]'\n\
Send duplicate files to the mask shop:    '%s -O [-<company>] <file>'\n\
Delete data after the first retrieval:     Use -F, -K, -N, -P, -T or -Z .\n\n"

#define RELEASE_MSG \
"To obtain InterCom documentation:     Type 'icom -r icom.doc'\n\
To obtain InterCom release notes:     Type 'icom -r icom.rel'\n\
To retrieve an ASCII man page:        Type 'icom -r icom.a'\n\
To retrieve a troff man page:         Type 'icom -r icom.1'\n\
To retrieve the Solaris 10 version:   Type 'icom -r solaris10icom'\n\
To retrieve the Solaris 9 version:    Type 'icom -r solaris9icom'\n\
To retrieve the Solaris 8 version:    Type 'icom -r solaris8icom'\n\
To retrieve the Linux 32 version:     Type 'icom -r linux32icom'\n\
To retrieve the Linux 64 version:     Type 'icom -r linux64icom'\n\
To retrieve the Windows GUI:          Type 'icom -r InterCom.exe'\n\
For the Windows command line version: Type 'icom -r InterCom.tar'\n\
For a sample .icomrc file:            Type 'icom -r .icomrc'\n\
After retrieving a unix release:     Rename the release 'icom'\n\n"

#define ADMIN_MSG \
"Manual registration:    '%s -M [-<site>] <login> <subnet> <email1> [<email2>]'\n\
Registration reset:     '%s -M [-<site>]  <login> <subnet> reset'\n\
Registration deletion:  '%s -M [-<site>]  <login> <subnet> delete'\n\
Ping drop box machine:  '%s -w'\n\
Silent ping to dropbox: '%s -W'\n\n"
#endif /* Define for intercom_free_msg_h */
