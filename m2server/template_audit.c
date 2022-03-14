/****************************************************************************
 *
 * Function:	template_audit
 *
 * Description:	Verifies data integrity for templatess locally and remotely.
 *		Do a directory list to see templates present.
 *		Use check sums to verify shadowed correctly.
 *		If not shadowed correctly, refresh remote servers.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "dir.h"
#include "global_defines.h"
#include "global_params.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2mask_files.h"
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

void template_audit(char *fab_path, long *template_audit_time)
{
    int tmp_sock = sock;
    char audit_type[12];
    char *ptr;
    char *files[M2TOTAL_FILES];
    char *fab_files[500];
    char *tech_files[500];
    char *data_ptr;
    char *delta_ptr;
    char *db_data_ptr;
    char *db_delta_ptr;
    struct stat buf;
    int cnt;
    int cnt1;
    int cnt2 = 0;
    int audit_cnt = 0;
    int refresh_cnt[5] = { 0, 0, 0, 0, 0 };
    int len;
    char fab_dir[FILE_SIZE];

    /* Make a list of fabss */
    dir_list(fab_path, fab_files);
    /* Now make a list of technologies for each fab */
    for (cnt = 0; fab_files[cnt] != NULL; ++cnt)
    {
	strcpy3(fab_dir, fab_path, SLASH, fab_files[cnt]);
	if (strcmp(fab_files[cnt], "outt") == 0)
	{
	    unlink(fab_dir);
	    continue;
	} /* End if for temporary file */
	/* If this is a file rather than a directory, delete it */
	/* Use stat command to read info about directory/file */
	if ((stat(fab_dir, &buf) == 0)
	&& (S_ISREG(buf.st_mode) == 1))
	{
	    unlink(fab_dir);
	    continue;
	} /* End if for file instead of directory */
	mask_files_list(fab_dir, tech_files);
    /* Save audit type for later use */
    /* mask_msg.key1 get clobbered when data sent to remote server */
    strcpy(audit_type, mask_msg.key1);
	/* Now build compound name for big list */
	for (cnt1 = 0; tech_files[cnt1] != NULL; ++cnt1)
	{
	    len = strlen(fab_files[cnt]) + 1;
	    files[cnt2] = malloc(strlen(tech_files[cnt1]) + len);
	strcpy3(files[cnt2++], fab_files[cnt], SLASH, tech_files[cnt1]);
	}
    } /* End for for listing compound file names */
    /* Initialize pointers for folloing loop */
    data_ptr = strcpy2(mti.data_dir, fab_path, "/") +mti.data_dir;
    db_data_ptr = strcpy2(mti.db_data_dir, fab_path, "/") +mti.db_data_dir;
    delta_ptr = strcpy2(mti.delta_dir, FAB_DELTA, "/") +mti.delta_dir;
    db_delta_ptr = strcpy2(mti.db_delta_dir, FAB_DELTA, "/") +mti.db_delta_dir;
    /* Set the data type to template */
    /* Set master site in key3 */
    strcpy(mask_msg.key3, ip.master_site);
     for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* Build the data and delta directories */
	strcpy1(data_ptr, files[cnt]);
	strcpy2(db_data_ptr, files[cnt], "/.db");
	strcpy1(delta_ptr, files[cnt]);
	strcpy2(db_delta_ptr, files[cnt], "/.db");
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
	if ((access(mti.data_dir, 0) != 0) || (access(mti.delta_dir , 0) != 0)
	|| (access(mti.db_data_dir, 0) != 0) || (access(mti.db_delta_dir , 0) != 0)) continue;
	/* Build the state file name  */
	strcpy2(mti.state_file, mti.delta_dir, "/state_file");
	/* Make sure state file exists before reading it */
	if (access(mti.state_file, 0) != 0) continue;
	/* Read the state file */
	state_file_read();
	/* See if this node owns the data */
	/* Compare the update time to the audit update time */
	/* If it is newer, run the audit  */
  /* See if this server owns the data */
	if (strcmp(st.master_site, ip.master_site ) == 0)
  {
	if (((mask_msg.data_type == TEMPLATE) && (st.upd_time > *template_audit_time))
	|| ((mask_msg.data_type == OFF_TEMPLATE) && (st.app_time > *template_audit_time)))
	{
	    /* Set the keys needed by the audit */
	    template_keys_set(files[cnt]);
	log4audit("Auditing ", mask_msg.key1, mask_msg.key2);
	audit_data(refresh_cnt, 0, rmt_mpd.server_num);
	    ++audit_cnt;
	} /* End if for data needed to be audited */
	/* If only the touch time has changed, audit state file only */
	if ((mask_msg.data_type == TEMPLATE) && (st.touch_time > *template_audit_time))
	{
	    template_keys_set(files[cnt]);
	log4audit("Auditing ", mask_msg.key1, mask_msg.key2);
	audit_state(refresh_cnt, 0, rmt_mpd.server_num);
	    ++audit_cnt;
	} /* End else for touch time changed */
  } /* End if for this server owns the data */
    } /* end for */
    /* Indicate how many templates were audited */
    rtn.len = sprintf(rtn.msg, "Performed %s %s audit.\nFound %d modified templates.\n", 
    audit_type, data_array[mask_msg.data_type], audit_cnt);
    /* If data found to audit, print results */
    if (audit_cnt > 0) audit_print("templates", refresh_cnt);
    /* Restore original socket value */
    sock = tmp_sock;
    /* If made it to this point, audit with remote servers complete */
    /* Log message in weekly audit file */
    data_log(mti.audit_summary, rtn.msg);
    /* Send success message to the client */
    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
/*
    *template_audit_time = tr.upd_time;
*/
    exit(0);
} /* End template_audit */
/* Set tempalte keys needed by the audit */
void template_keys_set(char *file_name)
{
    char *ptr;

    /* Copy the template name into key1 */
    /* Function bld_header expects the keys to be initialized */
    strcpy(mask_msg.key1, file_name);
    /* Now put technology in key2 */
    ptr = strchr(mask_msg.key1, '/');
    if (ptr != NULL)
    {
	strcpy(mask_msg.key2, ptr+1);
	ptr[0] = '\0';
    } /* End if for slash found */
    /* Also copy name into key */
    strcpy(key, mask_msg.key1);
} /* End template_keys_set */
