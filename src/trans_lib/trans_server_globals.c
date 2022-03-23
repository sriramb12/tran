#include "global_defines.h"
#include "lock_file.h"
#include "trans_server_defines.h"
#include "transrc.h"
#include "trans_server_globals.h"
#include "rmt_server.h"
#include "rtn.h"
#include "server_params.h"
#include "user_params.h"
#include "trans_server_prototypes.h"
#include <mysql.h>

/* Holds number of times user can enter invalid keyword */
/* before being locked out of Transcend */
int global_lock_cnt;
int deletion_days;

/* Holds user's current lock cnt */
int user_lock_cnt;

/* Holds server version */
float server_version;

/* Holds info about Transcend disk */
TRANS_DISK disk;

/* Holds message returned to user */
RTN rtn;

/* Holds data passed to server from GUI */
TRANS_PARAMS gu;

/* Holds status flags for transaction */
TRANS_FLAGS fl;

/* Holds software info */
DB_SOFTWARE so;
/* Holds user's name and Email address if found */

/* Holds information about user i.e. login, machine, etc. */
/* Also holds data that specifies which database is being */
/* used i.e. machine, socket, password, etc. */
USER_PARAMS user;


/* Holds flags returned to the client from the server */
extern TRANS_FLAGS fl;


/* Holds global parameters set with each transaction */
TRANSACTION_PARAMS tr;

/* Holds temporary buffer space used by the database manager */
TMP_BUFFER bf;

int sock;
int timeout_extension;
char keyword_extension[2];
char secure_keyword_extension[4];
char keyword_link_extension[8];
char dtms_db[32];
SERVER_PARAMS server;
/* Holds time zone i.e. MST, EST, or MED */
char time_zone[4];

/* Holds list of remote Transcend servers */
RMT_SERVERS rmt_trans;
RMT_SERVERS rmt_mpd;

/* Holds list of remote InterCom servers */
RMT_SERVERS rmt_icom;

/* Indicates if email error message   sent for the current transaction */
/* Only one email error  message can be sent for each transaction */
int email_cnt;

/* Indicates how many log entries made for the current transaction */
int log_cnt;
TRANS_FILE_STAT fs;
int disk_cnt;

/* Holds Transcend or TransWeb */
char program_name[12];
int new_reg_entry;

DBADMIN_INFO admin;

char help_msg[BUFFER_SIZE];
char fss_admin[32];
int global_disk_usage;
char fromcompany[32];
char global_default_site; /* Holds default site if customer not found in subnet table */
char *lock_files[TOTAL_LOCK_FILES];
char dtms_mail_list[BUFFER_SIZE];
int secure_keyword_fnd;
/* Holds the complete path to cert7.db */
char cert7db[FILE_SIZE];
char external_cert7db[FILE_SIZE];
char service_password[64];
struct transrc_t transrc;
char transdirlist[64];
int global_file_cnt;
int global_dir_cnt;
    int shm_id;
char root_dir[FILE_SIZE];
char parallel_version[8];
int core_id_timeout;
unsigned long long parallel_configured_size;
unsigned long long separate_configured_size;
char key_file[64];
char ca_list[64];
char dh_file[64];
int request_logged;
char user_sector[32];
char global_mask[128];
char global_secure_keyword[32];
char disk_partition[3][64];
int request_logged;
int trans_sock;
int customer;
char expire_date[16];
char expire_version[10][8];
char unsupported_version[10][8];
int pending_timeout;
unsigned long long large_parallel_transfer_size;
char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
char global_keyword[32];
METADATA md;
int customer_type;
char dt_database[16];
char edc_database[16];
MYSQL dt_sql;
MYSQL edc_sql;
int edc_init;

/* Holds the start and end transfer time for the data */
unsigned long long start_transfertime;
unsigned long long end_transfertime;
char transfertype[24];
    char *secure_keyword_array[1000];
int not_approved_cnt;
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
