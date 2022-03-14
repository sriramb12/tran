/****************************************************************************
 *
 * Function:	process_pc
 *
 * Description:	Processes requests with data type of PC *
 *		Backward change made around line 655 in pc_build_name
 *		Backward change made at line 419 to not send version num in cancheckout
 *		Backward change made in pc_retrieve to not send version number
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <alloca.h>
#include <string.h>
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
void pc_read_gds(void)
{
    DBG();
    /* Verify pc library is present and can be read */
    state_file_read();
    pc_send_gds(st.last_version);
    /* rtn.msg is set in pc_send */
    return_success(rtn.msg, DBYES);
} /* End pc_read_gds */

void pc_checkout(void)
{
    int index;
    DBG();
    /* Verify pc library is present and can be read */
    index = state_file_read4update();
    /* Send data to user */
    /* Version number is returned in last_version */
    pc_send(st.last_version, CHKOUT);
    /* Update state file to reflect checked out */
    st.touch_time = tr.upd_time;
    st.state = CHECKED_OUT;
    ++st.transaction_num;
    strcpy(st.cwd, user.cwd);
    strcpy(st.user_id, user.email);
    state_file_write(index);
    /* rtn.msg set in pc_send */
    return_success(rtn.msg, DBYES);
} /* End pc_checkout */

/* Read approved version of PC */
void pc_read(void)
{
    DBG();
    /* Verify pc library is present and can be read */
    state_file_read();
    /* Send data to user */
    /* Version number is returned in last_version */
    pc_send(st.last_version, RTV);
    /* rtn.msg set in pc_send */
    return_success(rtn.msg, DBYES);
} /* End pc_read */


	/* Major revision made, and files not deleted from user's disk */
void pc_approve(void)
{
    char log_msg[FILE_SIZE];
    int len;
    int index;

    DBG();
    /* Make sure the PC library is present */
    index = state_file_read4update();
    /* Verify user authorized to approve this library */
    if (strcmp(user.email, st.approver) != 0)
    {
	if (entry_find(APPROVE_FILE, user.email) != SUCCESS)
	close_down1p(not_approver, mti.prefix, NOT_APPROVER);
    } /* end if */
    /* Determine if doing latest or by version */
    if (mask_msg.num == M2LATEST)
    sprintf(mask_msg.key2, "%2.1f", st.vnum);
    else if (mask_msg.num == M2APPROVED)
    sprintf(mask_msg.key2, "%2.1f", st.app_version);
    /* Make sure not trying to approve currently approved version */
    if (strcmp(st.version_type, mask_msg.key2) == 0)
    close_down2f(alr_approved, mask_msg.key2, mti.prefix, ALR_APPROVED);
    /* Make sure specified version is present */
    /* Build complete version path */
    len = strlen(mti.data_dir);
    strcpy2(mti.data_dir+len, "/", mask_msg.key2);
    if (access(mti.data_dir, 0) != 0)
    close_down2f(non_version_err, mask_msg.key2, mti.prefix, NON_VERSION_ERR);
    strcpy(mti.data_dir+len, "/version_list");
    /* build version string */
    sprintf(log_msg, "   %s %s (%s) %s", mask_msg.key2, user.name,
    user.email, ctime(&tr.upd_time));
    /* Chop off trailing carriage return */
    log_msg[strlen(log_msg)-1] = '\0';
    /* Change currently approved to previously approved */
    /* It is the currently approved version */
    entry_unapprove(mti.data_dir);
    /* Now approve new version */
    entry_approve(mti.data_dir, mask_msg.key2);
    /* Update the state file */
    st.touch_time = st.upd_time = tr.upd_time;
    ++st.transaction_num;
    strcpy(st.version_type, "+");
    st.app_version = atof(mask_msg.key2);
    state_file_write(index);
    /* Tell user transaction successful */
    mti.suffix_ptr = APP;
    put_short(SUCCESS);
    put_ret_code(DBYES);
} /* End pc_approve */




