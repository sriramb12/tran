#ifndef mask_clientver_globals_h
#define mask_clientver_globals_h

#include "whobu.h"
#include "m2client_defines.h"
#include "m2defines.h"
#include "mask_client_defines.h"
#include "../global_include/server_params.h"
#include "../global_include/mask_msg.h"

#define FILELIST_FILE ".m2filelist"
#define FABLIST_FILE ".m2FABLIST"
#define DIRLIST_FILE ".m2dirlist"
extern MASK_MSG gu;

/* Holds names and addresses of server machines */
extern MASK_PARAMS s2;

/*static char mask_machine[MAX_MASK_SERVERS][MACHINE_LEN] =
{
    "mds-db1",
    "mds-db2",
    "mds-db1",
    "mds-db2",
};*/

/* Holds IP addresses for default Mask Prep servers */
/*static char mask_address[MAX_MASK_SERVERS][MACHINE_LEN] =
{
    "10.81.198.121",
    "10.65.24.19",
    "10.81.198.121",
    "10.65.24.19",
};*/


/* Holds socket, address, and machine for Mesa Mask Shop server */
extern SERVER_PARAMS ms;

/* Holds socket, machine, and address for Dupont Mask Shop */
extern SERVER_PARAMS dp;

/* Holds special mask client flags */
extern MASK_CLIENT_FLAGS fl;

extern int sock;

extern char m2password[32];

/* Read file list */
extern char *read_files[M2TOTAL_FILES];


/* List of files to put back */
extern char *filelist[M2TOTAL_FILES];
extern char *fablist[500];
extern char *globallist[M2TOTAL_FILES];

/* List of directories to put back */
extern char *dirlist[1000];
extern char *dirlist_files[M2TOTAL_FILES];
extern DB_SOFTWARE so;

extern char user_info[USER_ROWS][USER_COLS];

extern int m2library_version;
#endif /* Define for mask_client_globals_h */
