#ifndef rtn_h
#define rtn_h

#include "global_defines.h"

typedef struct
{
    char msg[BIG_BLOCK_SIZE];
    int len;
    int flag;
    int transfer_status;
} RTN;

extern RTN rtn;


#endif /* define for rtn_h */
