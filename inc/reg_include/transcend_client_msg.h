#ifndef transcend_client_msg_h
#define transcend_client_msg_h

/* Transcend messages */
/* Message put in $HOME/trans.log file */
#define LOG_CWD_MSG "# Transcend puts logged in current working directory."
/* Transcend banner */
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
SERVICE NOW. \n\n"
#define TRANS_FORMAT_ERR "Error -101:  Invalid input format entered.\n\n\
To put a file:             Type '%s -p <file>'\n\
To get a file:             Type '%s -g <keyword>'\n\
To delete a file:          Type '%s -d <keyword>'\n\
To see file info:          Type '%s -i <keyword>'\n\
To see file history:       Type '%s -h <keyword>'\n\
To see advanced commands:  Type '%s -a'\n\
To view put/get options:   Type '%s -A'\n\
To display command help:   Type '%s -H -<command>'\n\n"
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
     <email> - Multiple email addresses i.e. transbug@nxp.com, etc.\n\
    <keyword> - 9 character keyword returned from previous Transcend command.\n\
    <site> - Server sites:  -wro | -tem | -aus | -mun | -idc | -isrl | -brzl | -dtms.\n\n\
Optional arguments are enclosed in brackets.\n\n\
View list of Transcend server sites:      '%s -x'\n\
Display detailed help on a command:       '%s -H <-command option>'\n\
Perform new Transcend registration:       '%s -R [-<site>] <core ID> <password>'\n\
Obtain user's registration status:        '%s -o [-<site>]'\n\
View program version  & other settings:   '%s -v [-<site>]'\n\
Retrieve new releases or documentation:   '%s -r [-<site>] [<file>]'\n\
Extend file expiration 1-3 extra days:    '%s -e <keyword> <days>'\n\
Allow only the owner to delete the data:  '%s -D <keyword>'\n\
Allow deletion by any user with keyword:  '%s -E <keyword>'\n\
Display original and stored file length:  '%s -l <keyword>'\n\
Lock data to prevent data retrieval:      '%s -L <keyword>'\n\
Unlock data and allow data retrieval:     '%s -U <keyword>'\n\
Mail multiple recipients secure keyword:  '%s -m <keyword> <email>'\n\
View list of secure keywords received:    '%s -V [-<site>]'\n\
View list of user keywords transferred:   '%s -i all'\n\
Delete data for all user keywords:        '%s -d all'\n\
Set Email notification for all files:     '%s -s [-<site>] all'\n\
Cancel Email notification for all files:  '%s -c [-<site>] all'\n\
Set Email notification for one file:      '%s -s <keyword>'\n\
Cancel Email notification for one file:   '%s -c <keyword>'\n\
Retrieve Benoit file list for data:       '%s -b <keyword>'\n\
Display Benoit file list for data:        '%s -B <keyword>'\n\
Release unused shared memory:             '%s -M'\n\
View LDAP/Transcend registration entry:   '%s -u [-<site>] <X500 user ID>'\n\
View LDAP directory user information:     '%s -1 [-<site>] <core ID>'\n\
View iCAP classification command options: '%s -I'\n\n"

#ifdef WINDOWS_NT4
#define PUT_GET_MSG \
"Explanation of parameters used below:\n\n\
    <email>  - Multiple email addresses i.e. b36283, rcbf90, etc.\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - Name of file which contains a list of files, one per line\n\
    <keyword> - 9 character keyword returned from previous Transcend command.\n\
    <site> - Transcend server sites:  -wro | -tem | -aus | -mun | -idc | -brzl | -dtms.\n\n\
    <icap> - iCAP classification:  -gbi | -pubi | -fiuo | -fcp.\n\
Optional arguments are enclosed in brackets.\n\n\
Delete data after the current retrieval:  '%s -G <keyword>'\n\
Put data on an alternate site:            '%s -p <-site> <file> '\n\
Retrieve data as a compressed file:       '%s -gz <keyword>'\n\
Put data  & send email to multiple users: '%s -p [-<site>] <file> <email>...'\n\
Delete data after the first retrieval:    '%s -P <keyword>'\n\n"
#else
#define PUT_GET_MSG \
"Explanation of parameters used below:\n\n\
    <dir> - Directory name, may include absolute or relative path.\n\
    <email>...  - Multiple email addresses i.e. b36283, rcbf90, etc.\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - File with list of file names separated by carriage returns.\n\
    <keyword> - 9 character keyword returned from previous Transcend command.\n\
    <site> - Server sites:  wro | tem | aus | mun | idc | isrl | brzl | dtms.\n\
    <icap> - iCAP classification:  gbi | pubi | fiuo | fcp.\n\n\
Optional arguments are enclosed in brackets.\n\n\
Delete data after the current retrieval:  '%s -G <keyword>'\n\
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
Tar and put entire directory tree:        '%s -t [-<site>] <dir> [<email>]'\n\
Put tree & do not follow symbolic links:  '%s -p [-<site>] <dir> [<email>]'\n\
Put multiple files with one transaction:  '%s -f [-<site>] <file....'\n\
Package & put files into directory:       '%s -k [-<site>] <dir> <file>...'\n\
Put files listed in named file:           '%s -n [-<site>] <file list>'\n\
Zip & put listed files into directory:    '%s -z [-<site>] <dir> <file list>'\n\
Delete data after the first retrieval:     Use -F, -K, -N, -P, -T or -Z .\n\n"
#endif

