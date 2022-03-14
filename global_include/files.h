#ifndef files_h
#define files_h

#include "global_defines.h"

typedef struct
{
    char name[TOTAL_FILES][FILE_SIZE];
    char dir[FILE_SIZE];
    int num;
} DB_FILES;


int files_compress(
    DB_FILES *files,
    char *compress_string,
    char compress_flag);


int files_list(DB_FILES *files);

void files_copy(
    DB_FILES *files,
    char *old_dir);

void
files_dlt(DB_FILES *files);
void 
file_copy(
    char *old_file,
    char *new_file);

int 
file_rename(
    char *old_file,
    char *new_file);

void files_sort(char *files[]);

#endif /* Define for files_h */
