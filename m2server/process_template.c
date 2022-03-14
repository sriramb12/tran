/****************************************************************************
 *
 * Function:	process_request
 *
 * Description:	Processes requests with data type of MASK *
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include "dir.h"
#include "close_down.h"
#include "entry.h"
#include "global_defines.h"
#include "list.h"
#include "mask_server_defines.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2mask_files.h"
#include "m2server_arrays.h"
#include "m2server_globals.h"
#include "m2state_table.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2server_paths.h"
#include "m2req_types.h"
#include "m2server_prototypes.h"
#include "mti.h"
#include "pre_suffix.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "m2msg.h"

void template_approve(void)
{
    int cnt;
    char *dirlist[500];
    char off_subdir[FILE_SIZE];
    char subdir[FILE_SIZE];
    char *off_subdir_ptr;
    char *subdir_ptr;
    int index;
    char tmp_dir[FILE_SIZE];
    char tmp_db_dir[FILE_SIZE];
    char *files[M2TOTAL_FILES];

    /* Verify  template is present and can be approved */
    index = state_file_read4update();
    /* Build name for official directory */
    strcpy5(mti.off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, SLASH, mask_msg.key2);
    strcpy2(tmp_dir, mti.off_data_dir, ".new");
    /* Build name for .db directory */
    strcpy2(mti.off_db_data_dir, mti.off_data_dir, "/.db");
    /* Build name for temporary db directory */
    strcpy2(tmp_db_dir, tmp_dir, "/.db");
    /* Rename current official directories */
    /* If the temporary directory is present do a recursive delete */
    if (access(tmp_dir, 0) == 0)
    dir_recursive_dlt(tmp_dir);
    if (access(mti.off_data_dir, 0) == 0)
    {
	if (rename(mti.off_data_dir, tmp_dir) != SUCCESS)
	close_down2f(rename_err, mti.off_data_dir, tmp_dir, RENAME_ERR);
    }
    /* Copy files to temporary directory */
    dir_copy(mti.off_data_dir, mti.data_dir, files);
    dir_copy(mti.off_db_data_dir, mti.db_data_dir, files);
    /* Copy in the subdirectories */
    /* First make a list of directories to copy */
    subdir_version_list(mti.delta_dir, dirlist);
    /* Build names of official and unofficial data directories */
    off_subdir_ptr = off_subdir + strcpy2(off_subdir, mti.off_data_dir, "/.dir/");
    subdir_ptr = subdir + strcpy2(subdir, mti.data_dir, "/.dir/");
    /* Make the .dir directory */
    dir_create(off_subdir);
    for (cnt = 0; dirlist[cnt] != NULL; ++cnt)
    {
	strcpy(off_subdir_ptr, dirlist[cnt]);
	strcpy(subdir_ptr, dirlist[cnt]);
	dir_copy(off_subdir, subdir, files);
    }
    /* Delete the original directory if present */
    if (access(tmp_dir, 0) == 0)
    dir_recursive_dlt(tmp_dir);
    /* Mark the template approved */
     st.version_type[0] = '+';
st.app_version = st.vnum;
    /* Document approved version in create file */
    /* This is used to display the version history */
    create_version_add(st.app_version);
    st.app_time = st.touch_time = st.upd_time = tr.upd_time;
    /* Write out the change and unlock the file */
    state_file_write(index);
    /* Tell client transaction successful */
    put_short(SUCCESS);
    mti.suffix_ptr = APP;
    put_ret_code(DBYES);
	/* Make the official directory the data directory for refresh transaction */
    strcpy(mti.data_dir, mti.off_data_dir);
    /* Set the data type to official template for remote transaction */
    mask_msg.data_type = OFF_TEMPLATE;
    /* Update the database */
/*     common_layers_update(); */
    /* Refresh data on remote servers */
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* End template_approve */

