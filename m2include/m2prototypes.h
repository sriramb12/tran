#ifndef m2prototypes_h
#define m2prototypes_h

#include "m2globals.h"

/* This function initializes default globals needed for all subsequent database calls */
/* It must be called before any other database functions are called */
/* Calling program must pass in its identification name */
/* server_init indicates whether calling program wants to initialize */
/* default server addresses */
/* Use DBYES for initialize defauls, DBNO for do not initialize */
/* Pass in user's X.500 user ID for registration check only */
/* library_version holds the library version number for data to be retrieved */
/* Pass in NULL for X.500 ID and DMI PIN check */
/* User is prompted for his X.500 user ID and DMI PIN */
int globals_init(DMI_PROGRAM program, int server_init, char *x500, int library_version);

void d2arrayfree(char *names[]);

/* Convert create file from old format to new format */
/* This function expects create file to be in CWD */
/* It writes new create file in CWD/.db */
/* Error occurs if old create file not in CWD */
/* or '.db file does not exist */
/* Returns M2SUCCESS or M2ERR */
short create_file_convert(char *mask);

/* Return name of fab, technology, flow, and design stored in create file */
/* This function assumes create file is in current working directory */
/* Returns SUCCESS if transaction successful */
/* Otherwise returns DB_ERR */
short create_file_read(char *fab, char *tech, char *flow, char *design, char *x500id);

/* Return pointer to value for current working directory */
/* Memory pointed to is global and does not need to be freed */
char * cwd_get(void);

/* Set current working directory */
/* Sets global value used by database client for current working directory */
void cwd_set(char *directory);

/* Retrieve the RET and MSO SQL passwords */
short ret_mso_passwords_rtv(char **ret_password, char **mso_password);

/* Retrieve the RET password used to access the SQL database */
short ret_password_rtv(char **ptr);

/* Initializes machine name and IP node address for server machine */
/* index is a value from 0 to 9 */
/* Fails only if index is out of range */
/* Return values are SUCCESS and FAILURE */
short dbserver_set(
    char *name,	/* Machine name */
    char *address,	/* IP node address of machine */
    int index);	/* 0 for primary, 1 for secondary, etc. */

/* Sets socket for Mask Designer database servers */
/* Currently all servers use the same socket value */
void dbsocket_set(int socket);

/* Initialize machine, IP address, and socket for Photronics mask shop at Mesa */
void maskshop_server_set(char *machine, char *address, int socket);

/* Returns pointer to machine name of primary database server */
char *primary_server_get(void);

/* Free list of file names previously allocated */
void files_free(char *files[]);

/* Return pointer to mesage received with database transaction */
/* A message is returned with all database transactions */
/* Storage for the message is handled by the database client */
/* Memory pointed to is global and does not need to be freed */
char * rtn_msg_get(void);

/* Return user's X.500 user ID */
/* The database client always knows the user's X.500 user ID */
/* and stores this information in global memory */
/* Memory pointed to is global and does not need to be freed */
char * x500id_get(void);

/* Create a new fab in the database */
/* The needed directories for the fab are created in the database, */
/* Also the technology file and the machine files are copied from the */
/* approved global directory to the new fab */
short waferline_create(char *new_fab, char *old_fab, char *machine_usergroup);

/* Rename an existing wafer line */
short waferline_rename(char *new_fab, char *old_fab);

/* Delete the specified wafer line and all technologies */
/* associated with the wafer line */
short waferline_delete(char *fab, int override);

/* Retrieve list of all defined fabs */
/* List is returned in an array of pointers */
/*List is terminated with a NULL pointer */
/* Calling function is responsible for freeing this list */
/* Returns the number of fabs retrieved */
/* Returns 0 for no fabs or negative return if an error occurs */
short waferline_list(char *files[]);

/* Read waferline, tech, flow, process step, RET type table */
short waferline_read(void);

short waferline_techflow_list(void);

/* Retrieve the fab tech table */
short fab_tech_rtv(void);

/* Update the fab technology table in the database */
short fab_tech_update(void);

/* Functions that manipulate fab specific files */
/* These functions have been removed because all these files are now manipulated as one group under '.fab' */
/* and are handled by the machine_file functions */
/* The functions removed are fabfile_create, fabfile_checkout, fabfile_checking, etc */

/* Functions that manipulate the file list */


/* Add a file name to the file list */
/* Duplicate names are not added */
/* Returns SUCCESS if name added */
int filelist_add(char *name);

/* Adds directory names to the directory list */
/* Duplicate names are not added */
/* Names not kept in alphabetical order */
/* Returns SUCCESS if name added */
int dirlist_add(char *name);

/* Delete name from file list */
/* Returns SUCCESS if name deleted */
int filelist_delete(char *name);

/* Functions to manipulate global files */
/* Unofficial copy of global tables is placed in the .tables */
/* directory under the user's current working directory */
/* In the database they reside in directory global_data */
/* Official copy of global tables is placed in directory*/
/* $DMIHOME/maskdesigner/tables */

/* The following functions are performed by an authorized administrator */

