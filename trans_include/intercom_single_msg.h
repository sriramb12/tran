#ifndef intercom_client_msg_h
#define intercom_client_msg_h
#ifdef SINGLE_CONNECT
#ifndef TRANS_FTP
#ifndef TRANSNET

/* InterCom messages */
/* Message put in $HOME/trans.log file */
#define LOG_CWD_MSG "# InterCom puts logged in current working directory."
/* InterCom banner */
#define TRANS_BANNER \
"\n            Welcome to version %s of InterCom\n\
               (Inter-Company Data Transfer)\n\
                    External release\n\
                 For use by external partners\n\
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
To put a file:             Type '%s -p <file> <e-mail>'\n\
To get a file:             Type '%s -g <keyword>'\n\
To delete a file:          Type '%s -d <keyword>'\n\
To see file info:          Type '%s -i <keyword>'\n\
To see file history:       Type '%s -h <keyword>'\n\
To see advanced commands:  Type '%s -a'\n\
To view put/get options:   Type '%s -A'\n\
To display command help:   Type '%s -H -<command option>'\n\n"
#define FORMAT_MSG \
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
    <email>   - Multiple email addresses i.e. transbug@nxp.com\n\
    <keyword> - 9 character keyword returned from previous InterCom command.\n\n\
View program version  & other settings:   '%s -v'\n\
View iCAP classification command options: '%s -I'\n\
Display detailed help on a command:       '%s -H -<command>'\n\
Retrieve new releases or documentation:   '%s -r [<file>]'\n\
Extend file expiration 1-3 extra days:    '%s -e <keyword> <days>'\n\
Display original and stored file length:  '%s -l <keyword>'\n\
Lock data to prevent data retrieval:      '%s -L <keyword>'\n\
Unlock data and allow data retrieval:     '%s -U <keyword>'\n\
Mail multiple recipients user keyword:    '%s -m <keyword> <email>'\n\
Query new secure keywords received:       '%s -q'\n\
View list of secure keywords received:    '%s -V' or '%s -Q'\n\
View list of file keywords transferred:   '%s -i all'\n\
Set Email notification for all files:     '%s -s all'\n\
Cancel Email notification for all files:  '%s -c all'\n\
Set Email notification for one file:      '%s -s <keyword>'\n\
Cancel Email notification for one file:   '%s -c <keyword>'\n\
Display URL for user registration:        '%s -R'\n\
Encrypt LDAP password for .icomrc file:   '%s -X'\n\
Release shared memory not being used:     '%s -M'\n\
Ping dropbox machine:                     '%s -w'\n\
Silent ping of dropbox machine:           '%s -W'\n\n"

#define PUT_GET_MSG \
"Explanation of parameters used below:\n\n\
    <email> - Multiple email addresses i.e. transbug@nxp.com\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - Name of file which contains a list of files, one per line\n\
    <keyword> - 9 character keyword returned from previous InterCom command.\n\
    <icap> - iCAP classification.\n\n\
Delete data after the current retrieval:  '%s -G <keyword>'\n\
Retrieve data as a compressed file:       '%s -gz <keyword>'\n\
Retrieve in blocks with retry option:     '%s -gr <keyword>'\n\
Retrieve forcing unbundling with tar:     '%s -gt <keyword>'\n\
Put forcing unbundling with tar:          '%s -pt <keyword>'\n\
Put data using serial transfer:           '%s -ps <file>'\n\
Put data using parallel transfer:         '%s -pl <file>'\n\
Put data using the retry option:          '%s -pr <file>'\n\
Put data as an uncompressed file:         '%s -pu <file>'\n\
Put data and assign iCAP classification:  '%s -p -<company> -<icap> <file> '\n\
Put data  & send email to multiple users: '%s -p <file> <email>'\n\
Tar and put entire directory tree:        '%s -t <dir> [<email>]'\n\
Put tree & do not follow symbolic links:  '%s -p <dir> [<email>]'\n\
Put multiple files with one transaction:  '%s -f <file>...'\n\
Package & put files into directory:       '%s -k <dir> <file>...'\n\
Put files listed in named file:           '%s -n <file list> [<email>]'\n\
Zip & put listed files into directory:    '%s -z <dir> <file list> [<email>]'\n\
Delete data after the first retrieval:     Use -F, -K, -N, -P, -T or -Z .\n\n"

/* Release message for various companies */
#ifdef GENERIC
#define RELEASE_MSG \
"To obtain InterCom documentation:   Type '%s -r icom.doc'\n\
To retrieve an ASCII man page:      Type '%s -r icom.a'\n\
To retrieve a troff man page:       Type '%s -r icom.1'\n\
To retrieve the Solaris release:    Type '%s -r icom'\n\
To retrieve the Windows release:    Type '%s -r icom.exe'\n\n"
#endif
#ifdef EXTERNAL
#define RELEASE_MSG \
"To obtain InterCom documentation:   Type '%s -r icom.doc'\n\
To retrieve an ASCII man page:      Type '%s -r icom.a'\n\
To retrieve a troff man page:       Type '%s -r icom.1'\n\
To retrieve the Solaris release:    Type '%s -r icom'\n\
To retrieve the Windows release:    Type '%s -r icom.exe'\n\n"
#endif
#ifdef VPN
#define RELEASE_MSG \
"New releases not available through this version of InterCom."
#endif /* for VPN */
#ifdef FSS
#define RELEASE_MSG \
"New releases not available through this version of InterCom."
#endif /* for FSS */

#define ADMIN_MSG \
"Manual registration:    '%s -M <login> <subnet> <email1> <email2>'\n\
Registration reset:     '%s -M  <login> <subnet> reset\n\
Registration deletion:  '%s -M  <login> <subnet> delete\n\
Ping drop box machine:  '%s -w\n\
Silent ping to dropbox: '%s -W'\n\n"
#endif /* for Not TransNet */
#endif /* for Not TransFTP */
#endif /* For SINGLE_CONNECT */
#endif /* Define for intercom_single_msg_h */
