#ifndef opc_server_prototypes_h
#define opc_server_prototypes_h
#include <stdio.h>
#include "opcdir_stat.h"
#include "trans_defines.h"
#include "gzio.h"

#include <sys/stat.h>
void return_success(char *msg);

void put_job_err(char *err_msg, int job_id, int err_code);

void opc_read_params();

void shut_down(char *rtn_msg, short rtn_code);

int names_build(char *date1, char *date2, char *files[]);

/* Retrieve data from specified OPC repository */
void opc_cvs_get(char *repository, char *module, char *tag);

void file_lock(char *file_name);


/* Sends directory list to client */
/* For each file sends stat structure and file name */
void dir_put(char *directory);

/* Sends directory list to client */
/* For each file sends 64 bit stat structure and file name */
void dir64put(char *directory);

void opcdir_alist                         (char *project_dir, char *user_dir);

void opcdir_list(char *project_dir, char *user_dir);

void opcdir64list(char *project_dir, char *user_dir);

void opcdir_list_roots(void);

void opcdir64list_roots(void);

void opc_rmdir                           (char *project_dir, char *user_dir);

void opcdir_dlt_files                    (char *project_dir, char *user_dir); 

long long opcjob_mkdir                        (char *project_dir, char *email_dir, char *user_dir);

void opcdir_add_files                    (char *project_dir, char *user_dir); 

/* Sends specified files to the client */
long long opcdir_get_files(char *project_dir, char *user_dir); 

/* Read data from file list and send it to client */

void opcdir_stat(char *path);

/* Send data using 64 bit structure */
void opcdir64stat(char *path);

/* See if file name in tmp_file is './ */
int tmp_file_query(char *tmp_file);

/* Copy elements from 64 bit structure back into 32 bit structure */
void stat32copy(struct stat *buf, OPC32STAT *local_buf);

void stat64copy(struct stat *buf, OPC64STAT *local_buf);

void auth_check(void);

long long block_retrieve(gzFile file, FILE *fp,
    TRANS_FILE_TYPE file_type,
    char *file_name,
    char get_block);


/* Get a job number for the client */
void client_job_num_get(void);

/* Log OPC request */
void request_log(char *log_file);
#endif
