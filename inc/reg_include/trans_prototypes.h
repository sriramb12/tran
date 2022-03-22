#ifndef trans_prototypes_h
#define trans_prototypes_h

#include <sys/types.h>
#include <sys/stat.h>
#include "company.h"
#include "trans_defines.h"
#include "disk_usage.h"
#include "zlib.h"


/* check_disk_space.c */
/* Checks to see if adequate disk space is available to retrieve specified file */
/* Currently not being used */
void check_disk_space(long long size);

/* chk_password_name.c */
void chk_password_name(char *user_name);

/* dbextend_big_file.c */
int dbextend_big_file(int argc, char **argv);

int extend_big_file(char *key, int days);
/* dbformat_email.c */
int dbformat_email(void);

/* Get big file from Transcend */
int get_big_file(REQ_TYPE req_type, REQ_TYPE parallel_type, char *primary_key, char *secondary_key, char *file_dir, char *file_ptr);

int get_big_file_init(
    REQ_TYPE req_type, 
    REQ_TYPE parallel_type, 
    char *primary_key,
    char *secondary_key,
    char *buffer,
    char *file_dir,
    char *alternate_file);

/* dbget_release.c */
int dbget_release(char *key, char *argv);

int dbget_icom_release(char *key, char *argv);

/* Get history of Transcend file */
int dbhist_big_file(char *secure_key, char *file_key);

/* Put big file in Transcend */
int dbput_big_file(REQ_TYPE req_type, TRANS_FILE_TYPE file_type, 
#ifdef TRANSCEND_GUI
    char *description,
#endif
#ifdef JAVA
    char *description,
#endif
    int argc, char **argv);

/* dbquery_user.c */
int dbquery_user(int req_type, char *key);

/* Read a keyword */
int dbread_keyword(int req_type);

/* dbreg_query.c */
int dbreg_query(char *site);

/* dbreg_user.c */
int dbreg_user(REQ_TYPE req_type,
               char *site,
               char *email1,
               char *email2);

/* dbsend_keyword.c */
int dbsend_keyword(char *secure_key, 
    char *file_key,
#ifdef TRANSCEND_GUI
    char *description,
#endif
#ifdef JAVA
    char *description,
#endif
    int argc, char **argv);

/* Sends numerous requests to the Transcend server */
int dbsend_request(char req_type, char *info);

/* TransWeb request needed to pass in two keys */
int dbsend_transweb_request(char req_type, char *primary_key, char *secondary_key);
/* Returns list of items to the client */
int dblist_request(char req_type, char *primary_key, char *secondary_key, char **list[]);

int dbsend_keyword_request(char req_type, char *info);

/* dlt_old_reg.c */
int dlt_old_reg(void);

/* email_send.c */
void email_send(char *replyto, char *recipients, char *subject, char *body);

char file_type_get(char type);
/* Gets the return code */
short get_ret_code(void);

/* Gets the return code plus a log string */
short get_log_string(void);
/* getwd.c */
char *getwd(char *name);

/* put_file.c */
int put_file(REQ_TYPE req_type, long long size, long mode, char *full_file_name, char *root_file_name, int argc, char **argv, int is_tar);

/* tmp.c */
int ip_addr_lookup(char *string, char *address, char *name);

/* trans_client.c */
int is_keyword(REQ_TYPE req_type, char *key);
int main(int argc, char **argv);

/* trans_client_globals.c */

/* xfer_status.c */
/*
void xfer_init(XFER_STATUS *x_status, unsigned long file_size);
void xfer_status_print(XFER_STATUS *x_status, long new_bytes_xfered);
*/

int init_email_addr(
    int argc,
    char **argv,
    char email_addr[6][64],
    char *send_email[]);


int remote_company_email_init(char email_addr[6][64]);

/* Get new password from the user */
int pwd_get_new(char *pwd);

/* Build message to send to Transcend server */
int init_trans_socket(
    REQ_TYPE req_type,
    char *key1,
    char *email1,
    char *email2,
    unsigned long long size,
    long file_mode,
    char file_type,
    char *par1,
    char *par2,
    char *par3,
    char *par4,
    char *par5,
    char *par6);

int put_gzip_file(
    REQ_TYPE req_type,
    REQ_TYPE parallel_type,
    long long size,
    long mode,
    TRANS_FILE_TYPE file_type,
    char *full_file_name,
    char *root_file_name,
    char *c_option_ptr,
#ifdef TRANSCEND_GUI
    char *description,
#endif
#ifdef JAVA
    char *description,
#endif
    int argc,
    char **argv,
    int is_tar);
int get_gzip_file(
    char *file_name,
    long long size,
    long mode,
    TRANS_FILE_TYPE file_type,
    int compression,
     char *tar_type,
    char *file_dir,
    char *file_ptr);

/* Retrieves file from server */
int get_file(
    REQ_TYPE req_type,
    char *file_name,
    unsigned long mode,
     char file_type);

