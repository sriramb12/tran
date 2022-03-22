#ifndef opc_functions_h
#define opc_functions_h

/* Holds define for OPC_FILE */
#include "opcq.h"

extern char opc_library[];
extern char opc_pin[];

/* Strings to indicate status returned */
/*
static char opc_status_array[3][12] =
{
    "Success",
    "Failure",
    "not present"
} RET_STATUS_ARRAY;
*/

/* Initialize data needed to perform a basic database transaction */
/* Initializes user's login, name, IP node address, etc. */
void init_globals(void);

/* Returns a pointer to the current active RET server */
char *opcserver_name(void);

/* Many of the functions return an array of pointers */
/* The last one points to NULL */
/* A function is provided to count the number of files on the list */
/* Returns the number of files being pointed to */
int dbfile_cnt(char *files[]);

/* The following functions are specific RET functions */

/* Set the value for the PC library name and PIN */
/* These are global variables known to the client */
/* After the library name and PIN is set, */
/* other functions use this information when making database requests */
void opc_lib_set(char *name, char *pin);

/* Create a new PC library */
/* If this function is successful, it calls opc_lib_set */
/* and returns DB_SUCCESS */
/* If it is unsuccessful, it returns a negative return code */
short opc_lib_crt(char *name, char *pin);

/* Delete one or more RET files */
/* Returns SUCCESS if delete is successful */
/* Returns DB_ERR if  delete fails */
short opc_files_dlt(char *opc_files[], char *status);

/* Read RET data */
/* Files retrieved are placed in the current working directory */
/* Returns SUCCESS if one or more files retrieved */
/* Returns DB_ERR if read fails */
short opc_files_read(
    char *wild_card,	/* Holds wild card needed to retrieve the data */
    char *files[]);	/* Returns list of files retrieved */

/* Lock one or more RET files for edit */
/* After RET files have been read, the user will wish to lock them for edit */
/* Thus, the files are not retrieved from the database, */
/* but simply locked for edit */
/* Returns SUCCESS if all files locked */
/* Returns DB_ERR if all files not locked */
/* Also returns lock status flag for each file */
short opc_files_lock(char *files[], char *status);

/* Cancel lock for edit */
/* Cancels lock for edit on one or more RET files */
/* Returns SUCCESS if cancel lock is successful for all specified files */
/* Returns DB_ERR if cancel lock fails for one or more files */
/* Also returns status flag for each specified file */
short opc_files_canlock(char *files[], char *status);

/* Check in RET data */
/* Checks in one or more specified RET files */
/* Returns SUCCESS if is able to check in all of the files */
/* Returns DB_ERR if cannot check in one or more files */
/* Also returns status flag for each file */
short opc_files_chkin(char *description, char *opc_files[], char *status);

/* View list of file versions. */
/* Gives the user a list of versions present for the specified RET file. */
/* List of versions is returned in a file */
/* This file is placed in a temporary directory */
/* Returns FILE pointer to open file if transaction successful */
/* Otherwise returns NULL */
OPC_FILE *opc_rtv_versions(
    char *opc_file[1]);	/* RET file to retrieve versions for */

/* Closes and deletes temporary version file */
int opc_version_close(OPC_FILE *fp);

#endif
