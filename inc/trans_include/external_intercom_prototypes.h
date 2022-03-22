#ifndef external_intercom_prototypes_h
#define external_intercom_prototypes_h

#include <mysql.h>
#include "trans_server_defines.h"
#include "trans_server_macros.h"
#include "global_defines.h"
#include "edc_defines.h"


/* Map status from the database to defined values */
int sql_status_map(char *status);
int sql_pending_request_insert(char *customer_email, char *own_id, char *description);

/* Check customer registration */
int sql_reg_check(void);

/* Verify authorized to perform get transaction */
    int sql_approve_check(char *email_addr);

/* Verify user is approved to retrieve data */
int sql_email_check(char *email_addr);

/* Verify user registered with specified domain */
int sql_domain_check(char *email_addr);

/* Map status from defined values to string */
void sql_status_map_str(APPROVE_STATUS st, char *status);

/* Look up user's site in the subnet table */
int site_subnet_lookup(char *site);

int check_reg(void);

void dlt_big_file(char *key);	/* key for data being deleted */


void extend_big_file(char *key, int days);



long long encrypt_send(char *local_file, char *real_file, char *remote_file, char *remote_dir, char *history_file, long *encrypt_time);


long elapsed_time_calc(long old_time, char *hms, char *time_string);
void hms_calc(long seconds, char *string);

/* Send the remote file via FTP */
void ftp_put(int offset, char *local_file, char *remote_file, char *remote_dir,
 char *history_file, char *extra_info, long encrypt_time);


int ftp_put1block (char *local_file, char *remote_file, char *remote_dir, char *dir_path, char *dir_ptr[]);

int ftp_append1block (int fd, char *remote_file, char *remote_dir);

int ftp_append1buffer(char *remote_dir, char *remote_file, char *buffer, int len, int cnt);

long long ftp_get_size(char *remote_dir, char *remote_file);

void name_build(char *name);


/* Try to login into ftp_gate0 50 times */
void ftp_login(void);
void ftp_resend(char *remote_dir, char *key);

/* Update the status in the meta data */
void ftp_status_update(char *key, char *string);

void ftp_complete_update(char *key, long long encrypt_size);

/* Gather all information needed to encrypt and send the dta */
/* this function initializes common things needed by encrypt_send and ftp_put */
/* It then calls both of these functions */
void ftp_send(char *remote_dir, char *description, char send_email, char *email_string, char *log_string, char **email_array);

/* Read encryption and FTP info for remote company */
void ftp_company_read(void);
/* Send email notification to additional users */
void user_email_send(void);

/* The directory name passed in must not have an ending slash */
int files_dlt_transient(char *directory);

int find_email(
    char *user_site,	/* site of user looking for */
    char *email_addr,	/* Email address returned to calling function */
    char address_type);	/* Indicates official or unofficial Email address */

void mod_email_big_file(char *key);

void lock_big_file(char *key);

/* Unlock previously locked big file */
void unlock_big_file(char *key);

void get_big_file(
    char *key, int reg_status);		/* Key for big file to retrieve */

void get_release(char *key);

void get_icom_release(char *key);

/* Send a release file */
void get_release_file(char *file_name, char *root_file);

void get_intercom_file(char *file_name, char *root_file);

void info_big_file(char *key);

void get_release_info(void);

void history_big_file(char *key);

void lookup_release_keyword(char *release_key, char *file_key);

/* Retrieve list of secure keywords for specified user */
void keyword_read(void);

void keyword_send(void);

FILE *open_big_file(char *keyword);

