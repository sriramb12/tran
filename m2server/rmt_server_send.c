/****************************************************************************
 *
 * Function:	m2server_send
 *
 * Description:	Finds IP node address and name for 
 *		specified server site.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <strings.h>
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "close_down.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "mask_server_defines.h"
#include "m2globals.h"
#include "m2state_file.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "m2server_errs.h"
#include "rmt_server.h"
#include "strcpy.h"
#include "rtn.h"
#include "server_lib.h"

void m2server_send(char *master_site)
{

    register int cnt;

    /* Tell client do not own the data */
    /* Verify that rmt_mpd.server_num has a valid value */
    if ((rmt_mpd.server_num > TOTAL_SERVERS) || (rmt_mpd.server_num < 0)) 
    rmt_mpd.server_num = TOTAL_SERVERS;
        /* Look for the remote site in the remote server table */
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
		if (strcmp(master_site, rmt_mpd.site[cnt]) == 0)
	{
	sprintf(rtn.msg, "%s %s %d", rmt_mpd.mach[cnt], rmt_mpd.addr[cnt], rmt_mpd.sock[cnt]);
	    put_short(M2ERR);
	    put_short(OWN_WRN);
	    put_string(rtn.msg);
	    close(sock);
	    /* Build the message to log */
	    strcpy3(rtn.msg, own_wrn, rmt_mpd.mach[cnt], "'");
	    data_log(tr.log_file, rtn.msg);
    	log3("Closing ", "transaction ", tr.pid_string);
	    exit(0);
	} /* End if for remote server found */
    } /* End for statement */
	/* If fell out of loop could not find the server */
    close_down1f(owner_not_fnd, master_site,  OWNER_NOT_FND);
} /* End m2server_send */
