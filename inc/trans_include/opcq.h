#ifndef opcq_h
#define opcq_h

#include <stdio.h>
#include "global_defines.h"
#include "opc_defines.h"

#define Q_SIZE 5000

#define OPCQ_INPUT "%d %d %d %d %d %ld %lld %ld %ld %ld %s %s %s %s %s %s %s %d %d %ld %s %s %s %s %s %s"
#define CLIENT_OPCQ_INPUT "%d %d %d %lu %lld %lu %lu %lu %s %s %s %d %d %lu %s %s "
#define DEPEND_CLIENT_OPCQ_INPUT "%d %d %d %d %lu %lld %lu %lu %lu %s %s %s %d %d %lu %s %s "
#define OPCQ_OUTPUT "%d %d %d %d %d %ld %lld %ld %ld %ld %s %s %s %s %s %s %s %d %d %ld %s %s %s %s %s %s\n"

/* Format used to send data to the OPC client */
#define CLIENT_OPCQ_OUTPUT "%d %d %d %ld %lld %ld %ld %ld %s %s %s %d %d %ld %s %s \n"
#define DEPEND_CLIENT_OPCQ_OUTPUT "%d %d %d %d %ld %lld %ld %ld %ld %s %s %s %d %d %ld %s %s \n"

/* File where all queues are stored */
#define Q_FILE "db_admin/opcq"

/* File where next available job ID stored */
#define JOB_ID_FILE "db_admin/opc_job_id"

/* Holds information about OPC jobs */
typedef struct
{
    int  job_id;	/* Unique number used to identify the job */
    int parent_id;
    int pid;
    int parent_status;
    long submittime;	/* Time in seconds job submitted */
    long transactiontime;	/* value in seconds last transaction performed */
    long executetime;	/* Time when job execution started */
    long long transfersize;
    long transfertime;	/* Amount of time in seconds it took to transfer the data */
    long waittime;	/* Total number of seconds job was waiting to run */
    long runtime;	/* Time when job is to be run */
    int status;
    char tech[TECH_LEN];
    char mask[8];
    char layer[LAYER_LEN];
    char top_cell[TOP_CELL_LEN];
    char script[FILE_SIZE];	/* Name of script needed to run job */
    char dir[FILE_SIZE];	/* Directory containing input files for the job */
    char machine[MACHINE_LEN];
    char address[ADDRESS_LEN];
    char cwd[256];
    char email[64];
    char name1[32];
    char name2[32];
    char client_version[16];
    int submit_by;	/* Estimated memory in kilobytes */
    int diskspace;	/* Estimated disk space in megabytes */
} OPCQ;

typedef enum
{
    ACTIVE,
    SUBMIT,
    HOLD,
    SEND,
    SEND_HOLD,
    COMPLETE,
    ABORT,
    NOTFOUND
} OPC_STATUS;

static char status_array[8][10] =
{
    "ACTIVE",
    "SUBMIT",
    "HOLD",
    "SEND",
    "send-HOLD",
    "COMPLETE",
    "ABORT",
    "CANCEL"
};

/* Holds pointer to FILE structure  and file name used to read data from q structure */
typedef struct
{
    FILE *fp;
    char file_name[1024];
} OPC_FILE;

/* General routine to open an OPC file */
OPC_FILE *opc_file_open(char *file_name);

/* Display active and queued OPC jobs */
/* Brings the queue file from the database and puts it in a temporary directory */
/* On the user's machine called '/tmp' */
/* Opens the temporary file brought down so it is ready to read */
OPC_FILE *opcq_open(void);


/* Displayed completed jobs associated with an OPC directory */
/* Brings down temporary file and opens it */
OPC_FILE *opcq_project_open(char *dir);

/* Read one entry from job queue display file */
/* Pass in OPCQ structure */
/* Function fills in structure and returns 0 on success */
/* Returns -1 if no more entries present */
int opcq_read_entry(OPC_FILE *fi, OPCQ *q);

/* Open job q and display job dependencies */
OPC_FILE *opcq_depend_open(void);


/* Read one entry from depend job queue display file */
/* Pass in OPCQ structure */
/* Function fills in structure and returns 0 on success */
/* Returns -1 if no more entries present */
/* Has a new member called depend_id */
int opcq_read_depend_entry(OPC_FILE *fi, OPCQ *q);

/* Close queue file */
int opcq_close(OPC_FILE *fp);

/* Close queue file */
int opcq_close(OPC_FILE *fp);

/* Display active and queued OPC jobs */
/* Brings the queue file from the database and puts it in a temporary directory */
/* On the user's machine called '/tmp' */
/* Opens the temporary file brought down so it is ready to read */
OPC_FILE *opcq2open(int req_type);

/* Audits the OPC queue */
short opcq_audit(void);

/* Remove job dependency from q */
void opcq_depend_remove(OPCQ *q[], int cnt);

int active_jobs_count(void);
#endif /* define for opcq */
