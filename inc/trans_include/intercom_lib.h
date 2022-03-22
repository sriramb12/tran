#ifndef intercom_lib_h
#define intercom_lib_h

#include "trans_defines.h"

#define DBYES 1
#define DBNO 2

/* Initialize data InterCom client must send to the server */
/* this function must be called before other InterCom functions are called */
/* ip_addr holds user's IP node address if known */
/* core_id holds user's core ID */
/* name holds user's name */
void defaults_init(char *ip_addr, char *core_id, char *friendly_email, char *name);

/* Perform OneIT authentication*/
/* This is used for internal NXP customers */
/* Returns SUCCESS if authentication successful */
/* If successful friendly email followed by  name returned in msg_ptr */
/* They are separated with a semicolon */
/* Returns negative error code if unable to authenticate user */
/* msg_ptr holds message returned from InterCom server */
int icom_authen(char *user_id, char *pwd);

/* Retrieve defined company names from server */
/* Used only for internal NXP requests */
/* If call to this function is successful, */
/* Company names returned with pointer passed to this function */
/* string format is company name;email address;TransNet access */
/* If company does not have default email address, email set to '0' */
/* TransNet acces set to 'yes' if a remote directory must be specified */
char **icom_company_rtv();

/* Retrieve list of available repositories */
/* Used only for internal NXP customers */
char **icom_repository_rtv();

/* Prepare to put uncompressed file into the repository */
/* Only called when data will not be compressed when written to the socket */
/* When this function is called, the file name, company, and description have been parsed and are */
/* passed into the function */
/* company_line initialized when function icom_company_rtv called */
/* remote_email holds  space separated email addresses */
/* These are supplied by the user */
/* If string is empty default email addresses used if present */
/* delete flag holds DBYES (1); for delete after first retrieval */
/* and DBNO (2); for no deletion after first retrieval */
/* local_compress holds compression status, DBYES for compression and DBNO for no compression */
/* msg_ptr holds ASCII message returned */
/* Returns 0 if transaction successful, otherwise returns negative return code */
int icom_put_init(char *file_name, char *company_line, char *description,
char *remote_email, TRANS_FILE_TYPE file_type, int delete, int local_compress, unsigned long long size);

/* Write uncompressed data to the socket */
/* string holds data to write to socket */
/* len holds length of string */
/* Testing has revealed optimal string length is 4096 bytes */
/* Data read  using HTTP protocol and written to socket until end of file reached */
/* Returns SUCCESS (0) if transaction successful */
/* Returns negative return code if transaction fails */
short put_binary_string(char *string, short len);

/* Let server know all data has been transferred */
/* Pass value of 0 to server */
/* Returns SUCCESS (0) if transaction successful */
/* Returns negative return code if transaction fails */
short put_short(short len);

/* Close connection when sending uncompressed data */
/* size holds final size of data transferred */
/* msg_ptr holds ASCII string returned */
/* Returns SUCCESS (0) if transaction successful */
/* Returns negative return code if transaction fails */
int icom_put_close(long long size, char *description, char file_type);

/* Prepare to write compressed data to the socket */
/* This function is called when transferring compressed data */
/* file_name holds name of file being transferred */
/* description holds file description */
/* delete indicates if file will be deleted after */
/* first retrieval, DBYES (1) or DBNO (2) */
FILE * icom_put_compress_init(char *file_name, char *company_line, char *description,
char *remote_email,  int remove);

/* Writes compressed data to the open socket */
/* Data is compressed as it is written to the socket */
/* file holds stream pointer returned by icom_put_compress_init */
/* buf holds string to write to socket */
/* len holds length of string */
/* Data read from HTTP and written to socket until end of file reached */
int trans_write (FILE *file, char *buf, unsigned len); 

/* close connection when sending compressed data */
/* file is the stream pointer used to write data to the socket */
/* This function also lets server know all data has been transferred */
/* msg_ptr holds ASCII message returned */
/* Returns 0 for successful transaction */
/* Returns negative number if transaction not successful */
int icom_put_compress_close(FILE *file, char *description);

