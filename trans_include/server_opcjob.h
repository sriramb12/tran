#ifndef server_opcjob_h
#define server_opcjob_h

/* Delete files associated with job ID */
void opcjob_files_dlt(int job_id, char *project_dir);

/* Submit a new OPC job */
void opcjob_submit(void); 

/* Modify runtime of job on the OPC queue */
void opcjob_mod_runtime(void); 

/* Set a job dependency, not currently being used */
void opcjob_set_depend (void); 

/* Release an OPC job from hold */
void opcjob_release(void); 
/* Put an OPC job on hold */
void opcjob_hold (void); 

/* Modify the priority of an OPC job */
void opcjob_mod_priority (int job_id, char *command_dir, long increment); 

/* Cancel an OPC job */
void opcjob_cancel (void); 

/* Calcel or abort all jobs dependent on job just cancelled or aborted */
void opcjob_depend_cancel(int depend_id, long current_time, char *status);
/* Move completed job off the queue and log statistics about the job */
void move2complete(int job_id, char *output_script);

/* See if a new OPC job can be run */
/* New job can be run if maximum number of jobs not already running */
/* and at least one job is present on the queue in the submit state */
int run_new_job(void);


/* Delete specified files when job completes */
void opcjob_files2dlt(int job_id);

/* verify enough disk space available for files being retrieved */
void opcjob_send_check( void);

/* Retrieve list of servers from server table */
void ret_server_rtv(char *file_name);


void active_job_run(int active_job, char *output_script);
void num_gmb_calc(char *string, long long size);
int opcjob_check4depend(int job_id);
void move_depend2complete(int parent_id);
int opcjob_depend_list(int depend_list[], int job_id);

/* Find how many dependent jobs not in submit state, but do not list them */
int opcjob_depend_find(int job_id);
#endif
