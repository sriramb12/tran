#ifndef opc_server_defines_h
#define opc_server_defines_h

#include <time.h>
#include "global_defines.h"
#include "opc_defines.h"


/* Holds data passed from the GUI to the server */
typedef struct
{
    int req_type;
    char script[FILE_SIZE]; /* Primary key for data accessing */
    char dir[PATH_SIZE]; /* Secondary key for data accessing */
    char tech[TECH_LEN];
    char mask[8];
    char layer[LAYER_LEN];
    char sql_server[32];
    char extra_param[32];
    char top_cell[TOP_CELL_LEN];
    int job_id;
    long increment;
    int submit_by;
    int diskspace;
    long long sendsize;
    long long file_size;
    long client_time;
} RET_MSG;

extern RET_MSG opc;

#endif /* Define for opc_server_defines_h */
