/****************************************************************************
 *
 * Function:	process_request
 *
 * Description:	Processes requests from Mask Designer client
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include "dir.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "mask_entry.h"
#include "m2files.h"
#include "m2msg.h"
#include "m2req_types.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "mti.h"
#include "rmt_server_lib.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"


/* Refresh a remote mask set or template */
void rmt_refresh_data(void)
{
    char tmp_data_dir[FILE_SIZE];
    char tmp_delta_dir[FILE_SIZE];

     /* Tell remote server ready to receive data */
    put_short(REFRESH);
    /* Move delta and data directories to temporary place */
    strcpy2(tmp_data_dir, mti.data_dir, ".tmp");
    strcpy2(tmp_delta_dir, mti.delta_dir, ".tmp");
    rename(mti.data_dir, tmp_data_dir);
    rename(mti.delta_dir, tmp_delta_dir);
    /* Get the data using the gtar command */
    command_put("gtar xf -");
    /* Delete temporary directories no longer needed */
    if (access(tmp_data_dir, 0) == 0) dir_recursive_dlt(tmp_data_dir);
    if (access(tmp_delta_dir, 0) == 0) dir_recursive_dlt(tmp_delta_dir);
    /* Indicate refresh successful */
    strcpy3(rtn.msg, mti.prefix, " refreshed on ", ip.master_site);
    return_success(rtn.msg, DBNO);
    exit(0);
} /* End rmt_refresh_data */

void rmt_refresh_global(void)
{

     /* Tell remote server ready to receive data */
    put_short(REFRESH);
    /* Get the data using the gtar command */
    command_put("gtar xf -");
    /* Indicate refresh successful */
    strcpy3(rtn.msg, mti.prefix, " refreshed on ", ip.master_site);
    return_success(rtn.msg, DBNO);
    exit(0);
} /* End rmt_refresh_global */

void rmt_create_data(void)
{

    /* If data or delta dir present, call refresh instead */
    if ((access(mti.data_dir, 0) == 0)
    || (access(mti.data_dir, 0) == 0))
    {
	rmt_refresh_data();
	return;
    }
     /* Tell remote server ready to receive data */
    put_short(REFRESH);
    /* Get the data using the gtar command */
    command_put("gtar xf -");
    /* Indicate refresh successful */
    strcpy3(rtn.msg, mti.prefix, " refreshed on ", ip.master_site);
    return_success(rtn.msg, DBNO);
} /* End rmt_create_data */

    
void rmt_mark_data(void)
{
    char file_name[32];
    /* Tell remote server ready to retrieve data */
    put_short(MARK);
    /* Get the state file */
    file_get(mti.delta_dir, file_name);
    /* Tell server transaction successful */
    strcpy3(rtn.msg, mti.prefix, " marked on ", ip.master_site);
    return_success(rtn.msg, DBNO);
} /* End rmt_mark_data */

/* Send refresh request to remote server */
/* No data is sent with this request */
void refresh_request(int req_type)
{
    register short cnt;
    short ret;

    /* Build header to send to remote server */
    strcpy(mask_msg.key3, ip.master_site);
    bld_header(bf.header_buffer, req_type, mask_msg.data_type, 0);
    /* Update each server in a for loop */
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
	    put_short(SUCCESS);
	/* Get the return code */
	get_rmt_ret_code(DBYES);
     } /* End for */
    /* Log end of transaction and exit */
    log3("Closing transaction ", tr.pid_string, ".");
    exit(0);
} /* end refresh_request */

/* Refresh data done after header already sent */
int refresh_data(void)
{
    char command[FILE_SIZE];

    /* Build the command */
    strcpy4(command, "gtar cf - ", mti.data_dir, " ", mti.delta_dir);
    command_get(command);
    /* Get the return code */
    return(get_rmt_ret_code(DBYES));
	} /* end refresh_data */

void refresh_global(void)
{
    char command[FILE_SIZE];

    /* Build the command */
    strcpy5(command, "gtar cf - ", mti.data_dir, " ", mti.delta_dir, "*");
    command_get(command);
    /* Get the return code */
    get_rmt_ret_code(DBYES);
	} /* end refresh_global */

void rmt_request_send(int req_type)
{
    register short cnt;
    short ret;

    /* Build header to send to remote server */
    strcpy(mask_msg.key3, st.master_site);
     mask_msg.transaction_time = 0;
    bld_header(bf.header_buffer, req_type, mask_msg.data_type, st.transaction_num);
    /* Update each server in a for loop */
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
	/* Get the return code */
	if (ret == SUCCESS)
	get_rmt_ret_code(DBYES);
    } /* End for */
} /* end rmt_request_send */
