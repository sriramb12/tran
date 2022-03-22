
void file_get(char *dir, char *file_name);

/* Send file with alternate name for client */
void file_get_alternate(char *dir, char *file_name, char *alternate_file);

void data_get(char *dir, char *file_name);

/* Send a group of files to the client */
void files_send(char *dir, char *files[]);

int static_files_send(char *dir, char *files[]);

void file_put(char *dir, char *file);

/* Get data from client and pipe it through a command */
void command_put(char *command);

/* Just like file get except do not pass file name */
int data_put(char *dir, char *file);

/* Returns number of files put */
int files_rcv(char *dir, char *files[]);

/* Get output from a command */
void command_get(char *command);

void command_get_line(char *command);

/* Delete a group of files and free memory for file names */
void files_delete(char *dir, char *files[]);

/* Free file list no longer needed */
void files_free(char *files[]);

/* build dynamic list of specified files */
int files_build_list(char *dir[], char *file0, char *file1,
char *file2, char *file3, char *file4);
/* Sort the files */
void tmp_files_sort(char *files[]);

/* Find root path name */
char *root_find(char *ptr, char c);

