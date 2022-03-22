#ifndef dtms_defines_h
#define dtms_defines_h
#include "global_defines.h"

#define DTMS_VERSION "1.1.6"
#define GLOBAL_EMAIL_FILE "db_admin/global_email"
#define DTMS_FORMAT_MSG "Error -176:  Incorrect format entered\n\n"

#define DTMS_HELP_MSG \
"To request a DRC waiver, type:\n\
dtms_drc_create_waivers -b badge -c control_file -D design -f run_file -I mask_id  \n\
-r results_file -s summary_file -t job_type -T technology\n\
\n\
Optional arguments include:\n\
\n\
-j job_name\n\
-l limit\n\
-M Map old name to new name\n\
-O x,y offset\n\
-p project\n\
-u  234                                              \n\
-z tool_type   \n\
-w -Write results to SQL database\n"

/* Command line parameters */
typedef struct
{
    char mask[16];
    char tech[FILE_SIZE];
    char design[FILE_SIZE];
    char tool[64];
    char control_file[FILE_SIZE];
    char run_file[FILE_SIZE];
    char results_file[FILE_SIZE];
    char summary_file[FILE_SIZE];
    char run_type[FILE_SIZE];
    char project[FILE_SIZE];
    char x_y[16];
    char job_name[FILE_SIZE];
    char old2new_name[FILE_SIZE];
    char kendall[FILE_SIZE];
    char new[64];
    char badge[64];
    char patch[64];
    char job_class[64];
    char extra4[64];
    char extra5[64];
    int server_type;
    int request_type;
    int limit;
    int database_write;
    int job_uid;
    int extra_int;
} DRC_DATA;

typedef enum
{
    MASK,
    TECHNOLOGY,
    DESIGN,
    CONTROL_FILE,
    RUN_FILE,
    RESULTS_FILE,
    SUMMARY_FILE,
    BADGE,
    JOB_TYPE,
} JOB_OPTION;

static char job_type_string[9][16]  =
{
     "Mask",
    "Technology",
    "Design",
    "Control file",
    "Run file",
    "Results file",
    "Summary file",
    "Badge",
    "Job type",
};

#define bad_param \
"Error -199:    Too few command line parameters.\n\
               %s not specified."
#endif
