#ifndef server_defines_h
#define server_defines_h

#define BADGE_LEN 48
#define FABINT_CLIENT_VERSION "1.0"
#define DBNO_REQ 'm'
/* Following two defines help server run as daemon under BSD */
#define DAEMON   /* become a daemon */
#define BSD
/* Defines used to print error messages */
#define LATEST 'l'
#define STARS "**********************************************************************\n\n"
#define HELP_MSG "For assistance, contact DMI database administrator:\n\n\
               Name:  Transcend Team\n\
               Email:  transbug@nxp.com"
#define TAB "               "
#define RET_TAB "\n               "
#define RET_TAB8 "\n        "
#define RET_TAB_QUOTE "\n               '"
#define RET2_TAB "\n\n                "
#define RET2_TAB_QUOTE "\n\n                '"
#define DOUBLE_RET "\n\n"
#define RET_TO "'\n            to '"
#define IN_TABLE "In table:      "

/* Lengths for global parameters */
#define USER_LEN 32
#define FILE_SIZE 256
#define PATH_SIZE 1024
#define MSG_SIZE 4096	/* Maximum size for sending message */
#define PWD_SIZE 32	/* Length of temporary encryption password */
#define NULL_PIN "000000"
#define MASK_LEN 8
#define MIN_PIN_LEN 6
#define MAX_PIN_LEN 12
#define KEY_LEN 64
#define PAR_LEN 64
#define TOTAL_FILES 50 	/* Number of files database can manipulate at one time */
#define NUM_SERVERS 5
#define SERVER_INDEX NUM_SERVERS-1
#define MAX_SERVERS 5

#define DBNO 2
#define DBEMAIL 3
#define FAILURE -1
#define SUCCESS 0

/* This structure holds variables passed from the client to the server */
typedef struct
{
    char transaction_type;
    char req_type;
    char sec_req_type;
    char data_type;
    char sec_data_type;
    float vnum;
    float sec_vnum;
    char version_flag;
    char sec_version_flag;
    char override_flag; /* Indicates whether transaction continues under abnormal conditions */
    char key1[KEY_LEN]; /* Primary key for data accessing */
    char key2[KEY_LEN]; /* Secondary key for data accessing */
    char key3[KEY_LEN]; /* Key for data accessing */
    char key4[KEY_LEN]; /* Key for data accessing */
    char key5[KEY_LEN]; /* Key for data accessing */
    char par1[PAR_LEN];	/* First extra parameter, Holds miscellaneous data */
    char par2[PAR_LEN];	/* Second extra parameter */
    char par3[PAR_LEN];	/* Third extra parameter */
    char par4[PAR_LEN];	/* Fourth extra parameter */
    char par5[PAR_LEN];	/* Fifth extra parameter */
char cwd[PATH_SIZE];		/* Current working directory */
    char pin1[MAX_PIN_LEN+1];	/* Primary PIN */
char pin2[MAX_PIN_LEN+1];	/* Secondary PIN number */
} GU_PARAMS;

typedef struct
{
char machine[MAX_SERVERS][MACHINE_LEN];
char address[MAX_SERVERS][ADDRESS_LEN];
int socket;
int server_num;	/* Holds number of alternate servers to connect to */
char password[PWD_LEN+4];	/* User's encrypted password */
    char client_version[12];	/* Version of database software client using */
char user_id[USER_LEN];	/* User ID */
char user_mach[MACHINE_LEN];	/* Machine user is working on */
char user_addr[ADDRESS_LEN]; /*   IP node address of the machine user is on. */
char user_name[64];
} DB_PARAMS;

/* Holds flags returned from a Mask Prep or Fabint transsaction */
typedef struct
{
    int wrn_flag;
    int info_flag;
    int local_wrn_flag;
    int dsgn_file_num;
    int tmpl_file_num;
    int tmp_file_offset;
    int file_offset;
    short rtn_msg_len;
char rtn_msg[1500];	/* Status message returned with each DB primitive */
} DB_FLAGS;

/* Miscellaneous globals */
/*
typedef struct
{
    long upd_time;
    char log_file[FILE_SIZE];
    char upd_time_string[16];	/* Used to generate unique file names */
} SERVER_PARAMS;
*/

typedef struct
{
    int server_num;	/* Number of remote servers */
    long transaction_num;
    int server_index;	/* Index of first server to update */
    char req_type;		/* Request type */
    char transaction_type;
    char log_file[20];	/* Holds log name for remote transactions */
    char site[TOTAL_SERVERS][32];
    char mach[TOTAL_SERVERS][32];
    char addr[TOTAL_SERVERS][ADDRESS_LEN];
    int sock[TOTAL_SERVERS];
} RMT_PARAMS;


typedef struct
{
    char name[16];	/* Name of disk partition */
    int used;	/* Percent of disk being used */
    int allowed;	/* Percent of disk allowed to be used */
} DB_DISK;
#endif
