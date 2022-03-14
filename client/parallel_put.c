#ifdef TRANSAPI
#ifndef WINDOWS_NT4
/****************************************************************************
 *
 * Function:	put_parallel_compressed_data
 *
 * Description:	Sends the Transcend server a big file or tar file.
 *		        Does not encrypt the data.
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
#include "ssl_client_lib.h"
#include "trans_hash.h"
#include "list.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "rtn.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

#ifndef WINDOWS_NT4
int trans_parallel_put(int index)
{
    DBG();

    extern int size_printed;
    int mem_key;
    unsigned long server_mem_key;
    int wrn_flag = DBNO;
    time_t starttime = time(0);
    unsigned long long checksum;
    int finished;
    int cnt;
    int cnt1;
    short ret;
    unsigned long long transferred_size;
    unsigned long long old_transferred_size = 0;
    unsigned long long last_transferred_size = 0;
    unsigned long long block_size;
    int num_blocks;

    size_printed = DBNO;
    block_size = trans[index]->original_size / 9;
    num_blocks = block_size / 4096;
    block_size = num_blocks * 4096l;

    DBG("block_size = %llu AND num_blocks = %d AND block_size = %llu", block_size, num_blocks, block_size);

    /* Get the mem_key from the server */
    server_mem_key = get_ssl_long();

    /* Store server mem key in par2 */
    sprintf(trans[index]->par2, "%lu", server_mem_key);

    /* Initialize the shared memory */
    mem_key = shared_memory_create(trans[index]->keyword, 10);
    tfile_hash_init(trans[index]->original_size, UNCOMPRESSED, "w", PARALLEL, index);

    /* Save original size as compressed size because each block has its own size */
    trans[index]->compressed_size = trans[index]->original_size;

    /* Spawn off children to perform the transfers */
    /* Just use 10 processes for now */
    for (cnt = 0; cnt < NUM_PARALLEL_PROCESSES; ++cnt)
    {
	    if (fork() == 0)
	    {
	        /* Store offset in size field */
	        trans[index]->original_size = cnt * block_size;

	        /* Store block number in file mode */
	        trans[index]->file_mode = cnt;

	        /* Try to send the block 3 times */
	        for (cnt1 = 0; cnt1 < 3; ++cnt1)
	        {
		        if (trans_block_put(index, num_blocks) == SUCCESS)
		            break;
	        } /* end for */

	        exit(0);
	    } /* End if for child */
    } /* End for */

    /* Get the return code from the server */
    if (IS_REQUEST_PUT_LARGET_PARALLEL(trans[index]->req_type))
    {
         DBG("before get_log_string()");
         get_log_string();
    }
    /* the parent waits for children to finish in a while */
    /* Need to wait a few seconds before printing amount transferred */
    sleep(2);
    cnt = 1;

    while (1)
    {
	    finished = shared_memory_read(&transferred_size, &checksum);
	    if ( (finished >= NUM_PARALLEL_PROCESSES) || (transferred_size >= trans[index]->compressed_size) )
        {
	        break;
        }
	    /* print number of bytes transferred */
	    tfile_hash(transferred_size-last_transferred_size, 0, index);
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
                {
                    shared_memory_release();
		            return(trans_exit(parallel_write_err, PARALLEL_WRITE_ERR));
                }
		        else
                {
		            wrn_flag = DBYES;
                }
	        }
	        else
	        {
		        wrn_flag = DBNO;
		        old_transferred_size = transferred_size;
	            cnt = 0;
	        } /* End if for checking for failure to send data */
	    } /* End if for printing size */
    } /* End while */

    tfile_hash_close();

    shared_memory_release();

    if (size_printed == DBYES)
        PRINTF("\n");

    /* Send a put ack request to the server */
    if (IS_REQUEST_PUT_PARALLEL(trans[index]->req_type) ||
            IS_REQUEST_PUT_LARGET_PARALLEL(trans[index]->req_type))
    {
        DBG("sending DBPUT_ACK");
	    /* convert mem key to string */
	    sprintf(trans[index]->par2, "%lu", server_mem_key);
	    trans[index]->req_type = DBPUT_ACK;
        if ((ret = trans_socket_init(index)) < 0)
	        return(ret);
    } /* end if */

    /* Send the size and the checksum */
    put_ssl_longlong(transferred_size);
    put_ssl_longlong(checksum);

    DBG("trans[index]->req_type=%d", trans[index]->req_type);
    if (trans[index]->req_type == DBPUT_ACK)
    {
        time_t seconds = time(0) - starttime;
        put_ssl_long(seconds);
    } /* end if for large transfer */

    /* The parent process returns message to user interface */
    DBG("before get_log_string()");
    ret = get_log_string();
    close(sock);
    DBG("will return");
    return(ret);
} /* end trans_parallel_put */