/* Read big file */
/* Reads in data from big file */
FILE *read_big_file(char *key, char line[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs);

/* Reads in big file and makes sure user owns file */
FILE *read4own_big_file(char *key, char lines[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs);

/* Reads in big file, makes sure user owns data, makes sure company authorized */
void read4company_big_file(char *key, char lines[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs, int reg_status);

FILE *read4info_big_file(char *key, TRANS_FILE_STAT *fs, int close_flag);

/* Write out modified contents of big file */
void write_big_file(FILE *fp, char line[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs);

void owner_dlt(char *key);

void user_dlt(char *key);

void process_request(int socket);

void put_big_file(
    char *remote_dir,
    char *key,	/* File name passed from user */
    long long file_size);



void put_success_msg(char *key);
void return_success(char *msg, int log);

unsigned long long rcv_file(
    char *path,
    char *new_file,
    int dtms, /* DBYES or DBNO */
    long long *final_size);



void put_msg(char *key, char *log_string);



void read_icom_params(char *file_name);


void send_secure_keyword(
    char *file_keyword,	/* File keyword to be encrypted */
    char *email_addr[],	/* Email address used as encryption key */
    char *file_name,	/* Name of file in repository */
    char *file_size_string,	/* Original size of file */
    char file_type,
    long dlt_time,
    char *local_time,
    char *gm_time,
    char *description);

void size_check_big_file(char *key);

void send_rtv_email(
    char *from_email,
    char lines[TRANS_LINE_NUM][TRANS_LINE_LEN]);

/* Send one file to the client */
int send_file(TRANS_FILE_STAT *fs);

/* Shorten name of file to 32 characters */
void name_shorten(char *name, int len);




/* Sends last message, closes socket and cleans up temporary files */
void shut_down(
    char *msg,		/* Points to message sent to user */
    short rtn_code);	/* Holds return code sent to user */

/* Return nonexistent error to user */
void return_non_err(char *key);


void set_trans_disk_usage(void);

/* Retrieve tar file wht email messages */
void get_tar_file(char *file_name, char *root_file);

/* Initialize port number based on program type */
void db_socket_init(char option, int *db_socket);

void fss_rtv_logs(void);

/* Send file to client with alternate name */
void file_get_fss_log(char *dir, char *file_name, char *alternate_file);

/* Logs transaction in FSS report log */
void fss_report_log(char *keyword, char *file_name, char *mask);

/* Verify keyword is specified as the file keyword */
void fss_keyword_find(char *old_key, char *new_key);

void fss_rtv_logs(void);

/* Send file to client with alternate name */
void file_get_fss_log(char *dir, char *file_name, char *alternate_file);

/* Logs transaction in FSS report log */
void fss_report_log(char *keyword, char *file_name, char *mask);

/* Verify keyword is specified as the file keyword */
void fss_keyword_find(char *old_key, char *new_key);

void fss_err_log(char *msg);

/* Build md5sum for file before it is encrypted */
void md5sum_build(char *md5sum, char *local_file);

/* See if companies authorized to retireve the data */
int check_company_auth(char *user_specified_company, char *real_company, int len);

/* Verifies if is FSS, TransNet or AMD */
int is_fss_company(void);
/* Retrieve file from remote company */
void ftp_get(char *file_name, char *remote_dir);
/* Build FTP script to execute */
void ftp_script_build(char *keyword, char *file_name, char *remote_dir);
/* Decrypt the data */
long long ftp_decrypt(char *keyword, char *file_name);

/* Build the meta data */
long ftp_meta_data_create(char *file_keyword, char *file_name, 
long long file_size, char *dlt_time_string, char *gmt_time_string);

/* Send file keyword to sender */
void transweb_file_key_send(char *file, char *description, char *key, char *size, char *dlt_time, char *gmt_dlt_time,
     char **email_array, char **secure_key_array);

/* Verify external customer can access the keyword */
void transweb_keyword_verify(char *new_key, char *secure_key, char *file_key);

/* Verifies request is being made between NXP & external TransWeb customer */
void external_access_verify(void);


void transweb_key_find(char *new_key, char *secure_key, char *file_key);

/* Set customer type */  
int customer_set(char *remote_dir);


void email_file_write(char *email_addr, char *key, char *file_name, char *size_string, char file_type, char *local_time, char *gmt_time, char *description, char *msg_ptr);

/* parameters local_time and gm_time not used, but present for Transcend compatibility */
void send_transweb_keyword(
    char *file_keyword,	/* File keyword to be encrypted */
    char *email_addr[],	/* Email address used as encryption key */
    char *file_name,	/* Name of file in repository */
    char *file_size_string,	/* Original size of file */
    char file_type,
    long dlt_time,
    char *local_time,
    char *gm_time,
    char *to_company,
    char *description);


/* Code to retrieve and send email messages */
void fss_email_send(void);
void static_files_send(char *dir, char *files[]);

void file_put(char *dir, char *file_name);

/* Send the Email message */
/* Build message to send */
void secure_email_send(char *file_name, char file_type, char *description, char *file_size_string, char *secure_key, char *file_key, char *local_time, char *gmt_time, char *email_addr, char *msg_ptr);

/* Build MM message to send */
void external_mm_email_send(char *file_name, char file_type, char *description, char *file_size_string, char *secure_key, char *file_keyword, char *local_time, char *gmt_time, char *email_addr, char *msg_ptr);

/* Set message to send based on customer type and E-mail address */
char * customer_msg_set(char *email_addr, int reg_status);

void send_external_keyword(
    char *file_keyword,	/* File keyword to be encrypted */
    char *email_addr,	/* Email address used as encryption key */
    char *file_name,	/* Name of file in Transcend */
    char *file_size_string,	/* Original size of file */
    char file_type,
    long dlt_time,
    char *local_time,
    char *gmt_time,
    char *description[]);

/* See if at least one E-mail address ends with '@nxp.com' */
int check4email(char *email_list[]);

/* Log from and to addresses in email log */
void customer_email_log(char *email1, char *email2);

/* Build MM message to send */
void mm_email_send(char *file_name, char file_type, char *description, char *file_size_string, char *secure_key, char *local_time, char *gmt_time, char *email_addr, char *msg_ptr);


void transweb_keyword_send(void);
int email_send_type_set(char *file_keyword);

void cron_file_write(char *replyto, char *recipients, char *subject, char *msg, char *key);

/* Send e-mail retrieval notification via a cron job */
 void cron_rtv_email_send(char *to_email, char *from_email, char *msg); 

/* build message body for sending secure keywords */
char * secure_msg_body_build(char *msg_body,  char *email_addr, char *secure_key, char *file_key, char *file_name, char *size_string, char file_type, char *local_time, char *gmt_time, char *description);

/* Set program type sent in e-mail messages */
char *program_set(void);

void cron_file_write(char *replyto, char *recipients, char *subject, char *msg, char *key);

/* Create description if one not provided */
void description_create(char *description);

/* Add one or more users to the NXP Support group */
void support_add(char *email_file);

/* Delete one or more users from the NXP Support group */
void support_delete(char *email_file);

/* Verify user authorized to perform support transaction */
int support_check(char *email_file);


/* Get description and email and set other params */
int email_get_set(char *description,  char *email_array[], char *email, int *edc_auth);

/* Initialize mask set name */
void global_mask_set(void);

int maskshop_verify(char *company);

/* Generate mask set name from data sent */
void mask_name_generate(char *key);

/* Verify is a valid mask set name */
int is_mask(char *mask);

/* Rename the .incomplete dir to .split dir */
void incomplete_dir_rename(char *remote_dir, char *remote_file);

/* Transfer script to decrypt and combine files on FTP server */
int ftp_put_finale_script(int parallel, char *remote_file, char *remote_dir, char *remote_data_dir,
    char *cksum);

int ftp_put_decrypt_script(char *remote_file, char *remote_dir, char *remote_data_dir);

/* Increment the remote file name */
void remote_file_increment(char *new_remote_file);

long long ftp_get_parallel_size(char *remote_dir, int total_blocks, char *hms);

/* Split out directory components */
/* Make directories for sending parallel data */
void ftp_parallel_init (char *remote_dir, char *dir_path, char *dir_ptr[]);


int put_parallel_block(char *local_file, char *remote_file, char *remote_dir, 	
    unsigned long long offset, unsigned long long total_size, int block_num); 

int put_serial_block(char *local_file, char *remote_file, char *remote_dir, 	
    FILE *fp, unsigned long long total_size, int block_num); 

/* Send groups of blocks for the file */
void ftp_put_parallel_blocks
(
    char *local_file,
    char *new_remote_file,
    char *remote_dir,
    char *status_file,
    int total_blocks);

/* Send the remote file via FTP */
void ftp_put_parallel(int offset, char *local_file, char *remote_file, char *remote_dir,
 char *history_file, char *extra_info, long encrypt_time, char **email_array);

/* Send ack for large parallel transfer */
void put_big_file_ack(char *global_file, char *file_keyword,  char *email_array[], 
   char *email, char *remote_dir, int edc_auth);
void tmp_put_big_file_ack(char *global_file, char *file_keyword, 
    char *dlt_string, char *dlt_time_string, char *gmt_time_string, 
    char *email_array[], int line_cnt, char *remote_dir);

/* Receive data in parallel */
unsigned long long rcv_parallel(
    int parallel_type,
    char *path,
    char *real_name,
    char *file_keyword,
    long long *final_size);

unsigned long long rcv_separate(
    int parallel_type,
    char *path,
    char *real_name,
    char *file_keyword,
    long long *final_size);

void ftp_put_serial(int offset, char *local_file, char *remote_file, char *remote_dir,
 char *history_file, char *extra_data, long encrypt_time, char **email_array);

void get_big_file_ack(char *key);
/* See if SQL error occurred */
int sql_err_check(char *query, MYSQL *sql, char *err_msg);

/* Retrieve a column from an SQL table */
int sql_column_rtv(char *query, char *column);
/* Get manager's ID */
void sql_manager_rtv(char *manager, char *core_id);
/* Map status from the database to defined values */
int sql_status_map(char *status);

/* Write approval to requests table */
void sql_commandline_approve(char *customer_email);

/* Check customer registration */
int sql_reg_check(void);

/* Verify authorized to perform get transaction */
    int sql_approve_check(char *email_addr);

/* Verify user is approved to retrieve data */
int sql_email_check(char *email_addr);

/* Verify user registered with specified domain */
int sql_domain_check(char *email_addr);

/* Look up user's site in the subnet table */
/* This is used to tell what site user is coming from */
int site_subnet_lookup(char *site);


/* Add entry to keyword mapping table */
int sql_keyword_mapping_insert(char *file_keyword, char *secure_keyword, 
char *from_email, char *to_email, char *description, long upload_time);

/* Update entry to keyword mapping table */
int sql_keyword_mapping_update(void);

/* Add entry to intercom_history table */
void sql_intercom_history_insert( char *keyword, char *file_name, char *core_id, 
    unsigned long long original_size, unsigned long long compressed_size);

/* Update existing entry in intercom_history table */
void sql_intercom_history_update(char *keyword);

void sql_gdb_data_rtv(char *extra_data, int gdb_id);

int gdb_data_get(char *query, MYSQL_ROW *row);

/* Update the send2foundry_results table */
void sql_send2foundry_results_update(char *job_number, char *keyword);

/* Retrieve the mask, design and technology for a specified UID */
int sql_mtd_rtv(char *id, char *gdb_file, char *tag_name, char *mask, char *technology, char *design, char *gdb_extension);

/* Check approve status of customers */
/* Array of e-mail addresses passed in */
/* The purpose for this routine is to see if any approval requests are pending */
int new_sql_pre_auth_check(char *email_addr[], int auth_status[]);

/* Check approve status of customers */
/* Array of e-mail addresses passed in */
/* The purpose for this routine is to see if any approval requests are pending */
int sql_pre_auth_check(char *email_addr[]);

/* Calculate deletion time for specified number of days */
long day_dlt_time_calc(int days, char *current_gmt_time, char *dlt_time_string, char *gmt_time_string);

#endif
