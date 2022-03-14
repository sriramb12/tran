#ifndef m2server_prototypes_h
#define m2server_prototypes_h

#include <stdio.h>
#include "m2globals.h"

void archive_complete(void);

/* Backs out official data to a previous version */
void backout_data(void);

/* Verify user authorized to modify global tables */
void global_auth_check(char *file_name);

void global_tables_read(void);

/* Read one global table */
void global_table_read(void);

int line_get_new(
    FILE *fp,
    char *line,
    int len);



/* Determine action to take based on the request type */
int get_action(void);

/* Retrieve prior version of mask set */
void mask_read_prior(void);

/* Create version of files from prior release */
int prior_files_create(char *data_dir, char *delta_dir, char *suffix, char *tmp_dir, char *new_files[]);

/* build script to create prior version of file */
int script_file_create(char *script_file, char *delta_file);

void mask_restore(char *mask);

/* Verify user authorized to access data */
void owner_check(void);

/* Reads in a table and searches for a user and site */
/* gets user ID from global variable user.id */
/* Gets IP node address from global variable user.addr */
/* Steps performed include: */
/* Calculate the key to search for */
/* Read existing user table into memory */
/* Perform binary search for entry */


void process_partition(void);

/* Initialize paths needed to perform partition transactions */
void partition_paths_init(void);

/* List everything in specified partition */
int partition_list(void);

/* Create specified partition */
int partition_create(void);

int partition_delete(void);

void update_pc_tbl(void);

/* Searches for an entry consisting of a user name & site from specified table */
/* Builds the key to search for and returns it in parameter key */
/* Returns DBYES if bf.entry found, otherwise returns DBNO */
void search(char *file, char *key);


void process_fab(void);

void fab_create(void);

void fab_rename(void);

void rmt_fab_rename(void);

/* Delete an existing fab */
void fab_delete(void);


/* Delete fab from remote databases */
void rmt_fab_delete(void);

/* Send user list of fabs */
void fab_list(void);

void fab_audit(void);

/* audit the remote fabs */
void rmt_fab_audit(void);

/* Create a fab remotely */
void rmt_fab_create(void);

/* Shadow new fab data to remote servers */
void fab_create_send(void);

void create_version_add(float create_version);

void fablist_build(FILE *fp, char *dir);

/* This build the fab and tech list for MARS */
/* this is built once a week by the audits */
/* If this list is different than normal list, it is used */
/* If flows are needed, this list will no longer be built */
void tech_list_build(void);

void fab_tech_rtv(void);

void process_files(void);


void files_shadow(char *dir, char *files[]);

void files_shadow_rcv(char *dir);

void global_read(void);

void global_retrieve(void);

/* Retrieve official copy of global table */
void global_retrieve_all(void);

void global_create(void);

void global_checkout(void);

void global_checkin(void);

void global_update(void);


/* Delete one global table */
void global_delete(void);

/* Delete global table on remote servers */
void rmt_global_delete(void);

void global_transfer(void);

	/* Force shadow of global data to all database machines */
void global_shadow(void);


/* Approve a global table */
void global_approve(void);

/* Unapprove a global table */
void global_unapprove(void);

void global_refresh(int req_type, int start_index, int end_index);

void global_approve_refresh(void);

/* Refresh remote global data */
void rmt_refresh_global(void);

void rmt_global_refresh(void);

void process_global(void);

/* Initialize paths needed to perform transaction */
void global_paths_init(void);

void process_links(void);


/* Retrieve list of templates PC linked to */
void pc_links_list(void);

void process_machine(void);


/* Retrieve official copy of machine tables */
void machine_retrieve(void);

/* Retrieve unofficial copy of machine tables */
void machine_read(void);

/* Read the machine table log */
void machine_log_read(void);

/* Check out specified machine tables */
void machine_checkout(void);

	/* Force shadow of machine data to all database machines */
void machine_shadow(void);

/* Approve machine files */
void machine_approve(void);
void machine_init(char *files[]);