#define RELEASE_MSG \
"To obtain Transcend documentation:   Type 'trans -r trans.doc'\n\
To obtain Transcend release notes:   Type 'trans -r trans.rel'\n\
To retrieve an ASCII man page:       Type 'trans -r trans.a'\n\
To retrieve a troff man page:        Type 'trans -r trans.1'\n\
To retrieve the Solaris 10 version:  Type 'trans -r solaris10trans'\n\
To retrieve the Solaris 9 version:   Type 'trans -r solaris9trans'\n\
To retrieve the Solaris 8 version:   Type 'trans -r solaris8trans'\n\
To retrieve the Linux 64 version:    Type 'trans -r linux64trans'\n\
To retrieve the Linux 32 version:    Type 'trans -r linux32trans'\n\
For a sample .transrc file:          Type 'trans -r .transrc'\n\
After retrieving a Unix release:     Rename the release 'trans'\n\n"

#define ICOM_RELEASE_MSG \
"To obtain InterCom documentation:    Type 'trans -I icom.doc'\n\
To obtain InterCom release notes:    Type 'trans -I icom.rel'\n\
To retrieve an ASCII man page:       Type 'trans -I icom.a'\n\
To retrieve a troff man page:        Type 'trans -I icom.1'\n\
To obtain a sample .icomrc file:     Type 'trans -I .icomrc'\n\
For the HPUX 11 Intercom release:    Type 'trans -I hpux11icom'\n\
For the HPUX 10 Intercom release:    Type 'trans -I hpux10icom'\n\
To retrieve the Solaris 5.8 version: Type 'trans -I solaris8icom'\n\
To retrieve the Solaris 5.7 version: Type 'trans -I solaris7icom'\n\
To retrieve the Solaris 5.5 version: Type 'trans -I solaris5icom'\n\
To retrieve the Linux version:       Type 'trans -I linux_icom'\n\
For the Windows NT version:          Type 'trans -I icom.exe'\n\
For the Windows XP version:          Type 'trans -I icom.exe'\n\
For the Windows 2000 version:        Type 'trans -I icom.exe'\n\
To retrieve the accompanying DLL:    Type 'trans -I cygwin1.dll'\n\
For the accompanying tar utility:    Type 'trans -I tar.exe'\n\
After retrieving a unix release:     Rename the release 'icom'\n\n"

#define ADMIN_MSG \
"Cron delete:            '%s -C files'\n\
Manual registration:    '%s -M <login> <subnet> <email1> <email2>'\n\
Server registration:    '%s -S <login> <subnet> <email1> <email2>'\n\
Registration reset:     '%s -M <login> <subnet> reset'\n\
Registration deletion:  '%s -M <login> <subnet> delete'\n\
Site entry deletion:    '%s -D <year>'\n\
OneIT deletion:         '%s -X entries'\n\n"
#define CUST_FORMAT_MSG \
"Error -101:  Invalid input format entered.\n\n"
#define CUST_MSG \
"To add users to Mica Support group:     Type '%s -4a Userid1 UserITo d2 ...'\n\
To delete users from Mica Support:      Type '%s -4d UserID1 UserID2...'\n\
To view users in Mica Support group:    Type '%s -4v users'\n\n"

#endif /* Define for transcend_client_msg_h */
