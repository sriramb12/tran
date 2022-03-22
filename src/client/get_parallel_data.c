#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	get_parallel_file
 *
 * Description:	Retrieves a big file or tar file. in parallel mode
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
     #include <sys/types.h>
     #include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "client_lib.h"
#include "client_prototypes.h"
#include "global_defines.h"
#include "global_params.h"
#include "hash.h"
#include "server_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"
#include "rtn.h"
#include "strcpy.h"

#ifndef WINDOWS_NT4


int get_parallel_compressed_data(REQ_TYPE req_type, char *file_name, char *keyword,
long long total_size, long mode, TRANS_FILE_TYPE file_type)
{
    extern int size_printed;
    int mem_key;
    int server_mem_key;
    unsigned long long checksum;
    unsigned long long offset;
    long starttime = time(0);
    int num_blocks;
    int finished;
    int cnt = 0;
    int cnt1;
    short ret;
    unsigned long long transferred_size;
    unsigned long long old_transferred_size = 0;
    char server_mem_string[16];
    char buffer[80];

    /* Print size of file being retrieved */
    trans_hash_init(total_size, UNCOMPRESSED, "r", PARALLEL);
    /* Indicate size not printed */
    size_printed = DBNO;
    /* Initialize the shared memory */
    mem_key = shared_memory_create(keyword, 10);
    /* Spawn off children to perform the transfers */
    /* The server will send the offset for 10 children */
    /* Get offset data from the server */
    while (1)
    {
	if (get_string(buffer) <= 0) break;
	sscanf(buffer, "%d %llu %d", &num_blocks, &offset, &server_mem_key);
	if (fork() == 0)
	{
	    /* Try to get the block 3 times */
	    for (cnt1 = 0; cnt1 < 3; ++cnt1)
	    {
		ret = block_get(file_name, keyword, offset, num_blocks, cnt, server_mem_key);
	close(sock);
	if (ret >= 0) break;
	    } /* end inner for */
	    exit(0);
	} /* End if for child */
    ++cnt;
    } /* End while */
    /* the parent waits for children to finish in a while */
    /* If this is a large parallel transfer, get return code */
    if (req_type == DBGET_LARGE_PARALLEL)
    ret = get_ret_code();
    /* Wait a couple of seconds before checking on children */
    sleep(2);
    while (1)
    {
	finished = shared_memory_read(&transferred_size, &checksum);
	if ((finished >= NUM_PARALLEL_PROCESSES)
		|| (transferred_size >= total_size)) break;
		/* print number of bytes transferred */
    if (transferred_size - old_transferred_size > 0)
	trans_hash(transferred_size-old_transferred_size, 0);
	old_transferred_size = transferred_size;
	sleep(2);
    } /* End while */
    trans_hash_close();
    if (finished >= NUM_PARALLEL_PROCESSES && transferred_size == 0)
    {
	printf(bad_trans_write, file_name);
	exit(-1);
    }
    if (size_printed == DBYES) PRINTF("\n");
    /* If transferring large parallel data, get ack */
    if (req_type == DBGET_LARGE_PARALLEL)
    {
	/* convert mem key to string */
	sprintf(server_mem_string, "%d", server_mem_key);
	/* Send a get ack request to the server */
	if ((ret = init_trans_socket(DBGET_ACK, keyword, NONE, NONE, 0, 0,
	UNKNOWN, NONE, server_mem_string, NONE4STRING)) < SUCCESS)
	return(ret);
    } /* end if */
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_longlong(transferred_size);
    put_longlong(checksum);
    if (req_type == DBGET_LARGE_PARALLEL)
    put_long(time(0)-starttime);
    /* The parent process returns message to user interface */
    ret = get_ret_code();
    /* Release the shared memory */
    shared_memory_release();
    close(sock);
    return(ret);
} /* End get_parallel_compressed_data */