/* Check in and approve a global file */
/* the checked in file is copied to the approved bucket */
/* If an approved version of the file is already present, it is overwritten */
/* No versioning on approved data is done */
/* Returns SUCCESS if transaction is successful */
short global_approve(char *file_name);

/* Cancel check out on global file and erase from user's disk */
short global_cancheckout(char *file_name, int override);

/* Check global file into the database */
/* Check out status cancelled and file deleted from user's disk */
short  global_checkin(char *file_name, int override);

/* Check out copy of global file */
/* Done only by designated administrators */
/* File placed in .tables directory under current working directory */
/* .tables directory created if not already present */
short global_checkout(char *file_name, int override);

/* Create a new global file */
/* Copy of file in user's current working directory is copied to the database */
/* Root name of this file is passed to this function */
/* Unapproved and approved versions of the global files are present.  */
/* The approved version is copied into $DMIHOME/maskdesigner/tables */
/* when a user reads or checks out a mask set or template */
short global_create(char *file_name);

/* Delete copy of global file from database */
/* Both the approved and unapproved copy of the file is deleted */
/* Override condition provided so user must confirm deletion */
short global_delete(char *file_name, int override);

/* Retrieve copy of unapprove global file */
/* Copy of file is placed in .tables directory in current working directory */
/* .tables directory created if not already present */
short global_read(char *file_name, int override);

/* Retrieve copy of approve global file */
/* File is placed in .tables directory under current working directory */
/* this directory is created if not already present */
short global_retrieve(char *file_name, int override);

/* Read all approved global files into the system directory */
/* System directory is dmi_home/maskdesigner/tables */
short global_retrieve_all(void);

/* Force shadow of global  table to all database machines */
/* Shadow done from machine that owns global  table */
short global_shadow(char *table, char *server_index);;

/* Transfer ownership of global table to different database */
short global_transfer(char *file_name, char *site);

/* Unapprove a global file */
/* The approved copy of the file is deleted from the database */
/* Returns SUCCESS if transaction is successful */
short global_unapprove(char *file_name);

/* Update global file in the database */
/* Major revision of file made in the database */
/* File remains checked out and copy of file remains on user's disk */
short  global_update(char *file_name, int override);


/* Machine table functions */

/* Check in and approve specified machine tables */
/* The new copy of the machine tables is copied to the approve bucket */
/* If an approved version of the tables is already present, they are overwritten */
/* No versioning on approved data is done */
/* Returns SUCCESS if transaction is successful */
short mach_approve(char *fab);

/* Cancel checkout on machine tables */
/* Check out status is cancelled and machine tables are deleted from user's disk */
short mach_cancheckout(char *fab, int override);

/* Check machine tables into the database */
/* Check out status cancelled and tables deleted from user's disk */
/* Files are present in the .fab directory under the current working directory */
/* Returns SUCCESS if transaction successful */
short  mach_checkin(char *fab, int override);

/* Check out copy of machine tables */
/* Files are placed in .fab directory */
/* Local .fab directory created if not already present */
/* Returns SUCCESS if tables retrieved */
short mach_checkout(char *fab, int override);

/* Retrieve copy of unapprove machine tables */
/* The tables are placed in the user's .fab directory */
/* Local .fab directory created if not already present */
short mach_read(char *fab, int override);

/* Retrieve copy of approved machine tables */
/* Approved tables are placed in .fab directory in current working directory */
/* Local .fab directory created if not already present */
short mach_retrieve(char *fab, int override);

/* Retrieve copy of machine table log */
/* The log is placed in the user's .fab directory */
/* Local .fab directory created if not already present */
short mach_log_read(char *fab, int override);

/* Transfer ownership of machine tables to different database */
short mach_transfer(char *fab, char *site);

/* Unapprove machine tables */
/* The approved copy of the tables is deleted from the database */
/* Returns SUCCESS if transaction is successful */
short mach_unapprove(char *fab); 

/* Update machine tables in the database */
/* Major revision of tables made in the database */
/* Tables remains checked out and copy of tables remains on user's disk */
short  mach_update(char *fab, int override);

/* Returns the list of users and user groups for specified fab machine tables */
/* returns number of elements in each list in user_cnt, and group_cnt */
/* Returns M2SUCCESS if transaction successful */
/* Calling function can free each list separately with usergroup_free */
short mach_usergroup_list(char *fab, int *group_cnt, char **group_array[], int *user_cnt, char **user_array[], char **owner);

/* Modify usergroups/users with edit permission for machine tables */
/* The user must own the machine tables to modify the assigned user groups */
/* The user can assign one or more user groups or one or more users to the machine tables */
/* Calling function passes user groups and users in null terminated arrays */
/* If no usergroups or users present, first element of array is NULL terminated */
/* Calling function frees the list when function returns */
/* Returns M2SUCCESS if transaction successful */
/* Returns M2ERR if transaction failed */
short mach_usergroup_mod(char *fab, char *usergroups[], char *users[]);

short mach_shadow(char *fab, int server_index, char *server_site);
/* Append files to existing mask set */
short mask_append(char *mask, char *file_name, char *root_name, char *email_group);

