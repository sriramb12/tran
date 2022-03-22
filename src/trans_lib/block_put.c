/****************************************************************************
 *
 * Function:	block_put
 *
 * Description:	Retrieves a block of data and writes it to a file
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
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

void offset_write(int block_num, long long offset)
{
    DBG("this function does nothing");
}

int disk_index_read(char *key, char *real_name)
{
     extern TRANS_FILE_STAT  fs;
    char line[260];
    int disk_index;
    char file_name[32];
    FILE *fp;

    DBG();

    /* Build the file name */
    strcpy2(file_name, "global_data/", key);
    if ((fp = fopen(file_name, "r")) == NULL)
    close_down1m(rd_open_err, file_name, RD_OPEN_ERR);
    /* Read in first line of text */
    line_get(fp, line);
    sscanf(line, "%*c %d", &disk_index);
    /* Get second line where file name is */
    line_get(fp, line);
    strcpy(real_name, line+13);
    fclose(fp);
    strcpy(fs.key, key);
    strcpy(fs.real_name, real_name);
    return(disk_index);
} /* end disk_index_read */

void block_put(
    char *key, unsigned long long offset, int mem_key, int block_num)		/* Key for big file to retrieve */
{
    extern char transfertype[];
    char buffer[BUFFER_SIZE];
    char file_name[FILE_SIZE];
    char real_name[FILE_SIZE];
    int fd;
    int disk_index;
    int len;
    int cnt;
    unsigned long long checksum = 0;
    unsigned long long total_len = 0;

    DBG();

    /* Read in the disk index */
    disk_index = disk_index_read(key, real_name);
    /* Build the name of the file */
    if (strcmp(server.site, DTMS) == 0)
    sprintf(file_name, "data_files%d/%s.gz/%s", disk_index, key, real_name);
    else sprintf(file_name, "data_files%d/%s.gz", disk_index, key);
    /* Allocate memory for this structure */
    shared_memory_allocate(key, mem_key, 10);
    /* Initialize structure for this process */
    shared_memory_init(block_num);
    /* Open the file and go to the appropriate position */
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0770)) < 0)
    {
    /* Write to shared memory indicating that no data sent */
    shared_memory_finish(block_num);
	close_down1m(wr_open_err, file_name, WR_OPEN_ERR);
    } /* end if */
    /* Move to the correct position in the file */
    if (lseek(fd, offset, SEEK_SET) != offset)
    {
    /* Write to shared memory indicating that no data sent */
    shared_memory_finish(block_num);
	close_down1q(seek_err, file_name, SEEK_ERR);
    } /* end if */
    /* Tell client ready to retrieve block */
    put_short(SUCCESS);
    /* Calculate the starttime */
    start_transfertime_calc();
    /* Write text */
    while ((len = get_string(buffer)) > 0)
    {
	for (cnt = 0; cnt < len; ++cnt)
	checksum += buffer[cnt];
	total_len += write(fd, buffer, len);
    }
    /* Calculate the endtime */
    end_transfertime_calc();
    strcpy(transfertype, "Separate block put");
    strcpy(transfertype, "Parallel put");
    /* Write final size and check sum to shared memory */
        shared_memory_write(total_len, checksum, block_num, DBYES);
    /* Tell client put successful */
    close(fd);
    sprintf(rtn.msg, "Block %d (%llu bytes) transferred", block_num,total_len);
    /* Initialize size for loggin purposes */
    fs.original_size = fs.compressed_size = total_len;
    put_success_msg(key);
} /* End block_put */

