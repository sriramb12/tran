/****************************************************************************
 *
 * Function:	x500authenticate
 *
 * Description:	Verifies user is registered to use Transcend.
 *
 * Returns:	Returns 'DBYES' if Email address is found.
 *		    Sends error message to client and exits if
 *		    Email address is not found. The primary
 *		    Email address is returned in parameter
 *		    'user_email1' if it is found. Secondary
 *		    Email address is returned in 'user_email2'.
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
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
#include "trans_server_prototypes.h"
#include "reg_globals.h"
#include "user_params.h"
#include "rmt_server.h"
#include "rtn.h"
#include "server_lib.h"
#include "trans_server_prototypes.h"

#include "global_debug.h"

/* Lookup users with LDAP */
void x500lookup(char *user_id)
{
    int ret;

    DBG("user_id='%s'", user_id);

    ret = ldapfsl_user_lookup_and_copy_global_structures(user_id);

     if (ret == DBYES)
     {
         strcpy3(rtn.msg, gu.email2, ";", user.name);
         /* Now tell user authentication successful */
         put_short(SUCCESS);
         put_success_msg(NONE);
         exit(0);
    } /* End if for lookup successful */
    /* If return not successful, give failure */
    else shut_down(authen_err, AUTHEN_ERR);
} /* End x500lookup */
