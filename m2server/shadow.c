/****************************************************************************
 *
 * Function:	process_files
 *
 * Description:	xx
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "close_down.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2files.h"
#include "m2server_globals.h"
#include "m2server_errs.h"
#include "m2req_types.h"
#include "m2server_prototypes.h"
#include "pre_suffix.h"
#include <fcntl.h>
#include <stdio.h>
#include "mti.h"
#include "strcpy.h"
#include "rtn.h"
#include "server_lib.h"



void files_shadow(char *dir, char *files[])
{
    register short cnt;
    extern TMP_BUFFER bf;
    short ret;

    /* Build header to send to remote server */
    strcpy(mask_msg.key1, dir);
    bld_header(bf.header_buffer, SHADOW, CRON, 0);
    /* Update each server in a for loop */
    rtn.len = 0;
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
	if (ret != SUCCESS) continue;
	static_files_send(dir, files);
	/* Get the return code */
	get_rmt_ret_code(DBNO);
    } /* End for */
    /* Free memory no longer needed */
    files_free(files);
    /* Log messages received */
    data_log(tr.log_file, rtn.msg);
} /* End files_shadow */

void files_shadow_rcv(char *dir)
{
    char *files[M2TOTAL_FILES];
	
    /* Indicate ready to receive data */
    put_short(SUCCESS);
    files_rcv(dir, files);
    /* Send return code */
    strcpy4(rtn.msg, "Files in directory '", dir, "' shadowed to ", ip.master_site);
    return_success(rtn.msg, DBNO);
} /* End files_shadow_rcv */
