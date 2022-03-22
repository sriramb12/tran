#ifndef usr_h
#define usr_h

#define USR_INPUT "%s %s %s %s %s %s %s %s %s"
#define USR_OUTPUT "%s %s %s %s %s %s %s %s\n"
#define USR_FILE "registry/users"

/* Add new user to the user file */
int usr_add(char *email, char *dept_name, char *mbg);

/* Look up all information about RET user */
int usr_lookup(OPC_USER *usr, char *id, int mod_user);


/* Fill in user structure with defaults */
void usr_default(OPC_USER *usr, char *id);

int usr_entry_add(char *new_email, char *new_entry);
void usr_entry_mod(char *user_id, char *paging_num, char *status);
void usr_list(void);
void usr_entry_mod_build(OPC_USER *usr, char *user_id, char *paging_num, char *status);


/* Return user's paging number */
void usr_paging_get(char *user_id);

void usr_paging_set(char *user_id, char *paging_num);
void usr_paging_set(char *user_id, char *status);

#endif
