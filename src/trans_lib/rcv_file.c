/****************************************************************************
 *
 * Function:	rcv_file
 *
 * Description:	Basic routine used to retrieve files from the database client. 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"
#include "zlib.h"
#include "trans_lib_prototypes.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

unsigned long long rcv_file(
    char *path,
    char *new_file,
    int dtms,
    long long *final_size)
{
    extern char transfertype[];
    unsigned long long compressed_size = 0;
    struct stat buf;
    short ret;
    int fd;

    DBG("transfertype=%s", transfertype);

    /* Build complete file name */
    strcpy3(bf.file_name, path, new_file, ".gz");
    /* If on DTMS make the directory and put file in directory */
    if (strcmp(server.site, "DTMS") == 0)
    {
        mkdir(bf.file_name, 0770);
        strcpy2(bf.file_name + strlen(bf.file_name), "/", gu.key1);
    }
    /* Open file for writing */
    if ((fd = open(bf.file_name, O_WRONLY | O_CREAT, 0644)) < 0)
    {
        close_down1q(wr_open_err, bf.file_name,  WR_OPEN_ERR);
    }
    /* Tell client ready to get data */
    put_short(SUCCESS);
    /* Read in blocks of text and write them out to the file */
    start_transfertime_calc();
    while ((ret = get_string(bf.buffer)) > 0)
    {
        ret = write(fd, bf.buffer, ret);
        compressed_size += ret;
    } /* end while */
    end_transfertime_calc();
    strcpy(transfertype, "Serial put");
    /* Get the actual size */
     *final_size = get_longlong();
    /* Close the file just created */
    close(fd);
    /* Compare the two file sizes */
    /* Do a stat to compare compressed size with real size */
    stat (bf.file_name, &buf);
    if (buf.st_size != compressed_size)
    {
        sprintf(rtn.msg, bad_size, PROGRAM, fs.real_name, fs.key, compressed_size, buf.st_size);
        email_send("", admin.err_email, "Incorrect size", rtn.msg);
        /* Delete the file just transferred */
        unlink(bf.file_name);
        /* Delete the global file */
        /* build name for global file */
        strcpy2(bf.file_name, "global_data/", new_file);
        unlink(bf.file_name);
        /* Build error message to send to user */
        sprintf(rtn.msg, bad_size, PROGRAM, fs.real_name, fs.key, compressed_size, buf.st_size);
        /* Send error message */
        shut_down(rtn.msg, BAD_SIZE);
    } /* End if for file sizes do not match */
    return((long long )buf.st_size);
} /* end rcv_file */