/* Retrieve data that is uncompressed as it is written to disk */
int get_parallel_uncompressed_data(char *file_name, char *keyword,
long long total_size, long mode, TRANS_FILE_TYPE file_type)
{
    extern int size_printed;
    extern float compression_ratio;
    int fi_array[10][2];
    short len;
    int server_mem_key;
    unsigned long long checksum = 0;
    unsigned long long offset;
    int num_blocks;
    int finished = DBNO;
    int cnt = 0;
    int cnt1;
    short ret;
    unsigned long long transferred_size = 0;
    unsigned long long new_transferred_size = 0;
    char buffer[BUFFER_SIZE];
    char command[80];
    FILE *po;

    trans_hash_init(total_size, UNCOMPRESSED, "r", PARALLEL);
    /* Indicate size not printed */
    size_printed = DBNO;
    /* Spawn off children to perform the transfers */
    /* The server will send indication to spawn off 10 children */
    while (1)
    {
	if (get_string(buffer) <= 0) break;
	sscanf(buffer, "%d %llu %d", &num_blocks, &offset, &server_mem_key);
	ret = pipe(fi_array[cnt]);
	if (fork() == 0)
	{
	    random_block_get(fi_array[cnt][1], keyword, offset, num_blocks, cnt, server_mem_key, 4096);
	    exit(0);
	} /* End if for child */
    ++cnt;
    } /* End while */
    /* The parent now gets data from the children in a round robin fashion and unzips it before it is written to disk */
    /* Must build the command to get the data */
    if ((file_type == DIRECTORY)
    || (file_type == FILES))
    strcpy(command, "gtar zxf -");
    else strcpy2(command, "gunzip - >", file_name);
    po = popen(command, "w");
    while (1)
    {
      for (cnt = 0; cnt < 10; ++cnt)
      {
    /* Read data from the pipe */
	/* Must read the lenth and then the data */
	ret = read(fi_array[cnt][0], &len, 2);
	if ((len <= 0)
    || (ret < 2))
	{
	    finished = DBYES;
	    transferred_size += new_transferred_size;
	    break;
	} /* end if for found the end */
	read(fi_array[cnt][0], buffer, len);
	new_transferred_size += len;
	for (cnt1 = 0; cnt1 < len; ++cnt1)
	checksum += buffer[cnt1];
	/* Now write the data unzipping it etc */
	fwrite(buffer, 1, len, po);
      } /* end for */
      if (finished == DBYES) break;
	trans_hash(new_transferred_size*compression_ratio, 0);
	/* Add size for this lloop to total size */
	transferred_size += new_transferred_size;
	/* Set back to 0 for next time around the loop */
	new_transferred_size = 0;
    } /* End while */
    pclose(po);
    trans_hash_close();
    if (size_printed == DBYES) PRINTF("\n");
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_longlong(transferred_size);
    put_longlong(checksum);
    /* The parent process returns message to user interface */
    ret = get_ret_code();
    close(sock);
    return(ret);
} /* End get_parallel_uncompressed_data */
#endif

#if 0 // ifndef LINUX64 && ifndef LINUX32
int get_separate_compressed_data(char *file_name, char *keyword,
long long total_size, long mode, TRANS_FILE_TYPE file_type)
{
    int fi[2];
    extern int size_printed;
    int server_mem_key;
    unsigned long long checksum;
    unsigned long long total_checksum = 0;
    unsigned long long transferred_size;
    unsigned long long total_transferred_size = 0;
    unsigned long long offset;
    unsigned long long total_offset = 0;
    int num_blocks;
    int finished = 0;
    int cnt = 0;
    int block_cnt;
    int cnt1;
    short ret;
    char buffer[80];

    /* Print size of file being retrieved */
    trans_hash_init(total_size, 1, UNCOMPRESSED, "r", SEPARATE_BLOCKS);
    /* Indicate size not printed */
    size_printed = DBNO;
    /* The server will send the offset used */
    /* Get offset data from the server */
    get_string(buffer);
	sscanf(buffer, "%d %d %llu %d", &block_cnt, &num_blocks, &offset, &server_mem_key);
    /* Get the extra short indicating data sent successfully */
    ret = get_short();
	/* Must set up a pipe */
	pipe(fi);
    for (cnt = 0; cnt < block_cnt; ++cnt)
    {
	    /* Try to get the block 3 times */
	    for (cnt1 = 0; cnt1 < 3; ++cnt1)
	    {
	/* Must fork here */
	if (fork() == 0)
	{
	    /* This is the child */
		transferred_size = separate_block_get(file_name, keyword, total_offset, num_blocks, cnt, server_mem_key, &checksum) ;
	    /* Successful if transferred_size > 0 */
	    if (transferred_size > 0) ret = 0;
	    /* Send return to parent */
	    write(fi[0], &ret, 2);
	    /* Send the number of bytes transferred and the check sum */
	    write(fi[0], &transferred_size, 8);
	    write(fi[0], &checksum, 8);
	    exit(ret);
	} /* End if for child */
	    else
	    {
		/* This is the parent who waits for the child to finish */
	read(fi[1], &ret, 2);
	read(fi[1], &transferred_size, 8);
	read(fi[1], &checksum, 8);
	    /* If successful add size and checksum */
	if (ret == 0)
	{
	    trans_hash(transferred_size, 1);
		total_transferred_size += transferred_size;
		total_checksum += checksum;
	    total_offset += offset;
	    /* Indicate one more block transferred */
	    ++finished;
		break;
	} /* end if for successful transfer */
	} /* End else for parent */
	    } /* end inner for */
    if (ret != SUCCESS) break;
    } /* End for */
    trans_hash_close();
    if (finished < block_cnt || ret != 0)
    {
	printf(bad_trans_write, file_name);
	exit(-1);
    }
    if (size_printed == DBYES) PRINTF("\n");
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_longlong(total_transferred_size);
    put_longlong(total_checksum);
    /* The parent process returns message to user interface */
    ret = get_ret_code();
    close(sock);
    return(ret);
} /* End get_separate_compressed_data */
#endif

