/****************************************************************************
 *
 * Function:	user_autoreg
 *
 * Description: Performs auto registration
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "edc_defines.h"
#include "reg_globals.h"
#include "dbreg_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_server_params.h"
#include "user_params.h"
#include "x500_defines.h"
#include "registry_prototypes.h"

#include "global_debug.h" // Mazieri's debug facilities

void user_autoreg(void)
{
    char user_name1[80];
    char user_name2[80];
    char user_name3[80];
    int fnd;

    DBG();
    
    /* If email address not already in gu.email1 and gu.email2, put it in */
    strcpy(gu.email1, gu.par1);
    strcpy(gu.email2, gu.par1);

    /* Try registering the user */
    sscanf(user.name, "%s %s %s", user_name1, user_name2, user_name3);
    fnd = dt_reg_user(user.login, tr.user_site, gu.email1, gu.email2, user_name1, user_name2, user_name3, gu.par2);
    put_short(SUCCESS);

    if (fnd == ACTIVE)
    {
	    /* Register Transcend, InterCom and Mask Prep server */
	    strcpy(rtn.msg, "You have been registered in the data transfer registry.");
	    put_success_msg(NONE);
    } /* End if */
    else if (fnd == DUP_ENTRY)
    {
	    strcpy(rtn.msg, "You have been re-registered to use Transcend.");
	    put_success_msg(NONE);
    } /* End if */
    else if (rtn.len > 0) shut_down(rtn.msg, -1);
    else shut_down("Registration not performed.", -1);

    exit(0);
} /* end user_autoreg */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
