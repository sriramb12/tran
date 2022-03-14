#ifndef tc_server_prototypes_h
#define tc_server_prototypes_h

#include "tcq.h"

/* Initialize TC params */
int tc_params_init(char option);

/* Copy elements from 64 bit structure back into 32 bit structure */


/* Sends directory list to client */
/* For each file sends stat structure and file name */
void dir_put(char *directory);

/* Sends directory list to client */
/* For each file sends 64 bit stat structure and file name */
void dir64put(char *directory);

void tc_dir_alist                         (char *project_dir, char *user_dir);

void tc_dir_list(char *user_dir);

void tc_dir64list(char *project_dir, char *user_dir);

void tc_dir_list_roots();

void tc_dir64list_roots();

void tc_rmdir                           (char *project_dir, char *user_dir);

void tc_dir_dlt_files                    (char *project_dir, char *user_dir); 

long long tc_get                        (char *email_dir, char *project_dir, char *transfer_dir);

void tc_dir_add_files                    (char *tech_dir, char *user_dir); 

/* Sends specified files to the client */
long long tc_dir_get_files(char *tech_dir, char *user_dir); 

/* Read data from file list and send it to client */

void tc_dir_stat(char *path);

/* Send data using 64 bit structure */
void tc_dir64stat(char *path);

/* See if file name in tmp_file is './ */
int tmp_file_query(char *tmp_file);

#define GLOBAL_EMAIL_FILE "db_admin/global_email"


/* Move completed job off the queue and log statistics about the job */
void move2complete(int job_id, char *output_script);

/* See if a new TC job can be run */
/* New job can be run if maximum number of jobs not already running */
/* and at least one job is present on the queue in the submit state */
int run_new_job(char *tool);


/* Takes time expressed as a long and converts to hours, minutes, seconds */
/* Returns pointer to this string allocated from static memory */
char *jtime(long time);


/* Verify enough disk space is present */
void opcjob_send_check(void);

void num_gmb_calc(char *size_string, long long size);

/* Cancel a TC job */
void tc_job_cancel                       (int job_num); 

void opc_ps			      (); 

/* Holds TC work space directories */
char workspace[5][64];
    int work_cnt;
/* Holds number of disks for Transcend files */
int disk_cnt;

/* Holds login and Email for database administrator */
DBADMIN_INFO admin;

void workspace_rtv();

/* Write TC queue from memory out to disk freeing memory */
void tcq_file_write(int index, TCQ *q, char *q_file);

void tcq_file_free(int index);


/* Returns SUCCESS if job set active */
int tcq_set_active (int job_id);

/* Make a list of Q files */
/* They are sorted and non digit files are taken off list */
int tcq_list(char *files[]);

/* Scan text from a line into an TCQ structure */
void tcq_scan(char *line, TCQ *q);

/* Write data from TCQ structure to a file */
int tcq_print(FILE *fp, TCQ *q);

/* Attempts to lock specified file */
/* Exits if unable to lock file */

/* Read the TC queue from disk into memory */
/* Returns lock index used when job freed */
int tcq_read(TCQ *q[]);


/* Free the memory used by TC queue, not written to disk */
void tcq_free( TCQ *q[]);

/* Add an entry to the TC queue */
int tcq_add_entry(TCQ *q, char *run_type,   TC_STATUS status, char *q_file, char *command_file);

/* Modify the status of a job on the TC queue */
int tcq_mod_status(TC_STATUS status, long long size, char *keyword);

/* Mark parent status of dependent job complete */
int tcq_mod_parent_status(int job_id);

/* Delete specified entry from q */
void tcq_file_dlt(int index, char *q_file);

/* Looks for active jobs with no associated process and throws them away */
int tcq_audit_data(void);

/* Reads in active processes */
int ps_read(int ps_table[500]);

/* Find specified PID in PS table */
int ps_find(int ps_table[], int pid, int ps_cnt);
int tcq_find_job_id(int job_id, char *files[]);

/* Search for specified directory on job queue */
/* Used to verify no job in specified directory running */
/* before deleting the directory */
int tcq_search4dir(char *dir);



/* Sends copy of TC Q to the client */
/* Filters out fields client does not know about */

/* Sends copy of TC Q to the client */
/* Filters out fields client does not know about */
void tcq_send(void);




int tcq_parent_status_find(int parent_id);

/* See if parent can be deleted from the queue */
void tcq_parent_dlt(int parent_id);

int tcq_file_read(TCQ *q, char *q_file, int job_id, int scan_data);

/* Count number of active jobs */
/* Returns number of active jobs */
int tcq_active_jobs_count(void);

int tcq_submit_find(char *command_file);

int tcq_script_check(TCQ *q, char *root_script, char *transfer_dir);

/* Create directories needed for user */
long long tc_mkdir                        (char *email_dir, char *project_dir, char *transfer_dir);

/* Contains commands to run the active job */
/* At this point know that we can run another job */
/* Also know job number to be run */
int tc_job_run(int active_job, char *command, char  *transfer_dir, char *output_script);

/* Submit a new TC job */
void tc_job_submit                       (char *run_type);

/* Takes time expressed as a long and converts to hours, minutes, seconds */
/* Returns pointer to this string allocated from static memory */
char *jtime(long time);

/* Delete specified files when job completes */

/* Delete files associated with job ID */

/* Verify enough disk space is present */
void opcjob_send_check(void);

void num_gmb_calc(char *size_string, long long size);

/* Read status message from Kendall when job completes */
/* This message is sent back to the user */
int tc_job_status_read(char *transfer_dir);
void process_request(char **argv, int socket);



/* Build email list for all users in specified time period */

/* Build name for temporary file */
/* Used to build temporary file name for multiple TC reports */



    /* Process the request sent */
    int tc_job_write(void);

/* Scan in params unique to TC transactions */

/* Set links to Design Sync */
long long ds_data_get(char *directory, char *version, char *transfer_dir);

/* Read in the chip and memory area */
void dfm_rdb_file_read(int job_id, char *chip_area, char *memory_area);

/* Update results file size, update time, and number of bjobs */
int job_results_update(TCQ *q);

int trans_data_get(TCQ *q, char *dir);
int trans_status_read(TCQ *q);

int trans_status_get(TCQ *q);

/* See if this is a DTMS GDS web upload */
void dtms_web_upload_get(TCQ *q, char *transfer_dir, int index, char *q_file);

/* Create directories needed for user */
/* Contains commands to run the active job */
/* At this point know that we can run another job */
/* Also know job number to be run */
int dtms_job_run(int active_job, char *command_file, char  *transfer_dir, char *output_script, int index, char *q_file);

/* Read status message from Kendall when job completes */
/* This message is sent back to the user */
int dtms_job_status_read(char *transfer_dir);

int sql_log_data(char *file_name, int job_id, int find_first_line);

#endif
