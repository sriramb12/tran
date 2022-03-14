#ifndef transcend_client_msg_h
#define transcend_client_msg_h

/* TransNet messages */
/* Message put in $HOME/trans.log file */
#define LOG_CWD_MSG "# TransNet puts logged in current working directory."
/* TransNet banner */
#define TNET_BANNER \
"\n        Welcome to version %s of TransNet\n\
               (Internet data transfer)
      NXP Semiconductor Confidential Proprietary software\n\
        Not for distribution outside NXP Semiconductor\n\n\
By using this service you agree that all items and data\n\
to be sent/received by TransNet are in compliance with\n\
existing NXP Semiconductor export regulations and licenses.\n\n"
#define TNET_FORMAT_ERR "Error -101:  Invalid input format entered.\n\n\
To put a file:                   Type '%s -p <file> [<email>]'\n\
To see transfer status:          Type '%s -i <keyword>'\n\
To see transaction history:      Type '%s -h <keyword>'\n\
To transfer encrypted file:      Type '%s -E <keyword>'\n\n"
#define FORMAT_MSG \
"To put a file:                   Type '%s -p <file> [<email>]'\n\
To see transfer status:          Type '%s -i <keyword>'\n\
To see transaction history:      Type '%s -h <keyword>'\n\
To transfer encrypted file:      Type '%s -E <keyword>'\n\n"

/* message for advanced commands */
#define ADV_MSG \
"Explanation of Parameters used below:\n\n\
     <email> - 1-6 email addresses i.e. b36283, rcbf90, etc..\n\
    <keyword> - 9 character keyword returned from previous TransNet command.\n\
    -<site> - TransNet server sites:  -wac | -aus | -mun.\n\n\
View list of TransNet server sites:      '%s -x'\n\
Display detailed help on a command:       '%s -H <-command option>'\n\
Perform new TransNet registration:       '%s -R <core ID> <OneIT password>'\n\
Obtain user's registration status:        '%s -o'\n\
View program version  & other settings:   '%s -v [-<site>]'\n\
Retrieve new releases or documentation:   '%s -r [-<site>] [<file>]'\n\
Retrieve InterCom releases/documentation: '%s -I [-<site>] [<file>]'\n\
Extend file expiration 1-3 extra days:    '%s -e <keyword> <days>'\n\
Allow only the owner to delete the data:  '%s -D <keyword>'\n\
Allow deletion by any user with keyword:  '%s -E <keyword>'\n\
Display original and stored file length:  '%s -l <keyword>'\n\
Lock data to prevent data retrieval:      '%s -L <keyword>'\n\
Unlock data and allow data retrieval:     '%s -U <keyword>'\n\
View OneIT/TransNet registration entry:  '%s -u <X500 user ID>'\n\
Mail 1 to 6 recipients secure keyword:    '%s -m <keyword> <email>'\n\
View list of secure keywords received:    '%s -V [-<site>]'\n\
Set Email notification for all files:     '%s -s [-<site>] all'\n\
Cancel Email notification for all files:  '%s -c [-<site>] all'\n\
Set Email notification for one file:      '%s -s <keyword>'\n\
Cancel Email notification for one file:   '%s -c <keyword>'\n\n"