/* Delete specified machine tables */
void machine_dlt(void);

/* checkin specified machine tables */
void machine_checkin(void);

/* Update specified machine tables */
void machine_update(void);

/* Retrieve updated copy of machine tables */
int machine_put(void);

/* Unapprove machine tables */
void machine_unapprove(void);

/* Refresh machine tables on remote servers */
void machine_refresh(int req_type);

void machine_approve_refresh(void);
/* Initialize paths needed to perform machine transactions */
void machine_paths_init(void);

/* Functions for process mask */
void process_mask(void);

void mask_read(void);

void mask_create(void);

void mask_copy(void);

void mask_checkout(void);

void mask_checkin(void);

void mask_port(void);

/* Save .db files in .tmpdb directory */
void mask_save(void);

	/* Major revision made, and files not deleted from user's disk */
void mask_update(void);

	/* Perform check in with overwrite function */
void mask_ovrwr(void);

/* Retrieve mask files during check in, update and overrwrite */
int mask_put(void);

/* Append data to an existing mask file in the database */
void mask_append(void);

	/* change the owner */
void mask_mod_owner(void);

void mask_add_usergroup(void);

void mask_add_user(void);

void mask_mod_usergroup(void);

/* Grant overwrite */
void mask_grant_overwrite(void);

void mask_cancheckout(void);

/* Delete the mask set */
void mask_delete(void);

/* Transfer mask ownership to new site */
void mask_transfer(void);

void mask_transaction_log_rtv(void);
/* Make sure parent directory present */
void source_tech_check(void); 

/* Create new version files */
void version_files_create(char *files[], char *dir);

/* Retrieve C4mask value assigned to mask set */
/* Give error if no value assigned */
void mask_c4get(void);

/* Set a C4 mask set value */
void mask_c4set(void);

/* Send list of all defined mask sets with specified criteria */
/* Used by Mask Designer */
void mask_list(int user_type, int time_type, int time_flag);

/* Send list of all defined mask sets */
/* Used by Auto Stepper Download */
void mask_listall(void);

/* build script to create prior version of file */
int tmp_script_file_create(char *script_file, char *delta_file);

/* Retrieve version information about mask set */
/* Sends version number, month, day, and year version created in an array */
void mask_versions(void);

/* Send version history, parent fab, tech, owner, etc. to user */
void mask_history(void);

/* Send global data and fab data to client */
void global_files_send(char *data_dir);

/* Send global files to the user */
void global_files_rtv(char *data_dir);

/* See if user is creator, owner, or last user for mask */
int user_check(M2TIME user_type, char *user, char *creator, char *owner);

/* See if mask set already present in Mask Designer */
void mask_status(void);

void mask_refresh(int start_index, int end_index, int req_type);

void mask_send_delete(int start_index, int end_index);


void rmt_mask_refresh(void);

void rmt_mask_delete(void);


/* Initialize mask paths needed to perform mask transactions */
void mask_paths_init(void);
void mask_list_members(void);

void mask_usergroup_delete(char *mask, char *usergroup, char *suffix);

void mask_usergroup_add(char *mask, char *usergroup, char *suffix);

/* Check for valid mask */
int mask_valid(char *mask);

/* See if parent template is newer than mask set */
void parent_template_check(long upd_time);

void mask_view_users(void);

void mask_view_usergroups(void);

void mask_view_cwd(void);


void global_files_get(char *files[]);

/* Shadow mask set and template to remote data base */
void mask_shadow(int server_num);

void mask_duplicates_remove(char *files[], int file_num);

/* Send subdirectories to server */
void mask_subdir_send(char *dirlist[]);

/* Send the .db directory for mask sets and templates */
int mask_db_dir_convert(char *data_dir, char *db_data_dir, char *files[]);
void prior_mask_db_dir_convert(char *data_dir, char *db_data_dir, char *files[]);
void mask_db_dir_send(char *data_dir, char *db_data_dir, int convert, char *files[]);

/* PC functions */

