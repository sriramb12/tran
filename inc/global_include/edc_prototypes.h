#ifndef edc_prototypes_h
#define edc_prototypes_h

void edc_sites_build(char *final_sites, char *new_sites, char *old_sites);

/* Builds name for EDC registry file */
void edc_name_build(char *file_name, char *user_id, int edc_type);

/* Builds a new entry when existing entry being modified */
void new_entry_build(char *query, char *final_entry, char *new_entry, char *old_entry);

/* See if the domain name is correct */
int edc_domain_lookup(char *entry, char *user_site, int save_email);

/* If entry not present, 3 things that can change are the status, expiration time, and sites */
int edc_entry_add(char *login, int status, unsigned long expirationtime, char *email1, char *email2, char *name, char *sites, int edc_type);

int edc_transweb_entry_add(char *login, int status, unsigned long expirationtime, char *email1, char *email2, char *name, char *sites, int edc_type);

int edc_transweb_entry_lookup(char *login, char *site, int edc_type);

int edc_entry_lookup(char *login, char *user_domain, int edc_type, int save_email);

/* Verify user is registered to use Transcend, Intercom, or TransWeb  */
int edc_reg_check(int edc_type);

int edc_reg_auth_user(char *email, unsigned long expirationtime, int new_status);

int edc_white_list_entry_add(
    char *domain,
    int status,
    unsigned long expirationtime);

/* Check the approved status of an external customer */
/* This function is called as the secure keywords are being generated */
int edc_auth_check(char *buffer, char *email_addr, char *keyword, int trusted_sender);

/* Verify the external customers are authorized to retrieve data */
int edc_pre_auth_check(char *email_addr[], int edc_type);

/* Approve specified user */
void edc_auth_user(char *email, char *req_type, char *expirationtime_string);

/* Authorize a domain */
void edc_auth_domain(char *email, char *req_type, char *expirationtime_string);

/* Retrieve pending authorization requests */
void edc_auth_rtv(void);

void edc_commandline_rtv(void);

/* Release keywords pending the approval */
void edc_keywords_release(char *email);

/* Process keyword for deletion block */
void edc_keyword_process(char *keyword, char *email);

/* Make needed log entries when approval is needed */
void edc_entries_log(char *keyword, char *email);

/* Perform user registration */
/* Register the user */
int edc_reg_user(
    char *login,
    char *domain,
    char *user_email1,	/* User's primary Email address */
    char *user_email2,	/* User's secondary Email address */
    char *user_name1,
    char *user_name2,
    char *user_name3,
    char *user_pwd,
    int edc_type);

int autoreg_user(int edc_type);

/* Submit an approval request */
void edc_commandline_approve(char *domain, char *justification, char *requestor, char *approver, time_t expiration_time);

/* This function is called by Transcend when a user uses Transcend to keep track of usage */
int edc_reg_timestamp_update(char *login, char *sites, int edc_type);

/* See if customer's domain is approved */
int domain_table_check(char *email_addr);

/* Request EDC approval */
void edc_request_approval(char *email);

int edc_status_calc(char *user_login, char *user_site, int edc_type, int save_email);

int edc_file_index_read(char *keyword);

int edc_registry_entries_count(char *path);

#endif
