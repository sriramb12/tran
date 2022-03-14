#ifndef WINDOWS_NT4
/****************************************************************************
 *
 * Function:	Shared memory functions
 *
 * Description:	routines to allocate, access and released memory.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "global_defines.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"


#include "global_debug.h"


/* Define structure processes write to */
struct trans_process_info 
{
    unsigned int finished;
    unsigned long long size;
    unsigned long long checksum;
    time_t timestamp;
    int block_num;
};

struct trans_process_info *mem_ptr;
extern int shm_id;


/* Create the shared memory */
int shared_memory_create(char *keyword, int block_cnt)
{
    DBG();

    int key_id;
    pid_t mem_key;
    char tmp_key[8];

    strncpy(tmp_key, keyword+4, 4);
    key_id = atoi(tmp_key);
    
    /* Set the key */
     mem_key = getpid()*10000 + key_id;
    
     /* Allocate shared memory */
    if ((shm_id = shmget(mem_key, (sizeof(struct trans_process_info)*block_cnt), 0666 | IPC_CREAT | IPC_EXCL )) < 0)
    {
	    sprintf(rtn.msg, shmem_alloc_err);
	    return(SHMEM_ALLOC_ERR);
    } /* End if */

    if ((mem_ptr = (struct trans_process_info *)shmat(shm_id, NULL, 0)) < 0)
    {
	    sprintf(rtn.msg, shmem_attach_err);
	    return(SHMEM_ATTACH_ERR);
    }

    return(mem_key);
} /* end shared_memory_create */


int shared_memory_allocate(int mem_key)
{
    DBG("mem_key = '%d'", mem_key);

    /* Allocate shared memory */
    /* It is already created */
    if ((shm_id = shmget(mem_key, (sizeof(struct trans_process_info)), 0666 | IPC_CREAT)) < 0)
    {
	    sprintf(rtn.msg, shmem_alloc_err);
	    return(SHMEM_ALLOC_ERR);
    } /* End if */

    if ((mem_ptr = (struct trans_process_info *)shmat(shm_id, NULL, 0)) < 0)
    {
    	sprintf(rtn.msg, shmem_attach_err);
	    return(SHMEM_ATTACH_ERR);
    }

    return(shm_id);
} /* end shared_memory_allocate */

/* Write to the shared memory */
void shared_memory_write(unsigned long long size, unsigned long long checksum, int block_num, int finished)
{
    DBG();

    mem_ptr[block_num].size = size;
    mem_ptr[block_num].checksum = checksum;
    mem_ptr[block_num].timestamp = time(0);
    mem_ptr[block_num].finished = finished;
} /* End shared_memory_write */

/* Set finished to DBYES indicating transfer for this block has completed */
/* Update the shared memory */
void shared_memory_update( unsigned long long size, unsigned long long checksum)
{
    DBG();

    mem_ptr[0].size += size;
    mem_ptr[0].checksum += checksum;
    mem_ptr[0].timestamp = time(0);
    mem_ptr[0].finished += 1;
} /* End shared_memory_update */

void shared_memory_finish(int block_num)
{
    DBG();

    mem_ptr[block_num].timestamp = time(0);
    mem_ptr[block_num].finished = DBYES;
} /* End shared_memory_write */


/* Read from shared memory */
int shared_memory_read( unsigned long long *size, unsigned long long *checksum)
{
    DBG();

    int finished_cnt = 0;
    int cnt;

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
    return(finished_cnt);
} /* end shared_memory_read */

int shared_memory1read( unsigned long long *size, unsigned long long *checksum)
{
    DBG();

    int finished_cnt;

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
    DBG();

    /* Initialize values for this process */
    mem_ptr[block_num].size = 0;
    mem_ptr[block_num].checksum = 0;
    mem_ptr[block_num].finished = DBNO;
    mem_ptr[block_num].timestamp = time(0);
} /* end init */

/* Initialize shared memory */

    /* Removed the shared memory no longer needed */
void shared_memory_release(void)
{
    DBG();

    if (shmctl(shm_id, IPC_RMID, 0)< 0)
    {
	PRINTF(shmem_remove_err);
    } /* end if */
} /* End shared_memory_release */


/* cleanup shared memory that has not been released */
int shared_memory_cleanup(char *user)
{
    DBG();

    FILE *po;
    char command[80];
    char line[260];
    char mem_id[24];
    int cnt = 0;

    /* Build the command to read in the shared memory not released */
    strcpy2(command, "ipcs | grep ", user);
    po = popen(command, "r");
    while (line_get(po, line)  > 0)
    {
	++cnt;
	sscanf(line, "%*s %s", mem_id);
	strcpy2(command, "ipcrm -m ", mem_id);
	system(command);
    } /* end for */
    /* Close the pipe */
    pclose(po);
    /* Tell user memory removed */
    sprintf(rtn.msg, "Released shared memory for %d segments\n", cnt);
    return(SUCCESS);
} /* end shared_memory_cleanup */


#endif

