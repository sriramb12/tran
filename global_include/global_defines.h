#ifndef global_defines_h
#define global_defines_h

//#define DTMS_DB "inv0923"
#define DTMS_DB "dtmsdb.nxp.com"


#define EXTERNAL_DTMS_DB DTMS_DB

// used in TRANS_COMPANY site for clients
//         TRANS_COMPANY destinations for servers
#define MAX_SITE_DESTINATIONS  80
#define LAST_SITE_DESTINATION_INDEX (MAX_SITE_DESTINATIONS -1)




#define TOTAL_SERVERS 10
#define EMAIL_SUFFIX "@nxp.com"
#define  ENCRYPT  1
#define  DECRYPT  2

/* Define the machine type */
#ifdef AIX
#define MACHINE_TYPE "AIX"
#endif
#ifdef MACINTOSH
#define MACHINE_TYPE "macOS X"
#endif
#ifdef SOLARIS
#define MACHINE_TYPE "Solaris"
#endif
#ifdef SOLARIS5
#define MACHINE_TYPE "Solaris5"
#endif
#ifdef SOLARIS7
#define MACHINE_TYPE "Solaris7"
#endif
#ifdef SOLARIS8
#define MACHINE_TYPE "Solaris8"
#endif
#ifdef SOLARIS9
#define MACHINE_TYPE "Solaris9"
#endif
#ifdef SOLARIS10
#define MACHINE_TYPE "Solaris10"
#endif
#ifdef SOLARIS_X86
#define MACHINE_TYPE "Solaris_X86"
#endif
#ifdef SUNOS
#define MACHINE_TYPE "SunOS"
#endif
#ifdef LINUX
#define MACHINE_TYPE "Linux"
#endif
#ifdef LINUX64
#ifdef TRANSCEND_GUI
#define MACHINE_TYPE  "TransGUI"
#else
#define MACHINE_TYPE "Linux64"
#endif
#endif
#ifdef LINUX32
#define MACHINE_TYPE "Linux32"
#endif
#ifdef FEDORA
#define MACHINE_TYPE "Fedora"
#endif
#ifdef LINUX_GUI
#define MACHINE_TYPE  "Linux GUI"
#endif
#ifdef WINDOWS_NT4
#ifdef TRANSCEND_GUI
#define MACHINE_TYPE  "Windows GUI"
#else
#define MACHINE_TYPE "Windows 7"
#endif /* for Not GUI */
#endif /* for WINDOWS_NT4 */

#define EMAIL_LEN 64
#define EMAIL_LEN 64
#define BADGE_LEN 48
#define SECPERDAY 86400
#define FABINT_CLIENT_VERSION "1.0"
#define DBNO_REQ 'm'
/* Following two defines help server run as daemon under BSD */
#define DAEMON   /* become a daemon */
#define BSD
/* Defines used to print error messages */
#define LATEST 'l'
#define BY_DATE 'd'
#define BY_VNUM 'v'
#define STARS "**********************************************************************\n\n"
#ifdef MASKPREP
#ifdef NXP
#define HELP_MSG "For assistance, contact DMI database administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif
#endif
#ifdef M2
#define HELP_MSG "For assistance, contact Mask Designer Database administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef OPCTRANS
#define HELP_MSG "For assistance, contact D/MI database administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef DFM
#define HELP_MSG "For assistance, contact DMI database administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef TRANSWEB
#define PROGRAM "TransWeb"
#define HELP_MSG "For assistance, contact TransWeb administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif /* for TransWeb */
#ifdef TRANSNET
#define PROGRAM "TransNet"
#define HELP_MSG "For assistance, contact TransNet administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif /* for TransNet */
#ifdef TRANS_FTP
#define PROGRAM "TransFTP"
#define HELP_MSG "For assistance, contact TransFTP administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com\n\n\
To submit a help ticket, go to:\n\n\
https://servicedesk.nxp.com "
#endif /* for TransFTP */
#ifdef TRANSCEND
#define PROGRAM "Transcend"
#define HELP_MSG "For assistance, contact Transcend administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com\n\n\
To submit a help ticket, go to:\n\n\
https://servicedesk.nxp.com "
#endif
#ifndef TRANSNET
#ifdef INTERCOM
#define PROGRAM "InterCom"
#ifdef NXP
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com\n\n\
To submit a help ticket, go to:\n\n\
https://servicedesk.nxp.com "
#endif

/* This is for the external client ran without the server */
#ifdef VPN
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef FSS
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef GENERIC
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef LOCAL
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif
#endif /* for InterCom */
#endif /* For not TransNet */

/* Carlos Mazieri, Feb 2018, added this to  due to changed to compile server lib with -DEXTERNAL*/
#if  defined(INTERCOM) && !defined(HELP_MSG)
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"
#endif

