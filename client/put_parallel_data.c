#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	put_parallel_compressed_data
 *
 * Description:	Sends the Transcend server a big file or tar file.
	 *		Does not encrypt the data.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
     #include <sys/types.h>
     #include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "client_lib.h"
#include "client_prototypes.h"
#include "global_defines.h"
#include "global_params.h"
#include "hash.h"
#include "list.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"
#include "rtn.h"
#include "strcpy.h"


#ifndef WINDOWS_NT4
int put_parallel_compressed_data(REQ_TYPE req_type, char *file_name, char *keyword,
long long total_size, TRANS_FILE_TYPE file_type)
{
    extern int size_printed;
    int mem_key;
    int server_mem_key;
    int wrn_flag = DBNO;
    time_t starttime = time(0);
    unsigned long long checksum;
    int finished;
    int cnt;
    int cnt1;
    short ret;
    long long transferred_size;
    long long old_transferred_size= 0;
    long long last_transferred_size= 0;
    long long block_size;
    int num_blocks;
    char server_mem_string[16];

    size_printed = DBNO;
    block_size = total_size/9;
    num_blocks = block_size/4096;
    block_size = num_blocks * 4096l;
    /* Get the mem_key from the server */
    server_mem_key = get_long();
    /* Initialize the shared memory */
    mem_key = shared_memory_create(keyword, 10);
    trans_hash_init(total_size, UNCOMPRESSED, "w", PARALLEL);
    /* Spawn off children to perform the transfers */
    /* Just use 10 processes for now */
    for (cnt = 0; cnt < NUM_PARALLEL_PROCESSES; ++cnt)
    {
	if (fork() == 0) 
	{
	    /* Try to send the block 3 times */
	    for (cnt1 = 0; cnt1 < 3; ++cnt1)
	    {
		    if (block_put(file_name, server_mem_key, keyword, num_blocks, cnt*block_size, cnt) == SUCCESS)
		break;
	    } /* end for */
	    exit(0);
	} /* End if for child */
    } /* End for */
    /* Get the return code from the server */
    if (req_type == DBPUT_LARGE_PARALLEL)
    get_log_string();
    /* the parent waits for children to finish in a while */
    /* Need to wait a few seconds before printing amount transferred */
    cnt = 1;
    while (1)
    {
	finished = shared_memory_read(&transferred_size, &checksum);
	if ((finished >= NUM_PARALLEL_PROCESSES)
		|| (transferred_size >= total_size)) break;
		/* print number of bytes transferred */
	trans_hash(transferred_size-last_transferred_size, 0);
	last_transferred_size = transferred_size;
	++cnt;
	sleep(2);
	/* If 60 seconds passed, calculate new size */
	if (cnt == 20)
	{
	    /* If old size the same, set a warning flag */
	    /* If size stays the same for 60 seconds exit */
	    if (old_transferred_size == transferred_size)
	    {
		/* If warning flag already set exit */
		if (wrn_flag == DBYES)
		trans_exit(parallel_write_err, PARALLEL_WRITE_ERR);
		else wrn_flag = DBYES;
	    } else {
		wrn_flag = DBNO;
		old_transferred_size = transferred_size;
	    cnt = 0;
	    } /* End if for checking for failure to send data */
	} /* End if for printing size */
    } /* End while */
    trans_hash_close();
    if (size_printed == DBYES) PRINTF("\n");
    /* Send a put ack request to the server */
    if (req_type == DBPUT_LARGE_PARALLEL)
    {
	/* convert mem key to string */
	sprintf(server_mem_string, "%d", server_mem_key);
	if ((ret = init_trans_socket(DBPUT_ACK, keyword, NONE, NONE, 0, 0, 
	UNKNOWN, NONE, server_mem_string, NONE4STRING)) < SUCCESS) 
	return(ret);
    } /* end if */
    /* Send the size and the checksum */
    put_longlong(transferred_size);
    put_longlong(checksum);
    if (req_type == DBPUT_LARGE_PARALLEL)
    {
    time_t seconds = time(0)-starttime;
    put_long(seconds);
    } /* end if for large transfer */
    /* The parent process returns message to user interface */
    ret = get_log_string();
    /* Release the shared memory */
    shared_memory_release();
    close(sock);
    return(ret);
} /* End put_parallel_compressed_data */

