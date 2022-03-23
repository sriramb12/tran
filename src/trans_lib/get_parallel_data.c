/****************************************************************************
 *
 * Function:	get_parallel
 *
 * Description:	Sends a parallel file to the client
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <strings.h>
#include <string.h>

#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_lib_prototypes.h"
#include "trans_server_errs.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"
#include "zlib.h"
#include "trans_server_globals.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


/* Build buffer with block size, and number of blocks */
unsigned int  block_size_buffer_build(char *buffer, int mem_key, unsigned long long size, char *separate_size_string)
{
    extern unsigned long long separate_configured_size;
    unsigned long long offset = 0;
    unsigned long long block_size;
    unsigned int num_blocks;
    unsigned int block_cnt;

    DBG();

    /* Initialize the number of blocks */
    /* If the client sent a block size use it */
    block_size = atoll(separate_size_string);

    /* Otherwise use the default size on the server */
    if (block_size < 5000000)
        block_size = separate_configured_size;

    num_blocks = block_size/4096;
    offset = num_blocks*4096;
    block_cnt = (size/block_size) + 1;

    /* Build buffer with calculated values */
	sprintf(buffer, "%d %d %llu %d", block_cnt, num_blocks, offset, mem_key);

    return(block_cnt);
} /* end block_size_buffer_build */


/* Build the buffer with file name, etc. */
void file_string_build(char *single_file, char *buffer)
{
    char *real_file_ptr;
    float compression_ratio;
    int secondary_file_type;

    DBG("single_file='%s'", single_file);

    /* If the compressed size is negative, make it positive */
    if (fs.compressed_size < 0)
        fs.compressed_size = 0-fs.compressed_size;

    /* If the original size is negative, make it positive */
    if (fs.original_size < 0)
        fs.original_size = 0-fs.original_size;

    compression_ratio = (float)fs.original_size/fs.compressed_size;
    DBG("compression_ration='%f'", compression_ratio);

    /* If just getting a portion of the directory, use that name */
    /* This feature is not currently being used */
    if (strcmp(single_file, NONE) != 0)
    {
        DBG("entering if just getting portion of the directory");

	    /* Get the size of the file */
	    fs.original_size = size_get(single_file, &secondary_file_type);

        /* Want to report it as a file, but transfer it as a directory */
	    fs.file_type = secondary_file_type;
	    secondary_file_type = DIRECTORY;
	    strcpy(real_file_ptr, single_file);
    }

    /* If retrieving group of files on Windows, strip off trailing file names */
    if ((fs.file_type == FILES) && (is_windows() == DBYES))
    {
        DBG("entering if retrieving group of files on Windows");

	    char *ptr;
	    secondary_file_type = fs.file_type;

	    /* Find first white space in file name */
	    if ((ptr = strchr(fs.real_name, ' ')) != NULL)
	        ptr[0] = '\0';

	    real_file_ptr =  malloc(strlen(fs.real_name) +5);
	    strcpy2(real_file_ptr, fs.real_name, ".tar");

    } else {
        DBG("entering else retrieving group of files on Windows (normal retrieve)");

	    /* These are the same if retrieving normal data */
	    DBG("before secondary_file_type assigned, fs.file_type='%d'", fs.file_type);
	    secondary_file_type = fs.file_type;
	    DBG("after, secondary_file_type='%d'", secondary_file_type);

	    /* Exchange spaces for ` */
	    real_file_ptr = malloc(strlen(fs.real_name) +1);
	    strcpy_ex(real_file_ptr, fs.real_name, '`', ' ');
	    DBG("real_file_ptr='%s', fs.real_name='%s'", real_file_ptr, fs.real_name);
    } /* End else */

    strcpy(fs.real_name, real_file_ptr);
    DBG("fs.real_name='%s'", fs.real_name);

    if (fs.compression == 0)
        fs.compression = DBYES;

    /* iCAP classification already built */
    /* If sending the file as a zipped file */
    if (gu.req_type == DBGET_ZIPPED)
    {
	    if (fs.file_type == DIRECTORY)
            sprintf(buffer, "%s.tar.gz %llu %lu %c %d %d %2.1f %d %s", real_file_ptr, fs.compressed_size, fs.mode, fs.file_type, fs.compression, gu.req_type, compression_ratio, secondary_file_type, fs.icap_class);
	    /* Retrieving compressed, but not a directory */
	    else if (is_zipped_file(real_file_ptr) == DBNO)
	        /* If not already zipped add .gz suffix */
            sprintf(buffer, "%s.gz %llu %lu %c %d %d %2.1f %d %s", real_file_ptr, fs.compressed_size, fs.mode, fs.file_type, fs.compression, gu.req_type, compression_ratio, secondary_file_type, fs.icap_class);
	    /* If already compressed, do not compress again */
        else
            sprintf(buffer, "%s %llu %lu %c %d %d %2.1f %d %s", real_file_ptr, fs.compressed_size, fs.mode, fs.file_type, fs.compression, gu.req_type, compression_ratio, secondary_file_type, fs.icap_class);
	}
    /* Retrieving as a normal file or directory */
    else
        sprintf(buffer, "%s %llu %lu %c %d %d %2.1f %d %s", real_file_ptr, fs.original_size, fs.mode, fs.file_type, fs.compression, gu.req_type, compression_ratio, secondary_file_type, fs.icap_class);

    free(real_file_ptr);

    DBG("end file_string_build, buffer='%s'", buffer);
} /* end file_string_build */


