#ifndef opc_defines_h
#define opc_defines_h

#include "global_defines.h"
#define TECH_LEN 128
#define LAYER_LEN 64
#define TOP_CELL_LEN 64

#define CHECKSUM_FILE "db_admin/checksum"

#define RET_ADMIN "RET/administrator"
#define USER_FILE "registry/users"
#define HOME_PATH "/home/db_admin/"
#define OPT_SOCKET 6921
#define OPCQ_PATH "/home/db_admin/opctrans/db_admin/"
#define TIME_STAMP_PATH "/home/db_admin/opctrans/time_stamps/"
#define OPCQ_PATH_OFFSET 30
#define RET_KEY "OPCtrans"
#define RET_VERSION "4.4.6"
#define RET_RELEASE_DATE "Nov 1, 2005"
#define TOTAL_RET_FILES 200

#define USER_INPUT "%s %s %s %s %s %s %s %s %s"
#define USER_OUTPUT "%s %s %s %s %s %s %s %s"
/* Temporary directory where output from jobs placed */
#define TMP_RET_DIR "/tmp"

/* Fields kept track of in users file */
typedef struct
{
    char id[16];
    char name[48];
    char phone[20];
    char dept[32];
    char dept_name[64];
    char mbg[32];
    char paging_num[64];
    char status[16];
} OPC_USER;

/* Paths initialized when server starts up */
typedef struct
{
    int opcq_offset;
    int backup_q_offset;
    char opcq[FILE_SIZE];
    char backup_q[64];
     char db_admin[FILE_SIZE];
     char job_id[FILE_SIZE];
    char client[FILE_SIZE];
    char time_stamps[FILE_SIZE];
    char global_params[FILE_SIZE];
    char mirror[FILE_SIZE];
    char root_dir[FILE_SIZE];
    char q_file[FILE_SIZE];
} RET_PATHS;

extern RET_PATHS path;

/* Holds data from a DF command */
typedef struct
{
    long total;
    long used;
    long avail;
    char percent[8];
    char cwd[FILE_SIZE];
} RET_DISK;

#endif /* define for opc_defines_h */