/* Get random blocks of data */
void random_block_put(
    char *key, 
    int mem_key,
    int block_num)		/* Key for big file to retrieve */
{
    extern char transfertype[];
    long long offset = 0;
    long long total_len = 0;
    unsigned long long checksum = 0;
    char buffer[BUFFER_SIZE];
    char file_name[32];
    char real_name[FILE_SIZE];
    int fd;
    int cnt;
    int disk_index;
    int len;

    DBG();

    /* Read in the disk index */
    disk_index = disk_index_read(key, real_name);
    /* Build the name of the file */
    if (strcmp(server.site, DTMS) == 0)
    sprintf(file_name, "data_files%d/%s.gz/%s", disk_index, key, real_name);
    else sprintf(file_name, "data_files%d/%s.gz", disk_index, key);
    shared_memory_allocate(key, mem_key, 10);
    /* Initialize structure for this process */
    shared_memory_init(block_num);
    /* Open the file*/
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0664)) < 0)
    {
    /* Write to shared memory indicating that no data sent */
    shared_memory_finish(block_num);
	close_down1m(wr_open_err, file_name, WR_OPEN_ERR);
    } /* end if */
    /* Tell client ready to retrieve block */
    put_short(SUCCESS);
    /* Write text */
    /* Calculate the starttime */
    start_transfertime_calc();
    while ((len = get_string(buffer)) > 0)
    {
	for (cnt = 0; cnt < len; ++cnt)
	checksum += buffer[cnt];
	/* Now get the file offset */
	offset = get_longlong();
	/* Move to the position in the file to write the string */
	if (lseek(fd, offset, SEEK_SET) != offset)
	close_down1q(seek_err, file_name, SEEK_ERR);
    write(fd, buffer, len);
	total_len += len;
    } /* End while */
    /* Calculate the endtime */
    end_transfertime_calc();
    strcpy(transfertype, "Random parallel put");
    fs.original_size = fs.compressed_size = total_len;
    close(fd);
    /* Write final size and check sum to shared memory */
        shared_memory_write(total_len, checksum, block_num, DBYES);
    /* Tell client put successful */
    put_short(SUCCESS);
    sprintf(rtn.msg, "Block %d (%llu bytes)  transferred.", block_num, total_len);
    put_success_msg(key);
} /* End random_block_put */

void separate_block_put(
    char *key, long long offset, int mem_key, int block_num)		/* Key for big file to retrieve */
{
    extern char transfertype[];
    char buffer[BUFFER_SIZE];
    char file_name[FILE_SIZE];
    char real_name[FILE_SIZE];
    int fd;
    int disk_index;
    int len;
    int cnt;
    unsigned long long checksum = 0;
    long long total_len = 0;

    DBG();

    /* Read in the disk index */
    disk_index = disk_index_read(key, real_name);
    /* Build the name of the file */
    if (strcmp(server.site, DTMS) == 0)
    sprintf(file_name, "data_files%d/%s.gz/%s", disk_index, key, real_name);
    else sprintf(file_name, "data_files%d/%s.gz", disk_index, key);
    /* Allocate memory for this structure */
    shared_memory1allocate(key, mem_key, 1);
    /* Open the file and go to the appropriate position */
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0770)) < 0)
    {
    /* Write to shared memory indicating that no data sent */
	close_down1m(wr_open_err, file_name, WR_OPEN_ERR);
    } /* end if */
    /* Move to the correct position in the file */
    if (lseek(fd, offset, SEEK_SET) != offset)
    {
	close_down1q(seek_err, file_name, SEEK_ERR);
    } /* end if */
    /* Tell client ready to retrieve block */
    put_short(SUCCESS);
    /* Calculate the starttime */
    start_transfertime_calc();
    /* Write text */
    while ((len = get_string(buffer)) > 0)
    {
	for (cnt = 0; cnt < len; ++cnt)
	checksum += buffer[cnt];
	total_len += write(fd, buffer, len);
    }
    /* Calculate the endtime */
    end_transfertime_calc();
    close(fd);
    /* Write final size and check sum to shared memory */
        shared_memory_update(total_len, checksum);
    /* Tell client put successful */
    sprintf(rtn.msg, "Block %d (%llu bytes) transferred", block_num, total_len);
    put_success_msg(key);
} /* End separate_block_put */