long long size_get(char *file_name, char *secondary_file_type)
{
    struct stat buf;

    DBG();

    if (stat (file_name, &buf) != 0)
        close_down1f(stat_err, file_name, STAT_ERR);

    if (S_ISDIR(buf.st_mode) == 0)
    {
	    *secondary_file_type = 'e';
        return(buf.st_size);
    } else {
	    *secondary_file_type = DIRECTORY;
	    return(big_disk_usage(file_name));
    } /* End else */
} /* end size_get */


/* verify size and check sum from client correct */
void client_size_verify(unsigned long long server_size, unsigned long long checksum)
{
    unsigned long long client_compressed_size;
    unsigned long long client_checksum;


    /* Get the actual size */
    client_compressed_size = get_longlong();
    client_checksum = get_longlong();
    DBG("client_compressed_size=%llu fs.compressed_size=%llu", client_compressed_size, fs.compressed_size);
    if (client_compressed_size != fs.compressed_size)
    {
        DBG("bad_size");
	    sprintf(rtn.msg, bad_size, PROGRAM, fs.real_name, fs.key, fs.compressed_size, client_compressed_size);
        shut_down(rtn.msg, BAD_SIZE);
    } /* end if */

    /* Verify that the check sum is correct */
    if (checksum != client_checksum)
    {
        DBG("bad_size");
	    sprintf(rtn.msg, bad_checksum, PROGRAM, client_checksum, checksum);
	    shut_down(rtn.msg, BAD_CHECKSUM);
    } /* end if */
} /* end client_size_verify */


/*  Get a parallel file */
long long get_parallel(void)
{
    extern large_parallel_transfer_size;
    unsigned long long checksum = 0;
    unsigned long long server_size = 0;
    unsigned long long offset = 0;
    unsigned long long block_size;
    unsigned long long num_blocks;
    int block_cnt;
    int mem_key;
    short cnt;
    int fd;
    int finished;
    int len;
    char buffer[FILE_SIZE];

    DBG("[before] gu.req_type=%d", gu.req_type);

    block_size = fs.original_size/9;
    num_blocks = block_size/4096;
    block_size = num_blocks * 4096;

    /* Determine if this is random parallel or just parallel */
    if (fs.compressed_size == fs.original_size)
    {
/*
 * Sep 2018, Carlos Mazieri, commented the block below because LARGE_PARALLEL does not work
        // If version is 6.3.6 or later and size greter than parallel size
        // do large parallel transfer
#ifdef INTERCOM
	    if ((fs.original_size > large_parallel_transfer_size) && (strcmp(user.client_version, "7.0") >= 0))
        {
            gu.req_type = IS_REQUEST_GET_DELETE(gu.req_type) ?
                            DBGETDLT_LARGE_PARALLEL : DBGET_LARGE_PARALLEL;
        }
    	else
#endif
*/
        {
            gu.req_type = IS_REQUEST_GET_DELETE(gu.req_type) ?
                           DBGETDLT_PARALLEL : DBGET_PARALLEL;
        }
    } /* end if */
    else
    {
        gu.req_type = DBGET_RANDOM_PARALLEL;
    }

    DBG("[after] gu.req_type=%d", gu.req_type);

    sprintf(rtn.msg, "gu.req_type is %d\n", gu.req_type);
    data_log(tr.log_file, rtn.msg);

    /* Build name of new file */
    len = sprintf(bf.file_name, "data_files%d/%s.gz", fs.disk_index, fs.key);

    /* If this is DTMS  concatenate real file name */
    if (strcmp(server.site, DTMS) == 0)
        strcpy2(bf.file_name+len, "/", fs.real_name);

    /* Open file for reading */
    if ((fd = open(bf.file_name, O_RDONLY  )) < 0)
        close_down1q(rd_open_err, bf.file_name,  RD_OPEN_ERR);

    /* Tell client ready to send data */
    if (!IS_REQUEST_GET_SERIAL(gu.req_type))
        put_short(PARALLEL);
    else
        put_short(gu.req_type);

    close(fd);

    /* Build buffer to send */
    file_string_build(NONE, buffer);
    put_string(buffer);
    block_cnt = 10;

    /* Create the shared memory */
    mem_key = shared_memory_create(fs.key, block_cnt);

    /* Give client the offsets for the file */
    for (cnt = 0; cnt < block_cnt; ++cnt)
    {
	    sprintf(buffer, "%d %llu %d", num_blocks, offset, mem_key);
	    put_string(buffer);

	    if (gu.req_type == DBGET_RANDOM_PARALLEL) offset += 4096;
	    else offset += block_size;
    } /* End for */

    /* Tell client this is all of the data */
    put_short(0);

    /* If sending large parallel data, exit now */
    if (IS_REQUEST_GET_LARGE_PARALLEL(gu.req_type))
    {
        DBG("exiting this process due to 'IS_REQUEST_GET_LARGE_PARALLEL(gu.req_type)'");
        return_success("Transfer initiated", DBNO);
        exit(0);
    } /* end if */

    while (1)
    {
	    finished = shared_memory_read(&server_size, &checksum);
	    if (finished >= 10) break;
	    sleep(2);
    } /* end while */

    /* Remove shared memory no longer needed */
    shared_memory_release();

    /* Get client size and checksum and verify correct */
    client_size_verify(server_size, checksum);

    return(fs.original_size);
} /* end get_parallel */