/* Delete the pc library */
void pc_delete(void)
{
    int index;

    DBG();
    /* Make sure pc can be deleted */
    index = state_file_read4update();
    /* Make sure no approved versions of the PC library present */
    pc_approve_versions_check();
    /* Close and unlock the state file */
    state_file_close(index);
    /* Delete the data directory */
    dir_recursive_dlt(mti.data_dir);
    /* Indicate transaction successful */
    put_short(SUCCESS);
    /* Tell user pc library deleted */
    mti.suffix_ptr = DLT;
    put_ret_code(DBNO);
    /* Delete PC library from remote servers */
} /* End pc_delete */

/* Make list of PC libraries and send to user */
/* This list is created dynamicly */
void pc_libs_list(void)
{
    char *files[M2TOTAL_FILES];

    DBG();
    dir_list(PC_DATA, files);
    /* Indicate ready to send library names */
    put_short(SUCCESS);
    /* Send list of names */
    list_get(files);
    strcpy(mti.prefix, "List of PC libraries ");
    mti.suffix_ptr = RTV;
    put_ret_code(DBNO);
} /* End pc_libs_list */

/* Retrieve List of files present in specified PC library */
void pc_list(void)
{
    char *files[M2TOTAL_FILES];

    DBG();
    dir_list(mti.data_dir, files);
    /* Indicate ready to send library names */
    put_short(SUCCESS);
    /* Send list of names */
    list_get(files);
     strcpy3(mti.prefix, "List of files present in PC library '", mask_msg.key1, "'");
    mti.suffix_ptr = RTV;
    put_ret_code(DBNO);
} /* End pc_list */

/* Send updated data to remote servers */

void process_pc(void)
{
    DBG();
    /* Make sure user is retrieving data from Austin site */
    if (strcmp(ip.master_site, "wacc") != 0)
    m2server_send("wacc");
    /* verify user authorized to build and approve PC's */
    if ((mask_msg.req_type == CREATE) || (mask_msg.req_type == DELETE)
    || (mask_msg.req_type == CHECKOUT) || (mask_msg.req_type == CHECKIN)
    || (mask_msg.req_type == CANCHECKOUT) || (mask_msg.req_type == MOD_OWNER))
    pc_build_check();
    else if ((mask_msg.req_type == APPROVE) || (mask_msg.req_type == UNAPPROVE))
    pc_approve_check();
    /* Initialize paths needed for PC transactions */
    pc_paths_init();
    switch(mask_msg.req_type)
{
	/* Create a new pc library */
	case VERSIONS:  pc_versions_list(); break;
	case CREATE:  pc_create(); break;
	case CHECKIN:  pc_checkin(); break;
	case RETRIEVE:  pc_cwd_rtv(); break;
	case READ:
	case READ_ANY:
	{
	    if (strcmp(mask_msg.key3, "1") == 0) pc_read();
	    else pc_read_gds();
	}
	break;
	case CHECKOUT: pc_checkout(); break;
	case CANCHECKOUT: pc_cancheckout(); break;

	/* Update pc library in database */
	/* Major revision made, and files not deleted from user's disk */
	case APPROVE: pc_approve(); break;
	case UNAPPROVE: pc_unapprove(); break;
	/* Modify the owner of a pc library */
	/* Must be the owner to perform this transaction */
	/* The user must supply X.500 user ID of new owner */
	case MOD_OWNER:  mask_mod_owner(); break;
	/* Delete n existing pc library */
	case DELETE: pc_delete(); break;
	case HISTORY: pc_history(); break;
	/* Return directory hwere PC library checked out */
	/* Transfer ownership of pc library to new database */
	case TRANSFER:
	{
	    mask_transfer();
	}
	break;
	    case M2LIST:  pc_approvers_list(); break;
	case VIEW_USERS:  mask_view_users(); break;
	case VIEW_USERGROUPS:  mask_view_usergroups(); break;
	case VIEW_CWD:  mask_view_cwd(); break;
	case ADD_USERGROUP:  mask_add_usergroup(); break;
	case ADD_USER:  mask_add_user(); break;
	case TRANSACTION_LOG_RTV:
	{
	    /* Initialize delta directory for this transaction */
	    strcpy(mti.delta_dir, mti.data_dir);
	    mask_transaction_log_rtv();
	}
	break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
	    /* Send list of approvers */
    } /* End switch */
} /* End process_pc */
/* Initialize paths needed to perform PC transactions */
void pc_paths_init(void)
{
    char *ptr;
    char *ptr2;

    DBG();
    /* Look for spaces in the PC library name */
    ptr = strchr(mask_msg.key1, ' ');
    if (ptr == NULL)
	    strcpy(key, mask_msg.key1);
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
	else close_down1f(invalid_pc_name, mask_msg.key1, INVALID_PC_NAME);
    } /* End else */
    /* Temporarily put colons in library name */
    strexc(key, '/', ':');
    strcpy3(mti.prefix, "PC library '", key, "'");
    strcpy3(mti.suffix, "PC library '", key, "'");
    /* Get rid of colons in library name */
    strexc(key, ':', '/');
    if (mask_msg.req_type == CREATE)
    strcpy4(mti.data_dir, "pc_data/", mask_msg.key2, "/", key);
    else strcpy3(mti.data_dir, PC_DATA, "/", key);
    strcpy(mti.delta_dir, mti.data_dir);
    /* Build the state file name */
    strcpy2(mti.state_file, mti.data_dir, "/state_file");
} /* End pc_paths_init */