/* Prepare to retrieve data from repository */
/* Called when retrieving both compressed and uncompressed data */
/* primary_key holds 9 character secure or file keyword to retrieve */
/* secondary_key holds 9 character file keyword to retrieve */
/* Both keys needed for external customers */
/* For internal customers, string "0" is passed in for secondary_key */
/* Returns name of file, length of file, etc. */
/* This data must be parsed */
/* If file already present on user's disk */
/* ask user if he wants to overwrite the file */
/* delete holds DBYES if data should be deleted after retrieval */
/* delete holds DBNO if data not deleted after retrieval */
/* This value is set by the user */
int icom_get_init(char *primary_key, char *secondary_key, int remove);

/* Read data from socket */
/* Data is not uncompressed as it is read from socket */
/* Data read from socket is returned in string */
/* This data must then be transferred to the user with HTTP protocol */
/* This function called until length of 0 returned */
/* Length of 0 indicates end of file reached */
short get_string(char *string);

/* returns the global read buffer */

char * get_read_buffer();

/* Get return message from server and close socket */
/* Returns 0 if transaction successful */
int icom_get_close();

/* Prepare to retrieve data in its compressed form from repository */
/* Data is uncompressed as it is read from the socket */
/*This function is called after icom_get_init has been called */
/* Returns 0 if successful */
FILE * icom_get_compress_init(long long sizer);

/* Reads compressed data from the socket and uncompresses the data */
/* get_header  is always set to DBNO */
/* Data read from socket is returned in buf */
/* This data must then be transferred to the user with HTTP protocol */
/* Returns length of string retrieved */
/* Function called until length of 0 returned */
/* indicating end of file reached */
int trans_read (FILE *file, char *buf, char get_header); 

/* close connection when retrieving compressed data */
int icom_get_compress_close(FILE *file);

/************************************************************/
/* Functions to return information about data in the InterCom repository */

/* Retrieve information about specified file/directory */
/* present in InterCom repository using file keyword */
/* msg_ptr points to message returned from InterCom server */
/* Returns 0 if transaction successful */
/* Returns negative number if transaction fails */
int icom_info(char *primary_key, char *secondary_key);

/* Retrieve history of transactions on specified data */
/* History returned in msg_ptr */
/* Returns SUCCESS if transactyion successful */
/* Returns negative error code if transaction fails */
int icom_history(char *primary_key, char *secondary_key);

/* Retrieve original and compressed size of data specified with keyword */
/* keyword holds 9 character keyword */
/* ASCII string with sizes returned in msg_ptr */
/* Returns SUCCESS if transaction successful */
 int icom_size(char *primary_key, char *secondary_key);

/************************************************************/

/* Functions that modify data already present in the repository */

/* Delete specified data from InterCom repository */
/* Pass in keyword of data to delete */
/* Returns SUCCESS (0) if data deleted */
int icom_delete(char *primary_key, char *secondary_key);

/* Extend the expiration date on data 1-3 additional working days */
/* days holds number of days to extend expiration */
/* Returns SUCCESS (0) if transaction successful */
int icom_extend(char *primary_key, char *secondary_key, int days);

/* Send email to 1-6 users for data with specified keyword */
/* user_email holds one to six comma separated email addresses */
int icom_mail_keyword(char *primary_key, char *secondary_key, char *user_email, char *description);

/* Add one or more new companies access to existing data */
int icom_company_add(char *primary_key, char *secondary_key, char *company);

/* Turn on email notification for specified data */
/* Whenever a user retrieves specified data, */
/* the sending user is sent email notification */
int icom_set_file_email(char *primary_key, char *secondary_key);

/* Cancel previously set file email notification */
int icom_can_file_email(char *primary_key, char *secondary_key);

/* Lock specified data for retrieval */
/* This allows a user to prevent other users from retrieving */
/* data without deleting the data.  This transaction is */
/* useful when a user has given out a keyword and then wants */
/* to prevent the user from retrieving the data until he has */
/* verified the data is what the receiving user really needs */
int icom_lock(char *primary_key, char *secondary_key);

/* Unlock data previously locked for retrieval with the */
/* icom_lock function */
int icom_unlock(char *primary_key, char *secondary_key);

/* Set email notification on for all files user owns */
/* Whenever a file the user owns is retrieved, the user receives email */
int icom_set_user_email();

