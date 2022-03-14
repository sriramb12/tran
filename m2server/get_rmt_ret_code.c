/****************************************************************************
 *
 * Function:	get_rmt_ret_code
 *
 * Description:	xx
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
#include "m2server_globals.h"
#include "rmt_server_lib.h"
#include "rtn.h"
#include "server_lib.h"

int get_rmt_ret_code(int log)
{
    extern TMP_BUFFER bf;
    short ret;

    ret = get_short();
	if (ret == GET_SHORT_ERR) 
    {
	close(sock);
	return(GET_SHORT_ERR);
    } /* End if */
	if ((ret = get_rmt_string(bf.buffer)) <= 0) 
    {
	close(sock);
	rtn.len += strcpy(rtn.msg+rtn.len, get_err);
	return(GET_ERR);
    }
	/* Log a message */
	rtn.len += strcpy4(rtn.msg+rtn.len, bf.buffer, " by ", tr.pid_string, "\n");
    if (log == DBYES)
    {
	data_log(tr.log_file, rtn.msg);
	rtn.len = 0;
    }
	/* Close the connection */
	close(sock);
    return(ret);
} /* End get_rmt_ret_code */
