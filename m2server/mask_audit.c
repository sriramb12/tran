/****************************************************************************
 *
 * Function:	mask_audit
 *
 * Description:	Verifies data integrity for mask sets locally and remotely.
 *		Read into memory list of mask sets in global table.
 *		Do a directory list to see masks present.
 *		Verify they are defined in the global table.
 *		If they are not, add them.
 *		Use check sums to verify shadowed correctly.
 *		If not shadowed correctly, refresh remote servers.
 *		Verify there are no extra entries in the global mask table.
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
#include "create.h"
#include "dir.h"
#include "files.h"
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
#include "mask_entry.h"
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

void mask_audit(long *mask_audit_time)
{
     int tmp_sock = sock;
    char audit_type[12];
    char create_file[FILE_SIZE];
    char *files[M2TOTAL_FILES];
    char *data_ptr = mti.data_dir + strcpy2(mti.data_dir, MASK_DATA, "/");
    char *delta_ptr = mti.delta_dir + strcpy2(mti.delta_dir, MASK_DELTA, "/");
    struct stat buf;
    int cnt;
    int audit_cnt = 0;
    int refresh_cnt[5] = { 0, 0, 0, 0, 0};

    /* Build the audit summary file name */
    /* Make a list of mask sets */
    dir_list(MASK_DELTA, files);
    /* Set the data type to mask */
    mask_msg.data_type = MASK_SET;
    /* Set master site in key3 */
    strcpy(mask_msg.key3, ip.master_site);
    /* Preserve audit type to print later */
    strcpy(audit_type, mask_msg.key1);
    /* Read through each mask set */
     for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* Build the data and delta directories */
	strcpy1(data_ptr, files[cnt]);
	strcpy1(delta_ptr, files[cnt]);
	/* All mask sets are currently 4 characters in length */
	/* Delete anything not 4 characters in length */
	if (strlen(files[cnt]) != 4)
	{
	    /* If this is the tmp_files directory, leave it alone */
	    if (strcmp(files[cnt], "tmp_files") == 0) continue;
	    /* Delete all other directories and files */
	    else
	    {
		if (unlink(mti.delta_dir) != SUCCESS)
		dir_delete(mti.delta_dir);
	    if (access(mti.data_dir, 0) == 0)
	    {
		if (unlink(mti.data_dir) != SUCCESS)
		dir_delete(mti.data_dir);
	    }
		continue;
	    } /* End else */
	} /* End if */
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
	/* If all directories for mask set not present, do not audit it */
	/* Also if create file not present, do not perform audit */
	strcpy2(create_file, mti.data_dir, "/.db/create");
	if ((access(mti.data_dir, 0) != 0) || (access(mti.delta_dir , 0) != 0)
	|| (access(create_file, 0) != 0)) continue;
	/* Build the state file name  */
	strcpy2(mti.state_file, mti.delta_dir, "/state_file");
	/* Make sure state file exists before reading it */
	if (access(mti.state_file, 0) != 0) continue;
	/* Read the state file */
	state_file_read();
	/* See if this node owns the data */
  if (strcmp(st.master_site, ip.master_site) == 0)
  {
	/* Compare the update time to the audit update time */
	/* If it is newer, run the audit  */
	if ((st.upd_time >= *mask_audit_time) 
	/* Do not audit data if mask is archived */
	&& (st.state != ARCHIVED))
	{
	    /* Copy the mask name into key1 */
	    /* Function bld_header expects the keys to be initialized */
	    strcpy(mask_msg.key1, files[cnt]);
    log3("auditing ", mask_msg.key1, ".");
	    /* Also copy name into key */
	    strcpy(key, files[cnt]);
	log2audit("Auditing", mask_msg.key1);
	    audit_data(refresh_cnt, 0, rmt_mpd.server_num);
	    ++audit_cnt;
	} /* End if for data needed to be audited */
	else if (st.touch_time >= *mask_audit_time)
	{
	    /* Copy the mask name into key1 */
	    strcpy(mask_msg.key1, files[cnt]);
	    strcpy(key, files[cnt]);
	log2audit("Auditing", mask_msg.key1);
	    audit_state(refresh_cnt, 0, rmt_mpd.server_num);
	    ++audit_cnt;
	} /* End else for state file needed to be audited */
  } /* End if for server owns this data */
    } /* end for */
    /* Indicate how many masks were audited */
    rtn.len = sprintf(rtn.msg, "Performed %s %s audit.\nFound %d modified mask sets.\n", 
    audit_type, data_array[mask_msg.data_type], audit_cnt);
    if (audit_cnt > 0) audit_print("mask sets", refresh_cnt);
    /* Send success message to the client */
    sock = tmp_sock;
    put_short(SUCCESS);
    /* Audit status in the audit report file */
    data_log(mti.audit_summary, rtn.msg);

    return_success(rtn.msg, DBNO);
    /* If made it to this point, audit with remote servers complete */
/*
    *mask_audit_time = tr.upd_time;
*/
    exit(0);
} /* End mask_audit */

/* Refresh mask table using data from state and create files */
void mask_table_audit(void)
{
    char *files[M2TOTAL_FILES];
    extern long create_time;
    FILE *fo;
    int cnt;
    char *delta_ptr = strcpy2(mti.state_file, MASK_DELTA, "/") +mti.state_file;
    char *create_ptr = strcpy2(mti.data_dir, MASK_DATA, "/") + mti.data_dir;

    /* build name for temporary file */
    strcpy6(bf.tmp_file, user.login, "_", user.mach, "_", tr.upd_time_string, ".tmp_file");
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	strcpy3(rtn.msg, wr_open_err, bf.tmp_file, "'");
	data_log(tr.log_file, rtn.msg);
	return;
    }
    /* Make a list of mask sets */
    dir_list(MASK_DELTA, files);
    /* Sort the files */
    files_sort(files);
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
    /* If name not 4 characters in length, skip it */
    if (strlen(files[cnt]) != 4) continue;
	strcpy2(delta_ptr, files[cnt], "/state_file");
	if (access(mti.state_file, 0) != 0) continue;
	/* Read state file and get latest information */
	state_file_read();
	/* also read the create file */
	strcpy2(create_ptr, files[cnt], "/.db/create");
	/* If create file not present, skip this mask */
	if (access(mti.data_dir, 0) != 0) continue;
	create_read(mti.data_dir);
	/* build string using info from create and state file */
    fprintf(fo, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%lu\n",
files[cnt], mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4,
    mask_msg.par5, st.own_id, st.master_site, "No", create_time);
    } /* End for */
    /* Free file list no longer needed */
    files_free(files);
    /* Close the temporary file */
    fclose(fo);
    file_rename(bf.tmp_file, GLOBAL_MASK_FILE);
    put_short(SUCCESS);
    return_success("Mask audit complete", DBNO);
} /* End mask_table_audit */