/* Delete the  template */
void template_delete(void)
{
    char local_key[KEY_LEN];
    int index;

    /* Make sure template can be deleted */
    index = state_file_read4update();
    /* Close the state file */
    state_file_close(index);
    /* Delete the delta directory */
    dir_recursive_dlt(mti.delta_dir);
    /* Delete the data directory */
    dir_recursive_dlt(mti.data_dir);
    /* Delete the official data */
    /* Build name for official data directory */
    strcpy5(mti.off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, SLASH, mask_msg.key2);
    /* Delete this directory */
    if (access(mti.off_data_dir, 0) == 0)
    dir_recursive_dlt(mti.off_data_dir);
    /* Delete reference to template in user groups */
    strcpy3(local_key, mask_msg.key1, " ", mask_msg.key2);
    mask_usergroup_delete(xx, st.usergroup, ".templates");
    /* Delete reference to template from global fab table */
    tech_entry_delete(mask_msg.key1, mask_msg.key2);
    /* Indicate transaction successful so far */
    put_short(SUCCESS);
    /* Tell user  template deleted */
    mti.suffix_ptr = DLT;
    put_ret_code(DBNO);
    /* Delete template from remote databases */
    mask_send_delete(0, rmt_mpd.server_num);
} /* End template_delete */

/* Delete template from remote databases */
void rmt_template_delete(void)
{
    int index;

    /* Verify requesting database owns data */
    index = state_file_read4owner();
    /* Delete the delta directory */
    dir_recursive_dlt(mti.delta_dir);
    /* Delete the data directory */
    dir_recursive_dlt(mti.data_dir);
    /* Delete the official data */
    /* Build name for official data directory */
    strcpy5(mti.off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, SLASH, mask_msg.key2);
    /* Delete this directory */
    if (access(mti.off_data_dir, 0) == 0)
    dir_recursive_dlt(mti.off_data_dir);
    /* Indicate transaction successful so far */
    put_short(SUCCESS);
    /* Tell user  template deleted */
     strcpy3(rtn.msg, mti.prefix, " deleted on ", ip.master_site);
     return_success(rtn.msg, DBNO);
} /* End rmt_template_delete */

/* Send user list of technologies for specified fab */
void template_list(void)
{
    int ret;
    int cnt;
    int new_cnt;
    char *files[M2TOTAL_FILES];
    char *new_files[M2TOTAL_FILES];
    char *ptr;

    ptr = mti.state_file + strcpy3(mti.state_file, FAB_DELTA, SLASH, mask_msg.key1);
    /* Verify the fab is present */
    if (access(mti.state_file, 0) != 0)
    {
	strcpy3(mti.prefix, "Fab '", mask_msg.key1, "'");
	close_down1f(non_err, mti.prefix, NON_ERR);
    }
    if ((ret = mask_files_list(mti.state_file, files)) <= 0)
    {
	strcpy3(mti.prefix, "Fab '", mask_msg.key1, "'");
	close_down1f(no_tech_err, mti.prefix, NO_TECH_ERR);
    }
    /* Add approve status to list */
    for (new_cnt = 0, cnt = 0; files[cnt] != NULL; ++cnt)
    {
    /* build name of state file to query */
    strcpy3(ptr, SLASH, files[cnt], "/state_file");
    /* If not found, assume unapproved and not archived */
	if (access(mti.state_file, 0) != 0)
    {
	strcpy(st.version_type, "-");
	st.state = NO_ACTION;
    } /* end if */
	/* Otherwise read in the approve status */
	else state_file_read();
    /* If not listing archived templates and this one not archived, skip it */
    if ((mask_msg.num != DBYES) && (st.state == ARCHIVED)) continue;
	/* Copy in new value */
	new_files[new_cnt] = malloc(strlen(files[cnt]) + 3);
	if (st.state == ARCHIVED)
	strcpy3(new_files[new_cnt], "  [", files[cnt], "]");
	else strcpy2(new_files[new_cnt], "  ", files[cnt]);
    /* xx Make template change here */
	if (st.version_type[0] != '+')
	new_files[new_cnt][0] = st.version_type[0];
	++new_cnt;
	/* Free file space no longer needed */
/*	free(files[cnt]); */
    } /* End for */
     new_files[new_cnt] = NULL;
    /* If no techs found after filtering, give error */
    if (new_cnt <= 0)
    {
	strcpy3(mti.prefix, "Fab '", mask_msg.key1, "'");
	close_down1f(no_tech_err, mti.prefix, NO_TECH_ERR);
    }
    template_files_sort(new_files, new_cnt);
    /* Indicate that list of technologies will be retrieved */
    put_short(SUCCESS);
    /* Send list to client */
    list_get(new_files);
    /* Send user return code */
    mti.suffix_ptr = RTV;
    strcpy(mti.prefix, "List of technologies ");
    put_ret_code(DBNO);
} /* End template_list */


