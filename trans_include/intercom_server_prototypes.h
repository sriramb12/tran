#ifndef intercom_server_prototypes_h
#define intercom_server_prototypes_h


int check_reg(void);

int autoreg_user();

void init_company_params();


#ifdef MULTIPLE_SECNETS
int company_lookup(char *company, char *machine, char *address, int *socket);
#endif /* Define for MULTIPLE_SECNETS */

int find_email(
    char *user_site,	/* site of user looking for */
    char *email_addr,	/* Email address returned to calling function */
    char address_type);	/* Indicates official or unofficial Email address */

void get_big_file(char *key, int ret);

void get_release(char *key);

int get_ret_code();

void history_big_file(void);
void report_big_file(void);
extern int icom_sock;

/* Used by the data base server and mask shop server */
short get_icom_string(char *buffer);

short get_icom_short();

short get_timed_icom_short(void);

void put_icom_char(char data);

short put_icom_short(short data);

/* Used to send data to the client */
int put_icom_string(char *string);

void put_icom_long(long data);

void put_icom_longlong(long long data);

/* Used to send data to the client */

short put_icom_binary_string(char *string, int len);
/* Concatinates two strings and sends to client */
/* All messages sent via put_icom_string2 are logged */
void put_icom_string2(char *string1, char *string2);

/* Concatinates three strings and sends to client */
/* All messages sent via put_icom_string3 are logged */
short put_icom_string3(char *string1, char *string2, char *string3);

long get_icom_long();

void info_big_file(char *key);

/* Connect to the remote socket */
/* If the connect is unsuccessful */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
short init_icom_socket();


void send_secure_keyword(char *key);


char *keyword_process_mail(char *mail_ptr, char *description);



void process_request( int socket);

void put_big_file();

/* Send a group of files to the client */
void put_file(char *file_name, char *root_file);



void put_msg(char *key, char *log_string);

void put_mail_msg(char *key, char *log_string, char *mail_ptr);



void put_success_msg();

void put_local_msg();

void return_success(char *msg, int log);

void rcv_file(char *remote_file);


/* Holds login and Email for database administrator */

void read_global_params(char *file_name);

#ifdef MULTIPLE_SECNETS
void company_file_read();
#endif

void reg_query();

/* Send one file to the client */
void send_file();

void send_rtv_email(char *msg);



/* Sends last message, closes socket and cleans up temporary files */
void shut_down(
    char *msg,		/* Points to message sent to user */
    short rtn_code);	/* Holds return code sent to user */
    /* If data does not exist, give special warning to user */
/* Initialize the socket number based on program type */
void db_socket_init(void);


/* Send list to keys to client */
int user_keys_pass(int cnt);

int list_pass(void);

void process_autoreg(void);
void put_remote_msg(void);

void partial_list_pass(void);

void icom_files_get(void);

void email_notification_send(void);

#endif