/* Writes the compressed data to 10 parallel processes */
int trans_uncompressed_parallel_put(int index)
{
    extern int size_printed;
    int server_mem_key;
    unsigned long long offset = 0;
    unsigned long long child_checksum = 0;
    unsigned long long tmp_checksum = 0;
    unsigned long long child_compressed_size= 0;
    unsigned long long compressed_size = 0;
    unsigned long long size = 0;
    unsigned long long transferred_size = 0;
    int cnt;
    FILE *fp;
    int fi_array[10][2];
    int fo_array[10][2];
    int ret;
    short len;
    char buffer[BUFFER_SIZE];
    float ratio;

    DBG();

    /* Open the file for read */
    if ((fp = fopen(trans[index]->file_name, "r")) == NULL)
    return(-1);
    /* Get the mem_key from the server */
    server_mem_key = get_ssl_long();
    /* The server mem key is stored in par2 */
    sprintf(trans[index]->par2, "%d", server_mem_key);
    /* Initialize the requesttype for the children */
    trans[index]->req_type = DBRANDOM_BLOCK_PUT;
    /* Indicate no data printed */
    size_printed = DBNO;
ratio = .20;
    /* Get ready to write data */
    tfile_hash_init(trans[index]->original_size, UNCOMPRESSED, "w", PARALLEL, index);
    /* Spawn off children to perform the transfers */
    /* Just use 10 processes for now */
    for (cnt = 0; cnt < NUM_PARALLEL_PROCESSES; ++cnt)
    {
	ret = pipe(fi_array[cnt]);
	ret = pipe(fo_array[cnt]);
	if (fork() == 0)
	{
	    /* The block number is stored in the file mode */
	    trans[index]->file_mode = cnt;
	    trans_random_block_put(fi_array[cnt][0], fo_array[cnt][1], index);
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
	tfile_hash(transferred_size*ratio, 0, index);
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
    tfile_hash_close();
    if (size_printed == DBYES) PRINTF("\n");
    /* Send the number of bytes for the file */
    put_ssl_longlong(compressed_size);
    /* Now put the checksum */
    put_ssl_longlong(child_checksum);
    /* The parent process returns message to user interface */
    DBG("before get_log_string()");
    ret = get_log_string();
    close(sock);
    return(ret);
} /* end trans_uncompressed_parallel_put */
#endif

#if     0  // #ifndef FEDORA
#ifndef LINUX64
#ifndef LINUX32
#ifndef SOLARIS
/* Send separate processes using pipes */
int trans_put_separate_compressed_data(int index)
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

    DBG();

    size_printed = DBNO;
    /* Get offset data from the server */
    get_ssl_string(buffer);
	sscanf(buffer, "%d %d %llu %d", &num_processes, &num_blocks, &offset, &server_mem_key);
    /* Get the extra short indicating data sent successfully */
    ret = get_ssl_short();
    tfile_hash_init(trans[index]->original_size, UNCOMPRESSED, "w", SEPARATE_BLOCKS, index);
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
		    transferred_size = trans_separate_block_put(index, num_blocks, &checksum);
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
	    tfile_hash(transferred_size, 1, index);
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
    tfile_hash_close();
/*
    if (finished < NUM_PARALLEL_PROCESSES || ret != 0)
    {
	sprintf(rtn.msg, bad_tfile_write, file_name);
	return(BAD_TRANS_WRITE);
    }
*/
    if (size_printed == DBYES) PRINTF("\n");
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_ssl_longlong(total_transferred_size);
    put_ssl_longlong(total_checksum);
    /* The parent process returns message to user interface */
    ret = get_log_string();
    close(sock);
    return(ret);
} /* End put_separate_compressed_data */
#endif
#endif
#endif
#endif

/* Send separate processes using shared memory */
int trans_put_separate_compressed_data(int index)
{
    DBG();

    extern int size_printed;
    int num_processes;
    int mem_key;
    unsigned long long checksum = 0;
    unsigned long long offset = 0;
    unsigned long long transferred_size = 0;
    int finished = 0;
    int cnt;
    int cnt1;
    int finished_cnt;
    short ret;
    int num_blocks;
    char buffer[FILE_SIZE];

    size_printed = DBNO;
    tfile_hash_init(trans[index]->original_size, UNCOMPRESSED, "w", SEPARATE_BLOCKS, index);
    /* Set size back to 0 because it now becomes the offset */
    trans[index]->original_size = 0;
    /* Get offset data from the server */
    get_ssl_string(buffer);
    /* Server mem key is stored in par2 */
	sscanf(buffer, "%d %d %llu %s", &num_processes, &num_blocks, &offset, trans[index]->par2);
    /* Get the extra short indicating data sent successfully */
    ret = get_ssl_short();
    /* Create the shared memory */
    mem_key = shared_memory_create(trans[index]->keyword, 1);
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
	     /* Put block number in the file mode */
	    trans[index]->file_mode = cnt;
	    /* This is the child */
	for (cnt1 = 0; cnt1 < 3; ++cnt1)
	{
		    transferred_size = trans_separate_block_put(index, num_blocks,  &checksum);
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
	trans[index]->original_size += offset;
	    tfile_hash(transferred_size, 2, index);
    } /* End for */
    tfile_hash_close();
/*
    if (finished < NUM_PARALLEL_PROCESSES || ret != 0)
    {
	sprintf(rtn.msg, bad_tfile_write, trans[index]->file_name);
	return(BAD_TRANS_WRITE);
    }
*/
    if (size_printed == DBYES) PRINTF("\n");
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_ssl_longlong(transferred_size);
    put_ssl_longlong(checksum);
    /* The parent process returns message to user interface */
    DBG("before get_log_string()");
    ret = get_log_string();
    shared_memory_release();
    close(sock);
    return(ret);
} /* End trans_put_separate_compressed_data */

/* This is the new code that does not use shared memory or pipes */
#ifdef new

/* Send separate processes using shared memory */
int put_separate_compressed_data(int index)
long long trans[index]->original_size, int num_processes, TRANS_FILE_TYPE trans[index]->file_type)
{
    DBG();

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
    get_ssl_string(buffer);
    /* Server mem key goes in par2 */
	sscanf(buffer, "%d %d %llu %s", &num_processes, &num_blocks, &offset, trans[index]->par2);
    /* Get the extra short indicating data sent successfully */
    ret = get_ssl_short();
    tfile_hash_init(trans[index]->original_size, UNCOMPRESSED, "w", SEPARATE_BLOCKS, index);
    tmp_sock = sock;
    /* Create the shared memory */
#ifdef garbage
    mem_key = shared_memory_create(trans[index]->keyword, 1);
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
		    transferred_size = separate_block_put(trans[index]->file_name, server_mem_key, trans[index]->keyword, num_blocks, total_offset, cnt, &checksum);
	    /* Successful if transferred_size > 0 */
	    if (transferred_size > 0)  break;
	} /* End for */
	total_transferred_size += transferred_size;
	total_checksum += checksum;
    /* Increment the offset */
	total_offset += offset;
	    tfile_hash(transferred_size, 2, index);
    } /* End for */
    tfile_hash_close();
    /* Restore the socket value */
    sock = tmp_sock;
    if (size_printed == DBYES) PRINTF("\n");
    /* Already know how many bytes were transferred */
    /* Send the number of bytes for the file */
    put_ssl_longlong(total_transferred_size);
    put_ssl_longlong(total_checksum);
    /* The parent process returns message to user interface */
    ret = get_log_string();
    close(sock);
    return(ret);
} /* End trans_put_separate_compressed_data */
#endif /* for new */
#endif
#endif
