#ifndef delete_events_h
#define delete_events_h

#include <sys/types.h>
#include <time.h>
#define SECPERDAY 86400
/* Following structure holds deletion times for various events */
typedef struct
{
    int increment;
    int submit;
    int run;
    int complete;
    int cancel;
    int priority;
    int hold;
    int release;
    int create;
    int transfer;
    int retrieve;
    int delete;
    int list;
    int time_stamp;
} OPC_DELETE_EVENTS;

extern OPC_DELETE_EVENTS dlt;

/* Calculate deletion time for specified event */
long delete_time_calc(int days);
/* Clean up old directories no longer needed */
void opcdir_cleanup();
void projectdir_dlt(char *dir);


/* Set new delete time for specified directory */
void delete_time_set(int days);

/* Creates project directory where time stamps are placed */
void time_stamp_dir_crt();

/* Create time stamp for the first time */
void time_stamp_crt(char *user_dir);

void time_stamp_create(char *time_stamp_file, long time_stamp_int);
void time_stamp_delete(char *time_stamp_file, char *project_dir, char *mirror_dir, long dlt_time);

#endif
