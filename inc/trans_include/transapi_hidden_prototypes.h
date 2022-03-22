#ifndef transapi_hidden_prototypes_h
#define transapi_hidden_prototypes_h

#include <sys/stat.h>
#include <time.h>
#include "global_defines.h"
#include "rtn.h"
#include "transapi_defines.h"

extern int sock;


/* the following functions are hidden from the Trans API */

/* Set a simple request */
int trans_simple_request_set(int trans_index, REQ_TYPE req_type, char *key);

/* Perform a get request */
/* This function is hidden from the user */
int trans_get_request(int trans_index);

int trans_custom_request(int trans_index);

/* Send a simple request to the server */
int trans_request_send(REQ_TYPE req_type, int index);

/* Scan description sent to receivers when secure keywords are generated */
int trans_description_scan(int trans_index);

/* Set the iCAP class for data being transferred */
/* This is valid for put and get requests */
int icap_class_set(int index, int argc, char **argv, int increment);

/* Set the email addresses */
int trans_email_set(int trans_index, char **email_list);


/* Build list of e-mail addresses to send */
void commandline_email_set(int trans_index, int arg_cnt, int argc, char **argv);


/* Set e-mail notification to remote company */
/* Maskshops receive e-mail notification automatically when data transferred to them */
int trans_company_email_set(int trans_index);
/* Read file names from a file */
/* Then set symbolic links */
int trans_dir_files_read(int trans_index, char *file_name);

/* Set symbolic directory for the -k and -z options */
int trans_symbolic_dir_set(int trans_index, char *symbolic_dir);

/* Link files to symbolic directory before transfer performed */
int trans_dir_files_link(int trans_index, char **file_list);

/* Set the transfer type */
/* this simply stores the requested type, but does not guarantee it will be used */
int trans_put_transfer_type_set(int trans_index, char transfer_type);

/* Transfer data using serial transfer */
int trans_serial_put(int trans_index, FILE *fp);

/* Perform a put */
/* This is called from trans_perform */
int trans_put_request(int trans_index);

/* End of functions from trans_put */

/* Determine the request type to use */
/* this is done after transfer type file name and size set */
void trans_request_type_set(int trans_index);

void release_info_print(char **argv, int ret);
int trans_release_get(int trans_index);

/* Print commands to retrieve available releases */
int trans_release_print(char **argv);

/* Verify this is a valid keyword */
int trans_is_keyword (char *keyword);

/* Set the transfer type */
/* this simply stores the requested type, but does not guarantee it will be used */
int trans_get_transfer_type_set(int trans_index, char transfer_type);
int trans_get_parallel_compressed(int trans_index);

/* Retrieve data that is uncompressed as it is written to disk */
int trans_get_parallel_uncompressed_data(int trans_index);

/* Get separate using shared memory */
int trans_get_separate_compressed_data(int trans_index);

int trans_parallel_put(int trans_index);

/* Writes the compressed data to 10 parallel processes */
int trans_uncompressed_parallel_put(int trans_index);


/* Send separate processes using shared memory */
int trans_put_separate_compressed_data(int trans_index);


unsigned long long trans_separate_block_put(int trans_index, int num_blocks, unsigned long long *total_checksum);

unsigned long long trans_block_put(int trans_index, int num_blocks);

	    int trans_random_block_put(int read_pipe, int write_pipe, int trans_index);



unsigned long long trans_separate_block_get(int trans_index, unsigned long long *total_checksum);

/* This function is used for parallel block gets and uses shared memory */
short trans_block_get(int trans_index, int block_num);

/* Get random blocks of data from the server */
	    int trans_random_block_get(int fi,     int trans_index, int block_size);

short trans_socket_init(int trans_index);

/* Send simple request to the server */
/* This consists of sending a header and retrieving a response from the server */
int trans_simple_request(int trans_index);

unsigned long long trans_separate_block_get(int trans_index, unsigned long long *total_checksum);

/* This function is used for parallel block gets and uses shared memory */
short trans_block_get(int trans_index, int block_num);

unsigned long long trans_separate_block_put(int trans_index, int num_blocks, unsigned long long *total_checksum);

unsigned long long trans_block_put(int trans_index, int num_blocks);


