#ifndef transcend_lib_h
#define transcend_lib_h

/* Initialize data Transcend client must send to the server */
/* this function must be called before other Transcend functions are called */
void defaults_init(void);

/* Transfer file or directory to Transcend repository */
/* Symbolic links are not included */
/* file _name holds file or directory to be transferred */
/* Absolute or relative paths may be specified with file name */
/* user_array is an array of pointers with the official user ID */
/* of users to receive email notification with a secure keyword */
/* The list is terminated with a NULL pointer */
/* If no email notification is being sent, first pointer points to NULL */
/* If email is being sent, description holds short description user */
/* wishes to be sent in the email message */
/* If no email is being sent or if there is no description, */
/* the NULL pointer i.e. "" can be passed in */
/* File type allows user to specify binary or ASCII data */
/* In most cases he can ignore the file type, and it is set to UNSPECIFIED */
/* file type is a new feature for 3.0 of Transcend */
/* msg_ptr points to message returned from Transcend server */
/* Returns 0 if transaction successful */
/* Returns negative number if transaction fails */
int trans_put(char *file_name, char *user_array[], char *description, 
    TRANS_FILE_TYPE file_type, char **keyword_ptr, char **msg_ptr);

/* Perform put transaction on a directory */
/* Symbolic links are included */
int trans_put_links(char *file_name, char *user_array[], char *description, 
    TRANS_FILE_TYPE file_type, char **keyword_ptr, char **msg_ptr);

/* Perform put transaction and specify deletion after first retrieval */
int trans_put_delete(char *file_name, char *user_array[], char *description, 
    TRANS_FILE_TYPE file_type, char **keyword_ptr, char **msg_ptr);

/* Perform put transaction on a directory */
/* Symbolic links are included */
/* and the data is deleted after the first retrieval */
int trans_put_delete_links(char *file_name, char *user_array[], char *description, 
    TRANS_FILE_TYPE file_type, char **keyword_ptr, char **msg_ptr);

/* Retrieve a file or directory from the Transcend repository */
/* Path points to path where file will be placed */
/* Absolute or relative paths can be used */
/* Pass in "." if want to use current working directory */
/* file_name points to name of file retrieved */
/* Memory is allocated by trans_get */
/* Calling function is responsible for freeing this memory */
/* msg_ptr points to message returned from Transcend server */
/* Returns 0 if transaction is successful */
/* Returns negative number if transaction fails */
/* Returns file type in return_file_type */
int trans_get(char *keyword, char *user_path, TRANS_FILE_TYPE file_type,
    char **file_ptr, char **msg_ptr, TRANS_FILE_TYPE **return_file_type);

/* Retrieve a file or directory from Transcend repository */
/* and delete data from repository after is is successfully retrieved */
/* msg_ptr points to message returned from Transcend server */
/* Returns 0 if transaction is successful */
/* Returns negative number if transaction fails */
/* Returns file type in return_file_type */
int trans_get_delete(char *keyword, char *user_path, TRANS_FILE_TYPE file_type,
    char **file_ptr,  char **msg_ptr, TRANS_FILE_TYPE **return_file_type);

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

/* Send email to 1-6 users for data with specified keyword */
/* user_array is an array of pointers pointing to 1-6 user's corporate ID's */
/* The list is terminated with a NULL pointer */
/* If email is being sent, description holds short description user */
/* wishes to be sent in the email message */
/* If no email is being sent or if there is no description, */
/* the NULL pointer i.e. "" can be passed in */
int trans_mail_keyword(char *keyword, char *user_array[], char *description,  char **msg_ptr);

/* Set email notification on for all files user owns */
/* Whenever a file the user owns is retrieved, the user receives email */
int trans_set_user_email(char **msg_ptr);

/* Turn on email notification for specified data */
/* Whenever a user retrieves specified data, */
/* the sending user is sent email notification */
int trans_set_file_email(char *keyword, char **msg_ptr);

/* Cancel previously set email notification */
int trans_can_user_email(char **msg_ptr);

/* Cancel previously set file email notification */
int trans_can_file_email(char *keyword, char **msg_ptr);

/* Retrieve information about Transcend releases available for retrieval */
/* Returns message in command line format */
/* This will need to be changed for the Java release */
int trans_release_info(char **msg_ptr);

/* Retrieve specified Transcend release */
/* Returns message describing how to retrieve release using command line */
/* format if specified release is not present */
/* This will need to be changed for the Java release */
int trans_get_release(char *release, char **msg_ptr);

/* Set data type for specified data to ASCII */
/* Useful when transferring data between unix, NT and Mac machines */
 int trans_set_ascii(char *keyword, char **msg_ptr);

/* Set data type for specified data to binary */
/* Useful when transferring data between unix, NT, and Mac platforms */
int trans_set_binary(char *keyword, char **msg_ptr);

/* Retrieve information about specified user */
/* Information from the whois command along with information */
/* from the Transcend registry is returned */
/* Any string that identifies the user can be passed in */
/* If white space is present in the string, it is replaced with a '.' */
int trans_query(char *string, char **msg_ptr);

/* Verify user is registered in the Transcend registry */
/* Returns SUCCESS if registered */
/* Returns LOCK_ERR, (xx) if user's account is locked */
/* Returns REG_ERR (-520) if user not registered */
/* Returns negative number for other error conditions */
/* msg_ptr holds message returned from Transcend server */
int trans_reg_check(char **msg_ptr);

/* Perform auto registration */
/* Returns SUCCESS if registration successful */
/* Returns negative error code if unable to perform registration */
/* msg_ptr holds message returned from Transcend server */
int trans_reg(char *user_id, char *pwd, char **msg_ptr);

/* Authenticate user with X500 user ID and password */
/* Returns SUCCESS if authentication successful */
/* Returns negative error code if unable to authenticate user */
/* msg_ptr holds message returned from Transcend server */
int trans_authen(char *user_id, char *pwd, char **msg_ptr);

/* Modify default server being used */
/* Defined sites are WACC, Austin, and Munich */
int trans_site_set(char *site, char **msg_ptr);

#endif
