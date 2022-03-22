#ifndef m2client_prototypes_h
#define m2client_prototypes_h

int process_audit(char command);

int process_auth(int argc, char **argv);
int mdsadmin_call(int req_type, int data_type, char *key1, char *key2, char *key3);


int process_c4mask(char command, char *mask1, char *mask2);

int process_cap_wafer(
    int argc,
    char **argv);

int process_cron(int argc, char **argv);

int process_db_admin(
    char *files[],
    char command,
    char data_type);


int process_global(int argc, char *argv[]);

int process_machine(char *files[], int argc, char *argv[]);

void name_list_get(char *list[], char *name);
void tmp_list_get(char *list[], char *name);

int data_log(char *file_name, char *string);

void resend_entry_add(int argc, char **argv);

int process_mask(
    char *files[],
    int argc,
    char **argv);

int process_off_global(int argc, char *argv[]);

int process_off_template(
    char *files[],
    int argc,
    char **argv);

int process_partition(char *files[], int argc, char **argv);

int process_pc(char *files[], int argc, char **argv);
int list_print(int i, char *files[]);

int process_register(int argc, char **argv);


int process_release(char *files[], char command);

int process_serial(
    char *files[],
    int argc,
    char **argv);

int process_sql(int argc, char **argv);

int process_template(
    char *files[],
    int argc,
    char **argv);

int process_usergroup(int argc, char **argv);

int process_waferline(
    char *files[],
    int argc,
    char **argv);

#endif
