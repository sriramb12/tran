#ifndef trans_server_prototypes_h
#define trans_server_prototypes_h

#include "reg_globals.h"
#include "trans_server_defines.h"
#include "trans_server_macros.h"

void admin_reg(void);

int check_reg(void);

/* Lookup users with LDAP */
void x500lookup(char *user_id);

/* Authenticate user with LDAP */
void x500authenticate(char *user_id, char *user_pwd);

/* Verify registration entry present for user in Transcend registry */
int reg_site_check(int server_check);

void dlt_big_file(char *key);	/* key for data being deleted */

int dlt_huge_dir(TRANS_FILE_STAT *fs, char *key);

void extend_big_file(char *key, int days);

/* The directory name passed in must not have an ending slash */
int files_dlt_transient(char *directory);

int find_email(
    char *user_site,	/* site of user looking for */
    char *email_addr,	/* Email address returned to calling function */
    char address_type);	/* Indicates official or unofficial Email address */

int find_reg_request(char *user_site);


void get_big_file(
    char *key);		/* Key for big file to retrieve */

void get_release(char *key);

void get_icom_release(char *key);

/* Send a release file */
void get_release_file(char *file_name, char *root_file);

void get_intercom_file(char *file_name, char *root_file);

void get_release_info(void);

void history_big_file(char *key);

void info_big_file(char *key);

/* Retrieve list of secure keywords for specified user */
void keyword_read(void);

void keyword_send(void);

void lookup_release_keyword(char *release_key, char *file_key);

void mod_email_big_file(char *key);

void lock_big_file(char *key);

/* Unlock previously locked big file */
void unlock_big_file(char *key);

/* Retrieve get ack for large parallel data */
int get_ack(char *file_keyword, int mem_key);


FILE *open_big_file(char *key);

