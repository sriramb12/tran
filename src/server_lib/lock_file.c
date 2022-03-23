/****************************************************************************
 *
 * Functions:	lock_file_set, lock_file_clear
 *
 * Lock and unlock specified file
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "lock_file.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities


extern char *lock_files[TOTAL_LOCK_FILES];

/* Initialize index to be 0 */
/* Point all lock files to NULL */
void lock_files_init(void)
{
    int cnt;

    DBG();

    for (cnt = 0; cnt < TOTAL_LOCK_FILES; ++cnt)
        lock_files[cnt] = NULL;
} /* End lock_files_init */


/* Lock specified file */
/* Allocate space for the lock file name and increment index */
int lock_file_set(char *file_name)
{
    int index;
    DBG("file_name='%s'", file_name);

    /* If the file does not exist, do not bother locking it */
    if (access(file_name, 0) != 0) return(-1);

    /* look for first open slot in array */
    for (index = 0; index < TOTAL_LOCK_FILES; ++index)
        if (lock_files[index] == NULL) break;

    /* Allocate space for lock file */
    lock_files[index] = malloc(strlen(file_name) + 6);

    if (lock_files[index] == NULL)
        close_down1m(malloc_err, file_name, MALLOC_ERR);

    /* build lock file name */
    strcpy2(lock_files[index], file_name, ".lock");

     /* Try to lock the file up to ten times */
    if ( lock_file_wait(lock_files[index], 5) == SUCCESS) return(index);

    /* if reach this point, unable to lock file */
    /* Assume that process using the file has aborted */
    else close_down1m(lock_err, file_name, LOCK_ERR);

    return(SUCCESS);
} /* End lock_file_set */


/* Unlock file and free memory */
void lock_file_clear(int index)
{

    DBG();

    /* If invalid index given, return */
    if ((index < 0) || (index > TOTAL_LOCK_FILES)) return;

    /* If null file name found return */
    if (lock_files[index] == NULL) return;

    /* If lock file not present, return */
    if (access(lock_files[index], 0) == 0) 
    {
	    if (unlink(lock_files[index]) != 0)
	        close_down1m(unlock_err, lock_files[index], UNLOCK_ERR);
    } /* End if for unable to unlock */

    /* Free memory no longer needed */
    free(lock_files[index]);

    lock_files[index] = NULL;
} /* End lock_file_clear */


/* Unlock all files locked and free memory */
/* Called when exiting with error condition */
void lock_files_clear(void)
{

    DBG();

    int cnt;

    for (cnt = 0; cnt < TOTAL_LOCK_FILES; ++cnt)
    {
	    /* Skip it if not allocated */
	    if (lock_files[cnt] == NULL) continue;
	    /* Delete it if present */
	    if (access(lock_files[cnt], 0) == 0) unlink(lock_files[cnt]);
	
	    /* Free memory no longer needed */
	    free(lock_files[cnt]);
    } /* End for */

} /* End lock_files_clear */


/* Tries to access the file specified number of times */
/* If the PID for the start period is the same as for the end time */
/* assumes process has aborted and locks the file */
/* If the PID is not the same, gives error */
int lock_file_wait(char *lock_file, int wait_time)
{
    int fd;
    int old_pid = 0;
    int new_pid = 0;
    int cnt;

    DBG();

    old_pid = lock_file_read(lock_file);
    for (cnt = 0; cnt <wait_time; ++cnt)
    {
	    if (access(lock_file, 0) != 0) 
	    {
	        fd = open(lock_file, O_CREAT | O_WRONLY, 0777);
	        if (fd >= 0)
	        {
	            write(fd, tr.pid_string, strlen(tr.pid_string));
	            close(fd);
	            return(SUCCESS);
	        } /* End if for successful lock */
	    } /* End if for file not present */

	    /* Sleep for a second to see if file is now unlocked */
	    sleep(1);
    } /* End for */

    new_pid = lock_file_read(lock_file);
    
    if (old_pid == new_pid) return(lock_file_mod(lock_file));
    /* If reached this point, could not lock the file */
    else return(FAILURE);
} /* End lock_file_wait */


int lock_file_read(char *lock_file)
{
    int pid;
    FILE *fp;

    DBG();

    if ((fp = fopen(lock_file, "r")) ==   NULL)
        return(0);

    fscanf(fp, "%d", &pid);
    fclose(fp);

    return(pid);
} /* End lock_file_read */


/* Change the PID stored in the lock file */
int lock_file_mod(char *lock_file)
{
    FILE *fp;

    DBG();

    if ((fp = fopen(lock_file, "w")) == NULL) 
        close_down1m(wr_open_err, lock_file, WR_OPEN_ERR);

    fprintf(fp, "%d", tr.pid);
    fclose(fp);

    return(SUCCESS);
} /* End lock_file_mod */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
