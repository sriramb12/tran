#ifndef dfm_msg_h
#define dfm_msg_h

#include <time.h>
#include "global_defines.h"
#include "opc_defines.h"


/* Holds data passed from the GUI to the server */
typedef struct
{
    int server_type;
    int req_type;
    char mask[8];
    char tech[FILE_SIZE]; /* Primary key for data accessing */
    char design[64];
    char top_cell[TOP_CELL_LEN];
    char layer[LAYER_LEN];
    char control_file[PATH_SIZE]; /* Secondary key for data accessing */
    char gds_file[PATH_SIZE]; /* Secondary key for data accessing */
    char results[FILE_SIZE];
    char summary[FILE_SIZE];
    char run_type[32];
    char transfer_type[32];
    char global_email[1024];
    char extra4[32];
    char extra5[32];
    int job_id;
    long long sendsize;
    long long file_size;
    long client_time;
    int uid;
    int extra_int2;
} DFM_MSG;

extern DFM_MSG opc;

#endif /* Define for dfm_msg_h */