/* Verify user entered correct core ID and OneIT password */
int dbx500authen(char *x500_id, char *x500_pwd);

/* Find root file name */
char *root_find(char *ptr, char c);

int block_retrieve(gzFile file, FILE *fp,
    TRANS_FILE_TYPE file_type,
    char *file_name,
    char get_block);

int mac_header_read (gzFile file, voidp buf, char get_header); 

int no_echo(char *line);

int echo_stars(char *line, int parse_cr);

int echo__stars(char *line);


void email_files_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *file1,
    char *file2);

void email_file_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *file);

void email_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *body);


char * getwd(char *name);


/* Connect to the server socket */
/* Send the header information and get response from server */
/* If the connect fails or the response from the server is not SUCCESS, */
/* this function puts reason for failure in rtn_msg */
/* It also closes the socket */
int sock_connect(void);

short init_socket(char *buffer);

/* Used to get IP node address of server */
/* If cannot find address using gethostbyname, uses default specified */
int ip_addr_lookup(
    char *string,
    char *address,
    char *name);

/* Used to change IP address to network format */
int ip_addr_copy(char *string, char *address);

/* Looks up IP node address using gethostbyname */
/* Used to get IP node address of client machine */
/* If not found, uses default address */
void ip_addr_find(char *addr, char *name);

/* Encrypt user's PIN or password */
/* Encrypted password placed in new_pwd */
int password_crypt(char *new_pwd, char *old_pwd);

int help_print(char *ptr, char command, char help_option);

int version_display();

void dir_print(char *dir);

int dbput_zip_dir(
    REQ_TYPE req_type,
    int argc,
    char *argv[]);

int dbput_command_dir(
    REQ_TYPE req_type,
    int argc,
    char *argv[]);

/* Puts multiple files listed in a file into Transcend or InterCom */
int dbput_big_files(
    REQ_TYPE req_type,
    int argc,
    char *argv[]);

/* Put files from the command line */
int dbput_command_files(
    REQ_TYPE req_type,
    int argc,
    char *argv[]);

void transrc_read(char *rc_file);

void line_parse(char *line);

void site_copy(char *line, char *keyword6, TRANS_COMPANY *site);


int company_lookup(int argc, char **argv);

/* Find site user specified */
int site_find(char extension);

#ifdef MULTIPLE_INTERNAL_SERVERS
void site_print();
#endif

void transrc_read(char *rc_file);

void line_parse(char *line);

void site_copy(char *line, char *keyword6, TRANS_COMPANY *site);

/* Perform Mettracks command */
int dbmettracks(char *dir);

/* Check the site */
int dbsite_check(char *extension);

void password_encrypt(
    char *password,
    char *pwd,
    int len);
float get_random( int *ix );
void  user_password_encrypt(char *crypt_pwd, char *pwd);


/* Adapted from a function written by Howard Anderson */
/* This function does not perform any network functions */
void password_encrypt(
    char *password,
    char *pwd,
    int len);

void user_params_init(void);

int whereis_tar(char *directory);

int whereis(char *directory, char *command);

/* Register user if not already registered */
short user_autoreg(void);

int dbmulti_list_request(
    char req_type,
    char *keyword,
    char **list[],
    char **status);

int user_keys_rtv(void);

char **trans_keys_rtv(int req_type, char *site,  char **status);

void trans_repository_set(int index);


void trans_repository_scan(char *line);

/* Used to delete both secure and user keys from one or all repositories */
int trans_keys_delete(int req_type, char *site);

/* Send email messages collected on dropbox5 once a minute */
int fss_email_send(void);

/* Look for gnutar or gtar on user's system */
int whereis_tar(char *tar_path);

/* Get and look for company for put commands */
void company_get(char *extension, char *argv[]);

/* Used for retrieve options */
void repository_get(char *extension);

/* Used for getting release, registering, user information, etc. */
void local_repository_get(char *extension);


/* Companies only defined for InterCom */
/* Print list of defined companies */
void site_print(void);


int extension_calc(char *extension, int *search, int argc, char **argv);


int company_find(char *extension, char *transnet_access);




/* The following three functions used for both single and multiple connect */

void repository_print(void);

/* Sets default site for transaction */
int default_repository_set(void);

int default_site_set(void);



/* Get the latest company table from the data repository */
void company_table_rtv(void);
void repository_find(char extension);


/* Determine mask set name */
/* If can deduce mask set name from file name, simply use it */
/* If not, prompt user for mask set name */
int mask_info_get(char *file_name, char *mask);

int arg_scan(char *line);

/* Request FTP retrieval of a file from a remote company */
int ftp_trans_get(char *argv[], int argc);

int is_mask(char *mask);

int dbmanual_reg ( int req_type, int argc, char **argv);

int dblist_send(
    char req_type,
    char *primary_key,
    char **list[]);