/* Get number of days to extend file from user */
int trans_days_set(int argc, char **argv);

int trans_company_lookup(int argc, char **argv);

int extension_calc(char *extension, int *search, int argc, char **argv);

void repository_find(char extension);

int company_find(char *extension, char *transnet_access);

/* Finds address of server where request is sent to */

/* Version of company_lookup used for companies with only one connection */

/* The following three functions used for both single and multiple connect */

/* Encrypt ITAR data before sending to the maskshop */
int trans_encrypt_itar_data(int trans_index, char **file_name, char **request_type);
/* Send secure keyword to one or more customers */
int trans_secure_keyword_send(int trans_index);

/* Retrieve history for keyword */
int dbhistory_big_file(int trans_index);

int trans_query_user(int trans_index);


/* Retrieve the trans_dirlist.txt file and write output to a file */
int trans_dirlist_file_rtv(int trans_index);

/* Retrieve the trans_dirlist.txt file and write output to standard out */
int trans_dirlist_rtv(int trans_index);

/* Transfer dirlist to repository */
int trans_dirlist_put(int trans_index);

/* Add new file name to the temporary trans_dirlist file */
void trans_dirlist_add(char *file_name, unsigned long long size);

/* Add file name to trans_dirlist and include directory path */
void trans_dirlist2add(char *dir_name, char *file_name, unsigned long long size);

/* build name for trans_dirlist file and set log options */
void trans_dirlist_set(void);

int trans_dirlist_read(char **files);


/* Log to the trans_dirlist file */
void trans_dirlist_log(char *file_name, unsigned long long size);
int trans_dirlist_name_clip(char *file_name);

int trans_dirlist_lines_count(char *file_name);

/* Get a file and uncompress it during transfer */
int trans_uncompress_get( FILE *fp, int trans_index);

/* Retrieves a file without uncompressing it */
int trans_compress_get(FILE *fp, int trans_index);


int trans_file_get(int trans_index);

/* Print warning if parallel type requested but not performed */
void parallel_warning_print(TRANS_FILE_TYPE file_type, unsigned long long size, int index);

/* See if file already exists */
/* if it already exists, ask user if he wants to overwrite it */
int get_delete_check(char *file_name, char *file_dir, char *alternate_file, TRANS_FILE_TYPE file_type);

int auto_get_delete_check(int trans_index);

short trans_get_init(int trans_index,
    char *file_dir);
 

int trans_get_data(int index, int parallel_ret);
/* Allocate space for a field and assign value */
/* This function may not be used */
int trans_field_allocate(char **field, char *value);

/* Perform EDC request */
int  trans_edc_request(int trans_index, REQ_TYPE req_type, char *key1, char *par2, char *par1);

/* View support users */
int  trans_support_view(int trans_index, char *email_list);

/* Perform support request */
int  trans_support_request(int trans_index, int argc, char **argv, char *email_list, char *admin_list);

/* Create the shared memory */
int shared_status_create(char *keyword);

int shared_status_allocate(int mem_key);

/* Write to the shared memory */
void shared_status_write(
    int finished, unsigned long long size);

/* Write final status of transfer */
void shared_status_finish(int finished);



/* Read from shared memory */
int shared_status_read( unsigned long long *size);


    /* Removed the shared memory no longer needed */
void shared_status_release(void);
/* Set custom request not requiring a valid 9 character keyword */
int trans_custom_basic_request_set(int trans_index, REQ_TYPE req_type, char *keyword);

/* Set custom keyword request for request requiring a 9 character keyword */
int trans_custom_keyword_request_set(int trans_index, REQ_TYPE req_type, char *keyword);


/* Set specified site */
int trans_site_set(char *extension);

int trans_site_print(void);


/* Print defined sites for Transcend and InterCom */
int transapi_site_print(int index);

/* Retrieve status of Transcend servers */
/* This function is used by the GUI to report server status */
int transapi_server_status_rtv(int index);

/* Print list of repositories for this tool */
int trans_repository_print(void);


/* Find the repository for the specified keyword extension */
void trans_repository_find(char extension);

void trans_repository_scan(char *line);

int trans_site_find(int index, char *extension);