/* Unapprove a template */
void template_unapprove(void)
{
    int index;

    /* Verify template can be unapproved */
    index = state_file_read4update();
    if (st.version_type[0] == '-')
    close_down1f(non_err, mti.prefix, NON_ERR);
    dir_recursive_dlt(mti.data_dir);
    /* Document unapproved version in create file */
    /* This is used to display the version history */
    create_version_add(st.app_version);
    /* Increment the transaction number */
    ++st.transaction_num;
    st.app_version= 0;
    st.touch_time = st.upd_time = tr.upd_time;
    /* Mark the file unapproved */
    st.version_type[0] = '-';
    state_file_write(index);
    mti.suffix_ptr = UNAPP;
    put_short(SUCCESS);
    put_ret_code(DBYES);
    /* Update the database */
/*     common_layers_update(); */
} /* End template_unapprove */

void flow_retrieve(void)
{

    /* Verify the template is present */
    state_file_read();
    /* Verify approved version of template is present */
    if (st.version_type[0] == '-')
    {
	strcpy6(mti.prefix, " Approved copy of fab '", mask_msg.key1, "', technology '", mask_msg.key2, "'", RET_TAB);
	close_down1f(non_err, mti.prefix, NON_ERR);
    }
   /* Tell client ready to send data */
    put_short(SUCCESS);
    strcpy6(mti.off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, SLASH, mask_msg.key2, "/.db");
    /* Now send the file */
    file_put(mti.off_data_dir, "flow");
    /* Send return code */
    mti.suffix_ptr = RTV;
    strcpy(mti.prefix, "file 'flow' ");
    put_ret_code(DBYES);
} /* end flow_retrieve */

/* Initialize paths needed to perform template transactions */
void template_paths_init(void)
{
    /* First strip off any brackets from the tech name */
    template_brackets_strip(mask_msg.key2);
	    strcpy3(key, mask_msg.key1, "_", mask_msg.key2);
    if (mask_msg.req_type == RETRIEVE) 
    {
	strcpy5(mti.data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, SLASH, mask_msg.key2);
	strcpy5(mti.prefix, "Approved copy of fab '", mask_msg.key1, 
	"', technology '", mask_msg.key2, "'");
	strcpy5(mti.suffix, "approved copy of fab '", mask_msg.key1, 
	"', technology '", mask_msg.key2, "'");
    }
    else if (mask_msg.req_type == UNAPPROVE)
    {
	strcpy5(mti.data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, SLASH, mask_msg.key2);
	strcpy5(mti.prefix, "Fab '", mask_msg.key1, 
	"', technology '", mask_msg.key2, "'");
	strcpy5(mti.suffix, "fab '", mask_msg.key1, 
	"', technology '", mask_msg.key2, "'");
    } else {
	strcpy5(mti.data_dir, FAB_DATA, SLASH, mask_msg.key1, SLASH, mask_msg.key2);
	strcpy5(mti.prefix, "Fab '", mask_msg.key1, 
	"', technology '", mask_msg.key2, "'");
	strcpy5(mti.suffix, "fab '", mask_msg.key1, 
	"', technology '", mask_msg.key2, "'");
    } /* end else */
    strcpy2(mti.db_data_dir, mti.data_dir, "/.db");
    strcpy2(mti.tmpdb_data_dir, mti.data_dir, "/.tmpdb");
    strcpy5(mti.delta_dir, FAB_DELTA, SLASH, mask_msg.key1, SLASH, mask_msg.key2);
    strcpy2(mti.db_delta_dir, mti.delta_dir, "/.db");
    /* Build the state file name */
    strcpy2(mti.state_file, mti.delta_dir, "/state_file");
} /* End template_paths_init */

/* Verify user authorized to build/approve templates */
void template_auth_check(void)
{
    if (entry_find(TEMPLATE_AUTH_FILE, user.email) != SUCCESS)
    {
    /* Send error to user */
	if (mask_msg.req_type == APPROVE)
	{
	    strcpy3(rtn.msg, approve_auth, DOUBLE_RET, help_msg);
	    shut_down(rtn.msg, APPROVE_AUTH);
	} else {
	    strcpy3(rtn.msg, build_auth, DOUBLE_RET, help_msg);
	    shut_down(rtn.msg, BUILD_AUTH);
	 } /* End else */
    } /* End if */
} /* End template_auth_check */

