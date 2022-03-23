/****************************************************************************
 *
 * Function:	file_rcv
 *
 * Description:	Receives a file from the client or remote server.
 *		Puts names of file received in array 'files'.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "dir.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "server_lib.h"
#include "strcpy.h"

int file_rcv(
    char *file[1],
    char *dir)
{
    extern TMP_BUFFER bf;
    short ret;
    short fd;
    char *file_ptr;	/* Points to end of path name */

    /* Build directory name where files will be placed */
    file_ptr = bf.file_name + strcpy2(bf.file_name, dir, "/");
    /* Send SUCCESS to indicate ready to receive files */
    put_short(SUCCESS);
    /* First get file name */
    /* If invalid file name, return */
    if ((ret = get_string(file_ptr)) <= 0) return(FAILURE);
    /* Open file for writing */
    if ((fd = open(bf.file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
    {
    close_down1m(rd_open_err, bf.file_name,  RD_OPEN_ERR);
    }
    /* Read in blocks of text and write them out to the file */
    /* Tell client ready to get text */
    put_short(SUCCESS);
    while (1)
    {
    /* Read a block from the file */
	ret = get_string(bf.buffer);
	if (ret == 0) break;
	/* Write the text out to the file */
	write(fd, bf.buffer, ret);
    } /* end while */
    /* Close the file just created */
    close(fd);
    /* Copy file name into file array */
    /* First allocate space for file */
    file[0] = malloc(strlen(file_ptr) +1);
    strcpy(file[0], file_ptr);
    /* Restore file name to original state */
    file_ptr[0] = 0;
    return(SUCCESS);
} /* end file_rcv */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