/* Sets default site for transaction */
int trans_default_repository_set(void);

int trans_default_site_set(void);

void trans_dtms_server_set(void);

/* Find the server closest to the customer */
int trans_closest_server_set(void);

/* Retrieve status of Transcend servers */
/* This function is used by the GUI to report server status */
int trans_server_status_rtv(int index);

/* Print out default repository */
void trans_current_repository_print(void);


/* Set repository */
int  trans_repository_set(int index);

/* Retrieve list of strings from the server */
int trans_list_rtv(int index, char **list[]);
/* Read values from the .transrc file */
void trans_rcread(char *rc_file);

/* Set the request type and key for a simple request */
/* These transactions are for requests which do not use a keyword */
/* This function is hidden from the API */
int trans_simple_basic_request_set(int index, REQ_TYPE req_type, char *key);

/* Sends header to the server */
int trans_header_send(char *buffer);

/* See if user wants to abort transaction */
/* Resets to put verify if data going to mask shop */
void trans_maskshop_req_type_reset(int index);

/* Sends list of files being transferred to the maskshop */
int trans_maskshop_filelist_send(void);

/* Pulls file names out of sendtomaskshop log */
int trans_maskshop_files_find(char *files[], char *file_name);

/* Determine mask set name */
/* If can deduce mask set name from file name, simply use it */
/* If not, prompt user for mask set name */
int trans_mask_name_set( char *file_name, char *mask);

int trans_is_mask(char *mask);

/* Open file with information about files being sent to the maskshop */
int trans_maskshop_file_open(void);

/* Begin building the file to return to the data base */
int trans_maskshop_buffer_build(int index, char *buffer);

/* Close the file with the mask set info */
void trans_maskshop_file_close(void);

/* Send maskshop file to Intercom server */
void trans_maskshop_file_send(void);

/* Log file name in maskshop file */
void trans_maskshop_file_log(char *file_name, struct stat  *buf, int get_buf);

/* Send file to Intercom server */
int trans_intercom_file_send(int first_index, int is_icom);

/* Upload the Intercom log file */
int trans_intercom_file_put(int first_index, int is_icom);

/* Retrieve mask set history */
int maskshop_history(char *mask);

/* The trans index is passed in with key1, par1, and par2 initialized */
int trans_edc_request(int index, REQ_TYPE req_type, char *key1, char *par2, char *par1);

/* Send approval request to InterCom server */
/* Pass in index to Trans structure, e-mail address and description */
int trans_commandline_approve(int index, REQ_TYPE req_type, int argc, char **argv);

/* Routine for retrieving a file */
int custom_file_get(int index);

/* Used to delete both secure and user keys from one or all repositories */
int trans_keywords_delete(int index, int req_type, char *site);

/* Build header put in file sent to the maskshop */
int trans_maskshop_header_build(int index, char *buffer, int plate_cnt, int processstep_cnt, int file_cnt);

/* Scan in remote directory for data sent to the foundry */
void foundry_info_get(int index);

/* Used to delete both secure and user keys from one or all repositories */
int trans_keywords_delete(int index, int req_type, char *site);

/* Send secure keyword to one or more customers */
 int trans_secure_keyword_send(int index);

/* Retrieve history for keyword */
int trans_history_big_file(int index);

/* Retrieve LDAP infor for user */
int trans_query_user(int index);

/* Retrieve list of file keywords for this user */
int trans_user_keys_rtv(int index);

/* Retrieve multi list of keywords */
int trans_multi_keys_rtv(int index);

int trans_commandline_user_keys_rtv(int index, int *status_printed);

/* Perform auto registration */
int trans_reg_user(int index);

/* Perform transcend registration */
int trans_reg_perform(int index, int argc, char **argv);

/* Display help for specified command */
int trans_help_print(char *ptr, char command, char help_option);

/* Set the email addresses for transaction from a string */
int trans_email_string_set(int index, char *email_string);

/* Set the email addresses for transaction from an array of pointers */
int trans_email_list_set(int index, char **email_list);

/* Calculate disk usage for directory being transferred */
unsigned long long trans_disk_usage(char *directory, int follow_links, int offset, int dir_levels);

int trans_data_log(char *log_file, char *string);

#endif
