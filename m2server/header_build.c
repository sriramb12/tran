/****************************************************************************
 *
 * Function:	header_build
 *
 * Description:	Builds header send to remote servers for remote updates
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include "global_defines.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2req_types.h"
#include "m2server_arrays.h"
#include "m2state_file.h"
#include "rmt_server.h"
#include "user_params.h"
#include "rtn.h"
#include "m2msg.h"

void header_build(char *header_buffer, int req_type, int data_type)
{
    int cnt;

    /* First copy the password */
    for (cnt = 0; cnt <12; ++cnt)
    header_buffer[cnt] = mask_msg.password[cnt];
    sprintf(header_buffer+12, " %d %d %d %d %s %s %s %s %s %s %s %s %s %s %d %d %s %s %s %s %s %s;",
    /* Specific request type, general request type, type of data, etc */
    req_type, data_type, mask_msg.override_flag, mask_msg.program,
    user.login, user.email, user.mach, user.addr, user.cwd, user.machine_type,
    mask_msg.key1, mask_msg.key2, mask_msg.key3, user.pin, mask_msg.num,  mask_msg.transaction_time,
    mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4, mask_msg.par5, user.name);
} /* End header_build */
