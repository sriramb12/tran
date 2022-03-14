/****************************************************************************
 *
 * Function:	Global variables
 *
 * Description:	xx
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include "global_defines.h"
#include "m2defines.h"
#include "mask_server_defines.h"
#include "rmt_server.h"
#include "server_params.h"
#include "m2state_file.h"
#include "user_params.h"
#include "mti.h"
#include "rtn.h"
#include "m2msg.h"


/* Holds data sent in mask message from client or remote server */
M2MSG mask_msg;

/* Holds return message */
RTN rtn;

/* Holds mask transaction info for this specific transaction */
MTI mti;

/* Holds information about user i.e. login, machine, etc. */
/* Also holds data that specifies which database is being */
/* used i.e. machine, socket, password, etc. */
/* Information about user set by calling dbinit_user_globals */
/* Information about the Mask Prep database set by calling dbinit_mask_globals */
USER_PARAMS user;

/* Holds data to specify the type of database request being done */
/* i.e. keys, data type, request type, etc. */
M2MSG gu;

/* Holds flags returned to the client from the server */
MASK_FLAGS fl;


/* Holds data for remote transactions */
RMT_SERVERS rmt_mpd;
RMT_SERVERS rmt_icom;
RMT_SERVERS rmt_trans;

/* Holds info about archive server */
SERVER_PARAMS server;

/* Holds global parameters used only by the server */
TRANSACTION_PARAMS tr;

/* Holds temporary buffer space used by the database manager */
TMP_BUFFER bf;

int sock;


/* Parameters set when the server initializes */
INIT_PARAMS ip;

/* System variables initialized and used by the server */
SYS_PARAMS sys;

/* System files */
SYS_FILES sy;


/* Holds variables stored in the state file */
M2STATE_FILE st;


/* Global variables used only by the server */
    char text_fnd[M2TOTAL_FILES];

int global_file_num;
char global_email[1024] = "mdsEvents@freescale.com";


char log_date[32];		/* Date transaction performed */
/* Indexes into the entries structure defined below */
char add_entry_cnt;
char add_pc_entry_cnt;
char add_mask_entry_cnt;
char dlt_pc_entry_cnt;
char dlt_mask_entry_cnt;
char dlt_entry_cnt;


char rmt_server_index;	/* Index of first server to update */
char global_table_cnt;
char file_cnt;		/* Holds number of files in a directory */
char rmt_req_type;		/* Request type */
char rmt_transaction_type;
char glbl_dir2[FILE_SIZE]; /* Mask or template directory being manipulated */
char remote_servers[30][30];

char date[9];		/* Holds current date */
long upd_time;		/* Last date files updated expressed in seconds since 1970 */

char password[13];	/* Encrypted form of password */
char key[FILE_SIZE];
char parent_key[FILE_SIZE];
    char client_version[8]; /* Client's current version number */

float pre_vnum;


/* Holds entries to be deleted or added if the transaction fails */
struct ENTRIES {
    char path[FILE_SIZE];
    char entry[FILE_SIZE];
}

entries[8];



    char mod_pc_link[FILE_SIZE]; /* New value when PC link for mask modified */

/* Holds software info */
DB_SOFTWARE so;

/* Holds info about the administrator */
DBADMIN_INFO admin;
char partial_path[64];

/* Holds names of files in .db directory */
char *db_filelist[M2TOTAL_FILES];

/* Holds names of programs */
int db_program[M2TOTAL_FILES];
char *local_db_filelist[M2TOTAL_FILES];
int local_db_program[M2TOTAL_FILES];

/* Holds names of top level files and associated programs */
char *m2filelist[M2TOTAL_FILES];
int m2program[M2TOTAL_FILES];
char *local_m2filelist[M2TOTAL_FILES];
int local_m2program[M2TOTAL_FILES];
/* Holds names of mask directories */
char mask_dir[5][16];

/* Holds names of global fab files */
char *global_fab_files[15];

char user_group[10][16];

/* Indicates if email error message   sent for the current transaction */
/* Only one email error  message can be sent for each transaction */
int email_cnt;

/* Indicates how many log entries made for the current transaction */
int log_cnt;

char help_msg[256];
char bin_dir[FILE_SIZE];
char admin_email[512];
char group_email[512];
char ret_password[32];
char mso_password[32];
char cert7db[FILE_SIZE];
char external_cert7db[FILE_SIZE];
char service_password[64];
char dtms_db[32];
