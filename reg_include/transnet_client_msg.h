#ifdef TRANSNET
#ifndef transnet_client_msg_h
#define transnet_client_msg_h

/* TransNet messages */
/* Message put in $HOME/trans.log file */
#define LOG_CWD_MSG "# TransNet puts logged in current working directory."
/* TransNet banner */
#define TRANS_BANNER \
"\n            Welcome to version %s of TransNet\n\
               (Inter-Company Data Transfer)\n\
          Copyright 1994-2020 NXP Semiconductor N.V.\n\n\
By using this service you agree that all items and data\n\
to be sent/received by TransNet are in compliance with\n\
existing NXP Semiconductor export regulations and licenses.\n\n"
#define TRANS_FORMAT_ERR "Error -101:  Invalid input format entered.\n\n\
To put a file:             Type '%s -p <file> <email>'\n\
To get a file:             Type '%s -g <secure key> <file key>'\n\
To delete a file:          Type '%s -d <secure key> <file key>'\n\
To see file info:          Type '%s -i <secure key> <file key>'\n\
To see file history:       Type '%s -h <secure key> <file key>'\n\
To see advanced commands:  Type '%s -a'\n\
To view put/get options:   Type '%s -A'\n\
To display command help:   Type '%s -H -<command option>'\n\n"
#define FORMAT_MSG \
"To put a file:             Type '%s -p <file> <email>'\n\
To get a file:             Type '%s -g <secure key> <file key>'\n\
To delete a file:          Type '%s -d <secure key> <file key>'\n\
To see file info:          Type '%s -i <secure key> <file key>'\n\
To see file history:       Type '%s -h <secure key> <file key>'\n\
To see advanced commands:  Type '%s -a'\n\
To view put/get options:   Type '%s -A'\n\
To display command help:   Type '%s -H -<command>'\n\n"

/* message for advanced commands */
#define ADV_MSG \
"Explanation of Parameters used below:\n\n\
    <email>   - Multiple email addresses i.e. transbug@nxp.com\n\
    <secure key> <file key> - 9 character keyword returned from previous TransNet command.\n\n\
View program version  & other settings:   '%s -v'\n\
Display detailed help on a command:       '%s -H -<command>'\n\
Ping dropbox machine:                     '%s -w'\n\
Silent ping of dropbox machine:           '%s -W'\n\
Extend file expiration 1-3 extra days:    '%s -e <secure key> <file key> <days>'\n\
Display original and stored file length:  '%s -l <secure key> <file key>'\n\
Lock data to prevent data retrieval:      '%s -L <secure key> <file key>'\n\
Unlock data and allow data retrieval:     '%s -U <secure key> <file key>'\n\
Mail one to six recipients user keyword:  '%s -m <secure key> <file key> <email>'\n\
Set Email notification for all files:     '%s -s all'\n\
Cancel Email notification for all files:  '%s -c all'\n\
Set Email notification for one file:      '%s -s <secure key> <file key>'\n\
Cancel Email notification for one file:   '%s -c <secure key> <file key>'\n\n"


#define PUT_GET_MSG \
"Explanation of parameters used below:\n\n\
    <email> - Multiple email addresses i.e. transbug@nxp.com\n\
    <file> - File or directory name, may include relative or absolute path.\n\
    <file>... - Multiple files, wild cards may be used.\n\
    <file list> - Name of file which contains a list of files, one per line\n\
    <secure key> <file key> - 9 character keyword returned from previous TransNet command.\n\n\
Delete data after the current retrieval:  '%s -G <secure key> <file key>'\n\
Put data  & send email to multiple users: '%s -p <file> <email>'\n\
Tar and put entire directory tree:        '%s -t <dir> [<email>]'\n\
Put tree & do not follow symbolic links:  '%s -p <dir> [<email>]'\n\
Put multiple files with one transaction:  '%s -f <file>...'\n\
Package & put files into directory:       '%s -k <dir> <file>...'\n\
Put files listed in named file:           '%s -n <file list> [<email>]'\n\
Zip & put listed files into directory:    '%s -z <dir> <file list> [<email>]'\n\
Set type to ASCII during data transfer:   '%s -pa <file>'\n\
Set type to binary during data transfer:  '%s -pb <file>'\n\
Retrieve data as an ASCII file:           '%s -ga <secure key> <file key>'\n\
Retrieve data as a binary file:           '%s -gb <secure key> <file key>'\n\
Delete data after the first retrieval:     Use -F, -K, -N, -P, -T or -Z .\n\n"

#define RELEASE_MSG \
"Download of releases not available for TransNet"

#endif /* Define for transnet_external_msg_h */
#endif /* For TRANSNET */