int gzip_file(
    REQ_TYPE req_type,
    REQ_TYPE parallel_type,
    long long size,
    long mode,
    TRANS_FILE_TYPE file_type,
    char *root_file_name,
    char *full_file_name,
#ifdef TRANSCEND_GUI
    char *description,
#endif
    char *tar_type,
    int argc,
    char **argv,
    int is_tar,
    char *tar_command,
    char *file_command);

/* Verify that DTMS sign-off has been completed */
int dtms_signoff_check(char *project, char *mask);

long long block_put(char *file_name,   int server_mem_key, char *keyword, int num_blocks, long long offset, int block_num);

unsigned long long separate_block_put(char *file_name,   int server_mem_key, char *keyword, int num_blocks, unsigned long long offset, int block_num, unsigned long long *checksum);

	    int random_block_put(int read_fd, int write_fd, char *keyword, int server_mem_key, int block_num);

/* Used to transfer blocks separately */
unsigned long long separate_block_get(char *file_name, char *keyword,  unsigned long long offset, int num_blocks, int block_num, int server_mem_key, unsigned long long *total_checksum);


/* Used to transfer parallel blocks */
short block_get(char *file_name, char *keyword,  unsigned long long offset, int num_blocks, int block_num, int server_mem_key);

/* Print amount of data transferred */
int size_print(long long total_size, long long size, int print_percent);

/* Read number of bytes transferred from the status file */
long long status_file_read(char *status_file, unsigned long long *checksum);

/* Read data from status file which tells approximately how much dta transferred */
long long status_file1read(char *status_file);

int put_parallel_compressed_data(char *file_name, char *keyword,
long long size, TRANS_FILE_TYPE file_type);

/* Writes the compressed data to 10 parallel processes */
int put_parallel_uncompressed_data(char *file_name, char *keyword,
long long size, TRANS_FILE_TYPE file_type, 	
FILE *fp);


int get_parallel_compressed_data(char *file_name, char *keyword,
long long size, long mode, TRANS_FILE_TYPE file_type);

char *time_left_calc(unsigned long long total_size, unsigned long long transferred_size, int elapsed_time);

/* Create the shared memory */
int shared_memory_create(char *keyword, int block_cnt);

int shared_memory_allocate(int mem_key);

/* Write to the shared memory */
void shared_memory_write(
    long long size, unsigned long long checksum, int block_num, int finished);

/* Set finished to DBYES indicating transfer for this block has completed */
void shared_memory_finish(int block_num);


/* Read from shared memory */
int shared_memory_read( unsigned long long *size, unsigned long long *checksum);
int shared_memory_1read( unsigned long long *size, unsigned long long *checksum);

/* Initialize shared memory */
void shared_memory_init( int block_num);
void shared_memory1init( void);

    /* Removed the shared memory no longer needed */
void shared_memory_release(void);

/* Calculate directory size */
long long dir_size_calc(char *full_file_name,  struct DIR_INFO *dir_ptr);

/* Find the extension ratio in the list */
float list_ratio_find(char *file_name);

/* Retrieve list of file extensions */
int dbsend_list_extension_request(void);

/* This does non SSL initialization */
/* It also handles going to alternate server sites */
int non_ssl_init(char req_type, char *buffer);

/* This does the connect for an SSL socket */
int ssl_init(char *buffer);

/* Checks for errors during connect and calls alternate server if needed */
int intermediate_ssl_init(char req_type, char *buffer);
/* Retrieve the trans_dirlist.txt file and write output to a file */
int transdirlist_file_rtv(char *keyword);

/* Retrieve the trans_dirlist.txt file and write output to standard out */
int transdirlist_rtv(char *keyword);

/* Transfer dirlist to repository */
int transdirlist_put(void);

/* build name for transdirlist file and set log options */
void transdirlist_set(void);

/* Add new file name to the temporary transdirlist file */
void transdirlist_add(char *file_name, long long size);

/* Add file name to transdirlist and include directory path */
void transdirlist2add(char *dir_name, char *file_name, long long size);

int tcp_connect(char *host,char *address, int port);

/*
void check_cert(SSL *ssl,char *host);
*/

/* Sends header to the server */
int header_send(char *buffer);

/* Build header sent to server */
/* This is used by all the data transfer servers */
void header_build(
    char *buffer,
    REQ_TYPE req_type,
    char *key1,
    char *email1,
    char *email2,
    unsigned long long size,
    long file_mode,
    char file_type,
    char *par1,
    char *par2,
    char *par3,
    char *par4,
    char *par5,
    char *par6,
    char *user_login);

/* Encrypt ITAR data being sent to Photronics */
void encrypt_itar_data(char **file_name, char **request_type);

int edc_request(REQ_TYPE req_type, char *key1, char *key2, char *key3);

/* Send approval request to InterCom server */
int edc_approve(char *domain);
#endif