#define TAB "               "
#define RET_TAB "\n               "
#define RET_TAB8 "\n        "
#define RET_TAB_QUOTE "\n               '"
#define RET2TAB "\n\n               "
#define RET_TAB_QUOTE "\n               '"
#define RET2TAB_QUOTE "\n\n                '"
#define DOUBLE_RET "\n\n"
#define RET_TO "'\n            to '"
#define IN_TABLE "In table:      "

/* Lengths for global parameters */

#define MACHINE_LEN 64
#define ADDRESS_LEN MACHINE_LEN  // same as a MACHINE_LEN

#define SITE_LEN  48
#define USER_LEN 32
#define PWD_LEN 12
#define FILE_SIZE 512
#define PATH_SIZE 1024
#define BLOCK_SIZE 4096	/* Maximum size for file blocks */
#define BIG_BLOCK_SIZE 8192	/* Maximum size for big file blocks */
#define MSG_SIZE 1024	/* Maximum size for sending message */
#define BUFFER_SIZE BLOCK_SIZE + 4 /* Length of buffer to hold file blocks */
#define PWD_SIZE 32	/* Length of temporary encryption password */
#define NULL_PIN "000000"
#define MIN_PIN_LEN 6
#define MAX_PIN_LEN 12
#define KEY_LEN 256
#define PAR_LEN 256
#define COMPANY_LEN 48
#define TOTAL_FILES 100 	/* Number of files database can manipulate at one time */

#define DBYES 1
#define DBNO 2
#define NOT_SPECIFIED 3
#define DBEMAIL 3
#define FAILURE -1
#define SUCCESS 0
#define NONE "0"
#define SLASH "/"
#define OPC_RMT_REG 20
#define PRINTF printf
#define DASHES "------------------------------------------------------------\n\n"

/* Used to sort file lists */
typedef struct {
	char * key;         /* the key used in the comparison */
	int balance;			/* info about the state of the tree at this node */
							/* 0 if balanced, -1 if heavy to less, 1 if heavy to more */
	struct tree_node *less;
	struct tree_node *more;
}tree_node;

/* Holds temporary buffer space */
typedef struct
{
char files[TOTAL_FILES][FILE_SIZE]; /* Temporary name */
char buffer[BUFFER_SIZE];	/* Temporarily holds messages, file names, etc. */
char header_buffer[MSG_SIZE];	/* Temporarily holds messages, file names, etc. */
char msg[MSG_SIZE];	/* Temporarily holds local messages */
char file_name[FILE_SIZE]; /* Holds local file names */
char dir_name[FILE_SIZE]; /* Holds local file names */
char tmp_data[FILE_SIZE]; /* Holds temporary local data */
char tmp_file[FILE_SIZE]; /* Holds temporary local file names */
char entry[FILE_SIZE];	/* Holds local entry name being manipulated */
} TMP_BUFFER;

/* Variables used for the database transacction */
typedef struct
{
    long upd_time;
    char log_file[FILE_SIZE];
    char upd_time_string[16];	/* Used to generate unique file names */
    char log_date[32];
    char user_site[SITE_LEN];
    char crypt_pwd[PWD_LEN+1];
    char user_email[EMAIL_LEN];
    char pid_string[8];
    int pid;
    char user_pin[16];
} TRANSACTION_PARAMS;

/* Administrator's login and Email */
typedef struct
{
    char name1[16];	/* Administrator's first name */
    char name2[16];	/* Administrator's last name */
    char phone[20];
    char from_email[EMAIL_LEN];	/* Holds address where email is coming from */	/* Account email came from */
    char err_email[EMAIL_LEN];	/* Email address to send error messages to */
    char reg_email[EMAIL_LEN];	/* Email address to send registration requests to */
    char login1[32];	/* Login name of database administrator */
    char login2[32];	/* Login name of database administrator */
    char program[16];
} DBADMIN_INFO;


/* Set default values for keys, data type, PIN, etc. */
/* Default values in global structure GU_PARAMS gu are set */
//void dbinit_default_globals();


/* Holds software name, version number and password */
typedef struct
{
    char version[16];
    char program[32];
} DB_SOFTWARE;
/* Status of database transaction returned to the GUI */
typedef enum
{
    DB_REG_WRN = -3,
    DB_OVR = -2,
    DB_ERR = -1,
    DB_SUCCESS = 0,
    DB_WRN  = 1,
    DB_INFO = 2,
    DB_PRV = 3
} DB_ERRS;


typedef struct
{
    int index;
    char type;	/* Keep, discard or no action */
    char value[KEY_LEN]; /* Holds value of string to keep or discard */
} FILTER_DATA;

#endif /* ifdef for global_defines_h */
