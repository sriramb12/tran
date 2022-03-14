/****************************************************************************
 *
 * Function:	block_get
 *
 * Description:	Send block of data to the client
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_defines.h"
#include "trans_server_macros.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "rtn.h"
#include "strcpy.h"
#include "server_lib.h"
#include "user_params.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

/* Key for big file to retrieve */
void block_get(char *key, long long offset, long num_blocks, int block_num, int mem_key)
{
    extern char transfertype[];
    unsigned long long checksum = 0;
    int block_offset = 36864; /* 4096 * 9 */
    long long total_len = 0;
    char buffer[BUFFER_SIZE];
    char file_name[FILE_SIZE];
    char real_name[FILE_SIZE];
    int fd;
    int disk_index;
    int len;
    int cnt;
    int cnt1;
    char tmp_key[10];

    DBG("key='%s', offset='%lld', num_blocks='%ld', block_num='%d', mem_key='%d'", key, offset, num_blocks, block_num, mem_key);

    strcpy(tmp_key, key);

    /* If the key is found, it is returned in variable 'key' */
    keyword_find(tmp_key, key);

    /* Read in the disk index */
    disk_index = disk_index_read(key, real_name);

    /* Build the name of the file */
    if (strcmp(server.site, DTMS) == 0)
        sprintf(file_name, "data_files%d/%s.gz/%s", disk_index, key, real_name);
    else 
        sprintf(file_name, "data_files%d/%s.gz", disk_index, key);

    /* Allocate memory for this structure */
    shared_memory_allocate(key, mem_key, block_num);

    /* Initialize structure for this process */
    /* Open the file and go to the appropriate position */
    if ((fd = open(file_name, O_RDONLY )) < 0)
    {
        /* Write to shared memory indicating that no data retrieved */
        shared_memory_finish(block_num);
	    close_down1m(rd_open_err, file_name, RD_OPEN_ERR);
    } /* End if for unable to open file */

    /* Move to the correct position in the file */
    if (lseek(fd, offset, SEEK_SET) != gu.original_size)
    {
        shared_memory_finish(block_num);
	    close_down1q(seek_err, file_name, SEEK_ERR);
    } /* end if */

    /* Tell client ready to send block */
    put_short(SUCCESS);
    
    /* Calculate start transfer time */
    start_transfertime_calc();
    
    /* Read text */
    for (cnt = 0; cnt < num_blocks; ++cnt)
    {
	    if ((len = read(fd, buffer, BLOCK_SIZE)) <= 0) break;
	    for (cnt1 = 0; cnt1 < len; ++cnt1)
	        checksum += buffer[cnt1];
	    
	    total_len += len;
        put_binary_string(buffer, len);
	    if (gu.req_type == DBRANDOM_BLOCK_GET)
	        lseek(fd, block_offset, SEEK_CUR); 
    }

    /* Calculate end transfer time */
    end_transfertime_calc();

    /* Initialize the transfer type */
    strcpy(transfertype, "Parallel get");
    fs.original_size = fs.compressed_size = total_len;
    close(fd);

    /* Write final size and check sum to shared memory */
    sprintf(rtn.msg, "Block %d for %s successfully retrieved", block_num, key);
    shared_memory_write(total_len, checksum, block_num, DBYES);
    
    /* Tell client get successful */
    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
} /* End block_get */

void separate_block_get(
    char *key, long long offset, long num_blocks, int block_num, int mem_key)		/* Key for big file to retrieve */
{
    extern char transfertype[];
    unsigned long long checksum = 0;
    long long total_len = 0;
    char buffer[BUFFER_SIZE];
    char file_name[FILE_SIZE];
    char real_name[FILE_SIZE];
    int fd;
    int disk_index;
    int len;
    int cnt;
    int cnt1;
    char tmp_key[10];

    DBG();

    strcpy(tmp_key, key);
    /* If the key is found, it is returned in variable 'key' */
    keyword_find(tmp_key, key);
    /* Read in the disk index */
    disk_index = disk_index_read(key, real_name);
    /* Build the name of the file */
    if (strcmp(server.site, DTMS) == 0)
    sprintf(file_name, "data_files%d/%s.gz/%s", disk_index, key, real_name);
    else sprintf(file_name, "data_files%d/%s.gz", disk_index, key);
    /* Open the file and go to the appropriate position */
    if ((fd = open(file_name, O_RDONLY )) < 0)
    {
	close_down1m(rd_open_err, file_name, RD_OPEN_ERR);
    } /* End if for unable to open file */
    /* Move to the correct position in the file */
    if (lseek(fd, offset, SEEK_SET) != gu.original_size)
    {
	close_down1q(seek_err, file_name, SEEK_ERR);
    } /* end if */
    /* Tell client ready to send block */
    put_short(SUCCESS);
    /* Allocate memory for this structure */
    shared_memory1allocate(key, mem_key);
    /* Calculate start transfer time */
    start_transfertime_calc();
    /* Read text */
    for (cnt = 0; cnt < num_blocks; ++cnt)
    {
	if ((len = read(fd, buffer, BLOCK_SIZE)) <= 0) break;
	for (cnt1 = 0; cnt1 < len; ++cnt1)
	checksum += buffer[cnt1];
	total_len += len;
     put_binary_string(buffer, len);
    }
    /* Calculate end transfer time */
    end_transfertime_calc();
    /* Initialize the transfer type */
    strcpy(transfertype, "Separate block get");
    close(fd);
    fs.original_size = fs.compressed_size = total_len;
    /* Write final size and check sum to shared memory */
    sprintf(rtn.msg, "Block %d for %s successfully retrieved", block_num, key);
        shared_memory_update(total_len, checksum);
    /* Tell client get successful */
    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
} /* End separate_block_get */
