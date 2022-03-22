#ifndef dfmq_h
#define dfmq_h

#include <stdio.h>
#include "global_defines.h"
#include "opc_defines.h"

#define Q_SIZE 5000

#define DFMQ_INPUT \
"%d %d %ld %d %lld %ld %ld %ld %ld %ld %ld %[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%d %s"
#define DFMQ_OUTPUT  \
"%d %d %ld %d %lld %ld %ld %ld %ld %ld %ld %s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d %s"

/* Format used to send data to the OPC client */

/* File where all queues are stored */
#define Q_FILE "db_admin/dfmq"

/* File where next available job ID stored */
#define JOB_ID_FILE "db_admin/job_id"

/* Holds information about OPC jobs */
typedef struct
{
    int status;
    int  job_id;	/* Unique number used to identify the job */
    int pid;
    int uid;
    long long transfersize;
    long transfertime;	/* Amount of time in seconds it took to transfer the data */
    long transactiontime;	/* value in seconds last transaction performed */
    long submittime;	/* Time in seconds job submitted */
    long waittime;	/* Total number of seconds job was waiting to run */
    long executetime;	/* Time when job execution started */
    long runtime;	/* Time when job is to be run */
    char name[32];
    char email[64];
    char cwd[256];
    char machine[MACHINE_LEN];
    char address[ADDRESS_LEN];
    char mask[12];
    char tech[TECH_LEN];
    char design[80];
    char top_cell[TOP_CELL_LEN];
    char layer[LAYER_LEN];
    char control_file[FILE_SIZE];	/* Name of script needed to run job */
    char gds_file[FILE_SIZE];	/* file containing input files for the job */
    char results[64];
    char summary[64];
char run_type[16];
    char client_version[16];
    char calibre_version[80];
    char deck_version[24];
    char one_page_revision[8];
    int normalizor;
    char transfer_type[8];
    char metallization[8];
} DFMQ;

typedef enum
{
    ACTIVE,
    SUBMIT,
    HOLD,
    SEND,
    SEND_HOLD,
    COMPLETE,
    ABORT,
    CANCEL,
    FAILED,
    NOTFOUND,
    KCL_WAIT,
} DFM_STATUS;

static char status_array[11][10] =
{
    "ACTIVE",
    "SUBMIT",
    "HOLD",
    "SEND",
    "SEND-HOLD",
    "COMPLETE",
    "ABORT",
    "CANCEL",
    "FAILED",
    "NOT-FOUND",
    "KCL_WAIT",
};

/* Holds pointer to FILE structure  and file name used to read data from q structure */
typedef struct
{
    FILE *fp;
    char file_name[1024];
} OPC_FILE;


/* Display active and queued RET jobs */
/* Brings the queue file from the database and puts it in a temporary directory */
/* On the user's machine called '/tmp' */
/* Opens the temporary file brought down so it is ready to read */

int dfmq2open(int req_type, char *root_name);


OPC_FILE *opcq_open(void);


/* Close queue file */
int dfmq_close(char *file_name);

/* Opens an RET file */
int dfm_file_open(char *root_name);

/* Audits the DFM queue */
short opcq_audit();

#endif
