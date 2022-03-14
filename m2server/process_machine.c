/****************************************************************************
 *
 * Function:	process_machine
 *
 * Description:	Processes requests to access machine tables
 *		Machine tables are stored in the data base under 
 *		directory .fab under each fab.  Machine table
 *		names are hard coded on both the server and
 *		client side.
 *		Machine tables are copied from fab to another 
 *		when a new fab is created.
 *		A file list is not made when files sent
 *		to the user.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "dir.h"
#include "close_down.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "machine_files.h"
#include "mask_server_defines.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2mask_files.h"
#include "m2server_globals.h"
#include "m2state_table.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2server_paths.h"
#include "m2req_types.h"
#include "mti.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "m2msg.h"
#include "m2defines.h"
#include "m2server_prototypes.h"
#include "strcpy.h"
#include "pre_suffix.h"


/* Retrieve official copy of machine tables */
void machine_retrieve(void)
{
     char *files[MACHINE_NUM];

    /* Build name of approved data directory */
    strcpy4(mti.off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, FAB_DIR);
    /* Verify machine table can be read */
    state_file_read();
    /* Verify approved version of data is present */
/*
    if (st.version_type[0] == '-') 
    {
	strcpy3(mti.prefix, "Approved tables for fab '",
	mask_msg.key1, "'");
	close_down1f(machine_non_err, mti.prefix, NON_ERR);
    }
*/
    /* build list of files to retrieve */
    machine_init(files);
    /* Tell client ready to send files */
    put_short(SUCCESS);
    /* Send the files */
    files_send(mti.off_data_dir, files);
    /* Tell client files retrieved */
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
    log3("Closing ", "transaction ", tr.pid_string);
} /* End machine_retrieve */

/* Retrieve unofficial copy of machine tables */
void machine_read(void)
{
     char *files[MACHINE_NUM];

    /* Verify machine table can be read */
    state_file_read();
    /* build list of files to retrieve */
    machine_init(files);
    /* Tell client ready to send files */
    put_short(SUCCESS);
    /* Send the files */
    files_send(mti.data_dir, files);
    /* Tell client files retrieved */
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
    log3("Closing ", "transaction ", tr.pid_string);
} /* End machine_read */

/* Read the machine table log */
void machine_log_read(void)
{
     char *files[2];
    char file_name[64];

    /* Verify machine table can be read */
    state_file_read();
    /* Decide if reading official or unofficial copy */
    if (mask_msg.override_flag == DBYES)
    {
	/* This is to read the official copy */
    /* Build path for file */
    strcpy2(file_name, "off_", mti.data_dir);
	/* Need to change this path to point to official directory */
	strcpy(mti.data_dir, file_name);
	strcat(file_name, "/machine_table_log");
    } /* End if */
    else strcpy2(file_name, mti.data_dir, "/machine_table_log");
    if (access(file_name, 0) != 0)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* build list of files to retrieve */
    files[0] = malloc(18);
    strcpy(files[0], "machine_table_log");
    files[1] = NULL;
    /* Tell client ready to send files */
    put_short(SUCCESS);
    /* Send the files */
    files_send(mti.data_dir, files);
    /* Tell client files retrieved */
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
    log3("Closing ", "transaction ", tr.pid_string);
} /* End machine_log_read */