/* Retrieve directory where PC was checked out */
void pc_read(void);

void pc_checkout(void);


	/* Major revision made, and files not deleted from user's disk */
void pc_approve(void);




/* Delete the pc library */
void pc_delete(void);

/* Make list of PC libraries and send to user */
/* This list is created dynamicly */
void pc_libs_list(void);

/* Retrieve List of files present in specified PC library */
void pc_list(void);

/* Send updated data to remote servers */
void pc_refresh(int req_type);

void process_pc(void);
/* Initialize paths needed to perform PC transactions */
void pc_paths_init(void);


/* Create specified pc */
void pc_create(void);

/* checkin specified pc */
/* New version of PC created and entry for new version put in version file */
void pc_checkin(void);

void pc_cancheckout(void);

void pc_versions_list(void);
/* See if any approved versions of the PC library are present */
void pc_approve_versions_check(void);

void pc_send(char *version, char *action);

/* Sends the GDS file only during a read transaction */
/* this is done when the user retrieves the PC with its original file name */
void pc_send_gds(char *version);

/* Retrieve current working directory */
void pc_cwd_rtv(void);

/* Send version history, owner, etc. to user */
void pc_history(void);


/* Unapprove current PC library */
void pc_unapprove(void);

/* Send client list of PC approvers */
void pc_approvers_list(void);

/* Verify user authorized to build PC's */
void pc_build_check(void);

/* Verify user authorized to approve PC libraries */
void pc_approve_check(void);

/* Build PC library name empbedding in the version number */
int pc_version_name_build(char *library_name, char **output_name, char *version);

/* End PC functions */
/* PC history functions */

/* Retrieve directory where PC was checked out */
void pc_history_read(void);

void pc_history_checkout(void);


void process_pc_history(void);
/* Initialize paths needed to perform PC transactions */
void pc_history_paths_init(void);



/* Update specified pc */
/* New version of PC created and entry for new version put in version file */
void pc_history_checkin(void);

void pc_history_cancheckout(void);


void pc_approved_version_rtv(void);

/* Create the default history file created when PC created */
void pc_history_file_create(void);

void pc_info_generate(char *version,char *local_file, char *remote_file, char *alternate_file);

/* End pc history funtions */

void pin_mod(void);

void reg_manual(void);

/* Perform auto registration initiated by the user */
void reg_auto(void);

void rtv_all(void);

void process_register(void);


/* List all users in the DMI registry */
/* User's name and X.500 user ID returned */
/* Users are put in alphabetical order */
void users_list(void);

int users_alpha(char *users[], char *registry_file);
void process_request(int socket);
void process_versions(void); 
void process_db_admin(void); 
void process_cron(void); 
int lock_file(char *file, int fd); 
void version_file_update(void); 
void add_resend_file_entry(void); 
void send_rmt_files(void); 


void contract_num_retrieve(void);

void contract_log(char *log_file, char *company, char *serial_char);

void serial_log(char *log_file, int serial_num, int num);

void rtv_serial(char *serial_prefix);

void mask_name_rtv(void);

void rtv_pc_name(int pc_num, char *from_email, char *old_pc, char *to_email,
char *layers, char *maskshop, char *dye_size, char *maskshop_date, char *remarks);

void rtv_h_name(int h_num, char *from_email, char *old_h_name, char *to_email,
char *layers, char *maskshop, char *dye_size, char *maskshop_date, char *remarks);

void build_new_mask(char *new_mask, char *current_mask);

void data_write2log(char *log_prefix, char *mask, char *name, char *user_id, char *orig_mask, char *description, char *remarks);

/* Verifies correct information entered for requester */
/* Pass in user ID or commerce ID in first parameter */
/* Returns user ID, department, and name */
void requester_whobu(char *id, char *user_id, char *name, char *dept);

/* Verifies correct information entered for receiver */
/* First argument holds user's user ID or commerce ID */
/* User's user ID is returned in the second argument */
int receiver_whobu(char *id, char *user_id);


