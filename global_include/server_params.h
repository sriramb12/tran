#ifndef server_params_h
#define server_params_h

#include "global_defines.h"

/* Holds info needed to connect to the database server */
/* The GUI never assigns values to these variables */
/* They are set by the database client */
typedef struct
{
    char machine[MACHINE_LEN];
    char address[ADDRESS_LEN];
    char site [SITE_LEN];
    char site_name[SITE_LEN];
int socket;
} SERVER_PARAMS;

extern SERVER_PARAMS server;

#endif /* ifdef for server_params_h */
