#ifdef TRANSAPI
#ifndef WINDOWS_NT4
/****************************************************************************
 *
 * Function:	block_put
 *
 * Description:	Put a block of data in the Transcend data repository.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "server_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "ssl_client_lib.h"
#include "zlib.h"

#include "global_debug.h" // Mazieri's debug facilities

unsigned long long trans_separate_block_put(int index, int num_blocks, unsigned long long *total_checksum)
{
    DBG();

    int fd;
    int cnt;
    int cnt1;
    int cnt2;
    unsigned long long checksum= 0;
    char buffer[BUFFER_SIZE];
    int ret;
    unsigned long long len = 0;
    unsigned long long total_len = 0;

    if ((fd = open(trans[index]->file_name, O_RDONLY)) < 0) 
    return(0);
    /* go to offset in file */
    /* Offset stored in original size field */
    if (lseek(fd, trans[index]->original_size, SEEK_SET) != trans[index]->original_size)
    {
    close(fd);
	return(0);
    }
    /* Set the request type for the header */
    trans[index]->req_type = DBSEPARATE_BLOCK_PUT;
    /* Connect to the server */
    if ((ret = trans_socket_init(index)) < 0)
    {
	close(fd);
	return(0);
    }
    for (cnt = 0, cnt1 = 0; cnt < num_blocks; ++cnt)
    {
	ret = read(fd, buffer, 4096);
	if (ret <= 0) break;
	for (cnt2 = 0; cnt2 < ret; ++cnt2)
	checksum += buffer[cnt2];
	put_ssl_binary_string(buffer, ret);
	len += ret;
	++cnt1;
	if (cnt1 == 100)
	{
	total_len += len;
	cnt1 = 0;
	len = 0;
	} /* end if for printed to file */
		} /* end for */
    close(fd);
    /* If all data sent not recorded update the status file */
    if (len > 0)
    total_len += len;
    /* Indicate all data for this block sent */
    put_ssl_short(SUCCESS);
    /* Get the return code */
    ret = trans_ret_code_get();
    *total_checksum = checksum;
    return(total_len);
} /* end separate_block_put */

#ifndef WINDOWS_NT4
unsigned long long trans_block_put(int index, int num_blocks)
{
    DBG();

    extern TRANS_PARAMS *trans[];
    int fd;
    int cnt;
    int cnt1;
    int cnt2;
    unsigned long long checksum= 0;
    char buffer[BUFFER_SIZE];
    int block_num = trans[index]->file_mode;
    int ret;
    long long len = 0;
    unsigned long long total_len = 0;

    DBG();
    
    /* Initialize structure for this process */
    shared_memory_init(block_num);

    if ((fd = open(trans[index]->file_name, O_RDONLY)) < 0) 
    {
        /* Write to shared memory indicating that no data sent */
        shared_memory_finish(block_num);
	    return(-1);
    } /* End if for unable to open file */

    /* go to offset in file */
    /* Offset is stored in the original_size field */
    if (lseek(fd, trans[index]->original_size, SEEK_SET) != trans[index]->original_size)
    {
	    /* Write to status file indicating that no data for this block sent */
        shared_memory_finish(block_num);
	    close(fd);
	    return(-1);
    } /* End if for unable to move to correct position in file */

    /* Set the request type for the header */
    trans[index]->req_type = DBBLOCK_PUT;

    /* Connect to the server */
    if (trans_socket_init(index) < SUCCESS)
    {
	    /* Write to status file indicating that no data for this block sent */
        shared_memory_finish(block_num);
	    close(fd);
	    return(-1);
    } /* End if for unable to connect to server */

    for (cnt = 0, cnt1 = 0; cnt < num_blocks; ++cnt)
    {
	    ret = read(fd, buffer, 4096);
	    if (ret <= 0) break;

	    for (cnt2 = 0; cnt2 < ret; ++cnt2)
	        checksum += buffer[cnt2];

	    put_ssl_binary_string(buffer, ret);
	    len += ret;
	    ++cnt1;

	    if (cnt1 == 100)
	    {
	        total_len += len;
	        shared_memory_write(total_len, checksum, block_num, DBNO);

	        cnt1 = 0;
	        len = 0;
	    } /* end if for printed to file */
	} /* end for */

    /* Indicate all data for this block sent */
    put_ssl_short(SUCCESS);

    /* Get the return code */
    ret = trans_ret_code_get();
    if (ret < 0)
    {
	    shared_memory_write(0, checksum, block_num, DBYES);
	    return(ret);
    }

    /* If all data sent not recorded update the status file */
    if (len > 0)
    {
	    total_len += len;
	    shared_memory_write(total_len, checksum, block_num, DBYES);
    }
    else
        shared_memory_finish(block_num);

    /* Must close the file after writing to shared memory */
    close(fd);

    /* Indicate transfer for this block has completed */
    return(ret);
} /* end trans_block_put */

	    int trans_random_block_put(int read_pipe, int write_pipe, int index)
{
    DBG();

    unsigned long long offset;
    unsigned long long total_len = 0;
    unsigned long long checksum = 0;
    char buffer[BUFFER_SIZE];
    int ret;
    int cnt2;
    short len = 0;

    /* Connect to the server */
    if (trans_socket_init(index) < SUCCESS)
    {
	/* Close the pipe */
	close(read_pipe);
	close(write_pipe);
	printf("Parallel process unable to connect to server.  Process exiting.\n");
	exit(1);
    }
    /* Read data from the pipe */
    /* Must read the length of the buffer, the buffer, and the offset */
    while (1)
    {
	ret = read(read_pipe, &len, 2);
	if (len == 0) break;
	/* Now get the data */
	read(read_pipe, buffer, len);
	for (cnt2 = 0; cnt2 < len; ++cnt2)
	checksum += buffer[cnt2];
	put_ssl_binary_string(buffer, len);
	/* Now get the offset */
	read(read_pipe, &offset, 8);
	put_ssl_longlong(offset);
	total_len += len;
    } /* End while */
    /* Send len to parent */
    write(write_pipe, &total_len, 8);
    write(write_pipe, &checksum, 8);
    /* Indicate all data for this block sent */
    put_ssl_short(SUCCESS);
    /* Get the return code */
    ret = trans_ret_code_get();
    exit(0);
} /* end random_block_put */

#endif
#endif
#endif
