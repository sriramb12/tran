#ifndef tcq_h
#define tcq_h

#include <stdio.h>
#include "global_defines.h"

#define Q_SIZE 5000
#define TECH_LEN 128
#define LAYER_LEN 64
#define TOP_CELL_LEN 64

#define TCQ_INPUT \
"%s %d %d %ld %d %lld %lld \
  %ld %ld %ld %ld %ld %ld \
%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c\
%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c\
%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c\
%[^;]%*c%[^;]%*c%[^;]%*c%ld\
 %[^;]%*c%[^;]%*c%[^;]%*c%d %d \
%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%d"

#define TCQ_OUTPUT  \
"%s %d %d %ld %d %lld %lld \
%ld %ld %ld %ld %ld %ld \
%s;%s;%s;%s;%s;\
%s;%s;%s;%s;%s;\
%s;%s;%s;%s;%s;\
%s;%s;%s;%ld \
%s;%s;%s;%d %d \
%s;%s;%s;%s;%s;%d"

/* Format used to send data to the TC client */

/* File where all queues are stored */
#define Q_FILE "db_admin/tcq"

/* File where next available job ID stored */
#define JOB_ID_FILE "db_admin/job_id"

/* Holds information about TC jobs */
typedef struct
{
    char tool[64];
    int status;
    unsigned long  job_id;	/* Unique number used to identify the job */
    int pid;
    int uid;
    long long transfersize;
    long long results_size;

    int bjobs;
    long resultsupdatetime;	/* value in seconds last transaction performed */
    long starttime;	/* Time job began and data transfer started */
    long submittime;	/* Time in seconds job send completed */
    long pendtime;	/* Time in seconds job send completed */
    long runtime;	/* Time when job execution started */
    long endtime;	/* Time when job ended */

    char name[48];
    char email[64];
    char cwd[1024];
    char machine[MACHINE_LEN];
    char address[ADDRESS_LEN];

    char mask[12];
    char tech[TECH_LEN];
    char design[96];
    char top_cell[TOP_CELL_LEN];
    char layer[LAYER_LEN];

    char control_file[FILE_SIZE];	/* Name of script needed to run job */
    char gds_file[FILE_SIZE];	/* file containing input files for the job */
    char results[FILE_SIZE];
    char summary[FILE_SIZE];
char run_type[20];

    char run_machine[64];
    char client_version[16];
    char calibre_version[80];
    int normalizor;

    char transfer_type[16];
    char metallization[16];
    char command_file[64]; 
    int lsf_id;
    int version_id;

    char trans_key[16];
    char readme_file[256];
    char chip_area[16];
    char memory_area[16];
    char trans_status_msg[1024];
    char transupdatetime[32];
    char job_keyword[16];
    int num_processors;
} TCQ;

typedef enum
{
    ACTIVE,
    SUBMIT,
    PEND,
    SEND,
    SEND_HOLD,
    COMPLETE,
    ABORT,
    CANCEL,
    FAILED,
    NOTFOUND,
    KCL_WAIT,
    EXPIRED,
} TC_STATUS;

static char status_array[12][10] =
{
    "ACTIVE",
    "SUBMIT",
    "PEND",
    "SEND",
    "SEND-HOLD",
    "COMPLETE",
    "ABORT",
    "CANCEL",
    "FAILED",
    "NOT-FOUND",
    "KCL_WAIT",
    "EXPIRED",
};

/* Holds pointer to FILE structure  and file name used to read data from q structure */
typedef struct
{
    FILE *fp;
    char file_name[1024];
} OPC_FILE;


/* Display active and queued TC jobs */
/* Brings the queue file from the database and puts it in a temporary directory */
/* On the user's machine called '/tmp' */
/* Opens the temporary file brought down so it is ready to read */

int tcq2open(int req_type, char *root_name);




/* Close queue file */
int tcq_close(char *file_name);

/* Opens an TC file */
int tc_file_open(char *root_name);


#endif
