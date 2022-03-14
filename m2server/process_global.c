/****************************************************************************
 *
 * Function:	process_global_
 *
 * Description:	Processes requests to access single global tables
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <unistd.h>
#include "dir.h"
#include "close_down.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
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
#include "mti.h"
#include "server_lib.h"
#include "user_params.h"
#include "rtn.h"
#include "m2msg.h"
#include "m2server_prototypes.h"
#include "strcpy.h"
#include "pre_suffix.h"

void global_read(void)
{

    /* Verify global table is present */
    state_file_read();
    /* Send the data to the user */
    global_files_rtv(mti.data_dir);
    /* Tell client file retrieved */
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* End global_read */

void global_retrieve(void)
{

    /* Verify global table is present */
    state_file_read();
    if (st.version_type[0] == '-')
    {
	strcpy2(mti.prefix, "Official copy of ", mti.suffix);
	close_down1f(non_err, mti.prefix, NON_ERR);
    }
    /* Send the data to the user */
    global_files_rtv(mti.off_data_dir);
    /* Tell client file retrieved */
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* End global_retrieve */

/* Retrieve official copy of global table */
void global_retrieve_all(void)
{
    char *files[M2TOTAL_FILES];

    /* Make list of files in global directory */
    dir_list(mti.off_data_dir, files);
    /* Tell client ready to send files */
    put_short(SUCCESS);
    /* Send the files */
    files_send(mti.off_data_dir, files);
    /* Tell client files retrieved */
    strcpy(mti.prefix, "Global files");
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* End global_retrieve_all */

void global_create(void)
{

    /* Create the state file */
    state_file_crt(NO_ACTION, NONE);
    /* Tell client ready to get file */
    put_short(SUCCESS);
    file_get(mti.data_dir, key);
    if (strcmp(key, FAB_FILE) == 0)
    file_get(mti.data_dir, mti.tmp_file_prefix);
    /* Create the version file */
    strcpy3(mti.off_delta_dir, mti.delta_dir, SLASH, key);
    data_log(mti.off_delta_dir, "Version 1.0");
    if (strcmp(key, FAB_FILE) == 0)
    {
	strcpy3(mti.off_delta_dir, mti.delta_dir, SLASH, CONFIG_FILE);
	data_log(mti.off_delta_dir, "Version 1.0");
    } /* End for 'fab' file */
    /* Send return code to client */
    mti.suffix_ptr = CRT;
    put_ret_code(DBYES);
    /* Send update to remote servers */
    global_refresh(REFRESH, 0, rmt_mpd.server_num);
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* End global_create */

void global_checkout(void)
{
    int index;

    /* Verify data can be checked out */
    index = state_file_read4update();
    /* Send the data to the user */
    global_files_rtv(mti.data_dir);
    /* Mark the data checked out */
     st.state = CHECKED_OUT;
    st.touch_time = tr.upd_time;
    ++st.transaction_num;
    strcpy(st.user_id, user.email);
    strcpy(st.cwd, user.cwd);
    state_file_write(index);
    /* Tell user data checked out */
    mti.suffix_ptr = CHKOUT;
    put_ret_code(DBYES);
    /* Send update to remote servers */
    global_refresh(MARK, 0, rmt_mpd.server_num);
} /* End global_checkout */

void global_checkin(void)
{
    int index;
    char *files[4];

    /* Verify table can be checked in */
    index = state_file_read4update();
    /* Tell client ready to perform transaction */
     put_short(SUCCESS);
    /* Get the file */
    global_files_get(files);
    /* Increment the version number */
    st.vnum += .1;
    /* Version the file */
    mask_files_version(mti.tmp_data_dir, mti.data_dir,  
    mti.tmp_delta_dir, mti.delta_dir, files);
    /* Mark the table checked in */
    st.state = NO_ACTION;
    ++st.transaction_num;
    st.touch_time = st.upd_time = tr.upd_time;
    /* Update the approval status */
    if (st.version_type[0] != '-')
    st.version_type[0] = '*';
    state_file_write(index);
    /* Send return code to client */
    mti.suffix_ptr = CHKIN;
    put_ret_code(DBYES);
    /* Send update to remote servers */
    global_refresh(REFRESH, 0, rmt_mpd.server_num);
} /* End global_checkin */

void global_update(void)
{
    int index;
    char *files[4];

    /* Verify table can be updated */
    index = state_file_read4update();
    /* Tell client ready to perform transaction */
     put_short(SUCCESS);
    /* Get the file */
    global_files_get(files);
    /* Increment the version number */
    st.vnum += .1;
    /* Version the file */
    mask_files_version(mti.tmp_data_dir, mti.data_dir,  
    mti.tmp_delta_dir, mti.delta_dir, files);
    /* Update the state file */
    ++st.transaction_num;
    st.touch_time = st.upd_time = tr.upd_time;
    /* Update the approval status */
    if (st.version_type[0] != '-')
    st.version_type[0] = '*';
    /* Write changes to state file and unlock the file */
    state_file_write(index);
    /* Send return code to client */
    mti.suffix_ptr = UPD;
    put_ret_code(DBYES);
    /* Send update to remote servers */
    global_refresh(REFRESH, 0, rmt_mpd.server_num);
} /* End global_update */


/* Delete one global table */
void global_delete(void)
{
    int index;
    char file_name[64];

    /* Verify table can be deleted */
    index = state_file_read4update();
    state_file_close(index);
    /* Delete the state_file */
    unlink(mti.state_file);
    /* Delete the data file */
    strcpy3(file_name, mti.data_dir, SLASH, key);
    unlink(file_name);
    /* Delete the version file */
	strcpy3(file_name, mti.delta_dir, SLASH, key);
	unlink(file_name);
    /* Delete the official data table */
    strcpy3(file_name, mti.off_data_dir, SLASH, key);
     unlink(file_name);
	/* If this is the 'fab' file, delete the configuration file also */
    if (strcmp(key, FAB_FILE) == 0)
    {
	/* Delete the data file */
	strcpy3(file_name, mti.data_dir, SLASH, CONFIG_FILE);
	unlink(file_name);
	/* Delete the version file */
	strcpy3(file_name, mti.delta_dir, SLASH, CONFIG_FILE);
	unlink(file_name);
	/* Delete the official data table */
	strcpy3(file_name, mti.off_data_dir, SLASH, CONFIG_FILE);
	 unlink(file_name);
    } /* End if */
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Send return message */
    mti.suffix_ptr = DLT;
    put_ret_code(DBYES);
    /* Send update to remote servers */
    mask_send_delete(0, rmt_mpd.server_num);
} /* End global_delete */

/* Delete global table on remote servers */
void rmt_global_delete(void)
{
    int index;
    char file_name[FILE_SIZE];

    /* Verify requesting server owns the data */
    index = state_file_read4owner();
    /* Delete the state_file */
    unlink(mti.state_file);
    /* Delete the data file */
    strcpy3(file_name, mti.data_dir, SLASH, key);
    unlink(file_name);
    /* Delete the version file */
    strcpy3(file_name, mti.delta_dir, SLASH, key);
    unlink(file_name);
    /* Delete the official data table */
    strcpy3(file_name, mti.off_data_dir, SLASH, key);
     unlink(file_name);
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Send return message */
     strcpy3(rtn.msg, mti.prefix, " deleted on ", ip.master_site);
     return_success(rtn.msg, DBNO);
} /* End rmt_global_delete */

void global_transfer(){ } 

	/* Force shadow of global data to all database machines */
void global_shadow(void)
{

     /* Verify the global data is present */
    state_file_read();
    /* Verify this site owns the data */
    if (strcmp(st.master_site, ip.master_site) != 0)
    m2server_send(st.master_site);
    /* Tell user shadow successful so far */
    put_short(SUCCESS);
    mti.suffix_ptr = SHADOWED;
    put_ret_code(DBYES);
    /* Shadow data to remote servers */
    global_refresh(REFRESH, 0, rmt_mpd.server_num);
} /* end global_shadow */


/* Approve a global table */
void global_approve(void)
{
    int cnt;
    int index;
    char *files[4];
    char old_file[64];
    char new_file[64];

    /* change the request type to check in */
    /* This is done so check to check in/approve is successful */
    mask_msg.req_type = CHECKIN;
    /* Verify table can be checked in */
    index = state_file_read4update();
    /* Tell client ready to perform transaction */
     put_short(SUCCESS);
    /* Get the file */
    global_files_get(files);
    /* Copy unofficial table to official table */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	strcpy3(new_file, mti.off_data_dir, SLASH, files[cnt]);
	strcpy3(old_file, mti.tmp_data_dir, SLASH, files[cnt]);
	file_copy(old_file, new_file);
    }
    /* Increment the version number */
    st.vnum += .1;
    /* Version the file */
    mask_files_version(mti.tmp_data_dir, mti.data_dir,  
    mti.tmp_delta_dir, mti.delta_dir, files);
    /* Indicate table has been approved */
    approve_version_add(key, st.vnum);
    /* Mark the table checked in */
    st.state = NO_ACTION;
    ++st.transaction_num;
    st.app_time = st.touch_time = st.upd_time = tr.upd_time;
    /* Update the approval status */
    st.version_type[0] = '+';
    state_file_write(index);
    mti.suffix_ptr = APP;
    put_ret_code(DBYES);
    /* Send updated global table to remote servers */
    global_refresh(REFRESH, 0, rmt_mpd.server_num);
    strcpy(mti.data_dir, mti.off_data_dir);
    global_refresh(REFRESH, 0, rmt_mpd.server_num);
    exit(0);
} /* end global_approve */

/* Unapprove a global table */
void global_unapprove(void)
{
    int index;

    /* Verify table can be unapproved */
    index = state_file_read4update();
    if (st.version_type[0] == '-')
    shut_down("DATa not approved.", -1);
    /* Delete official copy of file */
    strcpy3(mti.tmp_data_dir, mti.off_data_dir, SLASH, key);
    if (unlink(mti.tmp_data_dir) != 0)
    {
	sprintf(rtn.msg, dlt_err, errno, mti.tmp_data_dir);
	shut_down(rtn.msg, DLT_ERR);
    }
    if (strcmp(key, FAB_FILE) == 0)
    {
	strcpy3(mti.tmp_data_dir, mti.off_data_dir, SLASH, CONFIG_FILE);
	unlink(mti.tmp_data_dir);
    } /* End if */
    /* Increment the transaction number */
    ++st.transaction_num;
    st.touch_time = st.upd_time = tr.upd_time;
    /* Mark the file unapproved */
    st.version_type[0] = '-';
    state_file_write(index);
    mti.suffix_ptr = UNAPP;
    put_short(SUCCESS);
    put_ret_code(DBYES);
} /* end global_unapprove */

void global_refresh(int req_type, int start_index, int end_index)
{

    /* Set mti.data_dir and mti.delta_dir to point to global files */
    /* These files are sent using tar */
    strcpy2(mti.data_dir+strlen(mti.data_dir), "/", mask_msg.key1);
    strcpy3(mti.delta_dir+strlen(mti.delta_dir), "/", mask_msg.key1, "*");
    /* Call mask_refresh to build header and send request to each remote server */
    mask_refresh(start_index, end_index, req_type);
} /* End global_refresh */


void process_global(void)
{

    /* Initialize paths needed to perform global transaction */
     global_paths_init();
    switch(mask_msg.req_type)
{
    /* Create a new global file by porting it from the user's disk */
	case CREATE:
	{
	    global_create(); 
	}
	break;
    /* Retrieve unofficial copy of a global table */
	case READ: global_read(); break;
    /* Retrieve official copy of global table */
	/* Retrieve official copy of all files in global table directory */
    case RETRIEVE:  global_retrieve(); break;
    case READ_ANY:  global_retrieve_all(); break;
	/* Check out global table */
	case CHECKOUT:
	{
	   global_checkout(); 
	}
	break;
	/* Cancel check-out on specified global table */
	case CANCHECKOUT:
	{
	   mask_cancheckout(); 
	    /* Send update to remote servers */
	    global_refresh(MARK, 0, rmt_mpd.server_num);
	}
	break;
	/* Check in global file */
	case CHECKIN:
	{
	   global_checkin(); 
	}
	break;
	case UPDATE:
	{
	   global_update(); 
	}
	break;
	/* Approve global data */
	case APPROVE:
	{
	   global_approve(); 
	}
	break;
	case UNAPPROVE:
	{
	   global_unapprove(); 
	}
	break;
	/* Delete n existing global table */
	case DELETE:
	{
	   global_delete(); 
	}
	break;

	/* Transfer ownership of global file to new database */
	case TRANSFER:
	{
	    mask_transfer(); 
    	 /* Send updated state file to remote servers */
	    global_refresh(MARK, 0, rmt_mpd.server_num);
	}
	break;
	/* Force shadow of global data to all database machines */
	case SHADOW: mask_shadow(global_refresh); break;
	case REFRESH: rmt_refresh_global(); break;
	case MARK: rmt_global_refresh(); break;
	case RMT_DELETE: rmt_global_delete(); break;
	case AUDIT:  
	{
	    long audit_time = audit_time_set();
	    global_audit(&audit_time);
	}
	break;
	case RMT_AUDIT:  rmt_audit_global(); break;
	case RMT_AUDIT_STATE:  rmt_audit_state(); break;
	case VIEW_USERS:  mask_view_users(); break;
	case VIEW_USERGROUPS:  mask_view_usergroups(); break;
	case VIEW_CWD:  mask_view_cwd(); break;
	case ADD_USERGROUP:  mask_add_usergroup(); break;
	case ADD_USER:  mask_add_user(); break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* End process_global */

/* Initialize paths needed to perform transaction */
void global_paths_init(void)
{
	    strcpy(key, mask_msg.key1);
    strcpy1(mti.data_dir, GLOBAL_DATA);
    strcpy1(mti.delta_dir, GLOBAL_DELTA);
    strcpy1(mti.tmp_data_dir, TMP_GLOBAL_DATA);
    strcpy1(mti.tmp_delta_dir, TMP_GLOBAL_DELTA);
    strcpy1(mti.off_data_dir, OFF_GLOBAL_DATA);
    strcpy1(mti.off_delta_dir, OFF_GLOBAL_DELTA);
    strcpy3(mti.prefix, "Global table '", mask_msg.key1, "'");
    strcpy3(mti.suffix, "global table '", mask_msg.key1, "'");
    /* Build the state file name */
    strcpy4(mti.state_file, GLOBAL_DELTA, SLASH, mask_msg.key1, ".state");
} /* End global_paths_init */
void rmt_global_refresh(void)
{
    int ret;

     /* Verify the global data is present */
    if ((ret = state_file_read4owner()) == NON_ERR) rmt_refresh_global();
    /* If not owner, return, message to rmt server already sent */
    else if ((mask_msg.req_type == MARK)  && (mask_msg.transaction_time == (st.transaction_num+1)))
    rmt_mark_data();
    else rmt_refresh_global();
} /* End rmt_global_refresh */

void approve_version_add(char *file_name, float new_version)
{
    FILE *fp;
    char tmp_dir[FILE_SIZE];
    char version_file[FILE_SIZE];

    /* build temp file name */
    strcpy2(tmp_dir, mti.delta_dir, "/tmp_version");
    if ((fp = fopen(tmp_dir, "w")) == NULL)
    close_down1m(wr_open_err, tmp_dir, WR_OPEN_ERR);
    fprintf(fp, "Version %2.3f %s %s %s\n",
    new_version, user.login, req_array[APPROVE], log_date);
    fclose(fp);
    /* Append existing  file */
    strcpy3(version_file, mti.delta_dir, "/", file_name);
    file_append(version_file, tmp_dir);
    /* Delete original file no longer needed */
    unlink(version_file);
    file_rename(tmp_dir, version_file);
} /* End */

/* Retrieve the global file */
/* Two files retrieved if this is fab data */
/* 3 files retrieved if associated log file present */
/* Returns the list of files retrieved */
void global_files_get(char *files[])
{
    int cnt = 0;

    /* Get the global file */
    file_get(mti.tmp_data_dir, key);
    files[cnt++] = key;
    if (strcmp(key, FAB_FILE) == 0)
    {
	file_get(mti.tmp_data_dir, mti.tmp_file_prefix);
	files[cnt] = malloc(strlen(mti.tmp_file_prefix) + 1);
	strcpy(files[cnt], mti.tmp_file_prefix);
    ++cnt;
	files[2] = NULL;
    } /* End if */
    else files[1] = NULL;
    if ((strcmp("design_group_table", key) == 0)
    || (strcmp("fab", key) == 0)
    || (strcmp("machine", key) == 0)
    || (strcmp("machine_table", key) == 0)
    || (strcmp("maskshop_table", key) == 0)
    || (strcmp("supply_management", key) == 0)
    || (strcmp("technology", key) == 0))
    {
	/* Get the log file */
	file_get(mti.tmp_data_dir, mti.tmp_file_prefix);
	files[cnt] = mti.tmp_file_prefix;
    ++cnt;
	files[cnt] = NULL;
    } /* end if */
} /* End global_files_get */

void global_files_rtv(char *data_dir)
{
    /* Tell client ready to send file */
    put_short(SUCCESS);
    /* Send the file */
    file_put(data_dir, mask_msg.key1);
    if (strcmp(key, FAB_FILE) == 0)
    file_put(data_dir, CONFIG_FILE);
    if ((strcmp("design_group_table", key) == 0)
    || (strcmp("fab", key) == 0)
    || (strcmp("machine", key) == 0)
    || (strcmp("machine_table", key) == 0)
    || (strcmp("maskshop_table", key) == 0)
    || (strcmp("supply_management", key) == 0)
    || (strcmp("technology", key) == 0))
    {
	strcat(key, ".log");
	file_put(data_dir, key);
    } /* End if */
} /* end global_files_rtv */
