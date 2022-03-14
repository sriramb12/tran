#ifndef refresh_h
#define refresh_h

/* Refresh 1-5 database directories */
void refresh_data(int start_index, int end_index, int req_type, char *dir[], char *files[5][TOTAL_FILES]);

/* Refresh a remote mask set or template */
void rmt_refresh_data(void);

void rmt_create_data(void);
    
void rmt_mark_data(void);

/* Send refresh request to remote server */
/* No data is sent with this request */
void refresh_request(int req_type);

/* Refresh data done after header already sent */
void refresh(char *dir[], char *files[5][TOTAL_FILES]);


void bld_header(
    char *buffer,
     int req_type,
    int data_type,
    int transaction_num,
    int pre_state);

#endif
