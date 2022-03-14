#ifndef m2mask_files_h
#define m2mask_files_h

typedef enum
{
  M2OWNER,
  M2NOT_OWNER,
    M2NOT_FOUND
} m2mask_status;

extern int db_program[];
extern int m2program[];
extern int local_m2program[];
extern int local_db_program[];
extern char *db_filelist[];
extern char *m2filelist[];
extern char *local_m2filelist[];
extern char *local_db_filelist[];

/* Read in latest version number from delta file */
float mask_version_read(char *delta_file);

/* Does not include . .. .fab, .db or .tmpdb in directory list */
/* Returns the number of files found */
int mask_files_list(char *directory, char *files[]);

int mask_files_version_list(char *directory, char *files[], char *delta_dir, int block_flag);

/* Retrieve specified mask files */
int mask_files_rcv(char *dir, char *files[], int level);


/* Version new files just retrieved from client */
/* also do necessary replacements */
int mask_files_version(char *new_data_dir, char *old_data_dir, 
    char *new_delta_dir, char *old_delta_dir, char *files[]);

/* Version other files present */
void other_files_version(int level, char *delta_dir, char *filelist[], float version);
/* Read names of files defined for this mask set */
void other_files_read(char *files[], int program[]);

/* Verify mask file is at the latest version */
int mask_file_is_latest(char *root_file);

/* See if present on file list */
int filelist_check(char *name, int level);


/* Makes new directory and copies files from old directory to new directory */
/* Makes list of files in source directory */
/* and copies files to destination directory */
/* List of files copied returned to calling program */
/* The directory name passed in must not have an ending slash */
/* Only latest version of files copied */
int mask_dir_copy(char *new_dir, char *old_dir, char *delta_dir,  char *files[], int mod_flag);

#endif
