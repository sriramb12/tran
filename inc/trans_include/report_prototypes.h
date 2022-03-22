#ifndef report_prototypes_h
#define report_prototypes_h



void server_select(char *server_choice);

void report_list(int req_type);

void report(void);

/* Get report for user */
     int report_get(int req_type, char *key, char *time1, char *time2, char *filter1, char *filter2, char *join1, char *join2);

void report_save(char *file_name, char *save);

int fields_get(char *prompt, char *fields);

#endif
