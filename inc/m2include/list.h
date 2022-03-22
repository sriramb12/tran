#ifndef list_h
#define list_h

/* Get a list of file list */
int list_put(char *files[]);

/* Send list of list to client */
int list_get(char *files[]);

/* Delete files no longer needed */
void files_dlt(char *dir, char *files[]);

/* Read list of list from specified file */
int list_read(char *file_name, char *list[]);

/* Write list of list to specified file */
int list_write(char *file_name, char *list[]);

/* Add an entry to the list */
/* Names are kept in alphabetical order */
/* Returns 1 if entry added */
/* Returns 0 if current entry replaced */
/* Returns SUCCESS if name added */
int list_add(char *list[], char *entry, char *key, int total_cnt);

/* Delete name from  list */
/* Returns SUCCESS if name deleted */
/* Returns FAILURE if name not present */
int list_delete(char *list[],  char *key, int total_cnt);

/* Verify specified name is in the list array */
int list_find(char *list[], char *key);

/* Add the user's name before adding to the list */
int user_list_add(char *list[], char *key, int total_cnt);


/* Expand a technology list */
int tech_list_expand(char *dir, char *old_files[], char *new_files[]);

/* Convert list from string format to list format */
int string_list_convert(char *string, char *list[]);

/* convert list to string */
void list_string_convert(char *list[], char *string);

#endif
