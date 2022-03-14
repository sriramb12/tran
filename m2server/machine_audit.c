/****************************************************************************
 *
 * Function:	machine_audit
 *
 * Description:	Verifies data integrity for machine files locally and remotely.
 *		Read into memory list of machine files in global table.
 *		Do a directory list to see machines present.
 *		Verify they are defined in the global table.
 *		If they are not, add them.
 *		Use check sums to verify shadowed correctly.
 *		If not shadowed correctly, refresh remote servers.
 *		Verify there are no extra entries in the global machine table.
 *		If there are, delete them.
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

void machine_audit(long *machine_audit_time)
{
    int tmp_sock = sock;
    char *files[M2TOTAL_FILES];
    char audit_type[12];
    char *data_ptr;
    char *delta_ptr;
    struct stat buf;
    int cnt;
    int audit_cnt = 0;
    int refresh_cnt[5] = { 0, 0, 0, 0, 0 };

    /* Build name for audit file */
    /* Make a list of machine files */
    dir_list(FAB_DELTA, files);
    /* Save audit type for later use */
    strcpy(audit_type, mask_msg.key1);
    /* Initialize pointers for folloing loop */
    if (mask_msg.req_type == AUDIT)
    data_ptr = strcpy2(mti.data_dir, FAB_DATA, "/") +mti.data_dir;
    else data_ptr = strcpy2(mti.data_dir, OFF_FAB_DATA, "/") +mti.data_dir;
    delta_ptr = strcpy2(mti.delta_dir, FAB_DELTA, "/") +mti.delta_dir;
    /* Set the data type to machine */
    mask_msg.data_type = MACHINE;
    /* Set master site in key3 */
    strcpy(mask_msg.key3, ip.master_site);
     for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* Build the data and delta directories */
	strcpy2(data_ptr, files[cnt], "/.fab");;
	strcpy2(delta_ptr, files[cnt], "/.fab");
	/* If this is a temporary file, delete it */
	if (strcmp(files[cnt], "outt") == 0)
	{
	    unlink(mti.delta_dir);
	    unlink(mti.data_dir);
	    continue;
	} /* End if for temporary file */
	/* If this is a file rather than a directory, delete it */
	/* Use stat command to read info about directory/file */
	if ((stat(mti.delta_dir, &buf) == 0)
	&& (S_ISREG(buf.st_mode) == 1))
	{
	    unlink(mti.delta_dir);
	    unlink(mti.data_dir);
	    continue;
	} /* End if for file instead of directory */
	/* Build the state file name  */
	strcpy2(mti.state_file, mti.delta_dir, "/state_file");
	/* Make sure state file exists before reading it */
    /* Also make sure data directory exists */
	if ((access(mti.state_file, 0) != 0) || (access(mti.data_dir, 0) != 0))
    continue;
	/* Read the state file */
	state_file_read();
	/* See if this node owns the data */
  if  (strcmp(st.master_site, ip.master_site) == 0)
  {
	/* Compare the update time to the audit update time */
	/* If it is newer, run the audit  */
	if (st.upd_time >= *machine_audit_time) 
	{
	    /* Copy the machine name into key1 */
	    /* Function bld_header expects the keys to be initialized */
	    strcpy(mask_msg.key1, files[cnt]);
	    /* Also copy name into key */
	    strcpy(key, files[cnt]);
	log2audit("Auditing machine tables for", mask_msg.key1);
	audit_data(refresh_cnt, 0, rmt_mpd.server_num);
	    ++audit_cnt;
	} /* End if for data needed to be audited */
	else if (st.touch_time >= *machine_audit_time) 
	{
	    /* Copy the machine name into key1 */
	    strcpy(mask_msg.key1, files[cnt]);
	    strcpy(key, files[cnt]);
	log2audit("Auditing machine tables for", mask_msg.key1);
	audit_state(refresh_cnt, 0, rmt_mpd.server_num);
	    ++audit_cnt;
	} /* End else for state needed to be audited */
  } /* End if for this node owns the data */
    } /* end for */
    /* Indicate how many machines were audited */
    rtn.len = sprintf(rtn.msg, "Performed %s %s audit.\nFound %d modified machine tables.\n", 
    audit_type, data_array[mask_msg.data_type], audit_cnt);
    if (audit_cnt > 0) audit_print("machine tables", refresh_cnt);
     /* Restore original socket value */
    sock = tmp_sock;
    /* Send success message to the client */
    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
    /* If made it to this point, audit with remote servers complete */
    /* Log message in audit log file */
    data_log(mti.audit_summary, rtn.msg);
/*
    *machine_audit_time = tr.upd_time;
*/
    exit(0);
} /* End machine_audit */
