#ifndef client_prototypes_h
#define client_prototypes_h

#include <stdio.h>

/* line_get.c */
/* Gets line up to 260 characters in length */
    int line_get(FILE *fp, char *line);

/* line_get.c */
/* Gets line up to 2048 characters in length */
    int long_line_get(FILE *fp, char *line);

/* password_encrypt.c */
    void password_encrypt(char *password, char *pwd, int len);

/* password_crypt.c */
int password_crypt(char *new_password, char *old_password);
short get_ret_code(void);

short init_socket (char *buffer);

/* List functions used in all of the clients */

/* Get a list of file list */
int list_put(char *files[]);

/* Send list of list to client */
int list_get(char *files[]);

/* Delete files no longer needed */
void files_dlt(char *dir, char *files[]);

/* Read list of list from specified file */

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



/* Expand a technology list */
int tech_list_expand(char *dir, char *old_files[], char *new_files[]);

/* Convert list from string format to list format */
int string_list_convert(char *string, char *list[]);

/* convert list to string */
void list_string_convert(char *list[], char *string);


void static_list_string_convert(char *list[], char *string);

/* Convert string to list format */
/* Items on list are separated by the character specified */
int string2double_list_convert(char *string, char *extension_list[], char *ratio_list[]);

#endif /* define for client_prototypes_h */
