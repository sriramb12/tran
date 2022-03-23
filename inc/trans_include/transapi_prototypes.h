#ifndef transapi_prototypes_h
#define transapi_prototypes_h

#include <time.h>
#include <zlib.h>
#include "global_defines.h"
#include "rtn.h"
#include "trans_defines.h"
#include "transapi_defines.h"

/* The following functions can be called by software using the API */

/* ----------------------------------- General functions */

/* Get a pointer to a transaction structure and set default values */
/* This function is called before any actions or options are set */
int trans_init(
    /* Set to DBYES (1) to overwrite existing file during a download */
    int overwrite_files,
    /* Set to DBYES to perform transfer in background so status can be monitored */
    int perform_background_transfer,
    /* Suppress prompting for information if set to DBYES (1) */
    int suppress_prompt_output,
    /* Suppress transfer status display  to standard out if set to DBYES (1) */
    int suppress_transfer_status);

/* Initialize user options i.e. login machine, etc. */
/* this function is called after trans_init  */
/* and before other functions are called */
/* This function is called only once per session */
void trans_user_params_init(char *core_id);

/* Retrieve the return message */
/* This provides information for the transaction just performed */
/* this message is always set when an error occurs */
/* It is also always set after trans_perform is called */
/* It is not set for other successful function calls */
char *trans_rtn_msg_rtv(void);

/* Set the tool type */
int trans_tool_set(int trans_index, int tool_type);

/* Perform pending actions */
short trans_perform(int trans_index);

/* Reset default values for a transaction structure */
/* This function is called after a transaction is performed */
/* It is used to clear the structure before next transaction */
void trans_clear(int trans_index);

/* Deallocate the TRANS structure */
/* This function is called after all transactions have been performed */
void trans_cleanup(int trans_index);

/* ---------------------------------- Put request functions */

/* Set a put request */
/* This function is called before other Put functions listed below */
/* Supported transfer types are defined in transapi_defines.h */
/* They include UNSPECIFIED_TRANSFER, SERIAL_TRANSFER, PARALLEL_TRANSFER,  */
/* SEPARATE_BLOCK_TRANSFER, ZIP_DURING_TRANSFER, BUNDLE_WITH_TAR, */
/* this simply stores the requested type, but does not guarantee it will be used */
int trans_put_request_set(int trans_index, REQ_TYPE req_type, char transfer_type);

/* Following functions can be called after trans_put_request_set is called */
/* For put requests, can set  a file name, list of files, file descriptor or file stream */
/* Initialize file name for data being transferred */
/* Verifies file is present */
/* Store full file name in TRANS structure in field file_name */
/* Stores root file name in TRANS structure in keyword field */
/* also stores file mode and file type */
int trans_file_name_set(int trans_index, char *file_name);

/* Initialize a list of files */
/* Verifies all files are present */
/* Size for the files is also calculated */
int trans_files_set(int trans_index, char **files);

/* Build list of files stored in a file */
/* Verifies all files are present */
/* Size for the files is also calculated */
int trans_named_files_set(int trans_index, char *file_name);

/* Initialize a file descriptor for an already opened file */
int trans_file_descriptor_set(int trans_index, int fd);

/* Initialize a file stream for an already opened file */
int trans_file_stream_set(int trans_index, FILE *fp);

/* Set description sent to receivers when secure keywords are generated */
int trans_description_set(int trans_index, char *description);

/* Retrieve size of data being transferred */
unsigned long long trans_total_size_rtv(int trans_index);

/* Retrieve amount of data transferred */
unsigned long long trans_transfer_size_rtv(int trans_index);

/* Return status of transfer */
int trans_transfer_status_rtv(int trans_index);

/* Retrieve the Transcend keyword for a put request */
char *trans_keyword_rtv(int trans_index);

/* ----------------------------------- Get request functions */

/* Set a get request */
int trans_get_request_set(int trans_index, REQ_TYPE req_type,
    char transfer_type, char *keyword);

/* Retrieve file with alternate file name */
int trans_alternate_file_name_set(int trans_index, char *file_name);

