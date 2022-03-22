#ifndef m2shadow_prototypes_h
#define m2shadow_prototypes_h

#include "m2defines.h"

void bld_header(
    char *buffer,
     int req_type,
    int data_type,
    int transaction_num,
    int pre_state);

/* Sends updated data to remote data base server */
/* Passed a list of directories to shadow to remote server */
void shadow_dir2rmt(char req_type, char *dir[]);

/* Sends updated data to remote data base server */
/* Passed list of directories and one file for each directory */
void shadow_dir_file2rmt(char *dir[], char *file[]);

/* Gets shadowed data from remote database server */
void shadow_get(void);

/* Refresh 1-5 database directories */
void refresh_data(int start_index, int end_index, int req_type, char *dir[], char *files[5][M2TOTAL_FILES]);

/* Refresh a remote mask set or template */
void rmt_refresh_data(void);

void rmt_create_data(void);
    
void rmt_mark_data(void);

/* Send refresh request to remote server */
/* No data is sent with this request */
void refresh_request(int req_type);

/* Refresh data done after header already sent */
void refresh(char *dir[], char *files[5][M2TOTAL_FILES]);

int get_rmt_ret_code(void);

void m2server_send(char *master_site);
int wait_rmt2upd(int state_fd);


int bld_check_sums(
    char *dir,		/* Data or delta directory */
     char *cs_buffer,	/* Place where check sums put */
    char *files[]);	/* List of files to build check sums for */

/* Refresh 1-5 database directories */
void refresh_data(int start_index, int end_index, int req_type, char *dir[], char *files[5][M2TOTAL_FILES]);

/* Refresh a remote mask set or template */
void rmt_refresh_data(void);

void rmt_create_data(void);
    
void rmt_mark_data(void);

void resend_data(void);

void resend_entry_send(int server_index); 

void resend_paths_init(int data_type);

/* Deletes an entry from the resend table after it has been sent */
int resend_entry_dlt(char *server_site, char *entry);


/* Adds new entry when data must be resent */
int resend_entry_add(char *server_site, int remote_req_type);

#endif
