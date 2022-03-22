#ifndef mask_server_defines_h
#define mask_server_defines_h

#include "global_defines.h"
#include "m2defines.h"
/* Smallest number of files that is valid */
#define MIN_MASK_FILES 35
#define NO_STATE 0 	/* State of data unknown */
#define COMPAT_VERSION "3.0"	/* Lowest compatible client version */
#define TRANSACTION_OFFSET CRT_REQ
#define DBDSGN_INFO 1	/* User given design info file */
#define DBWL_INFO 2	/* User given wafer line and flow info file */
#define DBBOTH_INFO 3	/* User given both design and wafer line info files */
#define DBUNOFF_DSGN_INFO 4
#define DBUNOFF_FAB_INFO 8
#define MKDIR(dir) \
    if (access(dir, 0) == SUCCESS)\
    {\
	strcpy3(command, "rm -r -f ", dir, "/*");\
	system(command);\
    }\
    else { \
	strcpy2(command, "mkdir ", dir);\
	if (system(command) != SUCCESS)\
	close_down(mkdir_err, MKDIR_ERR);\
    }
#define MASK_KEY "mask_shop"
/* Valid ranges for serial numbers */
#define LOWER_SERIAL 0
#define UPPER_SERIAL 99999

/* Defines for template types in copy_tmpls.c */
#define MASK_MASK 1	/* Copying existing mask to new mask */
#define DSGN_MASK 2	/* Copying existing design files to new mask set */
#define TMPL_MASK 3	/* Copying existing template files to new mask set */
#define TMPL_TMPL 4	/* Copying existing template files to new template */
#define DSGN_DSGN 5	/* Copying existing design files to new design files */


#define DATA_OFFSET MASK /* Offset into the data type array */
#define PRESENT 0
#define NOT_PRESENT 1
#define NO_MOD -1
#define PREVIOUS 'p'

/* Defines used to index into message arrays */
#define NONEXIST 0
#define NO_OFFICIAL 1
#define NO_UNOFFICIAL 2
#define SERIAL_SIZE 5
#define ALLOW_ALL "allow-all"

/* Global variables used only by the server */
/* Following variables are pulled out with get_state and put back with set_state */
/* They are present in the state file */
typedef struct
{
    char pin[MAX_PIN_LEN+1];
    char master_site[32];
    char own_id[32];
    char own_mach[MACHINE_LEN];
    char own_addr[ADDRESS_LEN];
    char tmp_id[USER_LEN];
    char tmp_mach[MACHINE_LEN];
    char user_id[USER_LEN];	/* User ID of requester */
    char user_mach[MACHINE_LEN];	/* Machine name of last user to access data */
    char user_addr[ADDRESS_LEN];	/* IP node address of last user to access data */
    char user_cwd[PATH_SIZE];	/* Last user's working directory */
    char pc_link[FILE_SIZE];
int read_cnt;
int lock_cnt;		/* Number of times user tried to access mask set */
int state;		/* Holds current state of the mask set or template */
int pre_state;		/* Holds previous state of mask set or template */
    int transaction_num;
    char app_state[5];
    long upd_time;
    float vnum;
} STATE_PARAMS;

/* Following variables set during server initialization */
typedef struct
{
    char master_site[32];
    char master_addr[16];
    int dsgn_file_num;
    char archive_name[16];
    char archive_mach[MACHINE_LEN];
    char archive_addr[ADDRESS_LEN];
    int archive_sock;
} INIT_PARAMS;

/* Buffers that hold data sent back to user */
typedef struct
{
    char prefix[FILE_SIZE]; /* Prefix used in status messages to user */
    char sec_prefix[FILE_SIZE]; /* Prefix used in status messages to user */
    char suffix[FILE_SIZE]; /* Suffix used in status messages to user */
    char sec_suffix[FILE_SIZE]; /* Suffix used in status messages to user */
    char *suffix_ptr;
} MSG_PARAMS;

/* The following are global directories initialized when system brought up */
typedef struct
{
char source_path[FILE_SIZE];
char source_dir[FILE_SIZE];
char delta_source_dir[FILE_SIZE];
char data_dir[PATH_SIZE];
char data_path[PATH_SIZE];
char sec_data_dir[PATH_SIZE];
char delta_dir[PATH_SIZE];
char delta_path[PATH_SIZE];
char sec_delta_dir[PATH_SIZE];
char sec_data_path[PATH_SIZE];
char sec_delta_path[PATH_SIZE];
char tmp_data_dir[FILE_SIZE];
char tmp_delta_dir[FILE_SIZE];
char off_data_path[FILE_SIZE];	/* Used by the audits */
char off_delta_path[FILE_SIZE];
char sec_off_data_path[FILE_SIZE];
char sec_off_delta_path[FILE_SIZE];
char state_file[FILE_SIZE];    /* Holds the name of the state file */
char off_state_file[FILE_SIZE];    /* Holds the name of the state file */
char sec_state_file[FILE_SIZE];    /* Holds the name of the state file */
char site_file[FILE_SIZE]; /* file telling when sites last received PC library */
} TRANSACTION_INFO;

extern TRANSACTION_INFO ti;


/* Holds system files */
typedef struct
{
    char driver_files[M2TOTAL_FILES][FILE_SIZE];
    char rmt_log_file[20];	/* Holds log name for remote transactions */
    char audit_log_file[20]; /* File name where audit messages put */
} SYS_FILES;

/* System variables initialized and used by the server */
typedef struct
{
    long upd_time;
    char vnum_flag;
} SYS_PARAMS;

/* Holds global flags used in Mask Prep transactions */
typedef struct
{
    int info_flag;
    int local_wrn_flag;
    int dsgn_file_num;
    int tmpl_file_num;
    int tmp_file_offset;
    int file_offset;
    /* The following flags are used to indicate what needs to be cleaned up */
    /* when a database transaction fails */
    char *crt_dir1;
    char *crt_dir2;
    char *crt_dir3;
    char *crt_dir4;
    char cleanup_flag;
    char sec_state_flag;	/* Indicates state file must be modified if error occurs */
    char source_flag;
    char pc_flag;		/* Indicates if PC link has been updated */
    char app_flag;
    char sec_app_flag;
} MASK_FLAGS;

/* Audit types defined for auditing global files */
typedef enum
{
    WAFERLINE_FILE,
    DESIGNGROUP_FILE,
    WAFER_FLOW_FILE,
    DESIGN_FLOW_FILE,
} AUDIT_TYPE;
#endif /* ifdef for mask_server_defines_h */
