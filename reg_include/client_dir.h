#ifndef client_dir_h
#define client_dir_h


/* Delete directory */
int dir_dlt(char *directory);

/* List specified directory */
int dir_list(char *directory, char *files[]);



int dir_crt(char *dir);

/* Return pointer to value for current working directory */
/* Memory pointed to is global and does not need to be freed */
char * cwd_get(void);

/* Set current working directory */
/* Sets global value used by database client for current working directory */
void cwd_set(char *directory);

#endif
