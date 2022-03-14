/****************************************************************************
 *
 * Function:	Shared memory functions
 *
 * Description:	routines to allocate, access and released memory.
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <strings.h>

#include "global_defines.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_errs.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h"

/* Define structure processes write to */
struct trans_process_info 
{
    long long size;
    unsigned long long checksum;
    time_t timestamp;
    pid_t pid;
    int block_num;
    int finished;
};

struct trans_process_info *mem_ptr;
extern int shm_id;


/* Create the shared memory */
int shared_memory_create(char *keyword, int block_cnt)
{
    int key_id;
    pid_t mem_key;
    char tmp_key[8];

    DBG("keyword=%s", keyword);

    strncpy(tmp_key, keyword+4, 4);
    key_id = atoi(tmp_key);

    /* Set the key */
     mem_key = getpid()*10000 + key_id;

    /* Allocate shared memory */
    if ((shm_id = shmget(mem_key, (sizeof(struct trans_process_info)*block_cnt), 0666 | IPC_CREAT | IPC_EXCL )) < 0)
    {
	    PRINTF(shmem_alloc_err);
	    shut_down(shmem_alloc_err, SHMEM_ALLOC_ERR);
    } /* End if */

    if ((mem_ptr = (struct trans_process_info *)shmat(shm_id, NULL, 0)) < 0)
    {
	    PRINTF(shmem_attach_err);
	    shut_down(shmem_attach_err, SHMEM_ATTACH_ERR);
    }

    DBG("shared_memory_create() will return");
    return(mem_key);
} /* end shared_memory_create */


int shared_memory_allocate(char *keyword, int mem_key, int block_cnt)
{
    DBG("keyword=%s", keyword);

    /* Allocate shared memory */
    /* It is already created */
    if ((shm_id = shmget(mem_key, (sizeof(struct trans_process_info)*block_cnt), 0666 | IPC_CREAT)) < 0)
    {
        DBG("Shared Memory already created, will call shut_down()");
	    PRINTF(shmem_alloc_err);
	    shut_down(shmem_alloc_err, SHMEM_ALLOC_ERR);
    } /* End if */

    if ((mem_ptr = (struct trans_process_info *)shmat(shm_id, NULL, 0)) < 0)
    {
        DBG("Shared Memory was not proper attached, will call shut_down()");
	    PRINTF(shmem_attach_err);
	    shut_down(shmem_attach_err, SHMEM_ATTACH_ERR);
    }

    DBG("shared_memory_create() will returns");
    return(shm_id);
} /* end shared_memory_allocate */


int shared_memory1allocate(char *keyword, int mem_key)
{
    DBG("keyword=%s", keyword);

    /* Allocate shared memory */
    /* It is already created */
    if ((shm_id = shmget(mem_key, (sizeof(struct trans_process_info)), 0666 | IPC_CREAT)) < 0)
    {
	    PRINTF(shmem_alloc_err);
	    shut_down(shmem_alloc_err, SHMEM_ALLOC_ERR);
    } /* End if */

    if ((mem_ptr = (struct trans_process_info *)shmat(shm_id, NULL, 0)) < 0)
    {
	    PRINTF(shmem_attach_err);
	    shut_down(shmem_attach_err, SHMEM_ATTACH_ERR);
    }

    DBG("Will return shm_id");
    return(shm_id);
} /* end shared_memory1allocate */

/* Write to the shared memory */
void shared_memory_write(
    long long size, unsigned long long checksum, int block_num, int finished)
{
    DBG("block_num=%d", block_num);
    mem_ptr[block_num].size = size;
    mem_ptr[block_num].checksum = checksum;
    mem_ptr[block_num].timestamp = time(0);
    mem_ptr[block_num].finished = finished;
} /* End shared_memory_write */

/* Update the shared memory */
void shared_memory_update(
    long long size, unsigned long long checksum)
{
    DBG();
    mem_ptr[0].size += size;
    mem_ptr[0].checksum += checksum;
    mem_ptr[0].timestamp = time(0);
    mem_ptr[0].finished += 1;
} /* End shared_memory_update */

/* Set finished to DBYES indicating transfer for this block has completed */
void shared_memory_finish(int block_num)
{
    DBG("block_num=%d", block_num);
    mem_ptr[block_num].timestamp = time(0);
    mem_ptr[block_num].finished = DBYES;
} /* End shared_memory_finish */


/* Read from shared memory */
int shared_memory_read(long long *size, unsigned long long *checksum)
{
    int finished_cnt = 0;
    int cnt;

    //DBG("size='%lld', checksum='%lld'", size, checksum);

    /* Set the size and checksum to 0 */
    *size = *checksum = 0;

    /* See if the processes are done */
    /* Calculate the checksum and size */
    /* Also see if all processes have finished */
    for (cnt = 0; cnt < 10; ++cnt)
    {
	    if (mem_ptr[cnt].finished == DBYES)
	        ++finished_cnt;

	    *checksum += mem_ptr[cnt].checksum;
	    *size += mem_ptr[cnt].size;
    } /* end for */

    /* Let calling process know have finished */
    //DBG("finished_cnt='%d'", finished_cnt);
    return(finished_cnt);
} /* end shared_memory_read */


int shared_memory1read( long long *size, unsigned long long *checksum)
{
    int finished_cnt;


    DBG();

    /* Read in the values */
	*checksum = mem_ptr[0].checksum;
	*size = mem_ptr[0].size;
	finished_cnt = mem_ptr[0].finished;
    /* Let calling process know have finished */
    return(finished_cnt);
} /* end shared_memory1read */

/* Initialize shared memory */
void shared_memory_init( int block_num)
{
    DBG("block_num=%d", block_num);

    /* Initialize values for this process */
    mem_ptr[block_num].size = 0;
    mem_ptr[block_num].checksum = 0;
    mem_ptr[block_num].finished = -1;
    mem_ptr[block_num].timestamp = time(0);
} /* end init */

/* Initialize shared memory */
void shared_memory1init(void)
{
    DBG();

    /* Initialize values for this process */
    mem_ptr[0].size = 0;
    mem_ptr[0].checksum = 0;
    mem_ptr[0].finished = 0;
    mem_ptr[0].timestamp = time(0);
} /* end init */

    /* Removed the shared memory no longer needed */
void shared_memory_release(void)
{
    DBG("shm_id=%d", shm_id);
    if (shmctl(shm_id, IPC_RMID, 0)< 0)
    {
	PRINTF(shmem_remove_err);
    } /* end if */
} /* End shared_memory_release */

/* cleanup shared memory that has not been released */
/* This can be run only by the system administrator */
void shared_memory_cleanup(void)
{
    FILE *po;
    char server_user[16];
    char command[80];
    char line[260];
    char mem_id[24];
    int cnt = 0;

    DBG();

    /* First get the user this account is running under */
    if ((po = popen("whoami", "r")) == 0)
    shut_down("Could not open whoami command", -1);
    fscanf(po, "%s", server_user);
    pclose(po);
    /* Now build the command to read in the shared memory not released */
    strcpy2(command, "ipcs | grep ", server_user);
    po = popen(command, "r");
    while (line_get(po, line)  > 0)
    {
	++cnt;
	sscanf(line, "%*s %*s %s", mem_id);
	strcpy2(command, "ipcrm -M ", mem_id);
	system(command);
    } /* end for */
    /* Close the pipe */
    pclose(po);
    /* Tell user memory removed */
    sprintf(rtn.msg, "Removed shared memory for %d segments\n", cnt);
    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
} /* end shared_memory_cleanup */