/* Cancel previously set email notification */
int icom_can_user_email();

/* Retrieve information about InterCom releases available for retrieval */
/* Returns message in command line format */
/* This will need to be changed for the Java release */
int icom_release_info();

/* Retrieve specified InterCom release */
/* Returns message describing how to retrieve release using command line */
/* format if specified release is not present */
/* This will need to be changed for the Java release */
int icom_get_release(char *release);

/* Set data type for specified data to ASCII */
/* Useful when transferring data between unix, NT and Mac machines */
 int icom_set_ascii(char *primary_key, char *secondary_key);

/* Set data type for specified data to binary */
/* Useful when transferring data between unix, NT, and Mac platforms */
int icom_set_binary(char *primary_key, char *secondary_key);

/* Retrieve information about specified user */
/* Not available for use with the web interface */
/* Information from the whois command along with information */
/* from the InterCom registry is returned */
/* Any string that identifies the user can be passed in */
/* If white space is present in the string, it is replaced with a '.' */
int icom_query(char *string);


/* Verify user is registered in the InterCom registry */
/* Returns SUCCESS if registered */
/* Returns LOCK_ERR if user's account is locked */
/* Returns REG_ERR (-520); if user not registered */
/* Returns negative number for other error conditions */
/* msg_ptr holds message returned from InterCom server */
int icom_reg_check();

/* Set repository equal to index passed in */
void icom_repository_set(int index);

void icom_repository_scan(char *line);


/* Defines for elements in status array */
/* These values can be used to access items in array returned from icom_status */
typedef enum
{
    GLOBAL_EMAIL_SET,
    FILE_EMAIL_SET,
    RETRIEVAL_LOCK_SET,
    DELETE_SET,
    DAYS_EXTENDED,
    OWN_DATA,
    TRANSFER_COMPLETE,
    END_STATUS,
} FILE_STATUS;

/* Return file status */
/* Lock status, number of days file extended, etc. returned */
/* status is returned in a dynamic array */
/* byte 0 is */
char **icom_status(char *primary_key, char *secondary_key);

char *icom_c_array_2_perl_csv_string(char **c_array);

/* Retrieve list of keywords for data user has put in the repository */
/* Can be used only for internal NXP customers */
char **icom_keys_rtv(int req_type, char *site);

/* Retrieve list of secure keywords for current user */
/* Can be used only for internal NXP customers */
char **icom_secure_keys_rtv(char *site);

char **icom_user_keys_rtv(char *site);

/* Delete all user data present in the data repositories */
int user_keys_delete(char *site);


/* Functions used for JAVA GUI, currently not being used */
/* These functions are not used with the web interface */

/* Transfer file or directory to InterCom repository */
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
/* file type is a new feature for 3.0 of InterCom */
/* msg_ptr points to message returned from InterCom server */
/* Returns 0 if transaction successful */
/* Returns negative number if transaction fails */
int icom_put(char *file_name, char *user_array[], char *description, 
    TRANS_FILE_TYPE file_type, char **keyword_ptr);

/* Perform put transaction on a directory */
/* Symbolic links are included */
int icom_put_links(char *file_name, char *user_array[], char *description, 
    TRANS_FILE_TYPE file_type, char **keyword_ptr);

/* Perform put transaction and specify deletion after first retrieval */
int icom_put_delete(char *file_name, char *user_array[], char *description, 
    TRANS_FILE_TYPE file_type, char **keyword_ptr);

/* Perform put transaction on a directory */
/* Symbolic links are included */
/* and the data is deleted after the first retrieval */
int icom_put_delete_links(char *file_name, char *user_array[], char *description, 
    TRANS_FILE_TYPE file_type, char **keyword_ptr);

/* Retrieve a file or directory from the InterCom repository */
/* Path points to path where file will be placed */
/* Absolute or relative paths can be used */
/* Pass in "." if want to use current working directory */
/* file_name points to name of file retrieved */
/* Memory is allocated by icom_get */
/* Calling function is responsible for freeing this memory */
/* msg_ptr points to message returned from InterCom server */
/* Returns 0 if transaction is successful */
/* Returns negative number if transaction fails */
int icom_get(char *primary_key, char *secondary_key, char *user_path, TRANS_FILE_TYPE file_type);


