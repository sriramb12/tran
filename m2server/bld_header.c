/****************************************************************************
 *
 * Function:	bld_header
 *
 * Description:	Builds header to send to remote server
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include "global_defines.h"
#include "m2msg.h"
#include "m2server_globals.h"
#include "user_params.h"

void bld_header(
    char *buffer,
     int req_type,
    int data_type,
    int transaction_num)
{
    register int cnt;

    /* Put password in message string */
    /* Must put in by hand so nulls will be put in */
    for (cnt = 0; cnt < 12; ++cnt)
    buffer[cnt] = mask_msg.password[cnt];
    sprintf(buffer+12, "%d %d %d %d %s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d %d %s;%s;%s;%s;%s;%s;",
    req_type, data_type, mask_msg.override_flag, mask_msg.program,
    user.login, user.email, user.mach, user.addr, user.cwd, user.machine_type,
    mask_msg.key1, mask_msg.key2, mask_msg.key3, user.pin,  mask_msg.num, transaction_num,
    mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4, mask_msg.par5, user.name);
} /* End bld_header */
