#ifndef dir_h
#define dir_h

int dir_archive_dlt(char *directory, char *mask);

/* Creates new directory */
/* If directory already present, deletes and re-creates */
void dir_create(char *dir);

void dir_make(char *dir);

int dir_delete(char *directory);

int dir_recursive_dlt(char *directory);

/* Remove specified directory */
/* If delete fails, client sent error message */
int dir_remove(char *directory);

void dir_rename(
    char *dir1,	/* Old directory */
    char *dir2);	/* New directory */

void dir_recursive_rename(
    char *dir1,	/* Old directory */
    char *dir2);	/* New directory */

/* Make list of files in directory except for those beginning with '.' */
int dir1list(char *directory, char *files[]);


/* Makes new directory and copies files from old directory to new directory */
/* Makes list of files in source directory */
/* and copies files to destination directory */
/* List of files copied returned to calling program */
/* The directory name passed in must not have an ending slash */
int dir_copy(char *new_dir, char *old_dir, char *files[]);

/* Make list of files in directory except for those beginning with '.' */
int dir_list(char *dir, char *files[]);

#endif