/* Mask set functions */
/* Check out mask set data into current working directory */
/* .db directory also retrieved */
/* Global files are placed in $DMIHOME/maskdesigner/tables */
/* Override flag is set to DBYES or DBNO */
/* Returns SUCCESS if checkout is successful */
/* Sets the autosave flag to AUTOSAVE_PRESENT if saved data present */
short mask_checkout(char *mask, int override_flag, int *autosave_flag);

/* Cancel check-out on specified mask set */
/* If successful, all the mask set files are deleted */
/* from the user's disk */
/* Files in $DMIHOME/maskdesigner/tables */
/* are never deleted by the Mask Designer database manager */
short mask_cancheckout(char *mask, int override);

/* Cancel read on a mask set */
/* Deletes all Mask Prep files on the file list and all files */
/* in .db directory */
/* Returns SUCCESS if transaction successful */
short mask_canread(char *mask);

/* Check in mask set files */
/* If check in is successful, mask set files are deleted */
/* from the user's disk */
/* Returns SUCCESS if check in is successful */
short mask_checkin(char *mask, int override);

/* Create a new mask set from an existing mask set */
/* Mask files are checked out into current working directory */
/* .db directory is also checked out */
/* Returns SUCCESS if create is successful */
short mask_copy(char *new_mask, char *old_mask);

/* Create a new mask set from an existing template */
/* All of the files from the latest version of the approved template are */
/* copied except for the 'History' file */
/* The mask set is checked out for edit and copied to the user's disk */
/* Global files are placed in $DMIHOME/maskdesigner/tables */
short mask_create(char *mask, char *fab, char *tech, char *flow, char *design, char *usergroup);

/* Delete n existing mask set */
/* Override condition provided so user must confirm deletion */
short mask_delete(char *mask, int override);

/* Retrieve list of mask sets with specified criteria */
/* File with list retrieved is placed in $DMI_HOME/maskdesigner/tables/mask.sets */
/* Used by Mask Designer */
/* User type indicates if looking for user who created the mask */
/* or user who modified the mask */
/* time_type indicates if looking for create time or modify time */
/* range specifies on, prior, range, or since */
short mask_list(char *wl, char *tech, char *flow, char *design, char *user_id,
    char *time1, char *time2, M2TIME user_type, M2TIME time_type, M2RANGE range, M2LIST_TYPE list_type, char **file_ptr);

/* Retrieve list of all defined mask sets */
/* File with list retrieved is placed in $DMI_HOME/maskdesigner/tables/mask.sets */
/* Used by auto Stepper Down Load */
short mask_list_all(void);

/* Read mask set files to current working directory */
/* .db directory also retrieved */
/* Global files are placed in $DMIHOME/maskdesigner/tables */
/* Returns SUCCESS if successfully read from Mask Designer database */
/* Returns M2CONVERT if read from mask Prep database */
/* Returns M2OVR if override condition occurs */
/* Returns M2ERR for all errors encountered */
/* Override flag is set to DBYES or DBNO */
/* If set to DBNO, database may return an override condition */
/* Sets the autosave flag to AUTOSAVE_PRESENT if saved data present */
short mask_read(char *mask, int override_flag, int *autosave_flag);

/* Read prior version of mask set into current working directory */
/* Prior version of .db directory also retrieved */
/* Global files are placed in $DMIHOME/maskdesigner/tables */
/* Returns SUCCESS if read successful */
short mask_read_prior(char *mask, int override_flag, char *version);

/* Brings down files when mask set created, copied, read or checked out */
/* Should never  e called from the GUI or any other non database function */
short mask_retrieve(void);

/* Update mask set in database */
/* Major revision made, and files not deleted from user's disk */
short mask_update(char *mask, int override);

/* Copy the .db directory to the database */
/* No versioning is done and files saved in .tmpdb directory on database */
/* Mask remains checked out and no files deleted from user's disk */
short mask_save(char *mask);

/* Perform check in with overwrite function */
/* If successful, mask set files deleted */
/* from the user's disk */
short mask_ovrwr(char *mask);

/* Modify the owner of a mask set */
/* Must be the owner to perform this transaction */
/* The user must supply X.500 user ID of new owner */
short mask_mod_owner(char *mask, char *new_userid);

/* Modify usergroups/users assigned to a mask set */
void mask_mod_usergroup(void);

/* Grant override on a mask set */
/* Must be the owner to perform this transaction */
/* The user must supply X.500 user ID of temporary user  */
short mask_grant_ovrwr(char *mask, char *new_userid);

/* Retrieve history on specified mask */
/* versions, create date, owner, state, etc. returned */
/* versions returned in an array that can be freed with files_free */
/* The rest of the information is returned in rtn.msg accessed with rtn_get */
short mask_history(char *mask, char *names[], char **msg);

/* Force shadow of mask data to all database machines */
/* Shadow done from machine that owns mask set */
short mask_shadow(char *mask, int server_index, char *server_site);

/* Transfer ownership of mask set to new database */
short mask_transfer(char *mask, char *new_site);

