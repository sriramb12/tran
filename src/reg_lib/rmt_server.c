#ifndef TRANS_FTP
/****************************************************************************
 *
 * Function:	rmt_server_read
 *
 * Description:	Reads remote servers during database initialization.
 *		First Reads in Mask Prep servers 
 *		Then reads in Transcend servers
 *		Finally reads in Intercom servers
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "rmt_server.h"
#include "rtn.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

void rmt_server_read(void)
{
    char line[260];
    FILE *fp;
    extern RMT_SERVERS rmt_trans;

    DBG();

    /* Make a list of the remote servers */
    if ((fp = fopen(RMT_SERVER_TABLE, "r")) == NULL)
    {
	    strcpy5(rtn.msg, rd_open_err, RET2TAB, "'", RMT_SERVER_TABLE, "'");
	    printf("%s\n", rtn.msg);
	    exit(1);
    }

    /* Read in the Transcend servers */
    rmt_trans.server_num = 0;
    while ( line_get(fp, line) > 0) 
    {
	    /* If this is a comment line skip it */
	    if (line[0] == '#') continue;

	    sscanf(line, "%s %s %s %d", rmt_trans.site[rmt_trans.server_num], rmt_trans.mach[rmt_trans.server_num], rmt_trans.addr[rmt_trans.server_num], &rmt_trans.sock[rmt_trans.server_num]);

	    ++rmt_trans.server_num;
    } /* End while */

    /* Close the file */
    fclose(fp);
} /* End rmt_server_read */


/* Find index for specified server site */
int rmt_server_find(char *site)
{
    int cnt;

    DBG("site='%s'", site);

    for (cnt = 0; cnt <rmt_mpd.server_num; ++cnt)
    {
	    if (strcmp(site, rmt_mpd.site[cnt]) == 0) return(cnt);
    } /* End for */

    /* If reached this point, did not find the server */
    return(FAILURE);
} /* End rmt_server_find */
#endif /* for not TRANS_FTP */