void process_metrics(void);

void event_log(void);
/* Process global data requests */
/* Also processes system administrator requests */
void process_serial(void);
/* See if this is a commerce ID */
int is_commerce_id(char *id);

/* Retrieve info about specified mask set */
void mask_info_rtv(char *mask, char *user_id, char *log_name);  
void old_mask_info_rtv(char *mask, char *user_id, char *log_name);  

/* Process global data requests */
/* Also processes system administrator requests */

/* Fucntions for process_template */
void process_template(void);

void template_approve(void);

/* Delete the  template */
void template_delete(void);

/* Delete template from remote databases */
void rmt_template_delete(void);

/* Send user list of technologies for specified fab */
void template_list(void);


/* Unapprove a template */
void template_unapprove(void);

void flow_retrieve(void);

void template_approve_refresh(int req_type);

/* Initialize paths needed to perform template transactions */
void template_paths_init(void);

/* Verify user authorized to build/approve templates */
void template_auth_check(void);

typedef int (*glf_t)(char *list[], char *key, int total_count);
/* GenericListFunction Type definition */

int user_list_add(char *list[], char *key, int total_count);

void process_usergroup(void);

/* Create a new usergroup */
void usergroup_create(char *users);

/* Modify an existing user group */
void usergroup_mod(char *users);

/* Called when user trying to complete user group and all members */
void usergroup_delete_all(void);

/* Delete specified users from group */
void usergroup_delete(void);

/* Send user list of users in user group */
void usergroup_list_users(void);

/* Send list of all user groups present */
void usergroup_listall(void);

/* Verify user entered correct PIN */
/* and that this server owns the data */
void usergroup_check_owner(void);

/* Verify user group is present and that this server owns the user group */
int usergroup_check_existence_owner(void);

/* Get the new names and add to the list */
int usergroup_list_mod(char *names, int total_cnt, glf_t list_function);

/* List masks and templates assigned to the user group */
 void usergroup_list_masks(void);

int members_list(char *file_name, char **ptr);

/* Count number of mask sets or templates accessing the user group */
int members_count(char *file_name);

/* Send usergroup override message to user */
void usergroup_override_send(char *override_msg);

/* Delete references to user group from all associated mask sets */
void usergroup_masks_delete(char *usergroup);

void usergroup_templates_delete(char *usergroup);

/* Sends user successful return code */
void put_ret_code(char log_msg);

/* Log message in data log */
void m2data_log(char *msg);



void put_success_msg(char *msg);

void put_job_err(char *err_msg, int job_id, int err_code);

/* Sends client global information needed when data base initializes */
void put_wafer_design(void);

void read_global_params(void);


/* Add authorization entry, done by MDS administrators */
void admin_auth_entry_add(char *file_name, char *string1, char *string2);

/* Verify user is an approved MDS administrator */
void admin_check(void);

/* Verifies entry for user present in registry */
/* Gathers the PIN and user group but does not verify PIN */
void reg_check(void);

/* Verify correct PIN entered */
void pin_check(void);

/* Perform new registration */
/* At this point X.500 password has already been verified */
void reg_perform(void);

void rmt_reg_perform(void);

/* Authenticate user */
/* First tries to verify correct X.500 ID and password entered */
/* If unsuccessful, tries to verify name in email core directory matches name in password file */
void x500_check(char *x500id, char *x500password);

/* Sends last message and closes socket */
void return_err(char *msg, int rtn_code);

void return_m2badpin(char *msg, int rtn_code);


/* Sends last message and closes socket */
void return_override(char *msg, int rtn_code);

/* Sends last message and closes socket */
void return_success(char *msg, char log_msg);

/* Sends last message and closes socket */
void return_warning(char *msg, int return_code, char log_msg);

/* Sends last message, closes socket and cleans up temporary files */
void shut_down(
    char *msg,		/* Points to message sent to user */
    register short rtn_code);	/* Holds return code sent to user */

void rmt_shut_down(
    char *msg,		/* Points to message sent to user */
    register short rtn_code);	/* Holds return code sent to user */