/* Set directory where file retrieved */
int trans_alternate_dir_set(int trans_index, char *dir_name);

/* ----------------------------------- Keyword request functions */


/* Set the request type and keyword for a keyword request */
/* These are for requests requiring a 9 character keyword */
/* DBHIST_DATA  Retrieve history of transactions for keyword */
/* BSEND_KEYWORD  Send secure keyword */
/* TRANSDIRLIST_RTV  Retrieve directory list for data and put in return message */
/* TRANSDIRLIST_FILE_RTV  Retrieve directory list for data and write it to a file */
/*  Supported actions include:  */
/* DBDLT_DATA:  Delete file with specified keyword */
/* DBEXTEND_DATA:  Extend file expiration for specified keyword */
/* DBINFO_DATA:  Retrieve metadata for keyword */
/* DBSIZE_CHECK:  Retrieve original and stored size for data */
/* DBLOCK_FILE:  Lock the data for retrieval */
/* DBUNLOCK_FILE:  Unlock data for retrieval */
/* DBSET_FILE_EMAIL:  Set e-mail notification for specified keyword */
/* DBCAN_FILE_EMAIL:  Cancel e-mail notification for specified keyword */
/* DBOWNER_DLT - Allow only owner to delete data */
/* DBUSR_DLT - Allow anyone with keyword  to delete the data */
/* DBFTP_RESEND - Resend encrypted data to foundry */
/* DBFILENAME_RTV - Retrieve file name for keyword, used before retrieving data */
/* DBSTATUS_DATA - Retrieve status of keyword */
int trans_keyword_request_set(int trans_index, REQ_TYPE req_type, char *keyword);

/* ------------------------- Simple request functions */

int  trans_simple_file_get(int trans_index, REQ_TYPE req_type, char *keyword);

/* ----------------------------------- Custom request functions */

/* Set the action and request type for a custom request */
/* These transactions are for requests which do not use a 9 character keyword */
/* Valid actions include */
/* DBFAST_QUERY_USER  Show LDAP info about user */
/* DBQUERY_USER  Show LDAP information and registration for core ID */
/* DBGET_RELEASE  Retrieve specified release */
/* SHARED_MEMORY_CLEANUP  Clean up shared memory */
/* DBAUTO_REG  Perform auto registration */
/* DBUSR_KEYS  Retrieve list of file keywords for user */
/* GUI_COMPANY_TABLE_RTV  Retrieve company list (used only in the GUI) */
int gui_company_table_rtv(int index);
/* DBSET_USER_EMAIL - Set e-mail notification for all keywords */
/* DBCAN_USER_EMAIL - Cancel e-mail notification for all keywords */
/* DBX500LOOKUP - Look up user name using LDAP  */
/* DBX500AUTHEN - Authenticate user */
/* DBSITE_CHECK - Verify registration at current site */
/* DBCRON_DLT - Delete expired files once an hour (admin request) */
/* DBREG_URL  - Display URL for external LDAP registration */
/* DBAUTO_REG - Perform auto registration */
/* DBREG_QUERY - Verify user is registered */
/* DBPING - Ping server to verify status */
int trans_custom_request_set(int trans_index, REQ_TYPE req_type, char *key);

/* Set specified option */
/* Options defined in the OPTION_TYPE structure */
/* Options remain set until they are clear */
/* or the TRANS structure is deallocated */
/* NUM_DAYS Number of days to extend file expiration */
/* FOLLOW_LINKS Follow links during upload */
/* DESCRIPTION Description of data for upload or sending of secure keyword */
/* EMAIL_LIST List of email addresses to send secure keywords to */
int trans_option_set(int trans_index, OPTION_TYPE option, char *value);

/* Send list of strings to the server */
int trans_list_send(int trans_index, char *list[]);

short trans_ret_code_get(void);

/* Used to get extra large return code */
short trans_big_ret_code_get(void);

int trans_block_retrieve(gzFile file, FILE *fp, TRANS_FILE_TYPE file_type,
    char *file_name, char get_block, int index);

#endif /* for transapi_prototypes_h */
