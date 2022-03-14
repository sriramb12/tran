#ifndef client_files_h
#define client_files_h

unsigned long file_mode_set(void);

short file_get(
    char *local_dir,	/* Already initialized */
    char *local_file);	/* Not yet initialized */

short file755get(
    char *local_dir,	/* Already initialized */
    char *local_file);	/* Not yet initialized */


short full_file_get(char *full_file_name);

/* Just like file_get except does not pass file name */
short data_get(
    char *local_dir,	/* Already initialized */
    char *local_file);	/* Already initialized */

short command_get(char *command);

long long command_get_compressed(char *command);

/* Erase files freeing the file list */
void files_erase(char *dir, char *files[]);

/* Get specified files */
int files_get(char *dir, char *files[]);

int mask_files_get(char *dir, char *files[]);

int execute_files_get(char *dir, char *files[]);

int file_put(
    char *dir,	/* Local directory for file */
    char *file_name);	/* Root file name used locally and remotely */

int file_put_alternate(
    char *dir,	/* Local directory for file */
    char *file_name,	/* Root file name used locally and remotely */
    char *alternate_name);	/* Root name used on the server */

short command_put( char *command, long long size);	/* Command to execute to get output */

short command_put_compressed( char *command, long long size);	/* Command to execute to get output */

/* Send data to server */
/* Server already knows name of file being sent */
short data_put(
    char *file_name,	/* Root file name used locally and remotely */
    char *dir);	/* Local directory for file */


int files_put(char *dir, char *files[]);

/* Writes string to specified file */

/* Retrieve list of names and place in array of strings */
/* Number of names returned */
/* Returns negative return code if transaction fails */
int names_get(char *names[]);

/* Free names on array of pointers */
void files_free(char *files[]);

char *root_find(char *root_ptr, char c);

void print_init(long long size);

/* See if status message must be printed */
void print_status(int bytes);

#endif
