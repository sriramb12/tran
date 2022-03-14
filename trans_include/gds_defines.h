#ifndef gds_defines_h
#define gds_defines_h
#include "global_defines.h"

#define GDS_VERSION "1.0.3"

/* Command line parameters */
typedef struct
{
    int server_type;
    int request_type;
    char design[FILE_SIZE];
    char mask[16];
    char tech[FILE_SIZE];
    char kendall[64];
    char comment_file[FILE_SIZE];
    char gds_file[FILE_SIZE];
    char state[64];
    char email_addr[64];
    char extra2[64];
    char extra3[64];
    char extra4[64];
    char extra5[64];
    int database_write;
} GDS_DATA;



#endif