/* Read big file */
FILE *read_big_file(char *key, char line[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs);

/* Write out modified contents of big file */
void write_big_file(FILE *fp, char line[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs);

void owner_dlt(char *key);

void user_dlt(char *key);

void process_request(int socket);

void put_big_file(
    char *key,	/* File name passed from user */
    unsigned long long file_size);	/* Size of file */


/* Send put ack */
void put_trans_file_ack(char *global_file, char *email_array[]);

/* This function decides whether to do serial or parallel transfer and performs the transfer */
void big_file_put(char *file_keyword);

int never_expires(void);
/* Strip off the Windows path */
void windows_path_strip(char *file_name);
    int dtms_send(char *key, char *file_keyword);

/* Send data to user when put completed */
void put_big_file_ack(char *global_file, char *email_array[]);


/* Verify at least one E-mail message ends in "@nxp.com */
int check4email(char *email_list[]);

/* Calculate number of milobyte for regular file */
void num_mbytes_calc(char *byte_string, long long bytes);

/* Calculate number of gbytes for huge directory */


void num_kbytes_calc(char *byte_string, unsigned long long bytes);



void put_msg(char *key, char *log_string);



void put_success_msg(char *key);
void return_success(char *msg, int log);

unsigned long long rcv_file(
    char *path,
    char *new_file,
    int dtms,
    long long *final_size);

long long rcv_huge_dir(
    char *path,
    char *new_file,
    long long *final_size,
    int *file_cnt);


void read_global_params(char *file_name);

    int line_split(char *line, char *keyword, char *value);

/* Assign value based on the keyword */
void value_assign(char *keyword, char *value);
    /* Assign default values */
void default_values_assign(void);

void reg_query(void);

int rtv_user_email(char *user_email);

/* Send one file to the client */
int send_file(TRANS_FILE_STAT *fs, char *key, int dtms);

/* Shorten name of file to 32 characters */
void name_shorten(char *name, int len);

/* Send one file to the client */
int send_huge_dir(TRANS_FILE_STAT *fs, char *key);
#define MAX_LENGTH 1024

void send_rtv_email(
    char *user_key,
    char *from_email,
    char *to_email,
    char *real_name,
    char file_type);

void send_secure_keyword(
    char *file_keyword,	/* File keyword to be encrypted */
    char *email_addr[],	/* Email address used as encryption key */
    char *file_name,	/* Name of file in Transcend */
    char *file_size_string,	/* Original size of file */
    char file_type,
    long dlt_time,
    char *local_time,
    char *gmt_time,
    char *description[]);


/* Indicates if email error message   sent for the current transaction */
/* Only one email error  message can be sent for each transaction */
int email_cnt;

/* Indicates how many log entries made for the current transaction */
int log_cnt;


void set_trans_disk_usage(void);



/* Sends last message, closes socket and cleans up temporary files */
void shut_down(
    char *msg,		/* Points to message sent to user */
    register short rtn_code);	/* Holds return code sent to user */

/* Return nonexistent error to user */
void return_non_err(char *key);

void size_check_big_file(char *key);

/* Verify NXP e-mail address entered */
void company_email_check(char *email_array[]);

void put_remote_msg(void);

/* Retrieves data from client and uncompresses and untars the data */
long long command_put_compressed_dir(char *keyword);

/* Retrieve compressed file and do not uncompress it as it is written to disk */
long long command_put_compressed_file(char *keyword);

/* Retrieve a compressed file and uncompress as written to disk */
long long command_put_uncompressed_file(char *keyword);

/* Sends directory to the client and tars and compresses the data as it is sent */
short command_get_compressed_dir(void);

/* Sends file to the client and compresses the data as it is sent */
short command_get_compressed_file(void);


long long file_put(char *dir, char *file);

/* Get data from client and pipe it through a command */
long long command_put(char *command);


/* Just like file get except do not pass file name */
int data_put(char *dir, char *file);

int files_put(char *dir, char *files[]);

/* Get output from a command */
long long command_get(void);



void command_get_line(char *command);

void file_data_get(char *file_data, char *file_name);

void block_get(
    char *key, long long offset, long num_bytes, int block_num, int mem_key);		/* Key for big file to retrieve */

int disk_index_read(char *key, char *real_name);

void block_put(
    char *key, unsigned long long offset, int mem_key, int block_num);		/* Key for big file to retrieve */

/* Get random blocks of data */
void random_block_put(
    char *key, 
    int mem_key,
    int block_num);		/* Key for big file to retrieve */

/* Read number of bytes transferred from the status file */
long long status_file_read(char *status_file, unsigned long long *checksum);

/* Delete data transferred and exit */
void delete_file_and_exit(char *keyword, char *file_name, char *msg, int err_code);

unsigned long long rcv_parallel(
    int parallel_type,
    char *path,
    char *real_name,
    char *new_file,
    long long *final_size);

/* See if file is a zipped file */
int is_zipped_file(char *file_name);

/* Read number of bytes transferred from the status file */
long long get_parallel(void);

/* Send a dynamicly created dirlist to the client */
int trans_dynamic_dirlist_rtv(char *key);

void transdirlist_rtv(char *key);

/* Receive a trans_dirlist file from the client */
void transdirlist_put(char *keyword);

/* Build a transdirlist on the server */
/* this is done when the data is transferred as a zipped tarred directory */
void transdirlist_build(char *file_keyword, char *real_name, int disk_index);

/* The directory name passed in must not have an ending slash */

void transdir_list(char *directory);

/* Create the shared memory */
int shared_memory_create(char *keyword, int block_cnt);

int shared_memory_allocate(char *keyword, int mem_key, int block_cnt);

/* Write to the shared memory */
void shared_memory_write(
    long long size, unsigned long long checksum, int block_num, int finished);

/* Set finished to DBYES indicating transfer for this block has completed */
void shared_memory_finish(int block_num);


/* Read from shared memory */
int shared_memory_read( long long *size, unsigned long long *checksum);

/* Initialize shared memory */
void shared_memory_init( int block_num);

    /* Removed the shared memory no longer needed */
void shared_memory_release(void);

/* Build string with file info for client when file retrieved */
void file_string_build(char *single_file, char *buffer);

/* See if this is a GUI */
int is_gui(void);


void ssl_err2string(char *string, int ret);

int core_id_crypt( char * core_id,  int mode );

#endif