/* Set C4 mask for specified mask set */
short mask_c4set(char *mask, char *c4mask);

/* Retrieve previously set C4 mask used by Bump Mask */
/* Calling function must supply memory for value returned */
/* C4 masks are currently 4 characters in length */
/* Thus a pointer to a buffer at least 5 characters in length must be supplied */
short mask_c4get(char *mask, char *c4mask);

/* Unarchive a previously archived mask */
short mask_unarchive(char *mask);
/* Get one or more directories from server */
/* Directories may not be present */
short dir_get(void);

/* Check out a prior version of a mask set */
/* Not yet implemented */
short mask_checkout_prior(char *mask, int override_flag, char *version); 

/* Retrieve versions for specified mask */
/* Version number, month, day, and year version created returned in an array */
/* The array can be freed by calling files_free */
short mask_versions(char *mask, char *names[], int name_size);

/* Shadow all PC files to other sites */
int pc_shadow(char *pc);

/* Transfer ownership of PC to new site */
int pc_transfer(char *pc, char *site);

/* Verify user is registered to use mask prep */
short reg_verify(void);

/* Verify registration with data entered previously */
short reg_check(void);

/* Perform a registration transaction */
short reg_transaction(int req_type);

/* Modify the user's PIN in the Mask Designer registry */
/* Existing PIN re-verified before new PIN assigned */
short m2pin_mod(char *x500id, char *old_pin, char *new_pin);

/* Perform registration from GUI */
/* First the X.500 user ID is checked */
/* If it is present in the registry, the PIN is checked */
/* If it is not present in the registry, authentication is attempted with the X.500 password */
/* If it is successful, user is registered with his new PIN */
short reg_perform(char *x500id, char *x500password, char *pin);

/* Perform Mask Designer registration */
short reg_auto(void);

/* Registration performed by system administrator */
short reg_manual(char *login, char *subnet, char *pin, char *email);

/* Audit the registry directory */
short reg_dir_audit(void);

short x500query(char *userid, char *string);

/* Send weekly report of MARS transactions to Drew Russell each week */
short mars_send_weekly(void);

/* Send MARS log for Mettracks */
short mars_send(void);

short contract_num_retrieve(char *mask, char *maskshop, char *contract_num);

/* Retrieve annual contract number for specified mask shop */
/* This function is going away because this information is in the */
/* global mask shop table */
short annual_retrieve(char *maskshop, char *cnb);

/* Retrieve a single mask set name */
/* This function can be called from Mask Designer or from MARS */
short mask_name_retrieve(int req_type, char *from_email, char *to_email, char *orig_mask, 
    char *fab, char *tech, char *mbg, char *leader, char *leader_id,
    char *product_num, char *product_name, char *chipset, char *duedate, char *eccn, char *desc, char *remarks);

/* Checks out data from the old database */
/* The GuI then converts the data to the M2 format */
/* Override condition given if the fab files are not present */
short maskprep_checkout(char *mask, char *pin, int override_flag);

/* Disable a mask set present in the Mask Prep database */
short maskprep_disable(char *mask, char *pin, int override_flag);

/* Enable a mask set present in the Mask Prep database */
short maskprep_enable(char *mask, char *pin);

/* Read mask set from the Mask Prep database */
short maskprep_read(char *mask);

/* Assign user one or more PC numbers */
/* This function can be called from Mask Designer or from MARS */
int pc_name_retrieve(char *from_email, char *to_email,
char *fab, char *tech, char *flow, char *mbg, char *leader,
char *dante, char *product, char *chipset, char *duedate, char *eccn, char *desc);

/* Assign user one or more H numbers */
/* This function can be called from Mask Designer or from MARS */
int h_name_retrieve(int num, char *from_email, char *old_hs, char *to_email,
char *layers, char *maskshop, char *size, char *date, char *remark);


/* Retrieve one or more serial numbers for mos6 */
/* If successful returns value of first number retrieved */
short mos6serial_retrieve(char *mask, int num);

/* Retrieve one or more serial numbers for mos16 */
/* If successful returns value of first number retrieved */
short mos16serial_retrieve(char *mask, int num);

/* Retrieve one or more serial numbers for mos17 */
/* If successful returns value of first number retrieved */
short mos17serial_retrieve(char *mask, int num);

/* Retrieve one or more serial numbers for mos21 */
/* If successful returns value of first number retrieved */
short mos21serial_retrieve(char *mask, int num);

/* Retrieve approved copy of 'flow' file from specified template */
/* The file is placed in CWD under directory '.fab' */
/* This function creates the '.fab' directory if it is not present */
short flow_retrieve(char *fab, char *tech);

/* Template functions */

/* Approve a template */
/* Unapproved template is made the official copy */
/* No versioning on official template files is done */
short template_approve(char *fab, char *tech);

/* Archive a template */
short template_archive(char *fab, char *tech);

/* Unarchive a template */
short template_unarchive(char *fab, char *tech);

/* Cancel check-out on specified template */
/* If successful, all the template files are deleted */
/* from the user's disk */
short template_cancheckout(char *fab, char *tech, int override);

