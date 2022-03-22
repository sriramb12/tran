#ifndef tc_client_defines_h
#define tc_client_defines_h

#include "global_defines.h"

/* Holds extra data put in RET report logs */
typedef struct
{
    char mask[8];
    char tech[64];
    char design[32];
    char top_cell[64];
    char layer[64];
    char results[FILE_SIZE];
    char summary[FILE_SIZE];
    char run_type[32];
    char transfer_type[16];
    char global_email[1024];
    char extra4[8];
    char extra5[8];
    int uid;
    int extra_int2;
} LOG_DATA;

extern LOG_DATA ld;


#endif 
