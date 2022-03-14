/****************************************************************************
 *
 * Function:	process_pc_history
 *
 * Description:	Processes requests with data type of PC *
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <alloca.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "dir.h"
#include "entry.h"
#include "close_down.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "list.h"
#include "mask_server_defines.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
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
#include "global_debug.h"


/* Retrieve directory where PC was checked out */
void pc_history_read(void)
{
    char history_file[FILE_SIZE];

    DBG();

    /* Verify pc library is present and can be read */
    state_file_read();
    /* Send data to user */
    /* Build name of file client will see */
    strcpy2(history_file, mask_msg.key1, ".history");
    /* Send history file to user */
    put_short(SUCCESS);
    file_get_alternate(mti.data_dir, "history", history_file);
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
} /* End pc_history_read */

void pc_history_checkout(void)
{
    char history_file[FILE_SIZE];
    int index;

    DBG();

    /* Verify pc library is present and can be checked out */
    index = state_file_read4update();
    /* Send data to user */
    /* Build name of file client will see */
    strcpy2(history_file, mask_msg.key1, ".history");
    /* Send history file to user */
    put_short(SUCCESS);
    file_get_alternate(mti.data_dir, "history", history_file);
    /* Update state file to reflect checked out */
   st.touch_time = tr.upd_time;
    st.state = CHECKED_OUT;
    ++st.transaction_num;
    strcpy(st.cwd, user.cwd);
    strcpy(st.user_id, user.email);
    state_file_write(index);
    /* Tell user all data sent */
    mti.suffix_ptr = CHKOUT;
    put_ret_code(DBYES);
} /* End pc_history_checkout */


void process_pc_history(void)
{
    DBG();
    /* Make sure user is retrieving data from Austin site */
    if (strcmp(ip.master_site, "wacc") != 0)
    m2server_send("wacc");
    /* verify user authorized to build and approve PC's */
    if ((mask_msg.req_type == CHECKOUT) || (mask_msg.req_type == CHECKIN)
    || (mask_msg.req_type == CANCHECKOUT) || (mask_msg.req_type == MOD_OWNER))
    pc_build_check();
    /* Initialize paths needed for PC transactions */
    pc_history_paths_init();
    switch(mask_msg.req_type)
{
	/* Create a new pc library */
	case APPROVE:  pc_approved_version_rtv();
	case READ: pc_history_read(); break;
	case CHECKOUT: pc_history_checkout(); break;
	case CANCHECKOUT: pc_cancheckout(); break;
	case CHECKIN: pc_history_checkin(); break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
	    /* Send list of approvers */
    } /* End switch */
} /* End process_pc_history */
/* Initialize paths needed to perform PC transactions */
void pc_history_paths_init(void)
{
    char *ptr;
    char *ptr2;

    DBG();
    /* Set program to 1 so line 1 of state file read */
    mask_msg.program = 1;
    /* Look for spaces in the PC library name */
    ptr = strchr(mask_msg.key1, ' ');
    if (ptr == NULL)
    {
        strcpy(key, mask_msg.key1);
    }
    else
    {
        /* Verify format is correct */
        /* Must be '-  ' or '+  ' */
        --ptr;
        if ((ptr[2] == ' ')
                && ((ptr[0] == '-') || (ptr[0] == '+')))
        {
            ptr2 = ptr+3;
            ptr[0] = '\0';
            strcpy2(key, mask_msg.key1, ptr2);
        }
        else
        {
            close_down1f(invalid_pc_name, mask_msg.key1, INVALID_PC_NAME);
        }
    } /* End else */
    /* Temporarily put colons in library name */
    strexc(key, '/', ':');
    strcpy3(mti.prefix, "History for PC library '", key, "'");
    strcpy3(mti.suffix, "history for PC library '", key, "'");
    /* Get rid of colons in library name */
    strexc(key, ':', '/');
    strcpy3(mti.data_dir, PC_DATA, "/", key);
    strcpy3(mti.delta_dir, PC_DATA, "/", key);
    /* Build the state file name */
    strcpy2(mti.state_file, mti.data_dir, "/state_file");
} /* End pc_history_paths_init */



/* Update specified pc */
/* New version of PC created and entry for new version put in version file */
void pc_history_checkin(void)
{
    char history_file[FILE_SIZE];
    int index;

    DBG();
    /* New PC name already initialized */
    /* Make sure PC can be checked in */
    index = state_file_read4update();
    /* Tell client ready to get history file */
    put_short(SUCCESS);
    /* Send down user's current working directory */
    put_string(st.cwd);
    /* Now get the history file */
    file_get(mti.data_dir, history_file);
    /* Update the state file */
    st.touch_time = st.upd_time = tr.upd_time;
    st.state = NO_ACTION;
    strcpy(st.cwd, user.cwd);
    strcpy(st.user_id, user.email);
    ++st.transaction_num;
    state_file_write(index);
    mti.suffix_ptr = CHKIN;
    put_ret_code(DBYES);
    /* Shadow PC library to remote servers */
} /* End pc_history_checkin */

void pc_history_cancheckout(void)
{
    char local_dir[PATH_SIZE];
    int index;

    DBG();

    /* New PC name already initialized */
    /* Make sure PC can be checked in */
    index = state_file_read4update();
    /* Update the state file */
    st.touch_time = tr.upd_time;
    st.state = NO_ACTION;
    strcpy(local_dir, st.cwd);
    strcpy(st.cwd, user.cwd);
    strcpy(st.user_id, user.email);
    ++st.transaction_num;
    state_file_write(index);
    put_short(SUCCESS);
    put_string(local_dir);
    mti.suffix_ptr = CAN;
    put_ret_code(DBYES);
} /* End pc_history_cancheckout */


void pc_approved_version_rtv(void)
{
    DBG();
    /* Put in different prefix in case error occurs */
    strcpy(mti.prefix, mti.prefix+12);
    /* Log the fact that this is retrieving the approved version */
    data_log(tr.log_file, "Retrieving approved version of PC");
    /* Read the state file */
    state_file_read();
    /* Give error if no currently approved version */
    if (st.app_version == 0)
    close_down1f(no_approved_version, mti.prefix, NO_APPROVED_VERSION);
    /* Indicate ready to send data */
    put_short(SUCCESS);
    /* Send the version number */
    sprintf(rtn.msg, "%2.3f", st.app_version);
    put_string(rtn.msg);
    /* Indicate transaction successful */
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
} /* end pc_approved_version_rtv */

/* Create the default history file created when PC created */
void pc_history_file_create(void)
{
    FILE *fp;
    char history_file[PATH_SIZE];
    char name[64];

    DBG();
    strcpy2(history_file, mti.data_dir, "/history");
    if ((fp = fopen(history_file, "w")) == NULL)
    close_down1m(wr_open_err, history_file, WR_OPEN_ERR);
    fprintf(fp, "    HISTORY FILE FOR %s\n\n", mti.prefix);
    /* Get the creator's name */
    ldapfsl_get_user_full_name(user.email, name);
    fprintf(fp, "Creator:  %s (%s)\n", name, user.email);
    fprintf(fp, "Created:  %s", ctime(&tr.upd_time));
    fclose(fp);
} /* End pc_history_file_create */
