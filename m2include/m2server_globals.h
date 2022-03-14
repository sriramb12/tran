#ifndef m2server_globals_h
#define m2server_globals_h

#include "mask_server_defines.h"
#include "m2state_file.h"
#include "rmt_server.h"


#include "m2defines.h"
#include "m2msg.h"

/* Parameters initialized by the GUI */
extern M2MSG mask_msg;
/* Global flags used by database */
extern MASK_FLAGS fl;

/* Parameters set when the server initializes */
extern INIT_PARAMS ip;

/* System variables initialized and used by the server */
extern SYS_PARAMS sys;

/* System files */
extern SYS_FILES sy;

/* Paths initialized when server starts up */

/* Holds data temporarily */
extern TMP_BUFFER bf;

/* Holds variables stored in the state file */

/* Holds data stored in the secondary state file */

/* Global variables used only by the server */
    extern char text_fnd[M2TOTAL_FILES];

extern int sock;		/* Socket number of connected socket */
extern int rvu_num;
extern int rvu_num;	/* Number of RVU files to send */
extern int global_file_num;

extern int state_fd;
extern int off_state_fd;
extern int sec_state_fd;

/* The following flags are used to indicate what needs to be cleaned up */
/* when a database transaction fails */
extern char log_date[];		/* Date transaction performed */
/* Indexes into the entries structure defined below */
extern char add_entry_cnt;
extern char add_double_entry_cnt;
extern char add_pc_entry_cnt;
extern char add_mask_entry_cnt;
extern char dlt_double_entry_cnt;
extern char dlt_pc_entry_cnt;
extern char dlt_mask_entry_cnt;
extern char dlt_entry_cnt;

extern char bo_index;		/* Index for first backout file */
extern char bo_cnt;		/* Index for last back-out file */
extern char sec_bo_index;	/* Secondary back-out index */
extern char sec_bo_cnt;	/* Last secondary backout index */
extern int own_sock;
extern int INFO_FLAG;

extern char info_cnt;	/* Indicates if info file is sent with the mask set */
extern char driver_cnt;	/* Holds number of driver files present */
extern char rmt_server_index;	/* Index of first server to update */
extern char global_table_cnt;
extern char file_cnt;		/* Holds number of files in a directory */
extern char rmt_req_type;		/* Request type */
extern char rmt_transaction_type;
extern char glbl_dir2[FILE_SIZE]; /* Mask or template directory being manipulated */
extern char remote_servers[30][30];

extern char date[9];		/* Holds current date */
extern long upd_time;		/* Last date files updated expressed in seconds since 1970 */

extern char password[13];	/* Encrypted form of password */
extern char key[FILE_SIZE];
extern char parent_key[FILE_SIZE];
extern char sec_parent_key[FILE_SIZE];
extern char sec_parent_path[FILE_SIZE];
extern char sec_key[FILE_SIZE];
    extern char client_version[8]; /* Client's current version number */

extern float pre_vnum;
float sec_pre_vnum;


/* Holds entries to be deleted or added if the transaction fails */
struct ENTRIES {
    char path[FILE_SIZE];
    char entry[FILE_SIZE];
}

entries[8];

/* Holds double entries to be deleted or added if the transaction fails */
struct DOUBLE_ENTRIES {
    char path[FILE_SIZE];
    char entry1[FILE_SIZE];
    char entry2[FILE_SIZE];
}

double_entries[8];

    extern char mod_pc_link[FILE_SIZE]; /* New value when PC link for mask modified */

extern char *db_filelist[M2TOTAL_FILES];
extern int db_program[M2TOTAL_FILES];
extern char *m2filelist[M2TOTAL_FILES];
extern int m2program[M2TOTAL_FILES];

/* Holds names of global fab files */
extern char *global_fab_files[15];

extern RMT_SERVERS rmt;

#endif /* ifdef for mask_server_globals_h */
