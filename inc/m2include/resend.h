#ifndef resend_h
#define resend_h


/* Read entry from resend file when trying to resend the data */
int resend_file_read(char *server_site);

int resend_entry_dlt(char *server_site, char *entry);

int resend_entry_add(char *server_site, int req_type);

/* Sends data from one resend entry to the remote server */
void resend_entry_send(int server_index);

/* Initialize paths based on data type */
void resend_paths_init(int data_type);



#endif