/* Writes the compressed data to 10 parallel processes */
int put_parallel_uncompressed_data(char *file_name, char *keyword,
long long total_size, TRANS_FILE_TYPE file_type, 	
FILE *fp)
{
    extern int size_printed;
    int server_mem_key;
    long long offset = 0;
    unsigned long long child_checksum = 0;
    unsigned long long tmp_checksum = 0;
    long long child_compressed_size= 0;
    unsigned long long compressed_size = 0;
    long long size = 0;
    long long transferred_size = 0;
    int cnt;
    int fi_array[10][2];
    int fo_array[10][2];
    int ret;
    short len;
    char buffer[BUFFER_SIZE];
    float ratio; 

    /* Get the mem_key from the server */
    server_mem_key = get_long();
    /* Indicate no data printed */
    size_printed = DBNO;
ratio = .20;
    /* Get ready to write data */
    trans_hash_init(total_size, UNCOMPRESSED, "w", PARALLEL);
    /* Spawn off children to perform the transfers */
    /* Just use 10 processes for now */
    for (cnt = 0; cnt < NUM_PARALLEL_PROCESSES; ++cnt)
    {
	ret = pipe(fi_array[cnt]);
	ret = pipe(fo_array[cnt]);
	if (fork() == 0)
	{
	    random_block_put(fi_array[cnt][0], fo_array[cnt][1], keyword, server_mem_key, cnt);
	    exit(0);
	} /* End if for child */
    } /* End for */
    /* Now the parent sends the data in a round robin fashion */
    cnt = 0;
    while (  (len = fread(buffer, 1, BLOCK_SIZE, fp)) > 0 )
    {
	compressed_size += len;
	ret = write(fi_array[cnt][1], &len, 2);
	write(fi_array[cnt][1], buffer, len);
	write(fi_array[cnt][1], &offset, 8);
	offset += len;
	++cnt;
	transferred_size += (long long)len;
	if (cnt == 10)
    {
	cnt = 0;
	trans_hash(transferred_size*ratio, 0);
	    transferred_size = 0;
	} /* End if */
    } /* End while */
    /* Now tell all 10 children, all data has been sent */
    len = 0;
    for (cnt =0; cnt < NUM_PARALLEL_PROCESSES; ++cnt)
    write(fi_array[cnt][1], &len, 2);
    /* The parent gets sizes and checksums from children */
    for (cnt = 0; cnt < 10; ++cnt)
    {
	read(fo_array[cnt][0], &size, 8);
    child_compressed_size += size;
	read(fo_array[cnt][0], &tmp_checksum, 8);
	child_checksum += tmp_checksum;
    } /* End for */
    trans_hash_close();
    if (size_printed == DBYES) PRINTF("\n");
    /* Send the number of bytes for the file */
    put_longlong(compressed_size);
    /* Now put the checksum */
    put_longlong(child_checksum);
    /* The parent process returns message to user interface */
    ret = get_log_string();
    close(sock);
    return(ret);
} /* End put_parallel_uncompressed_data */
#endif

#ifndef LINUX64
#ifndef LINUX32
/* Send separate processes using pipes */
int put_separate_compressed_data(char *file_name, char *keyword,
long long total_size, int num_processes, TRANS_FILE_TYPE file_type)
{
    extern int size_printed;
    int server_mem_key;
    unsigned long long checksum = 0;
    unsigned long long total_checksum = 0;
    unsigned long long offset = 0;
    unsigned long long total_offset = 0;
    unsigned long long total_transferred_size = 0;
    unsigned long long transferred_size = 0;
    int finished = 0;
    int cnt;
    int cnt1;
    int fi_array[2];
    short ret;
    int num_blocks;
    char buffer[FILE_SIZE];

    size_printed = DBNO;
    /* Get offset data from the server */
    get_string(buffer);
	sscanf(buffer, "%d %d %llu %d", &num_processes, &num_blocks, &offset, &server_mem_key);
    /* Get the extra short indicating data sent successfully */
    ret = get_short();
    trans_hash_init(total_size, UNCOMPRESSED, "w", SEPARATE_BLOCKS);
    /* Set up pipe to talk to children */
    ret = pipe(fi_array);
    /* Spawn off children to perform the transfers */
    /* Use calculated number of processes */
    for (cnt = 0; cnt < num_processes; ++cnt)
    {
	    /* Try to put the block 3 times */
	    for (cnt1 = 0; cnt1 < 3; ++cnt1)
	    {
	/* Must fork here */
	if (fork() == 0)
    {
	    /* This is the child */
		    transferred_size = separate_block_put(file_name, server_mem_key, keyword, num_blocks, total_offset, cnt, &checksum);
    fflush(stdout);
	    /* Successful if transferred_size > 0 */
	    if (transferred_size > 0) ret = 0;
    else ret = -1;
	    /* Send return to parent */
	    write(fi_array[0], &ret, 2);
	    /* Send the number of bytes transferred and the check sum */
	    write(fi_array[0], &transferred_size, 8);
	    write(fi_array[0], &checksum, 8);
	    /* Wait for parent */
	    exit(ret);
	    } /* end if for child */
	    else
	    {
		/* This is the parent who waits for the child to finish */
	read(fi_array[1], &ret, 2);
	read(fi_array[1], &transferred_size, 8);
	read(fi_array[1], &checksum, 8);
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
	} /* end else for parent */
	} /* end inner for */
    } /* End for */
    trans_hash_close();
/*
    if (finished < NUM_PARALLEL_PROCESSES || ret != 0)
    {
	printf(bad_trans_write, file_name);
	exit(-1);
    }
*/
    if (size_printed == DBYES) PRINTF("\n");
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_longlong(total_transferred_size);
    put_longlong(total_checksum);
    /* The parent process returns message to user interface */
    ret = get_log_string();
    close(sock);
    return(ret);
} /* End put_separate_compressed_data */
#endif
#endif