/* Get separate using shared memory */
int get_separate_compressed_data(char *file_name, char *keyword,
long long total_size, long mode, TRANS_FILE_TYPE file_type)
{
    extern int size_printed;
    int server_mem_key;
    int mem_key;
    unsigned long long checksum;
    unsigned long long transferred_size;
    unsigned long long offset;
    unsigned long long total_offset = 0;
    int num_blocks;
    unsigned int finished = 0;
    unsigned int finished_cnt;
    int cnt = 0;
    int block_cnt;
    int cnt1;
    int num_processes;
    short ret;
    char buffer[80];

    /* Print size of file being retrieved */
    trans_hash_init(total_size, UNCOMPRESSED, "r", SEPARATE_BLOCKS);
    /* Indicate size not printed */
    size_printed = DBNO;
    /* The server will send the offset used */
    /* Get offset data from the server */
    get_string(buffer);
	sscanf(buffer, "%d %d %llu %d", &num_processes, &num_blocks, &offset, &server_mem_key);
    /* Get the extra short indicating data sent successfully */
    ret = get_short();
    /* Create the shared memory */
    mem_key = shared_memory_create(keyword, 1);
    /* Initialize the memory */
    shared_memory_init(1);
    /* Spawn off children to perform the transfers */
    /* Use calculated number of processes */
    for (cnt = 0, finished_cnt = 1; cnt < num_processes; ++cnt, ++finished_cnt)
    {
	/* Must fork here */
	if (fork() == 0)
    {
	    /* This is the child */
	for (cnt1 = 0; cnt1 < 3; ++cnt1)
	{
		transferred_size = separate_block_get(file_name, keyword, total_offset, num_blocks, cnt, server_mem_key, &checksum) ;
	    /* Successful if transferred_size > 0 */
	    if (transferred_size > 0)  break;
	} /* End for */
        shared_memory_update(transferred_size, checksum);
        exit(0);
	    } /* end if for child */
	    else
	    {
	/* Wait in a while until child is done */
	while (finished < finished_cnt)
	{
	sleep(2);
	finished = shared_memory1read(&transferred_size, &checksum);
	}
	} /* end else for parent */
    /* Increment the offset */
	total_offset += offset;
	    trans_hash(transferred_size, 2);
    } /* End for */
    trans_hash_close();
    if (finished < num_processes || ret != 0)
    {
	printf(bad_trans_write, file_name);
	exit(-1);
    }
    if (size_printed == DBYES) PRINTF("\n");
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_longlong(transferred_size);
    put_longlong(checksum);
    /* The parent process returns message to user interface */
    ret = get_ret_code();
    close(sock);
    shared_memory_release();
    return(ret);
} /* End get_separate_compressed_data */
#endif /* For not TRANSAPI */
