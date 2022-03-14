/****************************************************************************
 *
 * Function:	turned_off_mask_checkout
 *
 * Performs functions for turned off masks
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include "global_defines.h"
#include <unistd.h>
#include "close_down.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "mask_server_defines.h"
#include "m2req_types.h"
#include "m2server_globals.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "m2server_errs.h"
#include "m2state_table.h"
#include "m2state_file.h"
#include "user_params.h"
#include "mti.h"
#include "rtn.h"
#include "server_lib.h"
#include "m2msg.h"
#include "strcpy.h"
#include "server_params.h"

void turned_off_mask_checkout(int state)
{
    char *mask_list[30];

    /* If override flag on, do not give override */
    if (mask_msg.override_flag == DBYES) return;
    /* Make this check only for read and check-out transactions */
    if (mask_msg.req_type != CHECKOUT)
    return;
    /* Read in the list of mask sets */
    if (access(TURNED_OFF_MASKS, 0) != 0) return;
    list_read(TURNED_OFF_MASKS, mask_list);
    /* If mask set not in the mask table */
       if (list_find(mask_list, mask_msg.key1) == FAILURE) return;
    /* Now send the override condition */
    return_override(bad_mask_ovr, BAD_MASK_OVR);
} /* end turned_off_mask_checkout */

void turned_off_mask_delete(char *mask)
{
    char msg[FILE_SIZE];
    char *mask_list[1000];

    int cnt = list_read(TURNED_OFF_MASKS, mask_list);
    if (m2list_delete(mask_list, mask, cnt) >= 0)
    {
    list_write(TURNED_OFF_MASKS, mask_list);
    sprintf(msg, "Turned off Mask set '%s' checked in by '%s'.\n",
    mask, user.email);
    data_log(tr.log_file, msg);
    turned_off_mask_email(mask, user.email, msg+11);
    } /* end if */
} /* end turned_off_mask_delete */

/* See if turned off mask set has been checked out */
/* If it has, log message and send Rob e-mail */
void turned_off_mask_log(char *mask)
{
    char msg[FILE_SIZE];
    char *mask_list[30];
    /* Read in list of mask sets */
    if (access(TURNED_OFF_MASKS, 0) != 0) return;
    list_read(TURNED_OFF_MASKS, mask_list);
    /* Look for the mask set */
    if (list_find(mask_list, mask) == SUCCESS)
    {
    sprintf(msg, "Turned off Mask set '%s' checked out by '%s'.\n",
    mask, user.email);
    data_log(tr.log_file, msg);
    /* Send e-mail */
    turned_off_mask_email(mask, user.email, msg+11);
    } /* End if for mask set found */
} /* End  */

void turned_off_mask_wrn_check(char *mask)
{
    char *mask_list[30];

    /* Read in list of mask sets */
    if (access(TURNED_OFF_MASKS, 0) != 0) return;
    list_read(TURNED_OFF_MASKS, mask_list);
    /* Look for the mask set */
    if (list_find(mask_list, mask) == SUCCESS)
    rtn.len += strcpy1(rtn.msg+rtn.len, bad_mask_wrn);
} /* End  */

void turned_off_mask_email(char *mask, char *user_id, char *msg)
{
    char subject[80];

    strcpy(subject, msg);
    strcat(msg, bad_mask_msg);
    email_send("", "mdsEvents@nxp.com", subject, msg);
    strcat(msg, bad_mask_msg);
}
