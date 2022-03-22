#ifndef mti_h
#define mti_h

/* Data initialized when transaction begins and maintained */
/* throughout the transaction */
typedef struct
{
    char prefix[FILE_SIZE]; /* Prefix used in status messages to user */
    char suffix[FILE_SIZE]; /* Suffix used in status messages to user */
    char *suffix_ptr;
char data_dir[PATH_SIZE];
char off_data_dir[PATH_SIZE];
char old_data_dir[PATH_SIZE];
char old_off_data_dir[PATH_SIZE];
char db_data_dir[PATH_SIZE];
char off_db_data_dir[PATH_SIZE];
char tmpdb_data_dir[PATH_SIZE];
char delta_dir[PATH_SIZE];
char off_delta_dir[PATH_SIZE];
char old_delta_dir[PATH_SIZE];
char db_delta_dir[PATH_SIZE];
char tmp_data_dir[FILE_SIZE];
char tmp_delta_dir[FILE_SIZE];
char state_file[FILE_SIZE];    /* Holds the name of the state file */
char tool_state_file[FILE_SIZE];    /* Holds the name of the state file */
    char tmp_file_prefix[64];
    char audit_summary[48];
    char rmt_audit_summary[48];
} MTI;


extern MTI mti;

#endif /* ifdef for mti_h */