/* Retrieve a file or directory from InterCom repository */
/* and delete data from repository after is is successfully retrieved */
/* msg_ptr points to message returned from InterCom server */
/* Returns 0 if transaction is successful */
/* Returns negative number if transaction fails */
int icom_get_delete(char *primary_key, char *secondary_key, char *user_path, TRANS_FILE_TYPE file_type);

/* Report functions */

/* req_type holds the request type, summary( 69) or usage (51) */
/* report_fields holds a comma separated list of fields to put in the report */
/* start_time holds the begin time in the form mm/dd/yy */
/* end_time holds the end time in the form mm/dd/yy */
/* Two keep and discard filters along with the value to filter */

     int icom_report_gen(int req_type,  char *company_line,
char *report_fields, char *start_time, char *end_time,
char *filter_string, char *file_name);

/* Functions used to generate reports */

/* Generate summary report using criteria specified by the user */
/* Fields are passed in as a comma separated list of the field numbers */
/* filters contain field number, = or !, and field value */
/* List of filters are separated by commas */
/* Dates have the form mm/dd/yy */
/* file_name holds name for local file set by calling function */
int icom_summary_report(char *company_line, char *fields,  char *time1, char *time2, 
char *filter_string, char *file_name);

/* Generate usage report using criteria specified by the user */
/* Fields are passed in as a comma separated list of the field numbers */
/* filters contain field number, = or !, and field value */
/* List of filters are separated by commas */
/* Dates have the form mm/dd/yy */
/* file_name holds name for local file set by calling function */
int icom_usage_report(char *company_line, char *fields,  char *time1, char *time2, 
char *filter_string, char *file_name);

/* Test function present in library */
long long icom_put_send(int fd);

/* Read compressed data from the socket */
/* The data is uncompressed as it is read */
/* file is the pointer returned from icom_get_compress_init */
/* buffer is the character array where data is placed */
/* Returns length of buffer read */
int icom_read (FILE *file_ptr, char *perl_buffer);


/* Read data from the socket and write it to standard out */
int icom_data_get(char *file_name, int local_compression, int remote_compression, long long size);

/* Get uncompressed file */
int icom_get_uncompressed();

FILE *icom_open(char *mode, long long size, int hash_enable, TRANS_FILE_TYPE file_type);


/* Write data to the socket */
/* Data is compressed as it is written to the socket */
/* file is the pointer returned from icom_put_compress_init */
/* buffer holds the data to be written to the socket */
/* len is the length of the data */
long long icom_write(FILE *file_ptr, char *buffer, unsigned len);


/* Delete all user keys present in the repository */
int icom_user_keys_delete(char *site);

/* Delete all secure keys present in the repository */
int icom_secure_keys_delete(char *site);

/* Generate report using criteria specified by the user */
/* Fields are passed in as a comma separated list of the field numbers */
/* filters contain field number, = or !, and field value */
/* List of filters are separated by commas */
/* Dates have the form mm/dd/yy */
/* file_name holds name for local file */
int icom_summary_report(char *company_line, char *fields, char *time1, char *time2, 
char *filter_string, char *file_name);

int icom_usage_report(char *fields, char *company_line,  char *time1, char *time2, 
char *filter_string, char *file_name);

/* req_type holds the request type, summary( 69); or usage (51) */
/* report_fields holds a comma separated list of fields to put in the report */
/* start_time holds the begin time in the form mm/dd/yy */
/* end_time holds the end time in the form mm/dd/yy */

     int icom_report_gen(int req_type,  char *company_line,
char *report_fields, char *start_time, char *end_time,
char *filter_string, char *file_name);

/* Currently not being used */
/* Set authorization for multiple companies */
int icom_multiple_companies_set(char *primary_key, char *secondary_key, char *user_email, char *description);

FILE *icom_open ( char *mode, long long hash_size, int hash_enable, TRANS_FILE_TYPE file_type); 
long long icom_close (FILE *file, char file_type); 

/* Use this function so the global file size will be updated */
long long icom_put_string(char *string, short len);


char * get_global_msg();

#endif
