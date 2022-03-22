#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	block_get
 *
 * Description:	Retrieve a block of data from the Transcend server.
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
#include "rtn.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "client_lib.h"
#include "zlib.h"


unsigned long long separate_block_get(char *file_name, char *keyword,  unsigned long long offset, int num_blocks, int block_num, int server_mem_key, unsigned long long *total_checksum)
{
    int fd;
    int cnt;
    int cnt1 = 0;
    unsigned long long checksum= 0;
    char buffer[BUFFER_SIZE];
    char block_num_string[8];
    char server_mem_string[16];
    int ret;
    unsigned long long len = 0;
    unsigned long long len1 = 0;
    unsigned long long transferred_len = 0;
    unsigned long long total_len = 0;

    fflush(stdout);
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0600)) < 0) 
	return(0);
    /* go to offset in file */
    if (lseek(fd, offset, SEEK_SET) != offset)
    {
	close(fd);
	return(0);
    } /* end if */
    /* Connect to the server */
    /* Put block number in string format */
    sprintf(block_num_string, "%d", block_num);
    sprintf(server_mem_string, "%d", server_mem_key);
    if ((ret = init_trans_socket(DBSEPARATE_BLOCK_GET, keyword, NONE, NONE, offset, num_blocks, 
    UNKNOWN, block_num_string, server_mem_string, NONE4STRING)) < SUCCESS) 
    {
	close(fd);
	return(0);
    } /* end if */
    /* Write text */
    while ((len = get_string(buffer)) > 0)
    {
	for (cnt = 0; cnt < len; ++cnt)
	checksum += buffer[cnt];
	len1 = write(fd, buffer, len);
	if (len1 != len)
	{
	    close(fd);
	        return(0);
	} /* end if */
	else transferred_len += len;
	++cnt1;
	if (cnt1 == 100)
	{
	total_len += transferred_len;
	cnt1 = 0;
	transferred_len = 0;
	} /* end if for printed to file */
    } /* end while */
    close(fd);
    if (transferred_len > 0)
	total_len += transferred_len;
    /* Get the return code */
    if ((ret = get_ret_code()) >= 0)
    {
	/* Update the total check sum */
	*total_checksum = checksum;
	return(total_len);
    } else {
	*total_checksum = 0;
	return(0);
    }
} /* end separate_block_get */

#ifndef WINDOWS_NT4
/* This function is used for parallel block gets and uses shared memory */
short block_get(char *file_name, char *keyword,  unsigned long long offset, int num_blocks, int block_num, int server_mem_key)
{
    int fd;
    int cnt;
    int cnt1 = 0;
    unsigned long long checksum= 0;
    char buffer[BUFFER_SIZE];
    char block_num_string[8];
    char server_mem_string[16];
    int ret;
    long long len = 0;
    long long len1 = 0;
    long long transferred_len = 0;
    long long total_len = 0;

    /* Initialize structure for this process */
    shared_memory_init(block_num);
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0600)) < 0) 
    {
    /* Write to shared memory indicating that no data sent */
    shared_memory_finish(block_num);
	return(-1);
    } /* End if for unable to open file */
    /* go to offset in file */
    if (lseek(fd, offset, SEEK_SET) != offset)
    {
    /* Write to shared memory indicating that no data sent */
    shared_memory_finish(block_num);
	close(fd);
	return(-1);
    } /* End if for unable to move to correct position in file */
    /* Connect to the server */
    /* Put block number in string format */
    sprintf(block_num_string, "%d", block_num);
    sprintf(server_mem_string, "%d", server_mem_key);
    if ((ret = init_trans_socket(DBBLOCK_GET, keyword, NONE, NONE, offset, num_blocks, 
    UNKNOWN, block_num_string, server_mem_string, NONE4STRING)) < SUCCESS) 
    {
    /* Write to shared memory indicating that no data sent */
    shared_memory_finish(block_num);
	close(fd);
	return(ret);
    } /* End if for unable to connect to server */
    /* Write text */
    while ((len = get_string(buffer)) > 0)
    {
	for (cnt = 0; cnt < len; ++cnt)
	checksum += buffer[cnt];
	len1 = write(fd, buffer, len);
	if (len1 != len)
	{
	    /* Write to shared memory indicating that no data sent */
	    shared_memory_write(0, 0, block_num, DBYES);
	    close(fd);
	    return(-1);
	} /* end if for length not the same */
	else transferred_len += len;
	++cnt1;
	if (cnt1 == 100)
	{
	total_len += transferred_len;
	    shared_memory_write(total_len, checksum, block_num, DBNO);
	transferred_len = 0;
	cnt1 = 0;
	} /* end if for printed to file */
    } /* end while */
    if (transferred_len > 0)
    {
	total_len += transferred_len;
	    shared_memory_write(total_len, checksum, block_num, DBYES);
    }
    else shared_memory_finish(block_num);
    /* Get the return code */
    ret = get_ret_code();
    /* Must close the file after writing to shared memory */
    close(fd);
    return(ret);
} /* end block_get */

/* Get random blocks of data from the server */
	    int random_block_get(int fi, char *keyword, 
    long long offset, int num_blocks, int block_num,
    int server_mem_key, int block_size)
{
    char buffer[BUFFER_SIZE];
    char block_num_string[8];
    char server_mem_string[16];
    int ret;
    short len = 0;

    /* Connect to the server */
    /* Put block number in string format */
    sprintf(block_num_string, "%d", block_num);
    sprintf(server_mem_string, "%d", server_mem_key);
    if ((ret = init_trans_socket(DBRANDOM_BLOCK_GET, keyword, NONE, NONE, offset, num_blocks, 
    UNKNOWN, block_num_string, server_mem_string, NONE4STRING)) < SUCCESS) 
    {
	len = 0;
	write(fi, &len, 2);
	exit(1);
    } /* end if */
    /* Write text */
    while ((len = get_string(buffer)) > 0)
    {
    /* Write data to the pipe */
    /* Must write the length of the buffer, the buffer, and the offset */
	write(fi, &len, 2);
	write(fi, buffer, len);
    } /* end while */
    /* Tell parent all data retrieved */
    len = 0;
    write(fi, &len, 2);
    /* Get the return code */
    ret = get_ret_code();
    return(ret);
} /* end random_block_get */
#endif
#endif /* For not TRANSAPI */