#ifdef LINUX64
/* Send separate processes using shared memory */
int put_separate_compressed_data(char *file_name, char *keyword,
long long total_size, int num_processes, TRANS_FILE_TYPE file_type)
{
    extern int size_printed;
    int server_mem_key;
    int mem_key;
    unsigned long long checksum = 0;
    unsigned long long offset = 0;
    unsigned long long total_offset = 0;
    unsigned long long transferred_size = 0;
    int finished = 0;
    int cnt;
    int cnt1;
    int finished_cnt;
    short ret;
    int num_blocks;
    char buffer[FILE_SIZE];

    size_printed = DBNO;
    /* Get offset data from the server */
    get_string(buffer);
	sscanf(buffer, "%d %d %llu %d", &num_processes, &num_blocks, &offset, &server_mem_key);
    /* Get the extra short indicating data sent successfully */
    ret = get_short();
    trans_hash_init(total_size, UNCOMPRESSED, "w", SEPARATE_BLOCKS);
    /* Create the shared memory */
    mem_key = shared_memory_create(keyword, 1);
    /* Initialize the memory */
    shared_memory_init(1);
	finished = shared_memory1read(&transferred_size, &checksum);
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
		    transferred_size = separate_block_put(file_name, server_mem_key, keyword, num_blocks, total_offset, cnt, &checksum);
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
/*
    if (finished < NUM_PARALLEL_PROCESSES || ret != 0)
    {
	printf(bad_trans_write, file_name);
	exit(-1);
    }
*/
    if (size_printed == DBYES) PRINTF("\n");
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_longlong(transferred_size);
    put_longlong(checksum);
    /* The parent process returns message to user interface */
    ret = get_log_string();
    shared_memory_release();
    close(sock);
    return(ret);
} /* End put_separate_compressed_data */
#endif /* for not LINUX64 */

/* This is the new code that does not use shared memory or pipes */
#ifdef new

/* Send separate processes using shared memory */
int put_separate_compressed_data(char *file_name, char *keyword,
long long total_size, int num_processes, TRANS_FILE_TYPE file_type)
{
    extern int size_printed;
    int server_mem_key;
    int mem_key;
    unsigned long long checksum = 0;
    unsigned long long total_checksum = 0;
    unsigned long long offset = 0;
    unsigned long long total_offset = 0;
    unsigned long long transferred_size = 0;
    unsigned long long total_transferred_size = 0;
    int finished = 0;
    int cnt;
    int cnt1;
    int finished_cnt;
    int tmp_sock = sock;
    short ret;
    int num_blocks;
    char buffer[FILE_SIZE];

    size_printed = DBNO;
    /* Get offset data from the server */
    get_string(buffer);
	sscanf(buffer, "%d %d %llu %d", &num_processes, &num_blocks, &offset, &server_mem_key);
    /* Get the extra short indicating data sent successfully */
    ret = get_short();
    trans_hash_init(total_size, UNCOMPRESSED, "w", SEPARATE_BLOCKS);
    tmp_sock = sock;
    /* Create the shared memory */
#ifdef garbage
    mem_key = shared_memory_create(keyword, 1);
    /* Initialize the memory */
    shared_memory_init(1);
	finished = shared_memory1read(&transferred_size, &checksum);
#endif
    /* Spawn off children to perform the transfers */
    /* Use calculated number of processes */
    for (cnt = 0, finished_cnt = 1; cnt < num_processes; ++cnt, ++finished_cnt)
    {
	for (cnt1 = 0; cnt1 < 3; ++cnt1)
	{
		    transferred_size = separate_block_put(file_name, server_mem_key, keyword, num_blocks, total_offset, cnt, &checksum);
	    /* Successful if transferred_size > 0 */
	    if (transferred_size > 0)  break;
	} /* End for */
	total_transferred_size += transferred_size;
	total_checksum += checksum;
    /* Increment the offset */
	total_offset += offset;
	    trans_hash(transferred_size, 2);
    } /* End for */
    trans_hash_close();
    /* Restore the socket value */
    sock = tmp_sock;
    if (size_printed == DBYES) PRINTF("\n");
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_longlong(total_transferred_size);
    put_longlong(total_checksum);
    /* The parent process returns message to user interface */
    ret = get_log_string();
    close(sock);
    return(ret);
} /* End put_separate_compressed_data */
#endif /* for new */
#endif /* For not TRANSAPI */