/* Create specified pc */
void pc_create(void)
{
    int len = strlen(mti.data_dir);
    char log_msg[260];
    char file_name[FILE_SIZE];
    char *files[M2TOTAL_FILES];

    DBG();

    if (access(mti.data_dir, 0) == 0)
    close_down1f(crt_err, mti.prefix, CRT_ERR);
    /* Now we are finally ready to create the pc */
    dir_make(mti.data_dir);
    /* Create the default history file */
    pc_history_file_create();
    /* Make the directory where the initial version will exist */
    strcpy(mti.data_dir+len, "/1.0");
    dir_make(mti.data_dir);
    /* Now create the state file */
    /* Build state file name */
    /* Different for create function */
    /* par1 holds usergroup for PC */
    pc_state_file_crt(NO_ACTION, mask_msg.par1);
    /* Tell client ready to get files */
    put_short(SUCCESS);
    /* Now get the GDS file */
    file_get(mti.data_dir, file_name);
    /* Now get the other files */
    files_rcv(mti.data_dir, files);
    /* Create the version file */
    sprintf(log_msg, "-  1.0 %s %s (%s) %s", mask_msg.key3, user.name,
    user.email, ctime(&tr.upd_time));
    /* Chop off trailing carriage return */
    log_msg[strlen(log_msg)-1] = '\0';
    strcpy(mti.data_dir+len, "/version_list");
    data_log(mti.data_dir, log_msg);
    /* Make the create file */
    strcpy(mti.data_dir+len, "/create");
    mask_create_write(mti.data_dir);
    mti.suffix_ptr = CRT;
    put_ret_code(DBYES);
} /* End pc_create */

