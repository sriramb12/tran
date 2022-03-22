#ifndef opc_server_files_h
#define opc_server_files_h


void file_get(char *dir, char *file_name);

long long data_get(char *dir, char *file_name);

/* Send a group of files to the client */
void files_get(char *dir, char *files[]);

long long file_put(char *dir, char *file);

/* Get data from client and pipe it through a command */
long long command_put(char *command);

long long command_put_compressed(char *command);

/* Just like file get except do not pass file name */
int data_put(char *dir, char *file);

int files_put(char *dir, char *files[]);

/* Get output from a command */
long long command_get(char *command);


short command_get_compressed( char *command);

void command_get_line(char *command);

void file_data_get(char *file_data, char *file_name);

int files_digit_list(char *files[]);
#endif
