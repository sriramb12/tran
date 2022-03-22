#ifndef dfm_server_prototypes_h
#define dfm_server_prototypes_h

#include "dfmq.h"

/* Contains commands to run the active job */
/* At this point know that we can run another job */
/* Also know job number to be run */
int dfm_job_run(int active_job);

/* Submit a new DFM job */
void dfm_job_submit                       (void); 

/* Modify runtime of job on the submit queue */
void opcjob_mod_runtime                  (void); 

/* Release a DFM job from hold */
void opcjob_release                      (void); 
/* Put a DFM job on hold */
void opcjob_hold                         (void); 

/* Cancel a DFM job */
void opcjob_cancel                       (void); 

/* Cancel dependent job */
void opcjob_depend_cancel(int job_id, long current_time, char *status);

/* Move completed job off the queue and log statistics about the job */
void move2complete(int index, DFMQ *q, char *q_file, int status);

/* See if a new RET job can be run */
/* New job can be run if maximum number of jobs not already running */
/* and at least one job is present on the queue in the submit state */
int dfm_run_new_job(void);


/* Takes time expressed as a long and converts to hours, minutes, seconds */
/* Returns pointer to this string allocated from static memory */
char *jtime(long time);

/* Delete specified files when job completes */
void opcjob_files2dlt(int job_id);

/* Delete files associated with job ID */

/* Verify enough disk space is present */
void opcjob_send_check(void);

void num_gmb_calc(char *size_string, long long size);


/* Checks to see if completed job has any dependencies */
/* If dependencies are found, job must remain on the q */
/* Otehrwise it can be taken off */
/* Returns DBYES if dependencies found */
int opcjob_depend_find(int job_id);

/* Moves parent status of dependent jobs to complete */
void move_depend2complete(int parent_id);

int opcjob_depend_list(int depend_list[], int job_id);

/* Transcend results to user */
void dfm_trans_send(DFMQ *q, char *command_output, int command_len, int status);

/* Build DFM control or deck file name */
void dfm_file_name_get(char *control_file, char *version_type);

/* Update the version number in tech_current_versions */
void version_update(float new_version, char *technology);

/* Retrieve current version number */
float dfm_current_version_rtv(char *suffix);


#endif
