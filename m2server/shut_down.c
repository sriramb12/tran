/****************************************************************************
 *
 * Function:	shut_down
 *
 * Description:	Sends return code to client during failed transaction
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <unistd.h>
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "lock_file.h"
#include "m2globals.h"
#include "m2server_errs.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "m2req_types.h"
#include "rtn.h"
#include "server_lib.h"

/* Sends last message, closes socket and cleans up temporary files */
void shut_down(
    char *msg,		/* Points to message sent to user */
    register short rtn_code)	/* Holds return code sent to user */
{

    /* Log the error */
    data_log(tr.log_file, msg);
    /* Send user return code and message */
    put_short(M2ERR);
    put_short(rtn_code);
    put_string(msg);
    close(sock);
    if (st.fd != 0) state_file_close(0);
    exit(1);
} /* End shut_down */

void rmt_shut_down(
    char *msg,		/* Points to message sent to user */
    register short rtn_code)	/* Holds return code sent to user */
{

    /* Log the error */
    data_log(tr.log_file, msg);
    /* Send user return code and message */
    put_short(rtn_code);
    put_string(msg);
    close(sock);
    if (st.fd != 0) state_file_close(0);
    lock_files_clear();
    log3("Closing transaction ", tr.pid_string, ".");
    exit(1);
} /* End shut_down */

void clean_up(char *msg)
{

    /* If state file still open, close it */
    if (st.fd != 0) close(st.fd);
    /* Unlock files if locked */
    lock_files_clear();
    /* Log message already sent */
    data_log(tr.log_file, msg);
    /* Exit program */
    exit(1);
} /* end clee_f/n_up */