/* Get separate blocks */
long long get_separate(void)
{
    unsigned long long checksum = 0;
    unsigned long long server_size = 0;
    unsigned int block_cnt;
    int mem_key;
    int fd;
    int finished;
    int len;
    char buffer[FILE_SIZE];

    DBG();

    /* Build name of new file */
    len = sprintf(bf.file_name, "data_files%d/%s.gz", fs.disk_index, fs.key);

    /* If this is DTMS  concatenate real file name */
    if (strcmp(server.site, DTMS) == 0)
        strcpy2(bf.file_name+len, "/", fs.real_name);

    /* Open file for reading */
    if ((fd = open(bf.file_name, O_RDONLY  )) < 0)
        close_down1q(rd_open_err, bf.file_name,  RD_OPEN_ERR);

    /* Tell client ready to send data */
    put_short(PARALLEL);
    close(fd);
    file_string_build(NONE, buffer);
    put_string(buffer);

    /* Create the shared memory */
    mem_key = shared_memory_create(fs.key, 1);

    /* Initialize the structure */
    shared_memory1init();

    /* Builf the buffer with block size, etc. */
    block_cnt = block_size_buffer_build(buffer, mem_key, fs.original_size, gu.par5);

    /* Send block size to client */
	put_string(buffer);

    /* Tell client this is all of the data */
    put_short(SUCCESS);

    while (1)
    {
	    finished = shared_memory1read(&server_size, &checksum);
	    if (finished >= block_cnt) break;
	    sleep(2);
    } /* end while */

    /* Remove shared memory no longer needed */
    shared_memory_release();

    /* Get client size and checksum and verify correct */
    client_size_verify(server_size, checksum);

    return(fs.original_size);
} /* end get_separate */


/* Retrieve get ack for large parallel data */
int get_ack(char *file_keyword, int mem_key)
{
    extern  TRANS_FILE_STAT fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    unsigned long seconds;
    int finished;
    unsigned long long checksum = 0;
    unsigned long long server_size;
    FILE *fp;

    DBG();

    /* Read in the file data */
    fp = read_big_file(file_keyword, lines, &fs);
    fclose(fp);

    /* Must allocate the memory before it can be read */
    shared_memory_allocate(file_keyword, mem_key, 10);
	finished = shared_memory_read(&server_size, &checksum);

    /* Remove shared memory no longer needed */
    shared_memory_release();

    /* Tell client ready to retrieve size and checksum */
    put_short(SUCCESS);

    /* Get client size and checksum and verify correct */
    client_size_verify(server_size, checksum);

    /* Get the transfer time */
    seconds = get_long();
    tr.upd_time = time(0) - seconds;

    /* Do final update to the file */
    /* and send the return code to the client */
    get_big_file_ack(file_keyword);
} /* end rtv_get_ack */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
