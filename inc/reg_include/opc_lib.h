#ifndef opc_lib_h
#define opc_lib_h
#define opc_server_functions_h

#include "global_defines.h"


/* Define for status returned */
typedef enum  
{
    OPC_SUCCESS,
    OPC_FAILURE,
    OPC_NOT_PRESENT,
    OPC_READ_ERR,
    OPC_WRITE_ERR,
     OPC_NOT_LOCKED,
    OPC_ALR_LOCKED,
    OPC_NOT_OWNER,
    OPC_INVALID_STATE,
    OPC_SYSTEM_FILE,
} RET_FILE_STATUS;
static char OPC_STATUS_ARRAY[16][16] =
{
    "SUCCESS",
    "FAILURE",
    "NOT_PRESENT",
    "READ_ERR",
    "WRITE_ERR",
     "NOT_LOCKED",
    "ALR_LOCKED",
    "NOT_OWNER",
    "INVALID_STATE",
    "SYSTEM_FILE",
};

/* Holds information about OPC library */
typedef struct
{
    char delta_dir[FILE_SIZE];
    char data_dir[FILE_SIZE];
    char app_delta_dir[FILE_SIZE];
    char app_data_dir[FILE_SIZE];
    char tmp_dir[FILE_SIZE];
    char *delta_ptr;
    char *data_ptr;
    char *tmp_ptr;
    char pin_file[FILE_SIZE];
    char status[64];
    char prefix[64];
    char suffix[64];
} OPC_LIB;

extern OPC_LIB lib;

#endif
