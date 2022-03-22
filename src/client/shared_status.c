#ifdef TRANSAPI
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
#include "transapi_hidden_prototypes.h"

#include "global_debug.h" // Mazieri's debug facilities


/* Define structure processes write to */
struct transfer_status
{
    unsigned int finished;
    unsigned long long size;
};

static struct transfer_status *mem_ptr;
int shm2id;

/* Create the shared memory */
int shared_status_create(char *keyword)
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
    if ((shm2id = shmget(mem_key, (sizeof(struct transfer_status)), 0666 | IPC_CREAT | IPC_EXCL )) < 0)
    {
	    sprintf(rtn.msg, shmem_alloc_err);
	    return(SHMEM_ALLOC_ERR);
    } /* End if */

    if ((mem_ptr = (struct transfer_status *)shmat(shm2id, NULL, 0)) < 0)
    {
	    sprintf(rtn.msg, shmem_attach_err);
	    return(SHMEM_ATTACH_ERR);
    }

    return(mem_key);
} /* end shared_status_create */

int shared_status_allocate(int mem_key)
{
    DBG();

    /* Allocate shared memory */
    /* It is already created */
    if ((shm2id = shmget(mem_key, (sizeof(struct transfer_status)), 0666 | IPC_CREAT)) < 0)
    {
	    sprintf(rtn.msg, shmem_alloc_err);
	    return(SHMEM_ALLOC_ERR);
    } /* End if */

    if ((mem_ptr = (struct transfer_status *)shmat(shm2id, NULL, 0)) < 0)
    {
	    sprintf(rtn.msg, shmem_attach_err);
	    return(SHMEM_ATTACH_ERR);
    }

    return(shm2id);
} /* end shared_status_allocate */

/* Write to the shared memory */
void shared_status_write(
    int finished, unsigned long long size)
{
    DBG();

    mem_ptr->size = size;
    mem_ptr->finished = finished;
} /* End shared_status_write */

/* Write final status of transfer */
void shared_status_finish(int finished)
{
    DBG();

    mem_ptr->finished = finished;
} /* End shared_status_finish */



/* Read from shared memory */
int shared_status_read( unsigned long long *size)
{
    DBG();

    *size = mem_ptr->size;
    return(mem_ptr->finished);
} /* end shared_status_read */


    /* Removed the shared memory no longer needed */
void shared_status_release(void)
{
    DBG();

    if (shmctl(shm2id, IPC_RMID, 0)< 0)
    {
	sprintf(rtn.msg, shmem_remove_err);
    } /* end if */
} /* End shared_status_release */

#endif

#endif /* For TRANSAPI */
