#ifndef transweb_lib_h
#define transweb_lib_h

#define DBYES 1
#define DBNO 2

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

/* Defines for Transcend file types */
typedef enum
{
    ASCII = 'a',
    BINARY = 'b',
    MAC_BINARY = 'c',
    DIRECTORY= 'd',
    UNSPECIFIED = 'e',
    FILES = 'f',
    UNKNOWN = 'g',
    HUGE_DIR = 'h',
    HUGE_FILE = 'i',
    HUGE_FILES = 'j',
    UNCOMPRESSED = 'k'
} TRANS_FILE_TYPE;

/* All functions which return an int return 0 for success & negative number for failure */
/* Messages from the InterCom server returned in msg_ptr */

/* Initialize data InterCom client must send to the server */
/* this function must be called before other InterCom functions are called */
/* It is called after external LDAP authentication is done */
/* External LDAP authentication not yet defined */
void defaults_init(char *core_id, char *name);

/* Data may be written to the repository as a compressed or noncompressed file */
/* Files compressed during transfer are uncompressed during retrieval */

/* Put file in the repository compressing data as it is transferred */
/* When this function is called, the file name, company, and description have been parsed and are */
/* passed into the function */
/* For external data transfer, the company name is set to NXP Semiconductor */
/* remote_email holds one to six space separated email addresses */
/* These email addresses are used to send recipients secure keywords they can use to retrieve the data */
/* delete flag holds DBYES (1); for delete after first retrieval */
/* and DBNO (2); for no deletion after first retrieval */
/* Returns stream pointer to be used to write data to repository */
/* NULL return indicates initialization failed */
FILE * icom_put_compress_init(char *file_name, char *company, char *description,
char *remote_email, int delete,   char **msg_ptr);

/* Writes compressed data to the open socket */
/* This function is called each time a block of data is read from the file on the user's disk */
/* Data is read in 4096 byte blocks */
int trans_write (FILE *file, char *buf, unsigned len); 

/* close connection when sending compressed data */
int icom_put_compress_close(FILE *file, char **msg_ptr);

/* Initialize connection when transferring noncompressed data to repository */
int icom_put_init(char *file_name, char *company_line, char *description,
char *remote_email, int delete, int local_compress,   char **msg_ptr);

/* Write uncompressed data to the repository */
int put_binary_string(char *buffer);

/* Close connection when sending uncompressed data */
int icom_put_close(long long size, char * *msg_ptr);

/* Function to retrieve data from the repository */
/* Called to set up connection to retrieve data from the repository */
int icom_get_init(char *keyword, int delete, char **msg_ptr);

/* Send short to InterCom server */
/* Used to tell server ready to proceed with data retrieval */
int put_short(void);

/* Retrieve string of data from the InterCom server */
/* Used to retrieve information about file being retrieved */
/* Also used to retrieved uncompressed data from the repository */
/* String of data returned in buffer */
int get_string(char *buffer);

/* If compressed data being retrieved, additional set up is needed */
/* This function is called to do additional set up */
/* Returns a stream pointer that can be used to retrieve data from the repository */
FILE * icom_get_compress_init(char **msg_ptr);

/* Reads compressed data from the socket */
/* Returns data in buffer as uncompressed data */
/* get_header indicates if Mac header should be read */
/* This is always set to DBNO */
/* This is for compatibility with earlier versions that supported Mac */
int trans_read (FILE *file, char *buf, char get_header); 

/* close connection when retrieving compressed data */
int icom_get_compress_close(FILE *file, char **msg_ptr);

/* Get return message from server and close socket */
/* Used to close connection when retrieving both compressed & noncompressed data */
int icom_get_close(char * *msg_ptr);

/************************************************************/
/* Functions to return information about data in the InterCom repository */

/* Retrieve information about specified file/directory */
/* present in InterCom repository using file keyword */
/* msg_ptr points to message returned from InterCom server */
/* Returns 0 if transaction successful */
/* Returns negative number if transaction fails */
int icom_info(char *keyword, char **msg_ptr);

/* Retrieve history of transactions on specified data */
int icom_history(char *keyword, char **msg_ptr);

/* Retrieve original and compressed size of data specified with keyword */
 int icom_size(char *keyword, char **msg_ptr);

/* Return file status */
/* Lock status, number of days file extended, etc. returned */
/* status is returned in a dynamic array */
/* byte 0 is */
char **icom_status(char *keyword, char **msg_ptr);


/************************************************************/

/* Functions that modify data already present in the repository */

/* Delete specified data from InterCom repository */
/* Pass in keyword of data to delete */
int icom_delete(char *keyword, char **msg_ptr);

/* Extend the expiration date on data 1-3 additional working days */
int icom_extend(char *keyword, int days, char **msg_ptr);

/* Send email to 1-6 users for data with specified keyword */
/* user_email holds one to six comma separated email addresses */
int icom_mail_keyword(char *keyword, char *user_email, char *description,  char **msg_ptr);

/* Turn on email notification for specified data */
/* Whenever a user retrieves specified data, */
/* the sending user is sent email notification */
int icom_set_file_email(char *keyword, char **msg_ptr);

/* Cancel previously set file email notification */
int icom_can_file_email(char *keyword, char **msg_ptr);

/* Lock specified data for retrieval */
/* This allows a user to prevent other users from retrieving */
/* data without deleting the data.  This transaction is */
/* useful when a user has given out a keyword and then wants */
/* to prevent the user from retrieving the data until he has */
/* verified the data is what the receiving user really needs */
int icom_lock(char *keyword, char **msg_ptr);

/* Unlock data previously locked for retrieval with the */
/* icom_lock function */
int icom_unlock(char *keyword, char **msg_ptr);

/* Set email notification on for all files user owns */
/* Whenever a file the user owns is retrieved, the user receives email */
int icom_set_user_email(char **msg_ptr);

/* Cancel previously set email notification */
int icom_can_user_email(char **msg_ptr);


#endif