/* Cancel read on template */
/* Deletes files associated with the template */
/* Returns SUCCESS if transaction successful */
/* Additional design on this function is needed */
short template_canread(char *fab, char *tech);

/* Check in template files */
/* Only files owned by the calling program are checked in */
/* The tool identifies itself when it calls globals_init */
/* Major version of files is made and check out status is cancelled */
/* If check in is successful, template files are deleted */
/* from the user's disk */
/* Returns SUCCESS if check in is successful */
short template_checkin(char *fab, char *tech, int override);

/* Check out template data */
/* Global files placed in $DMI_HOME/maskdesigner/tables */
/* Fab files place in current working directory in directory '.fab' */
/* Technology files placed in current working directory */
/* .db directory from database placed in current working directory in directory '.db' */
/* override_flag is set to either DBYES or DBNO */
/* Returns SUCCESS if checkout is successful */
short template_checkout(char *fab, char *tech, int override_flag);

/* Check out prior version of template */
/* Global files placed in $DMI_HOME/maskdesigner/tables */
/* Other files put in current working directory */
/* Returns SUCCESS if checkout is successful */
short template_checkout_prior(char *fab, char *tech, int override_flag, char *version);

/* Create a new template from an existing mask set */
/* All of the files from the latest version of the mask set are */
/* copied except for the 'History' file */
/* Template files created are checked out and placed */
/* in user's current working directory */
/* The .db directory is also checked out */
/* Global files are placed in $DMIHOME/maskdesigner/tables */
/* Returns SUCCESS if create is successful */
short template_copy(char *fab, char *tech, char *mask, char *usergroup);

/* Create a new template from an existing template */
/* All of the files from the latest version of the approved template are */
/* copied except for the 'History' file */
/* The template is checked out for edit and copied to the user's disk */
/* Global files are placed in $DMIHOME/maskdesigner/tables */
short template_create(char *fab, char *tech, char *old_fab, char *old_tech, char *usergroup);

/* Delete n existing template */
/* Override condition provided for user to confirm deletion */
short template_delete(char *fab, char *tech, int override);

/* Grant override on a template */
/* Must be the owner to perform this transaction */
/* The user must supply X.500 user ID of temporary user  */
short template_grant_ovrwr(char *fab, char *tech, char *new_userid);

/* Retrieve history on specified template */
/* versions, create date, owner, state, etc. returned */
/* versions returned in an array that can be freed with files_free */
/* The rest of the information is returned in rtn.msg accessed with rtn_get */
short template_history(char *fab, char *tech, char *names[], char **msg);

/* Retrieve list of technologies in specified template */
/* List returned in array of pointers */
/* the last pointer points to NULL */
/* Calling function is responsible for freeing this list */
/* Pass in DBYES, (1) for read_archive to list archived templates, */
/* Pass in DBNO (2) to not list archived templates */
/* Returns the number of technologies retrieved */
/* Returns 0 if no technologies present */
/* Returns negative number if an error occurs */
short template_list(char *fab, char *files[], int read_archive);

/* Modify the owner of a template */
/* Must be the owner to perform this transaction */
/* The user must supply X.500 user ID of new owner */
short template_mod_owner(char *fab, char *tech, char *new_userid);

/* Perform check in with overwrite function */
/* If successful, template files deleted */
/* from the user's disk */
short template_ovrwr(char *fab, char *tech);

/* Read unofficial template files to current working directory */
/* Unofficial copy of .db directory is retrieved */
/* Official copy of fab files are placed in current working directory */
/* Global files are placed in $DMIHOME/maskdesigner/tables */
/* override_flag is set to either DBYES or DBNO */
/* Returns SUCCESS if read successful */
short template_read(char *fab, char *tech, int override_flag);

/* Read prior version of template data */
/* Global files placed in $DMI_HOME/maskdesigner/tables */
/* Other files put in current working directory */
/* Returns SUCCESS if read is successful */
short template_read_prior(char *fab, char *tech, int override_flag, char *version);

/* Retrieve official template and fab files into current working directory */
/* Also retrieves official copy of .db directory */
/* Global files are placed in $DMIHOME/maskdesigner/tables */
/* Returns SUCCESS if read successful */
short template_retrieve(char *fab, char *tech, int override_flag);

/* Save template in data base */
/* Copy the .db directory to the database */
/* No versioning is done and files saved in .tmpdb directory on database */
/* Template remains checked out and no files deleted from user's disk */
short template_save(char *fab, char *tech);

/* Force shadow of template data to all database machines */
/* Shadow done from machine that owns template */
short template_shadow(char *fab, char *tech, int server_index, char *server_site);

/* Transfer ownership of template to new database */
short template_transfer(char *fab, char *tech, char *new_site);

/* Unapprove specified template */
/* Files in approved bucket are deleted, and template is marked unapproved */
/* Returns SUCCESS if transaction successful */
short template_unapprove(char *fab, char *tech);

/* Update template in database */
/* Template remains checked out to user */
/* Major revision made, and files not deleted from user's disk */
short template_update(char *fab, char *tech, int override);

