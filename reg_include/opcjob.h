#ifndef opcjob_h
#define opcjob_h

/* Temporary directory where output from jobs placed */
#define TMP_OPC_DIR "/tmp"

/* Submit specified OPC job */
short opcjob_submit(int depend_id, char *dir, char *script, time_t starttime, int submit_by,
    int diskspace, time_t cpu_time, char *files[], long long size,
char *technology, char *mask_set, char *mask_layer, char *top_cell);

/* Submit specified OPC job */
/* Uses 64 bit mode */
int opcjob64submit(int depend_id, char *dir, char *script, time_t starttime, int submit_by,
    int diskspace, time_t cpu_time, char *files[], long long size,
char *technology, char *mask_set, char *mask_layer, char *top_cell);

/*Cancel OPC job */
short opcjob_cancel(int job_id);

/* Modify the priority of a job */
/* command is set to up, down, top, or end */
/* increment is the number of positions to move job up or down */
/* Pass in 0 if moving to top or end of the queue */
short opcjob_mod_priority(int job_id, char *job_command, int increment);

/* Put an OPC job on hold */
short opcjob_hold(int job_id);

short opcjob_release(int job_id);

/* Runs new job if no active jobs and queued jobs are waiting to run */
short opc_run_new_job(void);

/* Runs multiple jobs if dependent jobs present on the queue */
short opc_run_multiple_jobs(int job_num);
/* Modify run time for specified job */
short opcjob_mod_runtime(int job_id, long run_time);

/* Send list of files to delete fafter specified job completes */
short opcjob_files2dlt(int job_id, char *files[]);

/* Set user's email address */
int opc_email_set(char *email);

/* Set up OPC machine, socket, address, etc. */
/* First parameter is name of alternate MySQL server */
/* Second parameter is flag to indicate if defaults should be overwritten */
/* with servers from MySQL table, pass 1 to overwrite */
/* Returns SUCCESS (0) if could use alternate SQL and if connect made */
/* Returns -110 if connect not made */
/* Returns another value if could not connect to alternate */
/* SQL server */
int opc_globals_init(char *mysql_server, int sql_server_flag);

int opcjob_view(int req_type);

void opcjob_project_view(char *project);

#endif /* define for opcjob_h */