/* Change month from form 'Jan' to '01' */
/*
int month_convert2int(char *date);
*/

long time_convert(char *time_string);

int time_calc(int time_flag, long lower_time, long upper_time, long mask_time);
int file_rename(char *old_file, char *new_file);
void file_append(char *old_file, char *new_file);

void file2lock(void);

void file2unlock(void);

void mars_send_weekly(void);

void file_info_get(char *log_file, FILE *fo);

void tech_entry_add(char *fab, char *tech);

void tech_entry_delete(char *fab, char *tech);
/* Functions for mask_files */

void mask_filelist_add(int level, char *name, char *filelist[], int program[]);

int mask_file_version_same(char *file, float version);

/* Read in latest version number from delta file */
float mask_version_read(char *delta_file);

int mask_files_append(char *new_dir, char *old_dir);

/* Version other files present */
void other_files_version(int level, char *delta_dir, char *filelist[], float old_version);

/* Moves all files with current version to the new version */
/* This function is called when mask_append is done */
void all_files_version(char *data_dir, char *delta_dir);
/* Read names of files defined for this mask set */
void other_files_read(char *files[], int program[]);

/* Verify mask file is at the latest version */
int mask_file_is_latest(char *root_file);

/* See if present on file list */
int filelist_check(char *name, int level);

/* Knowing the file list check for name in that file list */
int mask_filelist_check(char *filelist[], int program[],  char *name);


/* Makes new directory and copies files from old directory to new directory */
/* Makes list of files in source directory */
/* and copies files to destination directory */
/* List of files copied returned to calling program */
/* The directory name passed in must not have an ending slash */
/* Only latest version of files copied */
int mask_dir_copy(char *new_dir, char *old_dir, char *delta_dir,  char *files[], int mod_flag);

/* See if this file should be versioned */
/* If this tool does not own the file and file has old version */
/* It should be versioned */


/* Read in the mask file list for this mask set */
void mask_filelist_read(char *filelist[], int program[], int level);

int mask_dir_list(char *directory, char *list[], int index);

/* End functions for mask_files */

/* functions to manipulate entries */
/* Verify user authorized to build/approve templates */
int entry_find(char *file_name, char *entry);

/* Adds new entry to specified file */
/* If entry already present, replace old entry with new entry */
int entry_add(char *file1, char *entry, char *key);

/* Delete entry from specified file */
int entry_delete(char *file1, char *entry);

/* Returns SUCCESS or FAILURE */
int entry_find_write(char *file1, char *file2, char *entry, FILE **fi, FILE **fo, char *line);

/* Write out the rest of the file */
void entry_write(int index, char *file1, char *file2, FILE *fi, FILE *fo, char fnd);

void entry_close(int index, char *file1, char *file2, FILE *fi, FILE *fo);
void entry_approve(char *file1, char *version);

void entry_unapprove(char *file1);

/* Return version number for approved version */
void entry_approved_version_find(char *version_file, char *version);

/* Makes a list of subdirectories with version equal to latest version of data from state file */
int subdir_version_list(char *directory, char *dirlist[]);

/* Send subdirectories to server */
void subdir_send(char *dirlist[]);

/* Get files in other directories */
/* Returns list of directories and files retrieves */
/* offset holds offset for each file list returned */
/* The directory where the files are placed is initialized before the function is called */
int subdir_get(char *dir_name, char *dirlist[], char *files[], int file_offset[]);

/* Version the directory list */
int subdir_version(char *dirlist[], char *files[], char *new_data_dir, char *new_delta_dir, int *file_offset[]);

/* Refresh functions */

/* Refresh a remote mask set or template */
void rmt_refresh_data(void);

void rmt_create_data(void);

    
void rmt_mark_data();

/* Send refresh request to remote server */
/* No data is sent with this request */
void refresh_request(int req_type);

/* Refresh data done after header already sent */
int refresh_data(void);

void refresh_global(void);

