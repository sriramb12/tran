/****************************************************************************
 *
 * Function:	return_err, return_success, return_warning
 *
 * Description:	sends  message to user.
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
#include "mask_server_defines.h"
#include "m2globals.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "m2req_types.h"
#include "rtn.h"
#include "server_lib.h"

/* Sends last message and closes socket */
void return_err(char *msg, int rtn_code)
{

    /* Clear any lock files set */
    lock_files_clear();
    /* Send user return code and message */
    put_short(M2ERR);
    put_short(rtn_code);
    put_string(msg);
    close(sock);
    data_log(tr.log_file, msg);
    exit(1);
} /* end return_err */

void return_m2badpin(char *msg, int rtn_code)
{

    /* Send user return code and message */
    put_short(M2BADPIN);
    put_short(rtn_code);
    put_string(msg);
    close(sock);
    data_log(tr.log_file, msg);
    exit(1);
} /* end return_m2badpin */


/* Sends last message and closes socket */
void return_override(char *msg, int rtn_code)
{
    /* Clear any lock files set */
    lock_files_clear();
    /* Send user return code and message */
    put_short(M2OVR);
    put_short(rtn_code);
    put_string(msg);
    close(sock);
    data_log(tr.log_file, msg);
    exit(1);
} /* end return_override */

/* Sends last message and closes socket */
void return_success(char *msg, char log_msg)
{

    /* Send user return code and message */
    put_short(SUCCESS);
    put_string(msg);
    close(sock);
    if (mask_msg.req_type != EVENT_LOG)
    data_log(tr.log_file, msg);
    /* Make log entry in data_log file */
    if(log_msg == DBYES) m2data_log(msg);
} /* end return_success */

/* Sends last message and closes socket */
void return_warning(char *msg, int return_code, char log_msg)
{

    /* Send user return code and message */
    put_short(M2WRN);
    put_short(return_code);
    put_string(msg);
    close(sock);
    data_log(tr.log_file, msg);
    /* Log message in data_log file */
    if (log_msg == DBYES)
    m2data_log(msg);
} /* end return_warning */

void return_partial_warning(char *msg, int return_code, char log_msg)
{

    /* Send user return code and message */
    put_short(return_code);
    put_string(msg);
    close(sock);
    data_log(tr.log_file, msg);
    /* Log message in data_log file */
    if (log_msg == DBYES)
    m2data_log(msg);
} /* end return_warning */
