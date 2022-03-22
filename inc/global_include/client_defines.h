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
               Email Address:  transbug@nxp.com"
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
#define FILE_SIZE 1024
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
#define NONE "0"

/*
typedef struct
{
/* This structure holds variables shared by the database client and the GUI */
/* Default values for these variables are set by calling dbinit_default_globals */
/* The GUI must set appropriate values in this structure before performing database transactions */
typedef struct
{
    float vnum;
    float sec_vnum;
    char version_flag;
    char sec_version_flag;
    char override_flag; /* Indicates whether transaction continues under abnormal conditions */
} ;

/* Structure used for retrieving or passing file  */
typedef struct
{
    /* Array holding list of files  allocated and deallocated by the GUI */
    char *name[FILE_SIZE];
	/* Number of files retrieved or passed */
    int num;
    /* Current working directory files placed in or taken from */
char cwd[PATH_SIZE];
} GUI_FILES;

/* Variables set with function call */
typedef struct
{
    char pin1[MAX_PIN_LEN+1];	/* Primary PIN */
char pin2[MAX_PIN_LEN+1];	/* Secondary PIN number */
} GU_PINS;

typedef struct
{
char machine[MAX_SERVERS][MACHINE_LEN];
char address[MAX_SERVERS][ADDRESS_LEN];
int socket;
int server_num;	/* Holds number of alternate servers to connect to */
};

typedef struct
{
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
