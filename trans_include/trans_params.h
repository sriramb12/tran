#ifndef trans_params_h
#define trans_params_h

#include "trans_defines.h"
#include "server_params.h"

/* Holds server name, address, and socket */
extern SERVER_PARAMS server;

TRANS_DISK  disk;
extern int disk_cnt;	/* Holds number of disks where Transcend files put */
float server_version;
long current_time;
long server_size;
int global_lock_cnt;
int user_lock_cnt;
int rmt_server_num;	/* Number of remote servers */
#endif /* ifdef for trans_params_h */