/* Retrieve versions for specified template */
/* Version number, month, day, and year version created returned in an array */
/* The array can be freed by calling files_free */
short template_versions(char *fab, char *tech, char *names[], int name_size);

/************************************************************/

/* PC functions */

/* All PC functions return M2SUCCESS (0); if transaction is successful, */
/* M2ERR for error condition, M2WRN for warning condition */
/* M2OVR for override condition */
/* Override condition can only occur if one of the parameters */
/* passed to the function is the override flag */
/* All arrays returned are NULL terminated */
/* library_name always specifies the complete PC library path */

/* All PC functions return M2SUCCESS (0) if transaction is successful, */
/* M2ERR for error condition, M2WRN for warning condition */
/* M2OVR for override_flag condition */
/* Override condition can only occur if one of the parameters */
/* passed to the function is the override_flag flag */
/* All arrays returned are NULL terminated */
/* library_name always specifies the complete PC library path */

/* Create new partition in the database */
/* New partition with complete path passed in */
/* Path names always begin with a '/' */
short partition_create(char *partition);

/* List partitions and libraries present in specified partition */
/* list is a dynamicly allocated array that holds partition/PC library names */
/* Approved PC library names preceeded with a '+' */
/* Names of PC libraries never approved preceeded with a '-' */
/* Partition names not preceeded with anything */
/* If successful, returns number of items on the list */
/* Also returns NULL terminated list of names */
/* Calling function responsible for freeing list */
short partition_list(char *path, char ***list);

/* Retrieve approved version number for specified PC library */
/* Version number returned in dynamicly allocated string */
/* Calling function must free this string */
/* If no approved version present, M2ERR returned and string not allocated */
short pc_approved_version_rtv(char *library_name, char **approved_version);

short example_array_malloc(char *partition_path, char ***partition, char *status);

/* Delete specified partition */
/* Can only be deleted if contains no other partitions or libraries */
/* Complete partition path passed to this function */
short partition_delete(char *partition);

/* Create a new PC library */
/* library_name holds complete local path to library */
/* If path does not begin with a '/' library is in current working directory */
/* database_path holds path in database where library will be placed */
short pc_create(char *remote_name, char *local_name, char *partition,char *approver, char *usergroup, int num_versions);

/* Retrieve for read copy of PC library */
/* library_name holds complete path to library */
/* The GDS file is downloaded into the user's directory */
/* The other files are put in a directory in the user's current directory */
/* This directory has the name as the GDS file preceeded with a '.' */
/* version is the version number i.e. 1.0, 1.1, etc. */
/* If reading approved version look for version number embedded in PC name */
/* normal_read set to DBYES if retrieving GDS file and meta directory */
/* normal_read set to DBNO if only retrieving GDS file */
short pc_read(char *input_library, char **output_library, char *version, 
RTV_TYPE rtv_type, int override_flag, int normal_read);

/* Check out specified PC library for edit */
/* User must be a PC library maintainer to perform this function */
/* library_name holds complete PC library path */
/* files holds names of files retrieved */
short pc_checkout(char *input_library, char **output_library, char *version,
RTV_TYPE rtv_type, int override_flag);

/* Retrieve name of directory PC libraries was checked out into */
/* Returns M2SUCCESS if PC library still present in this directory */
/* Also returns pointer to complete PC library path */
/* This memory must be freed by calling function */
/* Returns M2OVR if PC library no longer present in this directory */
/* Library name passed in holds complete database path to PC library */
/* Library name has slashes instead of colons */
/* It is in the format taken from the outliner brouser */
short pc_checkout_path_rtv(char *library_name, char **checkout_path);

/* Check in a previously checked out library */
/* library_name contains full local path followed by database path */
/* and then by actual PC name */
/* If path does not begin with '/' library is in current working directory */
short pc_checkin(char *remote_name, char *local_name);
short pc_files_send(int req_type, char *local_dir, char *root_name, char *alternate_name, char *database_path, char *users, char *usergroup);

/* User cancels check out of PC library */
/* Tries to delete library from location where it was checked out */
short pc_cancheckout(char *library_name, int override_flag);

/* Return version info for specified PC library */
/* Version info is returned in a dynamicly allocated array */
/* A '+' preceeds approved PC library versions */
/* A '-' preceeds unapproved versions */
/* The user and date for creation of each version is provided */
/* We might also want to include user and date for each approval */
/* The complete PC path starting with a '/' is passed in */
/* Calling function must deallocate the array */
short pc_versions_list(char *library_name,  char **versions[]);


/* Delete PC library from database */
/* PC library can be deleted only if there are no approved versions of the data */
/* All versions and meta data deleted by this transaction */
short pc_delete(char *library_name, int override_flag);

/* Approve specified PC library */
/* User must be a PC library approver */
/* User must also be authorized to approve this library */
short pc_approve(char *library_name, char *version, RTV_TYPE rtv_type);

/* User must be a PC library approver to unapprove a PC */
/* User must also be authorized to approve specified PC library */
short pc_unapprove(char *library_name);

