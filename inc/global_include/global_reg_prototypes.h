#ifndef global_reg_prototypes_h
#define global_reg_prototypes_h

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "reg_globals.h"
#include "rmt_server.h"

int add_email_entry(char *entry);	/* Name of entry to add */

/* Adds entry to any file */
int add_file_entry(
    char *path,
    char *file_name,	/* Complete path name of file entry being added to */
    char *entry);	/* Name of entry to be added */

int add_file_line(
    char *file_name,	/* Complete path name of file entry being added to */
    char *entry);	/* Name of entry to be added */
int add_site_entry(
    char *new_site,
    char *new_email1,
    char *new_email2);

/* Signal handler for timeouts (SIGALRM);.
 * This function is called when the alarm(void); value that was set
 * counts down to zero. This indicates that we haven't received a
 * response. The function sets timeout_flag to indicate a timeout has occurred.
 * The signal interrupts the read system call which sets errno = EINTR.
 */
void timeout( int rcv_signal );

int read_with_timeout( int fd,  char *buf, int num_bytes );



int bsearch_registry(
    char *user_email1,
    char *user_email2,
    char *registry,
    REG_PARAMS *reg);	/* Holds user's name and Email address if found */

int compare_names(
    char *servername1,
    char *servername2,
    char *username1,
    char *username2,
    char *username3);

int compare_sites(
    char *site,	/* User's new site */
    char *email,	/* Address given by user */
    REG_PARAMS *reg);	/* Holds user's email if match found */

int breakup_site(
    char *site,
    char *login,
    char *octet1,
    char *octet2,
    char *octet3);

int dlt_file_entry(
    char *file_path,	/* Complete path name of file entry being deleted from */
    char *entry);	/* Name of entry to be deleted */

int dlt_file_line(
    char *file_name,	/* Complete path name of file entry being deleted from */
    char *entry);


int dlt_site_entry(char *new_site);

int find_email(
    char *user_site,	/* site of user looking for */
    char *email_addr,	/* Email address returned to calling function */
    char address_type);	/* Indicates official or unofficial Email address */

int find_reg_entry(
    char *user_email1,
    char *user_email2,
    char *user_name1,
    char *user_name2,
    char *user_name3,
    REG_PARAMS *reg);	/* Holds user's name and Email address if found */

int new_reg(
    char *site,	/* Needed only to add entry to site table */
    char *user_email1,	/* User's primary Email address */
    char *user_email2,	/* User's secondary Email address */
    char *user_name1,
    char *user_name2,
    char *user_name3,
    char *user_pwd,
    REG_PARAMS *reg);	/* Holds user's name and Email address if found */

/* Connect to the remote socket */
/* If the connect is unsuccessful */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
short init_rmt_socket(
    char *header_buffer,
    RMT_SERVERS *rmt,
    int indx);		/* Index into the server table */

int lookup_reg_user(
    char *new_email,
    char *email_addr,	/* Email address of registered user */
    char *append);	/* Indicates if contact info is needed */

int reg_lookup(void);

void rmt_trans_reg(char *login, char *site, char *email1, char *email2,
    char *name, unsigned long expiration_time);
void rmt_server_reg(RMT_SERVERS *rmt_trans, char *site, char *email1, char *email2);

void rmt_transweb_reg(RMT_SERVERS *rmt_trans, char *site, char *email1, char *email2);

#ifndef OPCTRANS
#ifdef PHOTRONICS
/* Sends registration entry only to remote Intercom servers */
void rmt_dmi_reg(char *site, char *email1, char *email2);
#endif
#endif /* For not OPCtrans */


/* Build name for registry file */
void reg_name_build(char *file_name, char *user_id);

void
rmt_server_read(void);

/* Find index for specified server site */
int
rmt_server_find(char *site);


short get_rmt_short(void);

void get_timed_short(char *buffer);


short put_rmt_short(short data);

short get_rmt_string(char *buffer);

/* Used to send data to the remote server */
short put_rmt_string(char *string);

short put_rmt_binary_string(
  char *string,
    register int len);


void resend_reg(void);

int strcmp1err(
    char *name1,
    char *name2);

void strip_email_trailors(char *entry);

void rmt_force_reg(
    char *key1,
    char *key2,
    char *key3,
    char *dpt,
    char *par1, 
    char *par2,
    char *par3,
    char *par4,
    char *par5);

void rmt_man_reg(char req_type, char *site, char *email1, char *email2);



/*
 * Host name of LDAP server
 */

/*
 * Port number where LDAP server is running
 */


/*
 * Subtree to search
 */

/*
 * These variables must reside independant of the runnin of the routines.
 * attrs is an array of pointers to the attribute names to search for.
 * value is the value for each attribute.
 */

int x500vfypwd(
    char *user_email,
    char *user_pwd,
    char *user_email1,
    char *user_email2,
    int use_secondary_email);

int strcmp1err(

    char *name1,
    char *name2);

void strip_email_trailors(char *entry);

void strdlt_quotes(char *string);

int bsearch_registry(
    char *user_email1,
    char *user_email2,
    char *registry,
    REG_PARAMS *reg);	/* Holds user's name and Email address if found */


/* Connect to the remote socket */
/* If the connect is unsuccessful */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
short init_rmt_socket(
    char *header_buffer,
    RMT_SERVERS *rmt,
    int indx);		/* Index into the server table */

int test_authenticate(char *user_id, char *password, char *first_name, char *last_name);

int fss_intranet_authenticate(char *user_id, char *password, char *first_name, char *last_name);

/* Called from Intercom server running on SecNet */
/* Used to authenticate FSS customer with LDAP */
/* Uses SSL authentication */
int extranet_authenticate(char *user_id, char *password, char *email, char *name);

/* Used to authenticate FSS customer with LDAP */
/* Uses SSL authentication */
int fss_authenticate(char *user_id, char *password, char *host, int port,
    char *searchbase, char *service_dn, char *service_password, char *first_name, char *last_name);



/* Authenticate with NXP test server */
int company_test_authenticate(char *user_id, char *password, char *corporate_email, char *friendly_email, char *name);

/* Authenticate NXP user with internal LDAP */
int company_authenticate(char *user_id, char *password, char *corporate_email, char *friendly_email, char *name);

/* Authenticate user with internal LDAP */
int intranet_authenticate(char *user_id, char *password, char *host, int port,
    char *searchbase, char *corporate_email, char *friendly_email, char *name,
    char *service_dn, char *service_pwd);

/*void ldap_name_get(LDAP *ld, LDAPMessage *user_result, char *name);

void ldap_company_get(LDAP *ld, LDAPMessage *user_result, char *company);*/

int extranet_name_lookup(char *user_id, char *name);

#endif
