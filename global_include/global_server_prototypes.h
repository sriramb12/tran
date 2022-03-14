#ifndef global_server_prototypes_h
#define global_server_prototypes_h

#include <stdio.h>

#define EQUAL '='
#define NOT_EQUAL '!'

/* Following structure used when creating file names within date range */
typedef struct
{
    int day;
    int month;
    int year;
} FILE_DATE;

int line_get(FILE *fp, char *line);

int long_line_get(FILE *fp, char *line);

/* Logs current transaction */
/*
int log(
     char *log_file,
       char *string);
*/

int data_log(
     char *log_file,	/* Holds name of log file where string will be put */
       char *string);

void log3(
    char *str1, char *str2, char *str3);

/* Build the log file name */
void bld_log_name(void);

/* The following allows me to use month '01' instead of 'Jan' */
void log_convert(char *log_date, char *time_stamp);

/* Encrypt client/server password */
/* Encryption is symetrical */
void password_encrypt(char *new_password, char *old_password, int len);

/* Build the user's site */
void get_site(char *ip_addr, char *site);

/* Send email using SMTP */
void email_smtp_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *body);

/* Send email using sendmail */
void email_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *body);

void email_file_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *file);

void email_msg_file_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *msg,
    char *file);

/* Sends error message to user and system administrator */
/* Also exits current transaction */
void email_send_user(char *user_email, char *subject, char *body, int error);

void jdc_sendmail(
    char *sender,
    char *replyto,
    char *recipients,
    char *subject,
    char *body);

/* Build a time stamp consisting of date and time */
/* Time stamp returned in 'time_stamp' variable */
void time_stamp_build(char *time_stamp);

/* Build names of all files that fall within date range */
int names_build(char *date1, char *date2, char *files[]);

int date_calc(FILE_DATE *d, char *date);

/* Scan through the file offset number of lines and log new message after offset */
int data_log_offset(char *log_file, char *string, int offset);

/* Log error for put short or put string */
void log_err(int ret, char *calling_functions);
void shut_down(char *msg, short ret_code);

void clean_up(char *msg);
void lock_files_init(void);

/* Lock specified file */
/* Allocate space for the lock file name and increment index */
int lock_file_set(char *file_name);

/* Unlock file and free memory */
void lock_file_clear(int index);

/* Unlock all files locked and free memory */
/* Called when exiting with error condition */
void lock_files_clear(void);

/* Wait to lock file for specified number of seconds */
int lock_file_wait(char *file_name, int wait_time);

/* Initialize socket on start-up */
int init_socket(int socket);

void startup_daemon(void);

int ip_addr_copy(char *string, char *address);

int ip_addr_lookup(
    char *string,
    char *address,
    char *name);

/* Looks up IP node address using gethostbyname */
/* Used to get IP node address of client machine */
/* If not found, uses default address */
void ip_addr_find(char *addr, char *name);

int add_entry(char *file_name, char *entry);
int dlt_entry(char *file_name, char *entry);

/* convert month i.e. Jan goes to 01 */
void month_convert(char *log_date, char *time_stamp);


/* Get a list of file list */
int list_put(char *files[]);

/* Send list of list to client */
int list_get(char *files[]);

/* Sends list but does not free list members */
/* Returns number of items sent */
int static_list_get(char *files[]);

/* Read list of list from specified file */
/* Returns number of elements found */
int list_read(char *file_name, char *list[]);

/* Write list of list to specified file */
int list_write(char *file_name, char *list[]);

/* Add an entry to the list */
/* Names are kept in alphabetical order */
/* Returns 1 if entry added */
/* Returns 0 if current entry replaced */
/* Returns SUCCESS if name added */
int m2list_add(char *list[], char *entry, char *key, int total_cnt);

/* Delete name from  list */
/* Returns SUCCESS if name deleted */
/* Returns FAILURE if name not present */
int m2list_delete(char *list[],  char *key, int total_cnt);

/* Verify specified name is in the list array */
int list_find(char *list[], char *key);

/* Look for element in list using binary search */
/* Returns SUCCESS if element found */
int list_binary_search(char *list[], char *name, int num);

/* Add the user's name before adding to the list */
int userlist_add(char *list[], char *key, int total_cnt);


/* Expand a technology list */
int tech_list_expand(char *dir, char *old_files[], char *new_files[]);

/* Convert list from string format to list format */
int string2list_convert(char *string, char *list[]);

/* convert list to string */
void list2string_convert(char *list[], char *string);

/* Put items on list in reverse order */
void m2list_reverse (char ***new_list, char **old_list, int num);

/* Compare elements in two lists */
/* Returns FAILURE if number of elements in two lists different */
/* Also returns FAILURE  if any two elements not the same */
int list_compare(char *list1[], char *list2[]);

/*Free members of list and set first element to NULL */
void m2list_free(char *list[]);

/* Generate usage and summary reports */
void report_usage(int req_type, char *time1, char *time2, char *fields);

/* Decrypt/encrypt user password */
int password_crypt(char *new_pwd, char *old_pwd);

/* SEnd log for previous day for transactions  for Intercom, Transcend, MARS, and Event_log */
void mettracks_log(char *log_prefix);

int entry_find(char *file_name, char *entry);

/* Free list of files */
void files_free(char *files[]);

int remote_email_send(void);

int add_line(char *file_name, char *entry);

#endif