char *vnum_strip(char *version_string);

/* Put PC name into correct format */
char *pc_name_format(char *library_name);

/* Retrieve history on specified PC */
/* versions, create date, owner, state, etc. returned */
/* versions returned in an arrayfree that can be freed with d2array */
/* The rest of the information is returned in rtn.msg accessed with rtn_get */
short pc_history(char *library_name, char **versions[],  char **msg);

/* View list of PC approvers */
/* Names returned in an array of pointers terminated with NULL pointer */
/* Each entry contains X.500 user ID and user's name */
/* Dynamicly allocated memory is freed by the calling function */
/* If successful, returns list of approvers */
short pc_approvers_list(char ***names);

/*  Retrieve PC history file  and put in the user's current working directory. */
/* The file name has the form :path:name.history. */
short pc_history_read(char *library_name, int override);

/*  Retrieve history file for edit and put in the user's current working directory. */
short pc_history_checkout(char *library_name, int override);

/*  Delete from user's disk and cancel check out status. */
short pc_history_cancheckout(char *library_name);

/*  Replace the current version of the file in the database with the new version from the user's disk. */
/* If update is successful, copy of file deleted from user's disk. */
short pc_history_update(char *library_name);
/* This function passes back the root PC name and the directory it is in */
void pc_name_parse(char *library_name, char **root_name, char *local_dir);

/* Retrieves the transaction log for mask, template, and PC data */
/* data_type holds data type i.e. MASK_SET, TEMPLATE, or PC */
/* key1 holds mask set name, fab name, or colon delimited PC name */
/* key2 holds technology name when retrieving template transaction  log */
/* key2 is ignored if data type is set to MASK_SET or PC */
/* file_name holds the name of file where the transaction log is put */
/* the name must be freed by the calling function */
/* Returns M2SUCCESS or M2ERR */
short transaction_log_rtv(int data_type, char *key1, char *key2, char **file_ptr);

/************************************************************/
/* User group functions */


/* Modify a user group */
/* User group is created if not already present */
/* List of users in user group is replaced if user group already present */
/* If list is blank, user given an override condition for confirmation to delete the list */
/* Anyone belonging to the user group can modify it */
/* Dynamicly allocated memory is freed by the calling function */
/* Returns M2SUCCESS if transaction successful */
/* Can return M2ERR for error condition and M2OVR for overwrite condition */
short usergroup_mod(char *usergroup, char *names[], int override);

/* Delete user group and all entries in the user group */
/* User is asked for override confirmation before group deleted */
/* Returns M2SUCCESS for successful delete */
/* Returns M2ERR for error condition and M2OVR for override condition */
short usergroup_delete(char *usergroup, int override);


/* View list of users in specified user group */
/* Names returned in an array of pointers terminated with NULL pointer */
/* Each entry contains X.500 user ID and user's name */
/* Dynamicly allocated memory is freed by the calling function */
/* If successful, returns number of users in user group */
short usergroup_list_members(char *usergroup, char **names[]);

/* Retrieve list of all defined user groups */
/* List returned in array of pointers */
/* Dynamicly allocated memory is freed by the calling function */
/* If successful, returns number of user groups in list */
short usergroup_list( char **names[]);

/* Display masks and templates assigned to a user group */
short usergroup_mask_list(char *usergroup, char **ptr);

/* Modify usergroups/users with edit permission for a mask set */
/* The user must own the mask to modify the assigned user groups */
/* The user can assign one or more user groups or one or more users to the mask */
/* Calling function passes user groups and users in null terminated arrays */
/* Calling function frees the list when function returns */
/* Returns M2SUCCESS if transaction successful */
/* Returns M2ERR if transaction failed */
short mask_usergroup_mod(char *mask, char *usergroups[], char *users[]);

/* Returns the list of users and user groups for a specified mask set */
/* returns number of elements in each list in user_cnt, and group_cnt */
/* Returns M2SUCCESS if transaction successful */
/* Calling function can free each list separately with usergroup_free */
short mask_usergroup_list(char *mask, int *group_cnt, char **group_array[], int *user_cnt, char **user_array[], char **owner);

/* Modify usergroups/users with edit permission for a template */
/* The user must own the template to modify the assigned user groups */
/* The user can assign one or more user groups or one or more users to the mask */
/* Calling function passes user groups and users in null terminated arrays */
/* Calling function frees the list when function returns */
/* Returns M2SUCCESS if transaction successful */
/* Returns M2ERR if transaction failed */
short template_usergroup_mod(char *fab, char *tech, char *usergroups[], char *users[]);

/* Returns the list of users and user groups for a specified template */
/* returns number of elements in each list in user_cnt, and group_cnt */
/* Returns M2SUCCESS if transaction successful */
short template_usergroup_list(char *fab, char *tech, int *group_cnt, char **group_array[], int *user_cnt, char **user_array[], char **owner);

/* Free memory allocated for user group functions */
void usergroup_free(char *ptr[]);

/* Add new user group in MDSadmin */
/* Returns M2SUCCESS if transaction successful */
/* Returns M2ERR if transaction failed */
short mask_usergroup_add(char *mask, char *usergroup);

