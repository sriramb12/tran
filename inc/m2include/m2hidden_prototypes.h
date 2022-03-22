#ifndef m2hidden_prototypes_h
#define m2hidden_prototypes_h
#include <stdio.h>
#include "maskshop.h"


/* Audit all mask sets */
int audit_mask(void);

int email_file_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *file);

int email_msg_file_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *msg,
    char *file);

int email_file_msg_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *file,
    char *msg);

void jdc_sendmail(
    char *sender,
    char *replyto,
    char *recipients,
    char *subject,
    char *body);

/* Connect to the server socket */
/* Send the header information and get response from server */
/* If the connect fails or the response from the server is not DB_SUCCESS, */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
short rmt_socket_init(
    int index,
    int req_type, int data_type, char *key1, char *key2, char *key3, int num,
    char *par1, char *par2, char *par3, char *par4, char *par5);

int m2line_get(FILE *fp, char *line);
/* Delete directory */
int m2dir_dlt(char *directory);

/* Used for template_read, template_checkout, */
/* template_read_prior,  and template_checkout_prior */
/* template_read_prior,  and template_checkout_prior */
/* Non-database functions should not call this function */
short template_get(void);

/* Build the tickler file */
int tickler_file_create(TICKLER *tk, MASKSHOP *ms);

/* Build the tickler stamp */
void tickler_stamp_build(char *tickler_stamp);

/* The following allows me to use month '01' instead of 'Jan' */
void tickler_stamp_convert(char *log_date, char *tickler_stamp);

/* Used to checkin machine files */
short mach_files_put(char *dir, char *files[]);

/* Initialize machine list */
short machine_init(char *files[]);

/* Put mask files back in database */
/* Does not send file if server does not want it */
short mask_files_put(char *dir, char *files[]);

short machine_files_put(void);

short machine_files_put_delete(void);

short machine_files_get(void);

/* Delete machine files from user's disk */
short machine_unlink(void);

/* Read the fab from the old create file */
/* This function expects create file to be in CWD */
/* Error occurs if old create file not present */
/* Returns M2SUCCESS or M2ERR */
short old_create_read(char *mask, char *fab);

short get_mask_ret_code(void);

short init_mask_socket(
    int req_type, int data_type, char *key1, char *key2, char *key3, int num,
    char *par1, char *par2, char *par3, char *par4, char *par5);
/* Perform total, weekly, or single audit */
/* For total and weekly audits, audit type passed in key1 */

short audit_data(int req_type, int data_type, char *key1, char *key2);


/* Retrieve X500 info from X500 database */
short user_rtv_fields(char *user_id);

short malloc_copy(char **string1, char *string2);

/* Send data to owner */
short own_server_send(int cnt, int req_type, int data_type, 
    char *key1, char *key2, char *key3, 
    int num, char *par1, char *par2, char *par3, char *par4, char *par5);

/* Get dynamicly allocated list */
short m2list_get(int req_type, int data_type, char *key, char **names[]);

/* convert string format to user ID */
void user_format_convert(char *user_string, char *user_array[]);

void close_down1f(char *err_msg, char *file);

void plates_find(char *files[], char *plate_list[], char *process_step_list[], int *plate_cnt, int *process_step_cnt);


/* Builds log file to send to Mask Designer database */
void maskshop_history_file_build(MASKSHOP *ms, TICKLER *tk, long long size, 
    int plate_cnt, int process_step_cnt, int file_cnt, char *size_array[], long time_array[],
char *plate_list[], char *process_step_list[], char *files[]);

/* Build list in multiple columns */
void col_list_print(FILE *fp, char *list[]);

/* Used to checkin, update, and overwrite mask and template data */
short data_update(int req_type, int data_type, char *key1, char *key2);

/* Shaow data to the databases once an hour */
short resend_data(void);

/* Send MARS report once a week */
int mars_resend_weekly(void);

short mask_rtv_info(int req_type, char *mask, char *requester);

/* Retrieve directories for templates and mask sets */
short mask_dir_get(char *dirlist[]);

/* Free file list no longer needed */
void m2filelist_free(char *filelist[]);

int m2filelist_add(char *name, char *filelist[], char *file_name);

/* Delete name from file list */
/* Returns SUCCESS if name deleted */
int filelist_delete(char *name);

/* Write the filelist to disk */
/* The file list is written to file  in current working directory */
/* file name is passed in */
/* Free the filelist, directory list and fab list */
void filelists_free(void);

/* Write out the filelist directory list and fab list */
int filelists_write(void);

/*Read in the file list,  directory list, and fab list */
int filelists_read(void);

int m2filelist_write(char *filelist[], char *file_name);

/* Read the file list from disk */
int m2filelist_read(char *filelist[], char *file_name);


/* Delete all files found on file lists from disk and free file lists */
void filelists_erase(void);

int dirlist_check(void);

/* Delete configuration_name if this is the 'fab' file */
void global_files_delete(char *root_file, int delete_flag);

short global_put(char *root_file, int override, int req_type, int delete);

/* Common routine for pulling down global tables */
short global_files_read(char *file_name);

/* See iff associated log file present */
short global_log_file_check(char *file_name);

/* Take version number out of PC library name */
short pc_version_strip(char *library_name, char *output_name);

/* Build PC library name embedding in the version number */
short pc_version_name_build(char *library_name, char *return_name, char *version);

short pc_version_rtv(char *library_name, char *version);

/* Build PC library name empbedding in the version number */
short pc_version_name_strip(char *library_name, char *output_name, char *version);

short is_version(char *string);
short pc_db_checkin(char *library_name);
short pc_local_checkin(char *library_name);

void rcfile_read(void);

short audit_report_send(int req_type);

short audit_network(int data_type, char *dir);

/* Check for possible error in the output file */
int ftp_possible_err(char *output_script);

#endif
