#ifndef TRANSAPI
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
#include "hash.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "client_lib.h"
#include "zlib.h"


unsigned long long separate_block_put(
    char *file_name, int server_mem_key,  char *keyword, int num_blocks, 
    unsigned long long offset, int block_num, unsigned long long *total_checksum)
{
    int fd;
    int cnt;
    int cnt1;
    int cnt2;
    unsigned long long checksum= 0;
    char buffer[BUFFER_SIZE];
    char server_mem_string[16];
    int ret;
    unsigned long long len = 0;
    unsigned long long total_len = 0;

    if ((fd = open(file_name, O_RDONLY)) < 0) 
    return(0);
    /* go to offset in file */
    if (lseek(fd, offset, SEEK_SET) != offset)
    {
    close(fd);
	return(0);
    }
    /* convert mem key to string */
    sprintf(server_mem_string, "%d", server_mem_key);
    /* Connect to the server */
    if ((ret = init_trans_socket(DBSEPARATE_BLOCK_PUT, keyword, NONE, NONE, offset, block_num, 
    UNKNOWN, NONE, server_mem_string, NONE4STRING)) < SUCCESS) 
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
	put_binary_string(buffer, ret);
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
    put_short(SUCCESS);
    /* Get the return code */
    ret = get_ret_code();
    *total_checksum = checksum;
    return(total_len);
} /* end separate_block_put */

#ifndef WINDOWS_NT4
long long block_put(char *file_name, int server_mem_key,  char *keyword, int num_blocks, long long offset, int block_num)
{
    int fd;
    int cnt;
    int cnt1;
    int cnt2;
    unsigned long long checksum= 0;
    char buffer[BUFFER_SIZE];
    char server_mem_string[16];
    int ret;
    long long len = 0;
    unsigned long long total_len = 0;

    /* Initialize structure for this process */
    shared_memory_init(block_num);
    if ((fd = open(file_name, O_RDONLY)) < 0) 
    {
    /* Write to shared memory indicating that no data sent */
    shared_memory_finish(block_num);
	return(-1);
    } /* End if for unable to open file */
    /* go to offset in file */
    if (lseek(fd, offset, SEEK_SET) != offset)
    {
	/* Write to status file indicating that no data for this block sent */
    shared_memory_finish(block_num);
	close(fd);
	return(-1);
    } /* End if for unable to move to correct position in file */
    /* convert mem key to string */
    sprintf(server_mem_string, "%d", server_mem_key);
    /* Connect to the server */
    if ((ret = init_trans_socket(DBBLOCK_PUT, keyword, NONE, NONE, offset, block_num, 
    UNKNOWN, NONE, server_mem_string, NONE4STRING)) < SUCCESS) 
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
	put_binary_string(buffer, ret);
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
    put_short(SUCCESS);
    /* Get the return code */
    ret = get_ret_code();
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
    else shared_memory_finish(block_num);
    /* Must close the file after writing to shared memory */
    close(fd);
    /* Indicate transfer for this block has completed */
    return(ret);
} /* end block_put */

	    int random_block_put(int read_pipe, int write_pipe, char *keyword, int server_mem_key,  int block_num)
{
    unsigned long long offset;
    unsigned long long total_len = 0;
    unsigned long long checksum = 0;
    char buffer[BUFFER_SIZE];
    char server_mem_string[16];
    int ret;
    int cnt2;
    short len = 0;

    /* convert mem key to string */
    sprintf(server_mem_string, "%d", server_mem_key);
    /* Connect to the server */
    if ((ret = init_trans_socket(DBRANDOM_BLOCK_PUT, keyword, NONE, NONE, 0, block_num, 
    UNKNOWN, NONE, server_mem_string, NONE4STRING)) < SUCCESS) 
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
	put_binary_string(buffer, len);
	/* Now get the offset */
	read(read_pipe, &offset, 8);
	put_longlong(offset);
	total_len += len;
    } /* End while */
    /* Send len to parent */
    write(write_pipe, &total_len, 8);
    write(write_pipe, &checksum, 8);
    /* Indicate all data for this block sent */
    put_short(SUCCESS);
    /* Get the return code */
    ret = get_ret_code();
    exit(0);
} /* end random_block_put */

#endif
#endif /* For not TRANSAPI */
