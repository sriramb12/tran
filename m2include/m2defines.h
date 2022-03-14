#ifndef m2defines_h
#define m2defines_h

#include "global_defines.h"

#define FAB_FILE "fab"
#define CONFIG_FILE "configuration_name"
/* Defines for types of audits */
#define WEEKLY "Weekly"
#define DAILY "Daily"
#define HOURLY "Hourly"
#define COMPLETE "Complete"

#define LOCAL_GLOBAL_DIR ".tables"
#define LOCAL_FAB_DIR ".fab"
/* Defines for long strings with NON in them */
#define NONE2 "0", "0"
#define NONE3 "0", "0", "0"
#define NONE4 "0", "0", "0", "0"
#define NONE5 "0", "0", "0", "0", "0"
#define NONE6 "0", "0", "0", "0", "0", "0"
#define NONE7 "0", "0", "0", "0", "0", "0", "0"
#define NONE8 "0", "0", "0", "0", "0", "0", "0", "0"
#define NONE9 "0", "0", "0", "0", "0", "0", "0", "0", "0"
/* Version number for current release of Mask Prep client and server */
#define MASK_VERSION "4.0"

/* Encrypted key sent in Mask header */
#define MASK_KEY "mask_shop"
#define MAX_MASK_SERVERS 10
#define MASK_SOCKET 6194

/* Default mask machine names */


#define EMAIL_LEN 64
#define DBNO_REQ 'm'
/* Following two defines help server run as daemon under BSD */
#define DAEMON   /* become a daemon */
#define BSD
#define M2SERVER_NUM 10
/* Defines used to print error messages */
#define LATEST 'l'
#define BY_DATE 'd'
#define BY_VNUM 'v'
#define STARS "**********************************************************************\n\n"
#define TAB "               "
#define RET_TAB "\n               "
#define RET_TAB8 "\n        "
#define RET_TAB_QUOTE "\n               '"
#define RET2_TAB "\n\n               "
#define RET2TAB_QUOTE "\n\n                '"
#define DOUBLE_RET "\n\n"
#define RET_TO "'\n            to '"
#define IN_TABLE "In table:      "

/* Lengths for global parameters */
#define USER_LEN 32
#define PWD_SIZE 32	/* Length of temporary encryption password */
#define MASK_LEN 8
#define SERVER_INDEX M2SERVER_NUM-1
#define DBEMAIL 3
#define MACHINE_NUM 9
#define M2TOTAL_FILES 4100

/* This structure holds variables shared by the database client and the GUI */
/* Default values for these variables are set by calling dbinit_default_globals */
/* The GUI must set appropriate values in this structure before performing database transactions */


/* Variables used for the database transacction */

/* Set default values for keys, data type, PIN, etc. */
/* Default values in global structure MASK_MSG gu are set */
void dbinit_default_globals(void);

/* Set information about the user i.e. login name, IP address */

typedef struct
{
    char data_type[8];
    char path[32];
    char name[16];	/* Name of disk partition */
    int used;	/* Percent of disk being used */
    int allowed;	/* Percent of disk allowed to be used */
} MASK_DISK;


typedef struct
{
char machine[M2SERVER_NUM][MACHINE_LEN];
char address[M2SERVER_NUM][ADDRESS_LEN];
int socket;
    int server_num;
} MASK_PARAMS;


#endif /* ifdef for m2defines_h */
