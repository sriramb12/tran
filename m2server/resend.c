/****************************************************************************
 *
 * Function:	process_resend
 *
 * Description:	Once an hour, the server is sent a  request  from  a  client
		through a cron job to resend any data it could not previous-
		ly send to remote servers because of  inaccessibility  prob-
		lems.   This  function  attempts to resend all data that has
		not previously been updated on remote servers.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "close_down.h"
#include "files.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "mask_server_defines.h"
#include "m2req_types.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "server_lib.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "resend.h"
#include "rmt_server.h"
#include "rtn.h"
#include "m2msg.h"
#include "mti.h"
#include "strcpy.h"

void resend_data(void)
{
    register int cnt;
    char tmp_resend_file[64];
    char resend_file[64];

    /* Tell the client the data has been processed */
    put_short(SUCCESS);
    put_short(SUCCESS);
    put_string("Remote data resent.");
    close(sock);
    data_log(tr.log_file, "remote data resent.");
    /* Put in the master site */
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	/* Build the temporary file name */
	strcpy2(tmp_resend_file, "resend_data/tmp_", rmt_mpd.site[cnt]);
	/* Build name for permanant resend file */
	strcpy2(resend_file, "resend_data/", rmt_mpd.site[cnt]);
	if (access(tmp_resend_file, 0) == 0) 
	{
	    system(tmp_resend_file);
	    /* Get rid of the temporary file no longer needed */
	    unlink(tmp_resend_file);
	} /* End if for temporary file exists */
	/* Now resend the data in the original file */
	if (access(resend_file, 0) != 0) continue;
	file_rename(resend_file, tmp_resend_file);
    system(tmp_resend_file);
	/* Get rid of the temporary file no longer needed */
	unlink(tmp_resend_file);
    } /* End big for */
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* End resend_data */


/* Adds new entry when data must be resent */
int resend_entry_add(char *server_site, int server_index)
{
    char entry[260];
    char resend_file[64];
    char local_data_type[2];

    /* Build the data type */
    switch(mask_msg.data_type)
    {
	case MASK_SET: strcpy(local_data_type, "m"); break;
	case TEMPLATE: strcpy(local_data_type, "t"); break;
	case OFF_TEMPLATE: strcpy(local_data_type, "T"); break;
	case GLOBAL: strcpy(local_data_type, "g"); break;
	case MACHINE: strcpy(local_data_type, "M"); break;
	default:  strcpy(local_data_type, "m"); break;
    } /* End switch */
    /* Build the entry to add */
    sprintf(entry, "cd resend_data; m2client -%s -S %s %s %d %s",
    local_data_type, mask_msg.key1, mask_msg.key2, server_index, server_site);
    /* Build name of file entry being added to */
    strcpy2(resend_file, "resend_data/", server_site);
    entry_add(resend_file, entry, entry);
    /* Give file execute mode */
    chmod(resend_file, 0700);
    return(SUCCESS);
} /* End resend_entry_add */
