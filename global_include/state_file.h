#ifndef state_file_h
#define state_file_h

/* Used by the Mask Prep server and the Mask Archive server */

#define NO_STATE 0 	/* State of data unknown */

#include "global_defines.h"

/* Holds data found in the state file */
typedef struct
{
    char pin[MAX_PIN_LEN+1];
    char master_site[32];
    char own_id[32];
    char own_email[MACHINE_LEN];
    char own_addr[ADDRESS_LEN];
    char tmp_id[USER_LEN];
    char tmp_email[MACHINE_LEN];
    char user_id[USER_LEN];	/* User ID of requester */
    char user_email[EMAIL_LEN];	/* Machine name of last user to access data */
    char user_addr[ADDRESS_LEN];	/* IP node address of last user to access data */
    char user_cwd[PATH_SIZE];	/* Last user's working directory */
    char pc_link[FILE_SIZE];
int read_cnt;
int lock_cnt;		/* Number of times user tried to access mask set */
int state;		/* Holds current state of the mask set or template */
int pre_state;		/* Holds previous state of mask set or template */
    int transaction_num;
    char app_state[5];
    long upd_time;
    long touch_time;
    float vnum;
    char archive_dir[FILE_SIZE];
} STATE_FILE;

extern STATE_FILE st;

int
state_file_read(char *state_file);


void
state_file_set(
    int new_state,
    short increment);


void
sec_state_file_init(void);

/* Set approved state */
void
state_file_set_app(void);

void
state_file_crt(
    int state,
    char *state_file,
    char *pin,
    char *pc_link,
    float *vnum);

int 
state_get_off(
    char *delta_dir,
    char *key);

void
state_file_get(void);

void
state_file_get_sec(void);

#endif /* ifdef for state_file_h */
