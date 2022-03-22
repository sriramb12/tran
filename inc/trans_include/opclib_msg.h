#ifndef opclib_msg_h
#define opclib_msg_h

#include "global_defines.h"


/* Holds data passed from the GUI to the server */
typedef struct
{
    int req_type;
     int data_type;
     char library[32];
    char formsgroup[32];
    char package[32];
     char source_library[32];
    char source_formsgroup[32];
    char source_package[32];
    char pin[13];
    long client_time;
} POSEIDON_MSG;

extern POSEIDON_MSG opc;

#endif /* Define for opc_server_defines_h */
