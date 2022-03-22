#ifndef mds_sql_lib_h
#define mds_sql_lib_h

/* Verify this is a valid mask name */
/* Returns DBYES if is a mask, else returns DBNO */
int is_mask(char *mask);

void new_data_log(char *log_file, char *msg);


/* Add mask entry to common_mars table */
    void mars_insert(char *mask, char *name,      char *from_email, char *orig_mask, 
char *fab, char *tech, char *mbg, char *project_leader, char *project_uid,
char *dante_num, char *project_name, char *chipset, char *due_date, char *eccn, char *description, char *remarks);

    void tmp_die_area_insert(
    char *     die_maskset, 
    double die_x_scribe, 
    double die_y_scribe, 
    double die_x_no_scribe, 
    double die_y_no_scribe, 
    double vertical_scribe, 
    double horizontal_scribe, 
    char *fab);

/* Update reticle_die_table */
int reticle_die_update(char *mask, char *rows, char *columns, char *num_die, int program_type);

    void mars_insert_old_data(char *request_date, char *mask, char *name,      char *from_email, char *orig_mask, 
char *fab, char *tech, char *mbg, char *project_leader, char *project_uid,
char *dante_num, char *project_name, char *chipset, char *due_date, char *eccn, char *description, char *remarks);


/* Put process steps into database */
/* Process steps are the same as layers */
int sql_insert_process_steps(char *user_id, int fab_num, int tech_num, char *tech_file, char *approve_status);

/* Find the technology number for a technology */
int sql_tech_num_find(char *technology);

/* Update the layers in the database */
void sql_layers_update(char *off_data_dir, char *fab, char *technology, char approve_status);

/* Look up fab in local file */
int fab_lookup(char *fab);

/* Inserts records for start and end of Reticle Builder */
int sql_reticle_bld_insert(char *mask, char *state, char *user_id);

/* Find the layer and comment in a line from the process_step table */
    int layer_comment_find(char *line, char *layer_field, char *comment_field);

/* Scan send2maskshop_log and put data */
/* in common_maskshop_layers when data sent to mask shop */
int send2maskshop_log_scan(char *file_name, int multiple_entries);

/* Add entry when new mask set created */
void sql_mask_create_add(char *mask, char *user_id);

int separate_logs_bld(char *file_name);

/* Convert date to SQL format */
void sql_date_convert(char *sql_date, char *line);

/* Find the fab for this mask set */
void fab_find(char *fab, char *mask);

/* Map the old fab name to the new name */
void die_fab_map(char *fab);


/* Update the fab name in the die_area_table */
void sql_die_area_fab_name_update(char *mask);

test(); { }
/* Insert data into the mask_ccc table */
int mask_ccc_insert(MYSQL *sql, char   *device, char *layer_id,  char *plate_id,
char *fab, char *tech,
 float percent_clear);
void ccc_scan(char *file_name, char *mask);

/* Map fab to fab ID */
int sql_fab_id_get(char *fab);

/* Update the common_layers record */
void sql_common_layers_record_update(MYSQL *sql, int fab_id, int technology_id, char *technology, char *flow,
  char *process_step, char *revision, char *booleans, char *description, char *approve_status);

/* Update the common_layers table */
void sql_common_layers_update(char *off_data_dir, char *approve_status);

