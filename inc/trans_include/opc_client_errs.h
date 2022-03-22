#ifndef opc_client_errs_h
#define opc_client_errs_h
/* Database client errors */


#include "global_client_errs.h"

/* Message put in $HOME/trans.log file */
#define LOG_CWD_MSG "# Transcend puts logged in current working directory."
/* RETtrans banner */
#define RET_BANNER \
"\n       Welcome to version %s of RETtrans\n\
               (Submit RET jobs remotely)\n\
      NXP Semiconductor Confidential Proprietary software\n\
        Not for distribution outside NXP Semiconductor\n\n"
#define RET_FORMAT_ERR "Error -101:  Invalid input format.\n\n\
To submit a RET job:        Type '%s -s <dir> <script> <runtime> <files>\n\
To cancel a RET job:        Type '%s -c <job_id>\n\
To modify RET job priority:  Type '%s -m <job_id> <direction> <increment>'\n\
To put a RET job on hold:   Type '%s -h <job_id>'\n\
To take a RET job off hold: Type '%s -r <job_id>'\n\
To modify runtime for RET job: type '%s -t <job ID> <time>\n\
To view active RET jobs:     Type '%s -v jobs'\n\n\
To list a RET directory:    Type '%s -l <dir>'\n\
To get a RET directory:     Type '%s -G <dir>'\n\
To get RET files:            Type '%s -g <dir> <files>'\n\
To delete a RET directory:  Type '%s -D <dir>'\n\
To delete RET files:         Type '%s -d <dir> <files>'\n\
To delete files associated with a RET job, type %s -x job_id files\n\
To view current RET server: type '%s -S'\n\
To perform auto registration:  Type '%s -R <user_ID> <password>\n\
To update the 'user' file:  Type '%s -F'\n\
To generate report on user disk usage:  Type '%s -u'\n\
To check for SQL errors:  type '%s -Q errs'\n\n"
#define RET_FORMAT_MSG \
"To submit a RET job:        Type '%s -s <dir> <script> <runtime> <files>\n\
To cancel a RET job:        Type '%s -c <job_id>\n\
To modify RET job priority:  Type '%s -m <job_id> <direction> <increment>'\n\
To put a RET job on hold:   Type '%s -h <job_id>'\n\
To take a RET job off hold: Type '%s -h <job_id>'\n\
To remove hold on RET job:   Type '%s -r <job_id>\n\
To modify runtime for RET job: type '%s -t <job ID> <time>\n\
To view active RET jobs:     Type '%s -v jobs'\n\n\
To list a RET directory:    Type '%s -l <dir>'\n\
To get a RET directory:     Type '%s -G <dir>'\n\
To get RET files:            Type '%s -g <dir> <files>'\n\
To delete a RET directory:  Type '%s -D <dir>'\n\
To delete RET files:         Type '%s -d <dir> <files>'\n\
To perform auto registration:  Type '%s -R <user_ID> <password>\n\
To build a RET library:  type '%s -b <library> <pin>\n\
To clean up expired project directories:  type '%s -X'\n\
To perform audits:  type '%s -a'\n\
To run a new job:  type '%s -n'\n\
To update the 'user' file:  Type '%s -F'\n\
To retrieve 64 bit mode directory list type '%s -6 <directory>'\n\
To get 32 bit file stat:  Type '%s -f'\n\
To get 64 bit file stat:  Type '%s -7 <file>'\n\
To check for SQL errors:  type '%s -Q errs'\n\n"

#define opc_server_set_err "Error -181:  Unable to find routing data for the '%s' server."
#define RET_SERVER_SET_ERR -181
#define sql_server_err "Error -182:  Unable to retrieve server table from MySQL database:\n\n\
  Database name:  %s\n\
  Error number:   %d\n\
  Error string:   %s"
#define SQL_SERVER_ERR -182

#endif /* define for opc_client_errs_h */