/* Show list of registered users */
/* List returned in an array of pointers */
/* for each user, his name and X.500 user ID returned */
/* If successful, returns number of users in list */
/* returns M2ERR if error condition encountered */
short users_list(char **users[]);

/* Verify user is a registered Mask Designer user */
/* Returns M2SUCCESS if entry for user found */
/* Returns M2NOTREG if no registration entry found for user */
/* Returns M2DB_UNAVAIL if all databases inaccessible */
/* Returns M2WRN if user given warning */
/* Returns M2ERR for all other error conditions */
short m2user_check(char *user_id);

/* Verify user entered correct X.500 user ID and password */
/* Returns M2SUCCESS if correct X.500 user ID and password entered */
/* Returns M2NOTREG if authentication fails */
/* Returns M2DB_UNAVAIL if all databases inaccessible */
/* Returns M2WRN if user given warning */
/* Returns M2ERR for all other error conditions */
short x500check(char *user_id, char *password);

/* Verify user entered correct X.500 user ID and DMI PIN */
/* Returns M2SUCCESS if successful, */
/* Returns M2NOTREG if user not registered, */
/* Returns M2BADPIN if incorrect PIN entered */
/* Returns M2DB_UNAVAIL if all databases inaccessible */
/* Returns M2WRN if user given warning */
/* Returns M2ERR for all other error conditions */
short m2pin_check(char *user_id, char *pin);

/* Assign a new DMI PIN */
/* Returns M2SUCCESS if successful, */
/* Returns M2DB_UNAVAIL if all databases inaccessible */
/* Returns M2WRN if user given warning */
/* Returns M2ERR for all other error conditions */
short m2pin_set(char *user_id, char *pin);

/* Returns user name */
short user_name_rtv(char *user_id, char **name);

/* Returns SUCCESS (0) if successful */
/* Returns M2ERR (-1) if fails */
/* Memory allocated that must be freed if successful */

short user_first_name_rtv(char *user_id, char **first_name);

/* Retrieve user's last name from corporate email directory **/
short user_last_name_rtv(char *user_id, char **last_name);

/* Retrieve user's department number **/
short user_dept_rtv(char *user_id, char **dept);

/* Retrieve user's location code **/
short user_loc_rtv(char *user_id, char **loc);

/* Retrieve user's phone number **/
short user_phone_rtv(char *user_id, char **phone);

/* Retrieve user's email address from email corporate directory **/
/* Friendly email address retrieved if present **/
int user_rtv_email(char *user_id, char **email);

/* Retrieve user's name, phone, department and location code */
short user_rtv_all(char *user_id, char **name, char **phone, char **dept, char **loc, char **email, char **commerce_id);

/* Retrieve user's commerce ID */
short user_commerce_id_rtv(char *user_id, char **commerce_id);


/* Retrieve list of projects associated with the specified mask set */
/* If successful, returns number of projects on the list */
/* If no associated projects present, returns 0 */
/* Also returns NULL terminated list of project names */
/* Calling function responsible for freeing list */
short mask_projects_rtv(char *mask, char ***projects);

/* Find out if DTMS sign off has completed for the associated project */
/* If completed, returns SUCCESS and date completed in rtn.msg */
/* If not completed, returns M2ERR */
short dtms_status_rtv(char *project);

/* Sends reticle data via FTP */
/* Name of file to transfer passed to function */
/* Returns 0 if transfer successful */
/* Otherwise returns a negative return code */
/* Message returned is in rtn.msg */
/* Sends e-mail to user with output from FTP if transfer fails */
int ftp_put(char *file_name);

/* Retrieve the cap wafer file from the primary mask set */
/* It is placed in the .db directory */
/* Returns SUCCESS (0) if the cap wafer file is retrieved */
/* Otherwise returns M2ERR (-1) */
/* The specific reason for the error is present in rtn.msg */
short cap_wafer_file_read(char *cap_wafer);

/* Add new primary mask name to a cap wafer list */
/* If the cap wafer list does not exist, it is created */
/* Returns SUCCESS if the new name is added or is already present on the list */
/* Returns M2ERR if the name cannot be added */
/* The name is not added if the primary mask set is called 'mask' */
short cap_wafer_mask_name_add(char *cap_wafer, char *primary_mask);

/* Return list of primary mask sets for a CAP wafer */
short cap_wafer_masks_list(char *cap_wafer, char **list[]);

/* Retrieve list of all cap wafers in the database */
short cap_wafers_list(char **list[]);

/* Return the name of the cap wafer for a specified mask set */
/* Returns SUCCESS if the name is returned */
/* The calling function must provide memory where the name can be written to */
short which_cap_wafer(char *mask, char *cap_wafer);

/* Send e-mail message to user */
/* If replyto parameter set to "", uses default e-mail address */
int email_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *body);

/* Update the die per reticle in the SQL database for use by MOS12 */
short reticle_layout_update(char *mask, int rows, int cols, int die_per_shot, int program_flag);
#endif
