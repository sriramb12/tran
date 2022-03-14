/****************************************************************************
 *
 * Function:	global_audit
 *
 * Description:	Verifies data integrity for global data locally and remotely.
 *		Use check sums to verify shadowed correctly.
 *		If not shadowed correctly, refresh remote servers.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "dir.h"
#include "global_caution_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2req_types.h"
#include "m2server_arrays.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "m2state_table.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "m2server_paths.h"
#include "m2server_paths.h"
#include "m2server_errs.h"
#include "user_params.h"
#include "mti.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "line.h"

void global_audit(long *global_audit_time)
{
    char audit_type[16];
    int refresh_cnt[5] = { 0, 0, 0, 0, 0 };
    int tmp_sock = sock;
    char *files[5000];
    char *data_ptr;
    char *delta_ptr;
    int cnt;
    int audit_cnt = 0;
    int len;

    /* Build name for audit file */
    /* Save audit type for later use */
    /* It gets clobbered when data is sent to remote server */
    strcpy(audit_type, mask_msg.key1);
    /* Make a list of global tables */
    dir_list(GLOBAL_DATA, files);
    /* Initialize pointers for folloing loop */
    len = strlen(mti.data_dir);
    data_ptr = mti.data_dir+len+strcpy1(mti.data_dir+len, "/");
    len = strlen(mti.delta_dir);
    delta_ptr = mti.delta_dir+len+strcpy1(mti.delta_dir+len, "/");
    /* Set master site in key3 */
    strcpy(mask_msg.key3, ip.master_site);
     for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* Build the data and delta directories */
	strcpy1(data_ptr, files[cnt]);
	strcpy1(delta_ptr, files[cnt]);
	/* If this is a temporary file, delete it */
	if (strcmp(files[cnt], "outt") == 0)
	{
	    unlink(mti.delta_dir);
	    unlink(mti.data_dir);
	    continue;
	} /* End if for temporary file */
	/* Build the state file name  */
	strcpy2(mti.state_file, mti.delta_dir, ".state");
	/* Make sure state file exists before reading it */
	if (access(mti.state_file, 0) != 0) continue;
	/* Read the state file */
	state_file_read();
	/* See if this node owns the data */
	/* Compare the update time to the audit update time */
	/* If it is newer, run the audit  */
	/* For official global data compare approval_time (app_time) */
	/* For global date compare touch_time */
	if (strcmp(st.master_site, ip.master_site ) == 0)
  {
	if (((mask_msg.data_type == GLOBAL) && (st.upd_time > *global_audit_time))
	|| ((mask_msg.data_type == OFF_GLOBAL) && (st.app_time > *global_audit_time)))
	{
	    /* Copy the global table name into key1 */
	    /* Function bld_header expects the keys to be initialized */
	    strcpy(mask_msg.key1, files[cnt]);
	    /* Also copy name into key */
	    strcpy(key, files[cnt]);
	log2audit("Auditing", mask_msg.key1);
	    audit_global(files[cnt], refresh_cnt);
	    ++audit_cnt;
	} /* End if for data needed to be audited */
	else if ((mask_msg.data_type == GLOBAL) && (st.touch_time > *global_audit_time))
	{
	    /* Copy the global table name into key1 */
	    strcpy(mask_msg.key1, files[cnt]);
	    strcpy(key, files[cnt]);
	log2audit("Auditing", mask_msg.key1);
	    audit_global(files[cnt], refresh_cnt);
	    ++audit_cnt;
	} /* End else for state needed to be audited */
      } /* End if for this server owns the data */
    } /* end for */
    /* Indicate how many global tables were audited */
    rtn.len = sprintf(rtn.msg, "Performed %s %s audit.\nFound %d modified global tables.\n",
    audit_type, data_array[mask_msg.data_type], audit_cnt);
    if (audit_cnt > 0)
    audit_print("global tables", refresh_cnt);
    /* Send success message to the client */
    /* Log message in audit report file */
    data_log(mti.audit_summary, rtn.msg);
    /* Restore original socket value */
    sock = tmp_sock;
    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
    /* If made it to this point, audit with remote servers complete */
/*
    *global_audit_time = tr.upd_time;
*/
    exit(0);
} /* End global_audit */
