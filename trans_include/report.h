#ifndef report_h
#define report_h

#include "opc_defines.h"
#include "opcq.h"

#define FILE_SIZE 256
/* Holds data used to generate RET reports */
typedef struct
{
    int job_id;
    char user_id[11];
    char name1[16];
    char name2[16];
    char tech[24];
    char mask[8];
    char layer[LAYER_LEN];	/* Maximum size in Mask 2000 is less than 45 */
    char top_cell[TOP_CELL_LEN];	/* Maximum size in Mask 2000 is 32 + 1 */
    char dir[32];
    char script[64];
    char status[13];
char opc_version[16];
    long long transfersize;
    unsigned long transfertime;
    unsigned long waittime;
    unsigned long runtime;
    unsigned long memory;
    unsigned long cpu;
    unsigned long submittime; 
    unsigned long executetime; 
    unsigned long completetime; 
} JOB_REPORT;

typedef struct
{
    long long transfersize;
    long transfertime;
    long waittime;
    long runtime;
    int size_job;
    int transfer_job;
    int wait_job;
    int run_job;
    char size_id[10];
    char transfer_id[10];
    char wait_id[10];
    char run_id[10];
} MAX_REPORT ;

typedef struct
{
    char user_id[11];
    char dir[FILE_SIZE];
    char file[FILE_SIZE];
    unsigned long time;
} PROJECT_REPORT;

/* The following fields are logged: */
/* job ID, user ID, name1, name2, technology, */
/* dir, script, status, machine, IP address */
/* client version, server version, size transfer time, wait time , */
/* execute time memory, cpu, submit time, complete time CWD */
/* Input for job report files */
#define REPORT_JOB_INPUT "%d %s %s %s %s %s %s %s %s %s %s %s %s %s %s %lld %ld %ld %ld %ld %ld %ld %ld %s"
#define REPORT_JOB_OUTPUT "%d %s %s %s %s %s %s %s %s %s %s %s %s %s %s %lld %ld %ld %ld %ld %ld %ld %ld %s\n"

/* Used for getting job profile report */
#define REPORT_JOB_INPUT1 "%*s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %lld %ld %ld %ld %ld %ld %ld %ld %s"
#define OLD_REPORT_JOB_INPUT "%d %s %s %s %s %s %s %s %s %s %s %s %ld %ld %ld %ld %ld %ld %ld %s"

/* Input for department technology report */
/* Scanning in user ID, technology, and status in fields 1, 5 and 11 */
#define DEPT_TECH_INPUT \
"%*d %s %*s %*s %s %*s %*s %*s %*s %*s %s"

/* Input used for Technology Report */
/* scans in technology from field 5 */
#define TECH_INPUT \
"%*d %*s %*s %*s %s"

/* Used for User Technology report */
/* User ID scanned in from field 1 and technology scanned in from field 5 */
#define USER_TECH_INPUT \
"%*d %s %*s %*s %s"

/* Output for job profile report */
#define JOB_PROFILE_OUTPUT \
"Memory:  %d\n\
CPU:  %d\n\
User ID:  %s\n\
Name:  %s %s\n\
Technology:  %s\n\
User machine:  %s\n\
User IP node address:  %s\n\
User CWD:  %s\n\
RETtrans client version:  %s\n\
RETtrans server version:  %s\n"

void report_job(OPCQ *q, char *status, char *output_script);

/* Retrieve user's technology from job file */
void tech_get(char *tech, int job_id, char *file_name);
            
/* Keep track of number of puts to RET server machine */
void report_put(char *user, long time, long long size);

/* Keeps track of gets from RET machine */
void report_get(char *user, long time, long long size);


/* Calculates disk usage for each RET user once a day */
void report_user_disk();
void report_log(char *report_log, char *string);
#define JUN99 930000000

void report_audit(char *date1, char *date2);

int find_user_id(char *user_id);

int first_name_check(char *name);

void old_audit_repair(char *date1, char *date2);

void audit_repair(char *date1, char *date2);

/* Retrieve list of RET users */
void report_delete_time();


