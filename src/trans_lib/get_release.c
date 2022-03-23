/****************************************************************************
 *
 * Function:	get_release
 *
 * Description:	Retrieves a Transcend release from the Transcend database.
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>

#include "close_down.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "strcpy.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "rtn.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


void get_release(char *key)
{
    DBG();

    /* Make sure the release exists */
    /* Build the file name */
    strcpy2(bf.file_name, "releases/", key);

    if (access(bf.file_name, 0) != 0)
        close_down1f(non_err, key, NON_ERR);

    get_release_file(bf.file_name, key);
    strcpy3(rtn.msg, "File '", key, "' retrieved.");

    /* Send the return code */
    put_success_msg(key);
} /* End get_release */


void get_icom_release(char *key)
{
    DBG();

    /* Make sure the release exists */
    /* Build the file name */
    strcpy2(bf.file_name, "intercom/", key);

    if (access(bf.file_name, 0) != 0)
        close_down1f(non_err, key, NON_ERR);

    get_intercom_file(bf.file_name, key);
    strcpy3(rtn.msg, "File '", key, "' retrieved.");

    /* Send the return code */
    put_success_msg(key);
} /* End get_release */


/* Retrieve information stored in the releases file */
void release_info_rtv(char *release_file)
{
    int fd;
    int len;

    DBG();

    /* Open release file */
    if ((fd = open(release_file, O_RDONLY)) == EOF)
        close_down1m(rd_open_err, release_file, RD_OPEN_ERR);

    if ((len = read(fd, rtn.msg, BLOCK_SIZE)) < 1)
        close_down1m(read_err, "releases/releases", READ_ERR);

    rtn.msg[len] = '\0';
    
    /* Close the file */
    close(fd);

    /* Tell client data will be sent */
    put_short(SUCCESS);

    /* Send return code */
    put_success_msg(NONE);
} /* End release_info_rtv */

/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
