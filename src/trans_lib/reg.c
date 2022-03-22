/****************************************************************************
 *
 * Function:	reg_site_check, autoreg_user
 *
 * Description:	Verifies user is registered to use Transcend.
 *
 * Returns:	Returns 'DBYES' if Email address is found.
 *		Sends error message to client and exits if
 *		Email address is not found.
 *		The primary Email address is returned in parameter
 *		'user_email1' if it is found.
 *		Secondary Email address is returned in 'user_email2'.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "close_down.h"
#include "dir.h"
#include "edc_defines.h"
#include "edc_prototypes.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "reg_globals.h"
#include "user_params.h"
#include "rmt_server.h"
#include "rtn.h"
#include "server_lib.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

/* Determine if registration check needed */
int reg_check_required(void)
{
    DBG();

    if ((gu.req_type == DBGET_RELEASE_INFO)
    || (gu.req_type == DBAUTO_REG)
    || (gu.req_type == DBRMT_REG)
    || (gu.req_type == DBSERVER_SET)
    || (gu.req_type == DBBLOCK_PUT)
    || (gu.req_type == DBBLOCK_GET)
    || (gu.req_type == DBRANDOM_BLOCK_PUT)
    || (gu.req_type == DBRANDOM_BLOCK_GET)
    || (gu.req_type == DBSEPARATE_BLOCK_PUT)
    || (gu.req_type == DBSEPARATE_BLOCK_GET)
    || (gu.req_type == DBPUT_ACK)
    || (gu.req_type == DBGETDLT_ACK)
    || (gu.req_type == DBPUTDLT_ACK))
    return(DBNO);
    else return(DBYES);
} /* end reg_check_needed */

