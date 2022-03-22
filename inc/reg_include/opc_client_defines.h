#ifndef opc_client_defines_h
#define opc_client_defines_h

#include "opc_defines.h"

/* Holds extra data put in RET report logs */
typedef struct
{
    char tech[TECH_LEN];
    char mask[8];
    char top_cell[TOP_CELL_LEN];
    char layer[LAYER_LEN];
    char sql_server[32];
    char extra_param[32];
} LOG_DATA;

extern LOG_DATA ld;


#endif 
