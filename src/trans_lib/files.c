/****************************************************************************
 *
 * Function:	files_get
 *
 * Description:	Basic function to send one or more
 *              files to the database client.
 *		        Files to send are passed in a file
 *		        array with one or more elements.
 *
 ***************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

#include "close_down.h"
#include "dir.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "opc_server_files.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities

#define gz_sockopen_err "test"
#define GZ_SOCKOPEN_ERR -256
#define DIRECTORY 'd'


void file_get(char *dir, char *file_name)
{
    short ret;
    char full_file_name[FILE_SIZE];
    int fd;

    DBG();

    /* Build complete file name to be opened */
    strcpy3(full_file_name, dir, "/",  file_name);

    /* Obtain a file descriptor to file */
    fd = open(full_file_name, O_RDONLY);

    if (fd == EOF)
        close_down1m(rd_open_err, full_file_name, RD_OPEN_ERR);

    /* Send name of file */
    /* Pass root name only without full path */
    put_string(file_name);

    /* Get acknowledgement */
    if ((ret = get_short()) != SUCCESS)
    {
        close(fd);
        /* If unsuccessful, log failure, clean-up and exit */
        shut_down(bad_ack, BAD_ACK);
    }

    /* Read text from the file and write them to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0)
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this file sent */
    put_short(SUCCESS);

    /* Close the file just sent */
    close(fd);
} /* End file_get */