/* checkin specified pc */
/* New version of PC created and entry for new version put in version file */
void pc_checkin(void)
{
    char log_msg[FILE_SIZE];
    char version_file[FILE_SIZE];
    char new_version[8];
    char tmp_file[FILE_SIZE];
    char *files[M2TOTAL_FILES];
    int index;

    DBG();

    /* New PC name already initialized */
    /* Make sure PC can be checked in */
    index = state_file_read4update();
/* Calculate new version number */
    st.vnum += .1;
    sprintf(new_version, "%2.1f", st.vnum);
    /* Build version file name */
    strcpy2(version_file, mti.data_dir, "/version_list");
    /* Make the directory where the new version will exist */
    strcpy2(mti.data_dir+strlen(mti.data_dir), "/", new_version);
    dir_make(mti.data_dir);
    /* Tell client ready to get files */
    put_short(SUCCESS);
    /* Now get the GDS file */
    gds_file_get(mti.data_dir);
    /* Now get the other files */
    files_rcv(mti.data_dir, files);
    /* Update the version list with new version number */
    sprintf(log_msg, "-  %2.1f %s %s (%s) %s",
     st.vnum, mask_msg.key3,  user.name,
    user.email, ctime(&tr.upd_time));
    /* Chop off trailing carriage return */
    log_msg[strlen(log_msg)-1] = '\0';
    /* build tmp file name */
    strcpy5( tmp_file, version_file, "_", user.login, "_", tr.pid_string);
    data_log(tmp_file, log_msg);
    file_append(version_file, tmp_file);
    /* Delete the original file */
    unlink(version_file);
    /* Rename the new file */
    file_rename( tmp_file, version_file);
    /* Update the state file */
    st.touch_time = st.upd_time = tr.upd_time;
    st.state = NO_ACTION;
    strcpy(st.user_id, user.email);
    ++st.transaction_num;
    state_file_write(index);
    mti.suffix_ptr = CHKIN;
    put_ret_code(DBYES);
    /* Shadow PC library to remote servers */
} /* End pc_checkin */

void pc_cancheckout(void)
{
    int index;

    DBG();

    /* New PC name already initialized */
    /* Make sure check out can be cancelled */
    index = state_file_read4update();
    /* Update the state file */
    st.touch_time = tr.upd_time;
    st.state = NO_ACTION;
    strcpy(st.user_id, user.email);
    ++st.transaction_num;
    state_file_write(index);
    put_short(SUCCESS);
    put_string(st.cwd);
    put_string(st.last_version);
    mti.suffix_ptr = CAN;
    put_ret_code(DBYES);
    /* Shadow PC library to remote servers */
} /* End pc_cancheckout */

void pc_versions_list(void)
{
    char *files[M2TOTAL_FILES];
    int cnt;

    DBG();
    /* Build name of version file */
    strcat(mti.data_dir, "/version_list");
    /* If version history not present, give error message */
    if (access(mti.data_dir, 0) != 0)
    close_down1f(non_approve_version, mti.prefix, NON_APPROVE_VERSION);
    /* Read in version list */
    cnt = list_read(mti.data_dir, files);
    /* Indicate that list of versions will be retrieved */
    put_short(cnt);
    /* Send list to client */
    list_get(files);
    /* Send user return code */
    mti.suffix_ptr = RTV;
    strcpy(mti.prefix, "List of PC versions ");
    put_ret_code(DBNO);
} /* End pc_versions_list */
/* See if any approved versions of the PC library are present */

void pc_approve_versions_check(void)
{
    FILE *fp;
    char line[260];
    char version_file[PATH_SIZE];

    DBG();

    strcpy2(version_file, mti.data_dir, "/version_list");
    if ((fp  = fopen(version_file, "r")) == NULL) return;
    while (line_get(fp, line) != EOF)
    {
        if (line[0] == '+')
        {
            fclose(fp);
            close_down1p(approve_dlt_err, mti.prefix, APPROVE_DLT_ERR);
        } /* End if */
    } /* End while */
    /* If reached this point no approved versions found */
    return;
} /* End pc_approve_versions_check */

