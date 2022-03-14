/* this header was used for the old Mica API and is now deprecated */
#ifndef trans_api_h
#define trans_api_h

#define DBYES 1
#define DBNO 2

/* Initialize data Transcend client must send to the server */
/* this function must be called before other Transcend functions are called */
/* initializes the default repository to use */
/* 0, 1, 2, 3, or 4 for the WROC, Tempe, Ed bluestein, Munich, and Sendaii  */
/* repositories respectively */
void defaults_init(int repository);

/* Verify user is registered in the Transcend registry */
/* Returns SUCCESS if registered */
/* Returns REG_ERR (-520); if user not registered */
/* Returns negative number for other error conditions */
/* msg_ptr points to message returned from Transcend server */
/* This pointer should never be freed */
int trans_reg_check(char **msg_ptr);

/* Perform auto registration */
/* Returns SUCCESS if registration successful */
/* Returns negative error code if unable to perform registration */
/* msg_ptr points to message returned from Transcend server */
int trans_reg(char *user_id, char *pwd, char **msg_ptr);

/* Get Transcend version */
char *trans_version_get(void);

/* Transfer file or directory to Transcend repository */
/* Symbolic links are not included */
/* file _name holds file or directory to be transferred */
/* Absolute or relative paths may be specified with file name */
/* email_array is an array of pointers  pointing to the e-mail addresses */
/* of users to receive email notification with a secure keyword */
/* The list is terminated with a NULL pointer */
/* If no email notification is being sent, first pointer points to NULL */
/* trans_put does not free any of the pointers */
/* If e-mail being sent, 'description' holds text sent in e-mail message */
/* If no email is being sent or if there is no description, */
/* the NULL pointer i.e. "" can be passed in */
/* msg_ptr points to message returned from Transcend server */
/* Returns 0 if transaction successful */
/* Returns negative number if transaction fails */
int trans_put(char *file_name, char *description,  char **email_array,
char **key_ptr, char **rtn_ptr);

/* Put data in repository & set deletion to occur after first retrieval */
/* This function can be used when the data will be retrieved only once */
/* See description for trans_put for explanation of parameters */
int trans_put_delete(char *file_name, char *description,  char **email_array,
char **key_ptr, char **rtn_ptr);

/* Transfer file or directory to Transcend repository */
/* Symbolic links are included */
/* file _name holds file or directory to be transferred */
/* Absolute or relative paths may be specified with file name */
/* email_array is an array of pointers  pointing to the e-mail addresses */
/* of users to receive email notification with a secure keyword */
/* The list is terminated with a NULL pointer */
/* If no email notification is being sent, first pointer points to NULL */
/* trans_put_with_links does not free any of the pointers */
/* If e-mail being sent, 'description' holds text sent in e-mail message */
/* If no email is being sent or if there is no description, */
/* the NULL pointer i.e. "" can be passed in */
/* msg_ptr points to message returned from Transcend server */
/* Returns 0 if transaction successful */
/* Returns negative number if transaction fails */
int trans_put_with_links(char *file_name, char *description,  char **email_array,
char **key_ptr, char **rtn_ptr);


/* Retrieve a file or directory from the Transcend repository */
/* Path points to path where file will be placed */
/* Absolute or relative paths can be used */
/* Pass in "." if want to use current working directory */
/* file_name points to name of file retrieved */
/* Memory for file_name returned is allocated by trans_get */
/* Calling function is responsible for freeing this memory */
/* msg_ptr points to message returned from Transcend server */
/* do not try to free msg_ptr */
/* Returns 0 if transaction is successful */
/* Returns negative number if transaction fails */
int trans_get(char *keyword, char *user_path,  int local_delete,
    char **file_ptr, char **msg_ptr);

/* Retrieve a file or directory from the Transcend repository */
/* Delete from Transcend repository when retrieval completes */
/* Path points to path where file will be placed */
/* Absolute or relative paths can be used */
/* Pass in "." if want to use current working directory */
/* file_name points to name of file retrieved */
/* Memory for file_name returned is allocated by trans_get */
/* Calling function is responsible for freeing this memory */
/* msg_ptr points to message returned from Transcend server */
/* do not try to free msg_ptr */
/* Returns 0 if transaction is successful */
/* Returns negative number if transaction fails */
int trans_get_delete(char *keyword, char *user_path,  int local_delete,
    char **file_ptr, char **msg_ptr);

/* Prepare to retrieve a file from the repository */
/* Returns DBYES if the file exists */
/* Returns DBNO if the file does not exist */
/* Returns return code less than 0 if transaction fails */
/* if successful, rtn_ptr holds file_name, size, file mode, file_type, */
/* and remote compression */
int trans_get_init(char *primary_key, int remote_delete, char **file_ptr, char **msg_ptr);

/* Retrieve data from the repository */
/* this function is called after trans_init has been alled */
/* Must pass in the pointer returned from trans_init */
/* the information passed in is replaced with the results from the transaction */
int trans_get_data(char **rtn_ptr);

/* Retrieve information about specified file/directory */
/* present in Transcend repository using file keyword */
/* msg_ptr points to message returned from Transcend server */
/* Returns 0 if transaction successful */
/* Returns negative number if transaction fails */
int trans_info(char *keyword, char **msg_ptr);

/* Delete specified data from Transcend repository */
/* Pass in keyword of data to delete */
int trans_delete(char *keyword, char **msg_ptr);

/* Retrieve original and compressed size of data specified with keyword */
 int trans_size(char *keyword, char **msg_ptr);

/* Lock specified data for retrieval */
/* This allows a user to prevent other users from retrieving */
/* data without deleting the data.  This transaction is */
/* useful when a user has given out a keyword and then wants */
/* to prevent the user from retrieving the data until he has */
/* verified the data is what the receiving user really needs */
int trans_lock(char *keyword, char **msg_ptr);

/* Unlock data previously locked for retrieval with the */
/* trans_lock function */
int trans_unlock(char *keyword, char **msg_ptr);

/* Retriev history of transactions on specified data */
/* First 1000 characters of history returned in msg_ptr */
/* Complete history written to [keyword].history file */
int trans_history(char *keyword, char **msg_ptr);

/* Extend the expiration date on data 1-3 additional working days */
int trans_extend(char *keyword, int days, char **msg_ptr);

/* Set email notification on for all files user owns */
/* Whenever a file the user owns is retrieved, the user receives email */
int trans_user_email_set(char **msg_ptr);

/* Turn on email notification for specified data */
/* Whenever a user retrieves specified data, */
/* the sending user is sent email notification */
int trans_file_email_set(char *keyword, char **msg_ptr);

/* Cancel previously set email notification */
int trans_user_email_can(char **msg_ptr);

/* Cancel previously set file email notification */
int trans_file_email_can(char *keyword, char **msg_ptr);

/* Send secure keyword to multiple users for specified file keyword */
/* user_array is an array of pointers pointing to multiple user's corporate ID's */
/* The list is terminated with a NULL pointer */
/* If email is being sent, description holds short description user */
/* wishes to be sent in the email message */
/* If there is no description */
/* the NULL pointer i.e. "" can be passed in */
int trans_mail_keyword(char *keyword, char *user_array[], char *description,  char **msg_ptr);

/* Retrieve information about specified user */
/* Information from the LDAP database command along with information */
/* from the Transcend registry is returned */
/* The user's CoreID must be passed in */
int trans_user_query(char *core_id, char **msg_ptr);

/* Retrieve information about specified user */
/* Information from the LDAP database command along with information */
/* from the Transcend registry is returned */
/* The user's CoreID must be passed in */
int trans_user_site_query(char *core_id, char **msg_ptr);

#endif