#ifdef WINDOWS_NT4
#define PUT_GET_MSG \
"Explanation of parameters used below:\n\n\
    <email>  - 1-6 email addresses i.e. b36283, rcbf90, etc..\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - File with list of file names separated by carriage returns.\n\
    <keyword> - 9 character keyword returned from previous TransNet command.\n\n\
Put data on an alternate site:            '%s -p <-site> <file> '\n\
Put data  & send email to 1-6 users:      '%s -p [-<site>] <file> <email>...'\n\
Alternate user for email notification:    '%s -p <file> -S <email>'\n\
Set type to ASCII during data transfer:   '%s -pa [-<site>] <file>'\n\
Set type to binary during data transfer:  '%s -pb [-<site>] <file>'\n\
Retrieve data as an ASCII file:           '%s -ga <keyword>'\n\
Retrieve data as a binary file:           '%s -gb <keyword>'\n\
Delete data after the current retrieval:  '%s -G <keyword>'\n\
Delete data after the first retrieval:    '%s -P <keyword>'\n\n"
#else
#define PUT_GET_MSG \
"Explanation of parameters used below:\n\n\
    <dir> - Directory name, may include absolute or relative path.\n\
    <email>...  - 1-6 email addresses i.e. b36283, rcbf90, etc..\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - File with list of file names separated by carriage returns.\n\
    <keyword> - 9 character keyword returned from previous TransNet command.\n\n\
Put data on an alternate site:            '%s -p <site> <file> '\n\
Put data  & send email to 1-6 users:      '%s -p [-<site>] <file> <email>'\n\
Alternate user for email notification:    '%s -p <file> -S <email>'\n\
Tar and put entire directory tree:        '%s -t [-<site>] <dir> [<email>]'\n\
Put tree & do not follow symbolic links:  '%s -p [-<site>] <dir> [<email>]'\n\
Put multiple files with one transaction:  '%s -f [-<site>] <file....'\n\
Package & put files into directory:       '%s -k [-<site>] <dir> <file>...'\n\
Put files listed in named file:           '%s -n [-<site>] <file list>'\n\
Zip & put listed files into directory:    '%s -z [-<site>] <dir> <file list>'\n\
Set type to ASCII during data transfer:   '%s -pa [-<site>] <file>'\n\
Set type to binary during data transfer:  '%s -pb [-<site>] <file>'\n\
Retrieve data as an ASCII file:           '%s -ga <keyword>'\n\
Retrieve data as a binary file:           '%s -gb <keyword>'\n\
Delete data after the current retrieval:  '%s -G <keyword>'\n\
Delete data after the first retrieval:     Use -F, -K, -N, -P, -T or -Z .\n\n"
#endif

#define RELEASE_MSG \
"To obtain TransNet documentation:  Type '%s -r trans.doc'\n\
To obtain TransNet release notes:  Type '%s -r trans.rel'\n\
To retrieve an ASCII man page:      Type '%s -r trans.a'\n\
To retrieve a troff man page:       Type '%s -r trans.1'\n\
To obtain user survey results:      Type '%s -r trans.survey'\n\
To obtain a sample .transrc file:   Type '%s -r .transrc'\n\n\
To obtain HP version of TransNet:  Type '%s -r hpux_tnet'\n\
To retrieve the SunOS version:      Type '%s -r sunos_tnet'\n\
To retrieve the Solaris version:    Type '%s -r solaris_tnet'\n\
To retrieve the AIX version:        Type '%s -r aix_tnet'\n\
To retrieve the AIX3/AIX4 version:  Type '%s -r aix3_tnet'\n\
To retrieve the IRIX version:       Type '%s -r irix_tnet'\n\
To retrieve the Linux version:      Type '%s -r linux_tnet'\n\
To retrieve the Windows NT version: Type '%s -r trans.exe'\n\
For the accompanying DLL:           Type '%s -r cygwin1.dll'\n\
For the accompanying tar utility:   Type '%s -r tar.exe'\n\
After retrieving a unix release:    rename the release 'trans'\n\n"

#define ICOM_RELEASE_MSG \
"To obtain InterCom documentation:  Type '%s -I icom.doc'\n\
To obtain InterCom release notes:  Type '%s -I icom.rel'\n\
To retrieve an ASCII man page:      Type '%s -I icom.a'\n\
To retrieve a troff man page:       Type '%s -I icom.1'\n\
To obtain a sample .icomrc file:   Type '%s -I .transrc'\n\n\
To obtain HP version of InterCom:  Type '%s -I hpux_icom'\n\
To retrieve the SunOS version:      Type '%s -I sunos_icom'\n\
To retrieve the Solaris version:    Type '%s -I solaris_icom'\n\
To retrieve the AIX version:        Type '%s -I aix_icom'\n\
To retrieve the IRIX version:       Type '%s -I irix_icom'\n\
To retrieve the Linux version:      Type '%s -I linux_icom'\n\
For  the Windows NT version:        Type '%s -I icom.exe'\n\
To retrieve the accompanying DLL:   Type '%s -I cygwin1.dll'\n\
After retrieving a unix release:    rename the release 'icom'\n\n"

#define ADMIN_MSG \
"Cron delete:            '%s -C files'\n\
Manual registration:    '%s -M <login> <subnet> <email1> <email2>'\n\
Registration reset:     '%s -M <login> <subnet> reset'\n\
Registration deletion:  '%s -M <login> <subnet> delete'\n\
Site entry deletion:    '%s -D <year>'\n\
OneIT deletion:         '%s -X entries'\n\n"
#endif /* Define for transcend_client_msg_h */