void pc_send(char *version, char *action)
{
    int cnt;
    int num;
    int len = strlen(mti.data_dir);
    int fnd = DBNO;
    char *files[M2TOTAL_FILES];
    char local_file[FILE_SIZE];
    char alternate_file[FILE_SIZE];
    char remote_file[FILE_SIZE];

    DBG();
    pc_info_generate(version, local_file, remote_file, alternate_file);
    strcpy2(mti.data_dir+len, "/", version);
    /* Send the GDS file */
    gds_file_put(mti.data_dir, local_file, remote_file);
    /* Make a list of pc files */
    num = dir_list(mti.data_dir, files);
    /* Take GDS file off of list */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
        if (strcmp(files[cnt], local_file) == 0)
        {
            fnd = DBYES;
            break;
        } /* End if */
    } /* end for */
    if (fnd == DBYES)
    {
        --num;
        if (cnt < num)
        {
            /* Free the GDS file */
            free(files[cnt]);
            /* Put last file on list in its place */
            files[cnt] = files[num];
        } /* End if */
       //----------------------------------------------------------------------------------
       // May 2019,  Carlos Mazieri
       // removed a line that free(files[num])
       // this area of memory was copied into files[cnt], so it cannot be released
       // it was causing PC libraries mask files to NOT be downloaded
       //----------------------------------------------------------------------------------
        files[num] = NULL;
    } /* end if for file found */
    /* Send the other files */
    files_send(mti.data_dir, files);
    strcpy4(rtn.msg, mti.prefix, action, "\nLocal file name:  ", remote_file);
} /* End pc_send */

/* Sends the GDS file only during a read transaction */
/* this is done when the user retrieves the PC with its original file name */
void pc_send_gds(char *version)
{
    int len = strlen(mti.data_dir);
    char local_file[FILE_SIZE];
    char remote_file[FILE_SIZE];
    char alternate_file[FILE_SIZE];

    DBG();

    pc_info_generate(version, local_file, alternate_file, remote_file);
    strcpy2(mti.data_dir+len, "/", version);
    /* Send the GDS file */
    gds_file_put(mti.data_dir, local_file, remote_file);
    strcpy5(rtn.msg, mti.prefix, "retrieved.\nLocal file name:  ", remote_file,
    "\nDatabase file name:  ", alternate_file);
} /* End pc_send_gds */

/* Retrieve current working directory */
void pc_cwd_rtv(void)
{
    char name[32];
    char phone[20];

    DBG();

    /* Read the state file */
    state_file_read();
    /* If not checked out, let user know */
    if(st.state != CHECKED_OUT)
    close_down1f(nco1err, mti.suffix, NCO1ERR);
    /* If two users not the same, give error */
    if (strcmp(st.user_id, user.email) != 0)
    {
	ldapfsl_get_user_name_phone(st.user_id, name, phone);
	close_down4f(out4err, mti.prefix, name, phone, st.user_id, OUT4ERR);
    }
    /* Tell user ready to send data */
    put_short(SUCCESS);
    /* Send the user's current working directory */
    put_string(st.cwd);
    /* Send the version */
    put_string(st.last_version);
    /* Indicate data retrieved */
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
} /* End pc_cwd_rtv */

version_approve() { }

/* Send version history, owner, etc. to user */
void pc_history(void)
{
    int len = strlen(mti.data_dir);
    int cnt;
    char *list[5000];

    DBG();

    /* Verify the PC is present */
    state_file_read();
    /* build name of version list file */
    strcpy(mti.data_dir+len, "/version_list");
    cnt = list_read(mti.data_dir, list);
    /* Build additional information needed */
    mti.data_dir[len] = '\0';
    create_info_read();
    /* Indicate ready to send data */
    put_short(cnt);
    /* Send the list of versions found */
    list_get(list);
    return_success(rtn.msg, DBYES);
} /* end PC_history */


/* Unapprove current PC library */
void pc_unapprove(void)
{
    int index;

    DBG();

    /* Make sure library present */
    index = state_file_read4update();
    /* Build version list name */
    strcat(mti.data_dir, "/version_list");
    /* Call function that does the unapproval */
    entry_unapprove(mti.data_dir);
    /* Update state file */
    strcpy(st.version_type, "*");
    st.touch_time = st.upd_time = tr.upd_time;
    ++st.transaction_num;
    st.app_version = 0;
    state_file_write(index);
    /* Indicate transaction successful */
    mti.suffix_ptr = UNAPP;
    put_short(SUCCESS);
    put_ret_code(DBYES);
} /* End pc_unapprove */

