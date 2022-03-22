#ifndef m2audit_prototypes_h
#define m2audit_prototypes_h



/* audit the remote data */
/* Used to audit mask, template and PC data */
void rmt_audit_data(void);

/* Special audit needed to audit remote global data */
void rmt_audit_global(void);

void global_audit(long *global_audit_time);

void machine_audit(long *machine_audit_time);

void mask_audit(long *mask_audit_time);

    /* The following checks are made every week */
    /* Read through mask table and make sure there are no extra entries */
    /* Delete any extra entries found */
    /* If there is not a state file for the mask, it does not exist */
    /* At least the audit thinks it does not exist */
void mask_table_audit(void);

void pc_audit(long *pc_audit_time);

    /* The following checks are made every week */
    /* Read through pc table and make sure there are no extra entries */
    /* Delete any extra entries found */
    /* If there is not a state file for the pc, it does not exist */
    /* At least the audit thinks it does not exist */
void pc_table_audit(void);
void process_audit(void);

void template_audit(char *fab_path, long *template_audit_time);


/* At this point data path, delta path, and key are set */
/* Build check sums for directories passed in */
/* Send these check sums and get results from remote server */
int audit_state(int refresh_cnt[], int start_index, int end_index);


int audit_dir(char *dir, int start_index, int end_index);

int audit_file(char *file, int start_index, int end_index);


/* Special audit needed for global data because it  a different structure than other data */
void audit_global(char *key, int refresh_cnt[]);

/* audit the remote data */
/* Used to audit mask and  template data */
void rmt_audit_state(void);

void rmt_audit_data(void);

void rmt_audit_dir(char *dir);

/* Special audit needed to audit remote global data */

long audit_time_set(void);

long bld_checksum(char *file_name);

/* Build check sums for global data */
/* Used to build both local and remote global check sums */
void bld_global_checksums(char *data_files[], char *delta_files[]);

/* Audits special types of data */
void network_audit(void);

#endif