/* Check out specified machine tables */
void machine_checkout(void)
{
    int index;
     char *files[MACHINE_NUM];

    /* Verify machine files are present */
    index = state_file_read4update();
    /* build list of files to retrieve */
    machine_init(files);
    /* Tell client ready to send files */
    put_short(SUCCESS);
    /* Send the files */
    files_send(mti.data_dir, files);
    /* Mark the file checked out */
    st.state = CHECKED_OUT;
    /* Increment the transaction number */
    ++st.transaction_num;
    /* Update the touch and update times */
    st.touch_time = tr.upd_time;
    /* Record user who has data checked out */
    strcpy(st.user_id, user.email);
    strcpy(st.cwd, user.cwd);
    /* Write out changes */
    state_file_write(index);
    mti.suffix_ptr = CHKOUT;
    put_ret_code(DBYES);
    /* Send mark transaction to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
    log3("Closing ", "transaction ", tr.pid_string);
} /* End machine_checkout */

/* Approve machine files */
void machine_approve(void)
{
    int index;
    char *files[MACHINE_NUM];

    /* Change request type to check in for checking state file */
    mask_msg.req_type = CHECKIN;
    /* Retrieve copy of updated machine files */
    index = machine_put();
    /* Build list of unofficial files */
    machine_init(files);
    /* Copy unofficial files to official table */
    strcpy4(mti.off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, FAB_DIR);
    dir_copy(mti.off_data_dir, mti.data_dir, files);
    /* Cancel check out and update the state file */
    st.state = NO_ACTION;
    st.app_time = st.touch_time = st.upd_time = tr.upd_time;
    ++st.transaction_num;
    /* Mark data approved */
    st.version_type[0] = '+';
    /* Write out changes and unlock state file */
    state_file_write(index);
    mti.suffix_ptr = APP;
    put_ret_code(DBYES);
    /* Send updates to remote server */
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* end machine_approve */

void machine_init(char *files[])
{
    char file_name[FILE_SIZE];
     char *file_ptr = strcpy2(file_name, mti.data_dir, "/") + file_name;
    int cnt;
    int cnt1;

     for (cnt = 0, cnt1 = 0; machine_files[cnt][0] != '\0'; ++cnt)
    {
	strcpy(file_ptr, machine_files[cnt]);
	if (access(file_name, 0) == 0)
	{
	if ((files[cnt1] = malloc(strlen(machine_files[cnt]) +1)) == NULL)
	    shut_down(malloc_err, MALLOC_ERR);
	    strcpy(files[cnt1], machine_files[cnt]);
	    ++cnt1;
	} /* End if */
    } /* End for */
    files[cnt1] = NULL;
} /* End machine_init */

/* Delete specified machine tables */
void machine_dlt(void)
{
    int index;

    /* Make sure machine tables can be deleted */
    index = state_file_read4update();
    state_file_close(index);
    /* Delete the delta directory */
    dir_delete(mti.delta_dir);
    /* Delete the data directory */
    dir_delete(mti.data_dir);
    /* Indicate transaction successful so far */
    put_short(SUCCESS);
    /* Tell user machine tables deleted */
    mti.suffix_ptr = DLT;
    put_ret_code(DBNO);
} /* End machine_dlt */

/* checkin specified machine tables */
void machine_checkin(void)
{
    int index;

    /* Retrieve copy of updated machine files */
    index =machine_put();
    /* Cancel check out and update the state file */
    st.state = NO_ACTION;
    st.touch_time = st.upd_time = tr.upd_time;
    ++st.transaction_num;
    /* Update the approval status */
    if (st.version_type[0] != '-') st.version_type[0] = '*';
    state_file_write(index);
    /* Tell user check in successful */
    mti.suffix_ptr = CHKIN;
    put_ret_code(DBYES);
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
    log3("Closing ", "transaction ", tr.pid_string);
} /* End machine_checkin */

/* Update specified machine tables */
void machine_update(void)
{
    int index;

    /* Retrieve copy of updated machine files */
    index = machine_put();
    /* Update the state file */
    st.touch_time = st.upd_time = tr.upd_time;
    ++st.transaction_num;
    state_file_write(index);
    /* Tell user update successful */
    mti.suffix_ptr = UPD;
    put_ret_code(DBYES);
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
    log3("Closing ", "transaction ", tr.pid_string);
} /* End machine_update */

/* Retrieve updated copy of machine tables */
int machine_put(void)
{
    int index;
    char new_data_dir[FILE_SIZE];
    char new_delta_dir[FILE_SIZE];
    char *files[MACHINE_NUM];

    /* Verify machine tables can be updated or checked in */
    index = state_file_read4update();
    /* Indicate ready to perform transaction */
    put_short(SUCCESS);
     /* Retrieve updated copy of machine tables */
    strcpy9(new_data_dir, "tmp_files/", user.login, "_", mask_msg.key1, "_", mask_msg.key2,
    "_", tr.pid_string, ".data_files");
    files_rcv(new_data_dir, files);
    /* Increment the version number */
    st.vnum += .1;
    /* Version the machine files */
    /* Build the new delta directory name */
    strcpy9(new_delta_dir, "tmp_files/", user.login, "_", mask_msg.key1, "_", mask_msg.key2,
    "_", tr.pid_string, ".delta_files");
    /* Create this directory */
    dir_create(new_delta_dir);
    mask_files_version(new_data_dir, mti.data_dir, new_delta_dir, mti.delta_dir, files);
    /* Delete the temporary directories no longer needed */
    if (rmdir(new_data_dir) != SUCCESS)
    dir_delete(new_data_dir);
    if (rmdir(new_delta_dir) != SUCCESS)
    dir_delete(new_delta_dir);
    return(index);
} /* end machine_put */

/* Unapprove machine tables */
void machine_unapprove(void)
{
    int index;
    char *files[MACHINE_NUM];

    /* Verify table can be unapproved */
    index = state_file_read4update();
    if (st.version_type[0] == '-')
    shut_down("Data is not approved", -1);
    /* Build names of files to delete */
    machine_init(files);
    /* Build name of official machine directory */
    strcpy4(mti.off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, FAB_DIR);
    /* Delete official copy of file */
    files_delete(mti.off_data_dir, files);
    /* Increment the transaction number */
    ++st.transaction_num;
    st.touch_time = st.upd_time = tr.upd_time;
    /* Mark the file unapproved */
    st.version_type[0] = '-';
    state_file_write(index);
    mti.suffix_ptr = UNAPP;
    put_short(SUCCESS);
    put_ret_code(DBYES);
} /* End machine_unapprove */

/* Initialize paths needed to perform machine transactions */
void machine_paths_init(void)
{
    strcpy4(mti.data_dir, FAB_DATA, SLASH, mask_msg.key1, FAB_DIR);
    strcpy4(mti.delta_dir, FAB_DELTA, SLASH, mask_msg.key1, FAB_DIR);
    strcpy1(mti.tmp_data_dir, TMP_FAB_DATA);
    strcpy1(mti.tmp_delta_dir, TMP_FAB_DELTA);
    if (mask_msg.req_type != RETRIEVE1FILE)
     strcpy3(mti.prefix, "Machine tables for fab '", mask_msg.key1, "'");
     else strcpy3(mti.prefix, "Machine log for fab '", mask_msg.key1, "'");
     strcpy3(mti.suffix, "machine tables for fab '", mask_msg.key1, "'");
    /* Build the state file name */
    strcpy2(mti.state_file, mti.delta_dir, "/state_file");
} /* End machine_paths_init */

void process_machine(void)
{

    /* Initialize paths needed to perform machine transactions */
    machine_paths_init();
    switch(mask_msg.req_type)
{
    /* Create new machine tables by porting it from the user's disk */
    /* Retrieve unofficial copy of machine files */
	case READ: machine_read(); break;
	case READ_ANY: machine_read(); break;
    /* Retrieve official copy of machine tables */
    case RETRIEVE:  machine_retrieve(); break;
	/* Read copy of machine table log */
    case RETRIEVE1FILE:  machine_log_read(); break;
	/* Check out machine tables */
	case CHECKOUT:
	{
	    pin_check();
	    machine_checkout(); 
	}
	break;
	/* Cancel check-out on specified machine tables */
	case CANCHECKOUT:
	{
	    pin_check();
	    mask_cancheckout(); 
	    /* Send update to remote servers */
	    mask_refresh(0, rmt_mpd.server_num, MARK);
	}
	break;
	/* Check in machine tables */
	case CHECKIN:
	{
	    pin_check();
	    machine_checkin(); 
	}
	break;
	case UPDATE:
	{
	    pin_check();
	    machine_update(); 
	}
	break;
	/* Approve machine tables */
	case APPROVE:
	{
	    pin_check();
	    machine_approve(); 
	}
	break;
	case UNAPPROVE:
	{
	    pin_check();
	    machine_unapprove(); 
	}
	break;
	/* Delete existing machine tables */
	case DELETE:
	{
	    pin_check();
	    machine_dlt(); 
	}
	break;
	case MOD_USERGROUP:  mask_mod_usergroup(); break;
	case ADD_USERGROUP:  mask_add_usergroup(); break;
	case ADD_USER:  mask_add_user(); break;
	case LIST_MEMBERS: mask_list_members(); break;

	/* Transfer ownership of machine tables to new database */
	case TRANSFER:
	{
	    pin_check();
	     mask_transfer(); 
    	 /* Send updated state file to remote servers */
	    mask_refresh(0, rmt_mpd.server_num, MARK);
	}
	break;
	/* Force shadow of machine tables to all database machines */
	case SHADOW:  mask_shadow(mask_msg.num); break;
	case REFRESH: rmt_mask_refresh(); break;
	case MARK: rmt_mask_refresh(); break;
	case AUDIT:
	{
	    long audit_time = audit_time_set();
	    machine_audit(&audit_time);
	}
	break;
	case RMT_AUDIT:  rmt_audit_data(); break;
	case RMT_AUDIT_STATE:  rmt_audit_state(); break;
	case VIEW_USERS:  mask_view_users(); break;
	case VIEW_USERGROUPS:  mask_view_usergroups(); break;
	case VIEW_CWD:  mask_view_cwd(); break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_machine */
