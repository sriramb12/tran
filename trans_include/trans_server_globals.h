#ifndef trans_server_globals_h
#define trans_server_globals_h

#include "trans_server_defines.h"
#include "server_params.h"

/* Holds data passed to server from GUI */
extern TRANS_PARAMS gu;

extern TRANS_FLAGS fl;

extern int user_lock_cnt;
extern int icom_sock;

/* number of times user can enter invalid keyword before locked out of Transcend */
extern int global_lock_cnt;

extern TRANS_DISK  disk;
extern int disk_cnt;	/* Holds number of disks where Transcend files put */
extern float server_version;

extern TRANS_FILE_STAT fs;


typedef struct MYSQL_COLUMN_RETRIEVE
{
    char *field[40];
    char fields_counter;

} MYSQL_COLUMN_RETRIEVE;


#endif /* define for trans_server_globals_h */
