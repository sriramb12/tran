#ifndef m2state_file_h
#define m2state_file_h

#include <stdio.h>
/* Used by the Mask Prep server and the Mask Archive server */

#define NO_STATE 0 	/* State of data unknown */

#include "global_defines.h"

#define GLOBAL_STATE_INPUT "%s %s %f %lu %lu %d %f %s %lu"
#define PARENT_STATE_INPUT "%*s %*s %*f %*d %*d %*d %*f %*s %lu"
#define GLOBAL_STATE_OUTPUT "%s %s %2.3f %lu\n%lu %d %2.3f %s %lu\n"
#define TOOL_STATE_INPUT "%d %s %s %s %s"
#define TOOL_STATE_OUTPUT "%d %s %s %s %s\n"
#define COMBINED_STATE_INPUT "%s %s %f %lu %lu %d %f %s %lu %d %s %s %s %s"

/* Holds data found in the global state file */
typedef struct
{
    char master_site[16];
    char c4mask[5];
    char approver[32];
    long touch_time;	/* Last time state file or data touched */
long app_time;	/* Last time template approved */
    int transaction_num;
    float vnum;
    float app_version;
    char version_type[8];
    long upd_time;	/* Last time data updated with checkin update, etc. */
    /* Holds state data for individual tools */
int pre_state;		/* Holds previous state of mask set or template */
    int state;
    char user_id[USER_LEN];	/* User ID of requester */
    char own_id[USER_LEN];
    char last_version[USER_LEN];
    char cwd[PATH_SIZE];	/* Last user's working directory */
    char usergroup[256];
    char users[256];
    int fd;
    char *line[15];
} M2STATE_FILE;

extern M2STATE_FILE st;


void state_file_crt(int state, char *usergroup);

/* Reads in values from a state file */
void state_file_read(void);

void source_state_file_read(char *delta_dir);

/* Read template state file to retrieve update time */
long template_state_file_read(void);

/* Read data from state file needed for mask list */
void state_file_list(char *mask);

/* Gets the state for update, read, approve, version and delete transactions */
/* Called by process_approve, process_update, and process_read */
int state_file_read4update(void);

/* Verifies data can be shadowed from remote server */
/* Verifies this site does not own the data */
void state_file_read4shadow(void);

/* Sees if server owns the data base */
/* Used only on remote server */
/* Returns SUCCESS if server owns data */
/* Returns NON_ERR if state file not present */
/* Otherwise returns SUCCESS */
int state_file_read4owner(void);

/* Writes out changes made to an already open state file */
void state_file_write(int index);

/* Unlock and close a state file */
void state_file_close(int index);


int state_file_lock_set(char *file_name);

void state_file_usergroup_add(FILE *fp, char *usergroup);

#endif /* ifdef for m2state_file_h */
