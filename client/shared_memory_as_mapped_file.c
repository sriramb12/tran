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

#include "mapped_file.h"
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


MAPPED_FILE mapped_file_data;

/* Create the shared memory */
int shared_memory_create(char *keyword, int block_cnt)
{
    DBG("keyword=%s block_cnt=%d", keyword, block_cnt);
    mapped_file_data = mapped_file_create(keyword, block_cnt, sizeof(struct trans_process_info));

    return((int)mapped_file_data.mem_key);
} /* end shared_memory_create */


int shared_memory_allocate(int mem_key)
{
   return DBYES;
} /* end shared_memory_allocate */

/* Write to the shared memory */
void shared_memory_write(unsigned long long size, unsigned long long checksum, int block_num, int finished)
{
    struct trans_process_info mem_ptr;
    mem_ptr.size      = size;
    mem_ptr.checksum  = checksum;
    mem_ptr.timestamp = time(0);
    mem_ptr.finished  = finished;
    mem_ptr.block_num = block_num;
    DBG("mem_ptr.block_num=%d", mem_ptr.block_num);
    DBG("mem_ptr.finished=%d", mem_ptr.finished);
    DBG("mem_ptr.size=%lld", mem_ptr.size);
    DBG("mem_ptr.checksum=%lld", mem_ptr.checksum);
    mapped_file_write(&mapped_file_data, &mem_ptr, block_num);
} /* End shared_memory_write */

/* Set finished to DBYES indicating transfer for this block has completed */
/* Update the shared memory */
void shared_memory_update( unsigned long long size, unsigned long long checksum)
{
    DBG();
    struct trans_process_info mem_ptr;
    // read existent data
    mapped_fileread(&mapped_file_data, &mem_ptr, 0);
    DBG("mem_ptr.block_num =%d", mem_ptr.block_num);
    DBG("mem_ptr.finished=%d", mem_ptr.finished);
    DBG("mem_ptr.size=%lld", mem_ptr.size);
    DBG("mem_ptr.checksum=%lld", mem_ptr.checksum);
    mem_ptr.size += size;
    mem_ptr.checksum += checksum;
    mem_ptr.timestamp = time(0);
    mem_ptr.finished += 1;
    DBG("mem_ptr.block_num =%d", mem_ptr.block_num);
    DBG("mem_ptr.finished=%d", mem_ptr.finished);
    DBG("mem_ptr.size=%lld", mem_ptr.size);
    DBG("mem_ptr.checksum=%lld", mem_ptr.checksum);
    // save new data
    mapped_file_write(&mapped_file_data, &mem_ptr, 0);
} /* End shared_memory_update */

void shared_memory_finish(int block_num)
{
    DBG();
    struct trans_process_info mem_ptr;
    // read existent data
    mapped_fileread(&mapped_file_data, &mem_ptr, block_num);
    mem_ptr.timestamp = time(0);
    mem_ptr.finished = DBYES;
    mem_ptr.block_num = block_num;
    // save new data
    mapped_file_write(&mapped_file_data, &mem_ptr, block_num);
} /* End shared_memory_write */


/* Read from shared memory */
int shared_memory_read( unsigned long long *size, unsigned long long *checksum)
{
    DBG();

    int finished_cnt = 0;
    int cnt;

    /* Set the size and checksum to 0 */
    *size = *checksum = 0;
    struct trans_process_info mem_ptr;
    /* See if the processes are done */
    /* Calculate the checksum and size */
    /* Also see if all processes have finished */
    for (cnt = 0; cnt < 10; ++cnt)
    {
        mapped_fileread(&mapped_file_data, &mem_ptr, cnt);
        if (mem_ptr.finished == DBYES)
        {
          ++finished_cnt;
        }
        *checksum += mem_ptr.checksum;
        *size += mem_ptr.size;
    } /* end for */
    /* Let calling process know have finished */
    return(finished_cnt);
} /* end shared_memory_read */

int shared_memory1read( unsigned long long *size, unsigned long long *checksum)
{
    DBG();

    int finished_cnt;
    struct trans_process_info mem_ptr;
     mapped_fileread(&mapped_file_data, &mem_ptr, 0);
    /* Read in the values */
    *checksum = mem_ptr.checksum;
    *size = mem_ptr.size;
    finished_cnt = mem_ptr.finished;
    /* Let calling process know have finished */
    return(finished_cnt);
}

/* Initialize shared memory */
void shared_memory_init( int block_num)
{
    DBG("block_num=%d", block_num);
    struct trans_process_info mem_ptr;
    /* Initialize values for this process */
    mem_ptr.size = 0;
    mem_ptr.checksum = 0;
    mem_ptr.finished = DBNO;
    mem_ptr.timestamp = time(0);
    mem_ptr.block_num = block_num;
    mapped_file_write(&mapped_file_data, &mem_ptr, block_num);
}



/* Removed the shared memory no longer needed */
void shared_memory_release(void)
{
    DBG();
    mapped_file_release(&mapped_file_data);
}


/* cleanup shared memory that has not been released */
int shared_memory_cleanup(char *user)
{
    DBG();
    shared_memory_release();
    return(SUCCESS);
} /* end shared_memory_cleanup */



