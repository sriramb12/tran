#ifndef opcdir_h
#define opcdir_h

#include <dirent.h>
#include <sys/stat.h>
#include "opcdir_stat.h"
#include <sys/types.h>
#include <time.h>
#include "global_defines.h"
#include "opcq.h"


#define OPCDIR_INPUT "%s %s %s %s %s %s %s %s %s"

/* Create directory on OPC machine with specified files */
/* Not currently being used */
int opc_mkdir                           (char *dir, char *files[], long long size);

int opc_send_dir                           (char *dir, char *files[], long long size);

/* Remove specified directory on OPC machine */
short opc_rmdir                           (char *dir); 

/* Remove old project directories on OPC machine */
short opcdir_cleanup(void);

/* Add specified files to directory on OPC machine */
/* List of files is terminated by pointer to NULL */
short opcdir_add_files                    (char *dir, char *files[]); 

/* Delete specified files from OPC directory */
/* Last file on list is followed by pointer to NULL */
short opcdir_dlt_files(char *dir, char *files[]);


/* Display list of specified directory from OPC machine */
/* Brings the list file from the database and puts it in a temporary directory */
/* On the user's machine called '/tmp' */
/* Opens the temporary file brought down so it is ready to read */
OPC_FILE *opcdir_open(char *dir);

/* Opens file with 64 bit mode structure */
OPC_FILE *opcdir64open(char *dir);

OPC_FILE *opcdir_project_open(void);

/* Return file in 64 bit mode structure, 40 bytes to file info */
OPC_FILE *opcdir64project_open(void);

/* Read one entry from directory list  file */
/* Function returns structure filled in */
/* Returns NULL if unable to read structure */
int opcdir_read(OPC_FILE *fi, OPC32INFO *buf);

/* Reads in one record using 64 bit mode structure */
int opcdir64read(OPC_FILE *fi, OPC64INFO *buf);

/* Close OPCDIR file */
int opcdir_close(OPC_FILE *fp);

/* List OPC roots present on the OPC machine */
/* Brings down temporary file with this list and opens it */
short opcdir_root_list(void);

/* Display listing of specified directory */
short opcdir_list(char *directory);
    /* Get specified file from OPC machine */
short opcdir_file_get(char *rmt_file, char *rmt_dir);

    /* Get specified files from OPC machine */
/* Files retrieved in background */
short opcdir_files_get(char *user_dir, char *files[]);

    /* Get specified files from OPC machine */
/* Files retrieved in foreground */
short opcdir_files_get_foreground(char *user_dir, char *files[]);

/* Returns buffer with information about specified file or directory */
/* Projet name is included in the file or directory path */
/* Useful to determine if files or directories are present on server */
/* Returns 0 if successful and -1 if an error occurs */
int opcdir_stat(char *path, OPC32INFO *buf);

/* Read one entry from directory list  file */
/* Function returns structure filled in */
/* Reads in members for 64 bit machine configuration */
/* Returns NULL if unable to read structure */
int opcdir64read(OPC_FILE *fi, OPC64INFO *buf);

/* Returns buffer with information about specified file or directory */
/* Projet name is included in the file or directory path */
/* Useful to determine if files or directories are present on server */
/* Returns 0 if successful and -1 if an error occurs */
/* Uses 64 bit structure */
int opcdir64stat(char *path, OPC64INFO *buf);

#ifdef SUNOS
/* Copies data from Solaris stat structure to SunOS stat structure */
void solaris2sunos_stat_copy(OPC_FILE_INFO *buf, struct solaris_stat *sbuf);
#endif

#endif /* define for opcdir_h */
