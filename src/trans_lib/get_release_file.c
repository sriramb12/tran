/****************************************************************************
 *
 * Function:	get_release_file
 *
 * Description:	Basic function to send a file to the database client.
 *		        Used to send user's new releases, man pages,
 *		        Transcend documentation, etc.  Files sent 
 *		        reside in directory 'releases'.
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>

#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_errs.h"
#include "user_params.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


/* Send a release file */
void get_release_file(char *file_name, char *root_file)
{
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */

    DBG();

    /* Obtain a file descriptor to file */
    fd = open(file_name, O_RDONLY);

    if (fd == EOF)
        close_down1f(non_err, file_name, NON_ERR);

    /* Indicate that file wil be sent */
    put_short(SUCCESS);

    /* Put size, real size not known */
    put_long(1722936);
    put_long(33279);

    /* Send name of file */
    /* Pass root name only without full path */
    put_string(root_file);

    /* Get acknowledgement */
    if ((ret = get_short()) != SUCCESS)
    {
	    /* If unsuccessful, log failure, clean-up and exit */
	    close(fd);
	    strcpy3(rtn.msg, "Retrieval of '", root_file, "' aborted.");
	    put_success_msg(NONE);
	    exit(0);
    } /* end if for retrieval aborted by client */

    /* Read text from the file and write them to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE))  > 0)
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this file sent */
    put_short(SUCCESS);

    /* Close the file just sent */
    close(fd);
} /* end get_release_file */


void get_intercom_file(char *file_name, char *root_file)
{
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */

    DBG();

    /* Obtain a file descriptor to file */
    fd = open(file_name, O_RDONLY);

    if (fd == EOF)
        close_down1f(non_err, file_name, NON_ERR);

    /* Indicate that file wil be sent */
    put_short(SUCCESS);

    /* Put size, real size not known */
    /* Send file name, size, mode, and file type */
    sprintf(bf.buffer, "%s %d %d %d", root_file, 0, 33279, 'b');
    put_string(bf.buffer);

    /* Get acknowledgement */
    /* If unsuccessful, log failure, clean-up and exit */
    if ((ret = get_short()) != SUCCESS)
        shut_down(bad_ack, BAD_ACK);

    /* Read text from the file and write them to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE))  > 0)
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this file sent */
    put_short(SUCCESS);

    /* Close the file just sent */
    close(fd);
} /* end get_intercom_file */

/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
