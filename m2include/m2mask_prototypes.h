#include "m2globals.h"

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

	/* change the owner */
void mask_mod_owner(void);

/* Grant overwrite */
void mask_grant_overwrite(void);

void mask_cancheckout(void);

/* Delete the mask set */
void mask_delete(void);

/* Transfer mask ownership to new site */
void mask_transfer(void);

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

/* See if user is creator, owner, or last user for mask */
int user_check(M2TIME user_type, char *user, char *creator, char *owner);

/* See if mask set already present in Mask Designer */
void mask_status(void);






