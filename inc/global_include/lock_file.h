#ifndef lock_file_h
#define lock_file_h

#define TOTAL_LOCK_FILES 10
int lock_index;

char *lock_files[TOTAL_LOCK_FILES];

/* Initialize index to be 0 */
/* Point all lock files to NULL */
void lock_files_init(void);

/* Lock specified file */
/* Allocate space for the lock file name and increment index */
int lock_file_set(char *file_name);

/* Unlock file and free memory */
void lock_file_clear(int index);

/* Unlock all files locked and free memory */
/* Called when exiting with error condition */
void lock_files_clear(void);
/* Tries to access the file specified number of times */
/* If the PID for the start period is the same as for the end time */
/* assumes process has aborted and locks the file */
/* If the PID is not the same, gives error */
int lock_file_wait(char *lock_file, int wait_time);

int lock_file_read(char *lock_file);

/* Change the PID stored in the lock file */
int lock_file_mod(char *lock_file);

#endif