struct DEPT_TECH * dept_tech_find(struct DEPT_TECH *top_ptr, char *dept, char *tech, char *status);

void dept_tech_add(struct DEPT_TECH *old_tech, struct DEPT_TECH *root_ptr);

/* Retrieve list of current technologies */
void report_dept_tech(char *date1, char *date2);

/* Report disk space for /home/opcrun/opctrans/projects, /tm/\p, */
/* /local/workspace/opcrun xx */
void report_disk();

/* Does df on specified directory */
int opc_df(char *buffer, char *directory);

void report_job_profile();


struct MBG_LIST * mbg_find(struct MBG_LIST *top_ptr, char *mbg);

struct MBG_LIST *mbg_add(struct MBG_LIST *old_mbg, struct MBG_LIST *root_ptr);

/* Retrieve list of current mbgnologies */
void report_mbg(char *date1, char *date2);

struct MBG_TECH * mbg_tech_find(struct MBG_TECH *top_ptr, char *mbg, char *tech);

struct MBG_TECH *mbg_tech_add(struct MBG_TECH *old_tech, struct MBG_TECH *root_ptr);

/* Retrieve list of current mbgans and technologies */
void report_mbg_tech(char *date1, char *date2);

void report_project();
void project_output(char *string, PROJECT_REPORT *data, char time_flag);

struct TECH_LIST * tech_find(struct TECH_LIST *top_ptr, char *tech);

struct TECH_LIST *tech_add(struct TECH_LIST *old_tech, struct TECH_LIST *root_ptr);

/* Retrieve list of current technologies */
void report_tech(char *date1, char *date2);

/* Retrieve list of RET users */
void report_user();

void email_list_all();


struct USER_TECH * user_tech_find(struct USER_TECH *top_ptr, char *id, char *tech);

void user_tech_add(struct USER_TECH *old_tech, struct USER_TECH *root_ptr);

int user_tech_list(char *date1, char *date2, struct USER_TECH *root_node, struct USER_TECH *end_node);


/* Retrieve list of current technologies */
void report_user_tech(char *date1, char *date2);

/* Build email list for all users in specified time period */
void email_list(char *date1, char *date2);


/* Build list of directories less than one day from time out */
void project_time_out();

void report1project();

void report_all_projects();

void report_root_projects();


struct USER_LIST * user_list_find(struct USER_LIST *top_ptr, char *id);

struct USER_LIST *user_list_add(struct USER_LIST *old_user, struct USER_LIST *root_ptr);

/* Retrieve list of current RET users */
void report_user_list(char *date1, char *date2);


void report_weekly(char *date1, char *date2);

void report_summary(char *date1, char *date2);

void report_max(char *date1, char *date2);

void report_init(JOB_REPORT *rep);

void max_report_init(MAX_REPORT *rep);

void min_report_init(MAX_REPORT *rep);

void summary_add(JOB_REPORT *total_rep, JOB_REPORT *new_rep);
void max_add(MAX_REPORT *max_rep, JOB_REPORT *rep);

void min_add(MAX_REPORT *min_rep, JOB_REPORT *rep);


int user_cnt(char *user[]);

void user_add(char *user[], char *user_id);

/* Builds report header used for multiple RET reports */
/* This header is used for reports written to a file on the server side */
/* Gives RETadmin version number and release date */
/* Also gives date generated and dates covered in report */
/* This information was requested by the POET group */
void report_header_print(FILE *fo, char *report_title, char *date1, char *date2);

/* Builds report header used for multiple RET reports */
/* This header is used for reports stored in memory on the server side */
/* and not written to a file until they reach the client */
/* Gives RETadmin version number and release date */
/* Also gives date generated and dates covered in report */
/* This information was requested by the POET group */
int report_header_build(char *string, char *report_title, char *date1, char *date2);

/* Build name for temporary file */
/* Used to build temporary file name for multiple RET reports */
void tmp_name_build(char *tmp_file, char *suffix);

int job_input_scan(char *line, JOB_REPORT *rep);

#endif
