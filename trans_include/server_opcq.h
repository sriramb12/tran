#ifndef server_opcq_h
#define server_opcq_h

#include "opcq.h"

/* Holds low level queue functions called by the server opcjob functions */

/* Attempts to lock specified file */
/* Exits if unable to lock file */

/* Read the OPC queue from disk into memory */
     #include <sys/types.h>
     #include <sys/stat.h>
     #include <fcntl.h>

/* Attempts to lock specified file */
/* Exits if unable to lock file */

/* Read the RET queue from disk into memory */
/* Returns lock index used when job freed */
int opcq_read(OPCQ *q[]);

/* Write RET queue from memory out to disk freeing memory */
void opcq_write_free(int index, OPCQ *q[]);


/* Write out a back-up copy of the Queue, requested by Bob Jarvis */
void opcq_write_backup(OPCQ *q[]);


/* Free the memory used by RET queue, not written to disk */
void opcq_free( OPCQ *q[]);

/* Add an entry to the RET queue */
int opcq_add_entry(  OPC_STATUS status, char *q_file);

/* Look for specified job on the RET queue */
/* Find specified job id on queue */
int opcq_find(OPCQ *q[]);

/* Finds job with specified job ID */
/* In this case the RET queue is already present in memory */
int opcq_read_find(OPCQ *q[], int *cnt_ptr);

/* Move job up the queue */
/* new_cnt is new position for job */
/* old_cnt is old position for job */
void move_job_up(int index, OPCQ *q[], int new_cnt, int old_cnt, int job_cnt, int sql_add);

/* Move job down the queue */
/* new_cnt is new position for job */
/* old_cnt is old position for job */
void move_job_down(int index, OPCQ *q[], int new_cnt, int old_cnt, int job_cnt);

/* Modify the status of a job on the RET queue */
int opcq_mod_status(OPC_STATUS status, long long size);


/* Delete specified entry from q */
void opcq_dlt_entry(int index,  OPCQ *q[], int old_entry);




/* Looks for active jobs with no associated process and throws them away */
int opcq_audit_data();

/* Reads in active processes */
int ps_read(int ps_table[500]);

int opcq_find_job_id(int job_id, char *files[]);

/* Search for specified directory on job queue */
/* Used to verify no job in specified directory running */
/* before deleting the directory */
int opcq_search4dir(char *dir);


/* Restore back-up copy of queue */
/* Requested by Bob Jarvis */
void opcq_restore();

/* Sends copy of RET Q to the client */
/* Filters out fields client does not know about */
void opcq_send(void);

/* Sends copy of dependent RET Q to the client */
/* Filters out fields client does not know about */
void opcq_depend_send(void);

/* Scan text from a line into an OPCQ structure */
void opcq_scan(char *line, OPCQ *q);

/* Write data from OPCQ structure to a file */
int opcq_print(FILE *fp, OPCQ *q);

/* Modify queue entry in SQL */
void opcq_mysql_mod(OPCQ *q);

/* Delete queue entry from SQL */
void opcq_mysql_delete(int job_num);

/* Remove all job dependencies from the q */
void opcq_depend_remove(OPCQ *q[], int job_cnt);

int opcq_parent_status_find(int parent_id);

/* Add entry to MySQL queue */
/* This is done when job submitted */
void opcq_mysql_add(OPCQ *q);

/* Modify entry in MySQL table */
/* This is done when status of job changes */
/* Not called when job completes */
void opcq_mysql_mod(OPCQ *q);

/* Read in and lock a Q file */
int opcq_file_read(OPCQ *q, char *q_file, int job_id);

/* Free without writing a single Q file */
void opcq_file_free(int index);

/* Write to a single Q file */
void opcq_file_write(int index, OPCQ *q, char *q_file);

/* Delete a single Q file */
void opcq_file_dlt(int index, char *q_file);

void opcq_parent_dlt(int parent_id);

int opcq_set_active(int job_id);
int opcq_submit_find(void);
int ps_find(int ps_table[], int pid, int ps_cnt);
int opcq_active_jobs_count(void);
int opcq_list(char *files[]);
int opcq_script_check(OPCQ *q);
#endif
