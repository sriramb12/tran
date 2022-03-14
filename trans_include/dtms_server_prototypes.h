#ifndef dtms_server_prototypes_h
#define dtms_server_prototypes_h

#include "dfmq.h"
#include "dtms_defines.h"
#include "gds_defines.h"

/* Initialize DTMS params */
int dtms_params_init(char option);

/* Copy elements from 64 bit structure back into 32 bit structure */


/* Sends directory list to client */
/* For each file sends stat structure and file name */
void dir_put(char *directory);

/* Sends directory list to client */
/* For each file sends 64 bit stat structure and file name */
void dir64put(char *directory);

void dfm_dir_alist                         (char *project_dir, char *user_dir);

void dfm_dir_list(char *user_dir);

void dfm_dir64list(char *project_dir, char *user_dir);

void dfm_dir_list_roots();

void dfm_dir64list_roots();

void dfm_rmdir                           (char *project_dir, char *user_dir);

void dfm_dir_dlt_files                    (char *project_dir, char *user_dir); 

long long dfm_mkdir                        (char *email_dir, char *project_dir, char *transfer_dir);

void dfm_dir_add_files                    (char *tech_dir, char *user_dir); 

/* Sends specified files to the client */
long long dfm_dir_get_files(char *tech_dir, char *user_dir); 

/* Read data from file list and send it to client */

void dfm_dir_stat(char *path);

/* Send data using 64 bit structure */
void dfm_dir64stat(char *path);

/* See if file name in tmp_file is './ */
int tmp_file_query(char *tmp_file);

#define GLOBAL_EMAIL_FILE "db_admin/global_email"

/* Contains commands to run the active job */
/* At this point know that we can run another job */
/* Also know job number to be run */
int dfm_job_run(int active_job);

/* Submit a new DFM job */
void dfm_job_submit                       (void); 

/* Move completed job off the queue and log statistics about the job */
void move2complete(int job_id, char *output_script);

/* See if a new DFM job can be run */
/* New job can be run if maximum number of jobs not already running */
/* and at least one job is present on the queue in the submit state */
int run_new_job(void);


/* Takes time expressed as a long and converts to hours, minutes, seconds */
/* Returns pointer to this string allocated from static memory */
char *jtime(long time);


/* Verify enough disk space is present */
void opcjob_send_check(void);

void num_gmb_calc(char *size_string, long long size);

/* Cancel a DFM job */
void dfm_job_cancel                       (void); 

void opc_ps			      (); 

/* Holds DFM work space directories */
char workspace[5][64];
    int work_cnt;
/* Holds number of disks for Transcend files */
int disk_cnt;

/* Holds login and Email for database administrator */
DBADMIN_INFO admin;

void dfm_read_params(void);

void workspace_rtv();

/* Write DFM queue from memory out to disk freeing memory */
void dfmq_file_write(int index, DFMQ *q, char *q_file);

void dfmq_file_free(int index);


/* Returns SUCCESS if job set active */
int dfmq_set_active (int job_id);

/* Make a list of Q files */
/* They are sorted and non digit files are taken off list */
int dfmq_list(char *files[]);

/* Scan text from a line into an DFMQ structure */
void dfmq_scan(char *line, DFMQ *q);

/* Write data from DFMQ structure to a file */
int dfmq_print(FILE *fp, DFMQ *q);

/* Attempts to lock specified file */
/* Exits if unable to lock file */

/* Read the DFM queue from disk into memory */
/* Returns lock index used when job freed */
int dfmq_read(DFMQ *q[]);


/* Free the memory used by DFM queue, not written to disk */
void dfmq_free( DFMQ *q[]);

/* Add an entry to the DFM queue */
int dfmq_add_entry(  DFM_STATUS status, char *q_file);

/* Modify the status of a job on the DFM queue */
int dfmq_mod_status(DFM_STATUS status, long long size);

/* Mark parent status of dependent job complete */
int dfmq_mod_parent_status(int job_id);

/* Delete specified entry from q */
void dfmq_file_dlt(int index, char *q_file);

/* Looks for active jobs with no associated process and throws them away */
int dfmq_audit_data(void);

/* Reads in active processes */
int ps_read(int ps_table[500]);

/* Find specified PID in PS table */
int ps_find(int ps_table[], int pid, int ps_cnt);
int dfmq_find_job_id(int job_id, char *files[]);

/* Search for specified directory on job queue */
/* Used to verify no job in specified directory running */
/* before deleting the directory */
int dfmq_search4dir(char *dir);



/* Sends copy of DFM Q to the client */
/* Filters out fields client does not know about */

/* Sends copy of DFM Q to the client */
/* Filters out fields client does not know about */
void dfmq_send(void);




int dfmq_parent_status_find(int parent_id);

/* See if parent can be deleted from the queue */
void dfmq_parent_dlt(int parent_id);

int dfmq_file_read(DFMQ *q, char *q_file, int job_id);

/* Count number of active jobs */
/* Returns number of active jobs */
int dfmq_active_jobs_count(void);

int dfmq_submit_find(void);

int dfmq_script_check(DFMQ *q, char *root_script, char *transfer_dir);

/* Create directories needed for user */
long long dtms_mkdir                        (char *email_dir, char *project_dir, char *transfer_dir);

/* Contains commands to run the active job */
/* At this point know that we can run another job */
/* Also know job number to be run */
void dtms_job_run(DRC_DATA *drc, GDS_DATA *gds, int active_job, char  *transfer_dir, char *output_script);

/* Submit a new DTMS job */
void dtms_job_submit                       (DRC_DATA *drc, GDS_DATA *gds); 

/* Cancel a DTMS job */
void dtms_job_cancel                       (void); 



/* See if a new DFM job can be run */
/* New job can be run if maximum number of jobs not already running */
/* and at least one job is present on the queue in the submit state */
int run_new_job(void);


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
int dtms_job_status_read(char *transfer_dir);
void process_request(int socket);



/* Build email list for all users in specified time period */

/* Build name for temporary file */
/* Used to build temporary file name for multiple DFM reports */



    /* Process the request sent */
    int dtms_job_write(DRC_DATA *drc);

/* Scan in params unique to DTMS transactions */
void dtms_params_scan(char *header_buffer, DRC_DATA *drc);
void gds_params_scan(char *header_buffer, DRC_DATA *drc, GDS_DATA *gds);

#endif
