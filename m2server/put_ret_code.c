/****************************************************************************
 *
 * Function:	put_ret_code
 *
 * Description:	Sends client return code and return message at the end of a 
 *		successful database transaction.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "mask_server_defines.h"
#include "mask_server_paths.h"
#include "m2globals.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "pre_suffix.h"
#include "m2server_errs.h"
#include "m2req_types.h"
#include "m2server_arrays.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "user_params.h"
#include "mti.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "m2msg.h"

/* Sends user successful return code */
void put_ret_code(char log_msg)
{
    extern TMP_BUFFER bf;

    /* Build message to send to user */
    strcpy2(bf.buffer, mti.prefix, mti.suffix_ptr);
	    /* If length of return message is 0, no warnings to send */
    if (rtn.len == 0)
    {
	put_short(SUCCESS);
	/* Send a status message to the client */
	put_string(bf.buffer);
    data_log(tr.log_file, bf.buffer);
    }
    /* Otherwise send warning */
    else
    {
	put_short(M2WRN);
	put_short(M2WRN);
	put_string(rtn.msg+2);
	data_log(tr.log_file, rtn.msg+1);
	rtn.len = 0;
    }
    /* Close the socket */
    close(sock);
    /* If not deleting data or processing global data, */
    /*  log the message in the data log file */
    /* If not processing global data, put message in data log file */
    if (log_msg == DBYES) m2data_log(bf.buffer);
} /* end put_ret_code */

/* Log message in data log */
void m2data_log(char *msg)
{

    /* Build the message to log */
    /* If this is a history transaction must build message to log */
    if (mask_msg.req_type == HISTORY)
    strcpy3(msg, "History for ", mti.suffix, " retrieved.");
    sprintf(bf.msg, "Date:  %s\nRequester:  %s(%s)\nTransaction:  %s %s\nResult:  %s\n",
    log_date, user.login, user.email, 
    req_array[mask_msg.req_type], data_array[mask_msg.data_type], 
    msg);
    /* Build the data log file name */
    if (mask_msg.data_type == GLOBAL)
    strcpy4(bf.file_name, mti.delta_dir, "/", key, ".data_log");
    else if (mask_msg.data_type == CAP_WAFER)
    strcpy3(bf.file_name, MASK_DLTA, mask_msg.key2, "/data_log");
    else strcpy2(bf.file_name, mti.delta_dir, "/data_log");
    data_log(bf.file_name, bf.msg);
} /* End m2data_log */