/* End refresh functions */

/* Audit functions */

/* At this point data path, delta path, and key are set */
/* Build check sums for directories passed in */
/* Send these check sums and get results from remote server */
int audit_data(int refresh_cnt[], int start_index, int end_index);

/* Special audit needed for global data because it  a different structure than other data */
void audit_global(char *key, int refresh_cnt[]);

long bld_checksum(char *file_name);

/* Build check sums for global data */
/* Used to build both local and remote global check sums */
void bld_global_checksums(char *data_files[], char *delta_files[]);

/* End Audit functions */

void bld_header(
    char *buffer,
     int req_type,
    int data_type,
    int transaction_num);

void resend_data(void);

/* Adds new entry when data must be resent */
int resend_entry_add(char *server_site, int server_index);


int get_rmt_ret_code(int log);

/* Check to see if create file is present on the .db filelist */
/* If not present add to list and copy into new dta directory */
/* The create file must be present for all versions of the data */
void create_file_check(char *new_data_dir, char *db_files[]);

void audit_report_send(void);

/* Build template keys needed to audit tempalte */
void template_keys_set(char *file_name);

/* Archive the template */
void template_archive(void);

/* Unarchive the template */
void template_unarchive(void);

/* Strip brackets off of technology name */
void template_brackets_strip(char *tech);

void template_files_sort(char *files[], int total_cnt);

/* Adds line to document global data approved in version file */
void approve_version_add(char *file_name, float version);

void registry_audit(void);


void mask_create_check(char *data_dir, char *files[]);

int mask_library_version_rtv(char *dir);

int metrics_dmi(void);

void site_map2x500(void);

/* Creates mask set from Mask prep to Mask Designer format */
int maskprep_create_convert(char *mask, char *data_dir);

/* Perform action when state file not present */
void state_file_not_present(int index);

/* Send audit log to administrators */
void audit_mail_send(char *audit_list);

void mask_fab_files_copy(char *maskprep_create_file, char *data_dir);

/* Delete .tmpdb directory during checkin, cancel check-out and update */
void mask_tmpdb_dlt(void);

void mask_convert(char *data_dir, char *db_data_dir, char *files[]);

void turned_off_mask_checkout(int state);

void turned_off_mask_delete(char *mask);

/* See if turned off mask set has been checked out */
/* If it has, log message and send Rob e-mail */
void turned_off_mask_log(char *mask);

void turned_off_mask_email(char *mask, char *user_id, char *action);

void off_template_paths_init(void);

void partition_name_check(char *partition);

/* Write entry to audit log */
void log2audit(char *str1, char *str2);
void log4audit(char *str1, char *str2, char *str3);
void rmt_log4audit(char *str1, char *str2, char *str3);

/* Print additional information to be put in audit log */
void audit_print(char *data_string, int refresh_cnt[]);

int mask_use_autosave(void);

/* Retrieve list of projects associated with the mask set */
void mask_projects_rtv(void);

/* Process SQL requests */
void process_sql(void);

/* Process cap wafer requests */
void process_cap_wafer(void);

/* Read a cap wafer file already present with the primary mask set */
void cap_wafer_file_read(void);


/* Map the cap wafer to the primary mask set name */
void cap_wafer_map(char *primary_mask, char *cap_wafer);

/* Add mask name to a cap wafer file */
void cap_wafer_mask_name_add(void);

/*  Purpose: This function is used to build a unique list of cell  */
/*           A return of '0' means the cell was already on the list*/
/*           A return of '1' means the cell was added.             */
int CellListAdd(struct cell_class *cell); 

/*  Purpose: This function is used to load the global CellList.    */
int load_cell_list(void); 

/*  Purpose: This function will return the matching cell from the  */
/*           global CellList.                                      */
struct cell_class *return_cell_from_cell_list(char *cell_name); 

/* Update the common_maskset_layers table */
int common_maskset_layers_update(void);

/* Update the common_template_layers_table */
int common_template_layers_update(char *approve_status);

#endif
