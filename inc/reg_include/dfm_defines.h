#ifndef dfm_defines_h
#define dfm_defines_h

#define DFM_VERSION "1.2.4"
#define DFM_AUTH_TABLE "db_admin/sysadmin.auth"
#define FROM_TC "JobSubmit@nxp.com"

#define DFM_FORMAT_MSG \
"To submit a DFM job:            Type 'dfm -s <control_file>'\n\
To view active DFM jobs:        Type 'dfm -v jobs'\n\
To kill a DFM job:              Type 'dfm -k <job number>'\n\
To list a DFM directory:        Type 'dfm -l <directory>'\n\
To list supported technologies: type 'dfm -l technologies'\n\
To retrieve a control file:     Type 'dfm -gc <technology>'\n\
To retrieve a technology file:  Type 'dfm -gt <technology>'\n\
To retrieve project files:      type 'dfm -g <directory>/<file>'\n\
To register:                    Type 'dfm -r'\n\
To display command help:        type 'dfm -h -<command>'\n\
To view admin commands:         type 'dfm -a'\n\n"

#define ADMIN_MENU \
"To upload a ctrl & technology  file: type 'dfm -pt <tech> <ctrl file> <tech file>'\n\
To list technology versions:     type 'dfm -lt <technology>'\n\
To delete old job directories:   type 'dfm -D'\n\
To delete specified directories: type 'dfm -d file_name'\n\
To run a new DFM job:            type 'dfm -R'\n\n"

/* To audit the DFM Queue:          type 'dfm -a'\n\ */
#define DFM_FORMAT_ERR "Error -101:  Invalid input format.\n\n\
To submit a DFM job:        Type 'dfm -s <control_file>'\n\
To cancel a DFM job:        Type 'dfm -c <job number>'\n\
To view active DFM jobs:    Type 'dfm -v jobs'\n\
To list a DFM directory:    Type 'dfm -l <directory>'\n\
To register:                Type 'dfm -r'\n\n"

#define DFM_BANNER \
"\n       Welcome to version %s of DFM Check\n\
               (Submit DFM jobs remotely)\n\
      NXP Semiconductor Confidential Proprietary software\n\
        Not for distribution outside NXP Semiconductor\n\n"

typedef enum
{
    BOTH,
    NEITHER,
    RESULTS,
    SUMMARY,
} TRANS_TYPE;

#define TRANSFER_MSG \
"Output for job %d was transcended to you.  A copy of the \n\
Transcend log file is given below.\n\n\
------------------------------------------------------------\n"

#endif
