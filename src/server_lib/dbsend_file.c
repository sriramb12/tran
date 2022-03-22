/****************************************************************************
 *
 * Function:	dbsend_file
 *
 * Description:	Sends one file to the client.
 *		Full file name of file to send is 
 *		passed to this function.
 *		The function separates the root file name from the path.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities

void dbsend_file(char *file_name)
{
    char *file_ptr;
    extern TMP_BUFFER bf;
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */

    DBG("file_name='%s'", file_name);

    /* Obtain a file descriptor to file */
    fd = open(file_name, O_RDONLY);
    if (fd == EOF)
        shut_down(no_data, TRANS_NO_DATA);

    /* Send name of file */
    /* Pass root name only without full path */
    file_ptr = strrchr(file_name, '/');
    if (file_ptr == NULL)
        put_string(file_name);
    else
        put_string(file_ptr+1);

    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        clean_up(bad_ack);

    /* Read text from the file and write it to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0) 
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this file sent */
    put_short(SUCCESS);

    /* Close the file just sent */
    close(fd);
} /* end dbsend_file */