void create_version_add(float new_version)
{
    FILE *fp;
    char tmp_dir[FILE_SIZE];
    char create_file[FILE_SIZE];

    /* build temp file name */
    strcpy2(tmp_dir, mti.db_delta_dir, "/tmp_version");
    if ((fp = fopen(tmp_dir, "w")) == NULL)
    close_down1m(wr_open_err, tmp_dir, WR_OPEN_ERR);
    fprintf(fp, "Version %2.3f %s %s %s\n",
    new_version, user.login, req_array[mask_msg.req_type], log_date);
    fclose(fp);
    /* Append existing create file */
    strcpy2(create_file, mti.db_delta_dir, "/create");
    file_append(create_file, tmp_dir);
    /* Delete original create file no longer needed */
    unlink(create_file);
    file_rename(tmp_dir, create_file);
} /* End */

/* Strip brackets off of technology name */
void template_brackets_strip(char *tech)
{
    int len;

    if (tech[0] == '[')
    {
	len = strcpy1(tech, tech+1)-1;
	if (tech[len] == ']') tech[len] = '\0';
    } /* End if */
} /* end template_brackets_strip */

/* Archive the template */
void template_archive(void)
{
    int index;

    /* Read and lock the state file */
    index = state_file_read4update();
    /* Mark archived */
    st.state = ARCHIVED;
    /* Increment the transaction number */
    ++st.transaction_num;
     /* Modify the touch time */
    st.touch_time = tr.upd_time;
    /* Write out new state file */
    state_file_write(index);
    /* Modify the global mask set file */
    mask_entry_mod();
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Tell user template archived */
    mti.suffix_ptr = "archived";
    put_ret_code(DBYES);
     /* Send updated state file to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
} /* end template_archive */

/* Unarchive the template */
void template_unarchive(void)
{
    int index;

    /* Read and lock the state file */
    index = state_file_read4update();
    /* Munark archived */
    st.state = NO_ACTION;
    /* Increment the transaction number */
    ++st.transaction_num;
     /* Modify the touch time */
    st.touch_time = tr.upd_time;
    /* Write out new state file */
    state_file_write(index);
    /* Modify the global mask set file */
    mask_entry_mod();
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Tell user template archived */
    mti.suffix_ptr = "unarchived";
    put_ret_code(DBYES);
     /* Send updated state file to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
} /* end template_unarchive */

void template_files_sort(char *files[], int total_cnt)
{
    int cnt;
    int num;
    int smallest;
    char *new_files[M2TOTAL_FILES];
    char end[5] = "zzz";

    /* Find out how many files there are */
    for (num = 0; num < total_cnt; ++num)
    {
	smallest = 0;
	for (cnt = 0; cnt <total_cnt; ++cnt)
	    if (strcmp(files[cnt]+2, files[smallest]+2) < 0) smallest = cnt;
	/* At this point, smallest for this iteration found */
	new_files[num] = files[smallest];
	files[smallest] = end;
    } /* End outer for */
    /* Now copy list back */
    for (cnt = 0; cnt < total_cnt; ++cnt)
    files[cnt] = new_files[cnt];
    files[cnt] = NULL;
}  /* End tempalte_files_sort */

/* Update the common_template_layers table */
void template_common_layers_update(void)
{
    state_file_read();
    if (st.state == ARCHIVED)
    close_down1f(arc1err, mti.prefix, ARC1ERR);
    if (common_template_layers_update(st.version_type) == FAILURE)
    shut_down(rtn.msg, -1);
    else
    {
	put_short(SUCCESS);
	strcpy2(rtn.msg, "Common template layers updated for ", mti.suffix);
	return_success(rtn.msg, DBYES);
    }
} /* End template_common_layers_update */

void process_template(void)
{

    /* If not reading a template, */
    /* verify user authorized to build and approve templates */
    if ((mask_msg.req_type != READ) && (mask_msg.req_type != RETRIEVE)
    && (mask_msg.req_type != M2LIST) && (mask_msg.req_type != RETRIEVE1FILE)
    && (mask_msg.req_type != HISTORY) && (mask_msg.req_type != VERSIONS))
    template_auth_check();
    /* Initialize paths needed to perform template transactions */
    template_paths_init();
    switch(mask_msg.req_type)
{
	/* Create a new template from an existing template */
	case M2LIST: template_list(); break;
	/* List user groups and users assigned to the template */
	case LIST_MEMBERS: mask_list_members(); break;
	case APPROVE:
	{
	     template_approve(); 
	}
	break;
	case UNAPPROVE:
	{
	     template_unapprove(); 
	}
	break;
	case CREATE:
	{
	     mask_create(); 
	}
	break;
	/* Create a new  template from an existing template */
	case COPY:
	{
	     mask_copy(); 
	}
break;
	case RETRIEVE:
	{
	    mask_read();
	}
	break;
	case READ: 
	case READ_ANY: 
	{
	    if (strcmp(mask_msg.par1, NONE) == 0)
	    mask_read(); 
	    else mask_read_prior(); 
	}
	break;
	/* Check out template data */
	case CHECKOUT:
	{
	    if (strcmp(mask_msg.par1, NONE) == 0)
	    mask_checkout(); 
	    else mask_read_prior(); 
	}
	break;
	/* Retrieve check out status */
	case CHECKOUT_STATUS_RTV: mask_checkout_status_rtv(); break;
	case COMMON_LAYERS_UPDATE:  template_common_layers_update(); break;
	/* Cancel check-out on specified  template */
	case CANCHECKOUT:
	{
	    mask_cancheckout(); 
	    /* Send update to remote servers */
	    mask_refresh(0, rmt_mpd.server_num, MARK);
	}
	break;

	/* Check in  template files */
	case CHECKIN:
	{
	    mask_checkin(); 
	}
	break;
	/* Update  template in database */
	/* Major revision made, and files not deleted from user's disk */
	case UPDATE:
	{
	    mask_update(); 
	}
	break;
	/* Perform check in with overwrite function */
	case OVERWRITE:
	{
	    mask_ovrwr(); 
	}
	break;
	/* Save .db files to database with no versioning */
	case SAVE:
	{
	     mask_save(); 
	}
	break;
	/* Modify the owner of a  template */
	/* Must be the owner to perform this transaction */
	/* The user must supply X.500 user ID of new owner */
	case MOD_OWNER:
	{
	     mask_mod_owner(); 
	}
	break;
	case MOD_USERGROUP:
	{
	     mask_mod_usergroup(); 
	}
	break;
	case ADD_USERGROUP:  mask_add_usergroup(); break;
	case ADD_USER:  mask_add_user(); break;
	case GRANTOVERWRITE:
	{
	     mask_grant_overwrite(); 
	}
	break;
	/* See version history on specified template */
	case VERSIONS: mask_versions(); break;
	case HISTORY: mask_history(); break;
	/* Delete n existing  template */
	case DELETE:
	{
	    template_delete(); 
	}
	break;
	case M2ARCHIVE:
	{
	    template_archive(); 
	}
	break;
	case UNARCHIVE:
	{
	    template_unarchive(); 
	}
	break;
	/* Transfer ownership of  template to new database */
	case TRANSFER:
	{
	    mask_transfer(); 
    	 /* Send updated state file to remote servers */
	    mask_refresh(0, rmt_mpd.server_num, MARK);
	}
	break;
	/* Force shadow of template data to all database machines */
	case SHADOW: mask_shadow(mask_msg.num); break;
	case RETRIEVE1FILE: flow_retrieve(); break;
	case REFRESH: rmt_mask_refresh(); break;
	case MARK: rmt_mask_refresh(); break;
	case RMT_DELETE:  rmt_template_delete(); break;
	case AUDIT:
	{
	    long audit_time = audit_time_set();
	    template_audit(FAB_DATA, &audit_time);
	}
	break;
	case RMT_AUDIT: rmt_audit_data(); break;
	/* Audit state file remotely */
	case RMT_AUDIT_STATE: rmt_audit_state(); break;
	case VIEW_USERS:  mask_view_users(); break;
	case VIEW_USERGROUPS:  mask_view_usergroups(); break;
	case VIEW_CWD:  mask_view_cwd(); break;
	case TRANSACTION_LOG_RTV:  mask_transaction_log_rtv(); break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_template */