/* Send client list of PC approvers */
void pc_approvers_list(void)
{
    char *users[1000] = {0};

    DBG();
    /* Build list of approvers */
    int cnt = users_alpha(users, PC_APPROVE_FILE);
    /* Sort list */
    files_sort(users);
    /* Send list */
    put_short(cnt);
    list_get(users);
    /* Tell client all data retrieved */
    return_success("List of approvers retrieved", DBNO);
} /* end pc_approvers_list */

/* Verify user authorized to build PC's */
void pc_build_check(void)
{

    DBG();
    /* If build file not present, create blank file */
    if (access(PC_BUILD_FILE, 0) != 0)
    {
    	FILE *fp = fopen(PC_BUILD_FILE, "w");
	fclose(fp);
    }
    if (entry_find(PC_BUILD_FILE, user.email) != SUCCESS)
    {
	strcpy3(rtn.msg, pc_build_err, DOUBLE_RET, help_msg);
	shut_down(rtn.msg, PC_BUILD_ERR);
    } /* End if */
} /* End pc_build_check */

/* Verify user authorized to approve PC libraries */
void pc_approve_check(void)
{
    DBG();
    /* If PC approve file not present, create blank file */
    if (access(PC_APPROVE_FILE, 0) != 0)
    {
    	FILE *fp = fopen(PC_APPROVE_FILE, "w");
	fclose(fp);
    }
    if (entry_find(PC_APPROVE_FILE, user.email) != SUCCESS)
    {
	strcpy3(rtn.msg, pc_approve_err, DOUBLE_RET, help_msg);
	shut_down(rtn.msg, PC_APPROVE_ERR);
    } /* End if */
} /* End pc_approve_check */

/* Build PC library name empbedding in the version number */
int pc_version_name_build(char *library_name, char **output_name, char *version)
{
    char *ptr;
    DBG();
    /* Find root name of PC library */
    for (ptr = (library_name+strlen(library_name)-1); ptr >= library_name; --ptr)
    {
        if (ptr[0] == ':')
        {
            ptr[0] = '\0';
            break;
        } /* End if */
    } /* End for */
    /* If only leaf name found, indicate no path present */
    *output_name = malloc(strlen(library_name) + strlen(version) +3);
    /* At this point ready to build the name */
    if (ptr != library_name)
    strcpy5(*output_name, library_name, ":", version, ":", ptr+1);
    else strcpy4(*output_name, ":", version, ":", ptr+1);
/*
    if (ptr != library_name)
    strcpy3(*output_name, library_name, ":", ptr+1);
    else strcpy2(*output_name, ":", ptr+1);
*/
} /* End pc_version_name_build */
void pc_info_generate(char *version,char *local_file, char *remote_file, char *alternate_file)
{
    char version_file[FILE_SIZE];
    char *root_ptr;

    DBG();
    /* If requesting latest, get version number from state file */
    if (mask_msg.num == M2LATEST)
    {
        sprintf(version, "%2.1f", st.vnum);
    }
    else if (mask_msg.num == M2APPROVED)
    {
        strcpy2(version_file, mti.data_dir, "/version_list");
        entry_approved_version_find(version_file, version);
    }
    else
    {
        strcpy(version, mask_msg.key2);
    }
    /* Build the local file name */
    root_ptr = root_find(mti.data_dir, '/');
    strcpy(local_file, root_ptr);
    /* Look up alternate file name */
    entry_alternate_name_find(alternate_file, version);
    /* Build remote file name */
    --root_ptr;
    root_ptr[0] = '\0';
    strcpy5(remote_file, mti.data_dir+8, ":", version, ":", local_file);
    /* Put back the slash just taken out */
    root_ptr[0] = '/';
    /* Change slashes in remote name to ':' */
    strexc(remote_file, '/', ':');
} /* End pc_info_generate */
