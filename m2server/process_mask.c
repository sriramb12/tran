/****************************************************************************
 *
 * Function:	process_mask
 *
 * Description:	Processes requests with data type of MASK *
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include <wait.h>
#include "create.h"
#include "dir.h"
#include "close_down.h"
#include "dir.h"
#include "entry.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "list.h"
#include "mask_entry.h"
#include "mask_server_defines.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2mask_files.h"
#include "m2mask_prototypes.h"
#include "m2globals.h"
#include "m2server_arrays.h"
#include "m2server_globals.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "m2req_types.h"
#include "server_lib.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2state_table.h"
#include "mti.h"
#include "pre_suffix.h"
#include "user_params.h"
#include "rmt_server.h"
#include "rtn.h"
#include "strcpy.h"
#include "m2msg.h"
#include "global_debug.h"


#define no_version_history "no_version_history"
#define NO_VERSION_HISTORY -1
#define convert_ovr "convert_ovr"
#define CONVERT_OVR -1

extern char global_email[];

/* Verify mask set and cap wafer not already present */
void mask_cap_wafer_check(void)
{
    char cap_wafer[32];

    /* Verify mask not already present */
    if (access(mti.state_file, 0) == 0)
    close_down1f(crt_err, mti.prefix, CRT_ERR);
    /* Make sure the associated Cap Wafer is not present */
    if (mask_msg.data_type == MASK_SET)
    {
    strcpy2(cap_wafer, "cap_wafer/", mask_msg.key1);
    if (access(cap_wafer, 0) == 0)
    close_down1f(maskset_create_err, mti.suffix, MASKSET_CREATE_ERR);
    } /* end if */
} /* end mask_cap_wafer_check */

void mask_read(void)
{
    int cnt;
    int convert;
    int ret;
    char *files[M2TOTAL_FILES];
    char *dirlist[500];
    char *db_files[M2TOTAL_FILES];
    char data_dir[FILE_SIZE];
    char db_data_dir[FILE_SIZE];

    /* Verify mask set is present and can be read */
    state_file_read();
    /* If database does not own data, send request to owner */
    /* Do not send request if request type is READ_ANY */
    if ((mask_msg.req_type != READ_ANY)
    && (strcmp(st.master_site, ip.master_site) != 0))
    m2server_send(st.master_site);
    /* If reading approved copy of template, verify approved data is present */
    if ((mask_msg.data_type == TEMPLATE)
    && (mask_msg.req_type == RETRIEVE) && (st.version_type[0] == '-'))
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Make a copy of the db_version file before building the list */
/*
    db_version_crt(version);
*/
    if (mask_use_autosave() == DBYES)
    {
    cnt = mask_files_list(mti.tmpdb_data_dir, db_files);
    strcpy(mti.db_data_dir, mti.tmpdb_data_dir);
    } /* end if */
    else cnt = mask_files_version_list(mti.db_data_dir, db_files, mti.db_delta_dir, DBNO);
    /* Make sure adequate number of files are present */
    if (cnt < MIN_MASK_FILES)
    {
    rtn.len = sprintf(rtn.msg, not_enough_files, mti.prefix, cnt);
    email_send("mdsEvents@nxp.com", global_email,
    "Error reading data", rtn.msg);
    strcpy2(rtn.msg+rtn.len, DOUBLE_RET, HELP_MSG);
    shut_down(rtn.msg, NOT_ENOUGH_FILES);
    }
    /* Make sure the db files can be converted before sending the data */
    /* Convert the .db directory */
    convert = mask_db_dir_convert(data_dir, db_data_dir, db_files);
    /* Send the global files and the fab files */
    global_files_send(mti.data_dir);
    /* Make list of files in top directory */
    ret = mask_files_version_list(mti.data_dir, files, mti.delta_dir, DBNO);
    ret = subdir_version_list(mti.delta_dir, dirlist);
    /* Send these files */
    files_send(mti.data_dir, files);
    /* Send the .db directory */
    mask_db_dir_send(data_dir, db_data_dir, convert, db_files);
    /* Now send other directories */
    subdir_send(dirlist);
    parent_template_check(st.upd_time);
    /* check for turned off mask set */
    if (mask_msg.data_type == MASK_SET)
    {
    if ((mask_msg.override_flag == DBNO)
    && (autosave_present() == DBYES))
    return_override(autosave_present, AUTOSAVE_PRESENT);
    }
    if (mask_msg.data_type == MASK_SET)
    turned_off_mask_wrn_check(mask_msg.key1);
    if (rtn.len > 0) return_warning(rtn.msg, CONVERT_WRN, DBYES);
    else
    {
    mti.suffix_ptr = RTV;
    put_ret_code(DBYES);
    } /* End else */
} /* End mask_read */

void mask_create(void)
{
    int convert;
    char new_file[64];
    char *files[M2TOTAL_FILES];
    char *global_files[M2TOTAL_FILES];
    char *db_files[M2TOTAL_FILES];
    char data_dir[FILE_SIZE];
    char db_data_dir[FILE_SIZE];
    int num;


    /* make sure mask and cap wafer not already present */
    mask_cap_wafer_check();
    /* Verify source fab and technology already present and approved */
    source_tech_check();
     /* Create the delta directory */
    dir_create(mti.delta_dir);
    /* Make the .db directory */
    dir_create(mti.db_delta_dir);
    /* Create the state file */
    state_file_crt(CHECKED_OUT, mask_msg.key3);
    /* Copy files from data directory */
     /* Build name for parent directory */
    strcpy5(mti.off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.par1, SLASH, mask_msg.par2);
    strcpy5(mti.off_delta_dir, FAB_DELTA, SLASH, mask_msg.par1, SLASH, mask_msg.par2);
    /* read in latest version of source template or mask set */
    source_state_file_read(mti.off_delta_dir);
    /* Copy files in data directory */
    mask_dir_copy(mti.data_dir, mti.off_data_dir, mti.off_delta_dir, files, DBYES);
    /* Create the .db files */
    strcat(mti.off_data_dir, "/.db");
    strcat(mti.off_delta_dir, "/.db");
    num = mask_dir_copy(mti.db_data_dir, mti.off_data_dir, mti.off_delta_dir, db_files, DBNO);
    /* Make the create file */
    strcpy2(new_file, mti.data_dir, "/.db/create");
    if (mask_msg.data_type == MASK_SET)  mask_create_write(new_file);
     /* If this is fab data write out new create data */
    else template_create_write(new_file);
    /* Add create file to list */
    db_files[num] = malloc(7);
    strcpy(db_files[num], "create");
    db_files[num+1] = NULL;
    /* Create associated version files */
    version_files_create(files, mti.delta_dir);
    /* Now create version files for .db directory */
    version_files_create(db_files, mti.db_delta_dir);
    /* Add .dir directory for extra directories to be added later */
    /* Create extra directories needed */
    strcpy2(mti.tmp_delta_dir, mti.delta_dir, "/.dir");
    dir_create(mti.tmp_delta_dir);
    strcat(mti.tmp_delta_dir, "list");
    dir_create(mti.tmp_delta_dir);
    strcpy2(mti.tmp_data_dir, mti.data_dir, "/.dir");
    dir_create(mti.tmp_data_dir);
    /* If this is mask data, add entry to mask table */
    if (mask_msg.data_type == MASK_SET) mask_entry_add(mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4);
    else if (mask_msg.req_type == TEMPLATE) tech_entry_add(mask_msg.key1, mask_msg.key2);
    /* Make sure the db files can be converted before sending the data */
    convert = mask_db_dir_convert(data_dir, db_data_dir, db_files);
    /* Indicate ready to perform transaction */
    put_short(SUCCESS);
    /* Make a list of global files */
    dir_list(OFF_GLOBAL_DATA, global_files);
    /* Send the global files */
    files_send(OFF_GLOBAL_DATA, global_files);
     /* If fab data present, send it */
    strcpy4(mti.tmp_data_dir, FAB_DATA, SLASH, mask_msg.par1, FAB_DIR);
    if (access(mti.tmp_data_dir, 0) == 0)
    {
    dir_list(mti.tmp_data_dir, global_files);
    files_send(mti.tmp_data_dir, global_files);
    } /* End if */
    /* Send data files to user */
    files_send(mti.data_dir, files);
    /* Send the .db directory */
  mask_db_dir_send(data_dir, db_data_dir, convert, db_files);
    /* Indicate no more directories to send */
    put_short(SUCCESS);
    /* Tell user mask set created */
    mti.suffix_ptr = CRT;
    put_ret_code(DBYES);
    /* common_layers not updated here */
/* because it is updated when the mask set or template is updated */
    /* Shadow data to remote servers */
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* End mask_create */

void mask_copy(void)
{
    int convert;
    char create_file[64];
    char new_file[64];
    char *files[M2TOTAL_FILES];
    char *db_files[M2TOTAL_FILES];
    char data_dir[FILE_SIZE];
    char db_data_dir[FILE_SIZE];
    int num;


    /* make sure mask and cap wafer not already present */
    mask_cap_wafer_check();
    /* Verify source mask present */
    strcpy3(mti.tmp_delta_dir, MASK_DELTA, SLASH, mask_msg.par1);
    if (access(mti.tmp_delta_dir, 0) != 0)
     {
    strcpy3(mti.prefix, "Mask set '", mask_msg.par1, "'");
    close_down1f(non_err, mti.prefix, NON_ERR);
    }
     /* Create the delta directory */
    dir_create(mti.delta_dir);
    /* Make the .db directory */
     dir_create(mti.db_delta_dir);
    /* Create extra directories needed */
    /* Create the state file */
    state_file_crt(CHECKED_OUT, mask_msg.key3);
    /* Copy files from data directory */
     /* Build name for parent directory */
    strcpy3(mti.tmp_data_dir, MASK_DATA, SLASH, mask_msg.par1);
    /* Read latest version number from source mask set */
    source_state_file_read(mti.tmp_delta_dir);
    mask_dir_copy(mti.data_dir, mti.tmp_data_dir, mti.tmp_delta_dir, files, DBYES);
    /* Create the .db files */
    strcat(mti.tmp_data_dir, "/.db");
    strcat(mti.tmp_delta_dir, "/.db");
    num = mask_dir_copy(mti.db_data_dir, mti.tmp_data_dir, mti.tmp_delta_dir, db_files, DBNO);
    /* Make the create file */
    strcpy2(new_file, mti.data_dir, "/.db/create");
    /* Read in data from old create file */
    if (mask_msg.data_type == MASK_SET)
    {
    /* Build name for existing create file */
    strcpy2(create_file, mti.tmp_data_dir, "/create");
    create_read(create_file);
    mask_create_write(new_file);
    }
     /* If this is fab data write out new create data */
    else
    {
    template_create_write(new_file);
    strcpy(mask_msg.par1, mask_msg.key1);
    } /* End else */
    /* Add create file to list */
    db_files[num] = malloc(7);
    strcpy(db_files[num], "create");
    db_files[num+1] = NULL;
    /* Create associated version files */
    version_files_create(files, mti.delta_dir);
    /* Now create version files for .db directory */
    version_files_create(db_files, mti.db_delta_dir);
    /* Add .dir directory for extra directories to be added later */
    strcpy2(mti.tmp_data_dir, mti.data_dir, "/.dir");
    dir_create(mti.tmp_data_dir);
    strcpy2(mti.tmp_delta_dir, mti.delta_dir, "/.dir");
    dir_create(mti.tmp_delta_dir);
    strcat(mti.tmp_delta_dir, "list");
    dir_create(mti.tmp_delta_dir);
    /* Add .dir directory for extra directories to be added later */
    strcpy2(mti.tmp_data_dir, mti.data_dir, "/.dir");
    dir_create(mti.tmp_data_dir);
     /* Add entry to mask set table */
    if (mask_msg.data_type == MASK_SET)
    mask_entry_add(mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4);
    else if (mask_msg.data_type == TEMPLATE) tech_entry_add(mask_msg.key1, mask_msg.key2);
    /* Make sure the db files can be converted before sending the data */
    convert = mask_db_dir_convert(data_dir, db_data_dir, db_files);
    /* Make a list of global files */
    global_files_send(mti.data_dir);
    /* Send data files to user */
    files_send(mti.data_dir, files);
    /* Send .db files to user */
    mask_db_dir_send(data_dir, db_data_dir, convert, db_files);
    /* Indicate all directories sent */
    put_short(SUCCESS);
    /* Tell user mask set created */
    mti.suffix_ptr = CPY;
    put_ret_code(DBYES);
    /* common_layers not updated here */
/* because it is updated when the mask set or template is updated */
    /* Shadow data to remote servers */
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* End mask_copy */

void mask_checkout(void)
{
    int cnt;
    int convert;
    char *dirlist[500];
    int index;
    char *files[M2TOTAL_FILES];
    char *db_files[M2TOTAL_FILES];
    char data_dir[FILE_SIZE];
    char db_data_dir[FILE_SIZE];

    /* Verify mask set can be checked out */
    index = state_file_read4update();
    /* Check for autosaved data */
    if (mask_use_autosave() == DBYES)
    {
    cnt = mask_files_list(mti.tmpdb_data_dir, db_files);
    strcpy(mti.db_data_dir, mti.tmpdb_data_dir);
    } /* end if */
    else cnt = mask_files_version_list(mti.db_data_dir, db_files, mti.db_delta_dir, DBNO);
    /* Make sure adequate number of files is present */
    if (cnt < MIN_MASK_FILES)
    {
    rtn.len = sprintf(rtn.msg, not_enough_files, mti.prefix, cnt);
    email_send("mdsEvents@nxp.com", global_email,
    "Error checking out data", rtn.msg);
    strcpy2(rtn.msg+rtn.len, DOUBLE_RET, HELP_MSG);
    shut_down(rtn.msg, NOT_ENOUGH_FILES);
    }
    /* Make sure the db files can be converted before sending the data */
    /* Convert the .db directory */
    convert = mask_db_dir_convert(data_dir, db_data_dir, db_files);
    /* Send global data and fab data */
    global_files_send(mti.data_dir);
    /* Make a list of mask files */
    mask_files_version_list(mti.data_dir, files, mti.delta_dir, DBNO);
    subdir_version_list(mti.delta_dir, dirlist);
    /* Send mask files to client */
    files_send(mti.data_dir, files);
    /* Send the .db directory */
    mask_db_dir_send(data_dir, db_data_dir, convert, db_files);
    /* Send the other subdirectories */
    subdir_send(dirlist);
    /* Mark the file checked out */
    st.state = CHECKED_OUT;
    /* Increment the transaction number */
    ++st.transaction_num;
    /* Update the touch and update times */
    /* Save upd time for later use */
    st.touch_time = tr.upd_time;
    /* Record user who has data checked out */
    strcpy(st.user_id, user.email);
    strcpy(st.cwd, user.cwd);
    /* Write out changes */
    state_file_write(index);
    if (mask_msg.data_type == MASK_SET)
    {
    parent_template_check(st.upd_time);
    /* If mask set turned off, log message in log file */
    turned_off_mask_log(mask_msg.key1);
    }
    if (rtn.len > 0) return_warning(rtn.msg, CONVERT_WRN, DBYES);
    else
    {
    mti.suffix_ptr = CHKOUT;
    put_ret_code(DBYES);
    } /* End else */
    /* Send mark transaction to remote servers */
     mask_refresh(0, rmt_mpd.server_num, MARK);
} /* End mask_checkout */

/* Retrieve check out status */
void mask_checkout_status_rtv(void)
{
    char *status_ptr;
    char update_time[32];
    char user_id[32];

    state_file_read();
    if (st.state == CHECKED_OUT)
    status_ptr = "Checked out";
    else status_ptr = "Not checked out";
    /* Read version info from the create file */
    create_version_read(user_id, update_time);
    /* Build string to send */
    if (mask_msg.data_type == MASK_SET)
    strcpy8( rtn.msg, "Mask set:  ",   mask_msg.key1,
    "\nStatus:  ", status_ptr,
    "\nLast update by:  ", user_id,
    "\nLast update on:  ", update_time);
    else strcpy10( rtn.msg, "Fab:  ",   mask_msg.key1,
    "\nTechnology:  ", mask_msg.key2,
    "\nStatus:  ", status_ptr,
    "\nLast update by:  ", user_id,
    "\nLast update on:  ", update_time);
    /* Indicate transaction successful */
    put_short(SUCCESS);
    return_success(rtn.msg, DBYES);
} /* End mask_checkout_status_rtv */

/* Update the common_layers table */
void mask_common_layers_update(void)
{
    state_file_read();
    if (common_layers_update() == FAILURE)
    shut_down(rtn.msg, -1);
    else
    {
    put_short(SUCCESS);
    return_success("common layers updated", DBYES);
    }
} /* End mask_common_layers_update */

void mask_checkin(void)
{
    int index;
    char command[FILE_SIZE];

    /* Retrieve the mask files */
    index = mask_put ();
    /* Delete .tmpdb directory if present */
    mask_tmpdb_dlt();
    /* Cancel check out and write out new state */
    st.state = NO_ACTION;
    /* Other state parameters updated in mask_put */
    state_file_write(index);
    /* Take mask set off bad mask list */
    if (mask_msg.data_type == MASK_SET)
    turned_off_mask_delete(mask_msg.key1);
    /* Tell user check in successful */
    mti.suffix_ptr = CHKIN;
    put_ret_code(DBYES);
    // -----------------------------------------------------------------------------------
    // May 2019, Carlos Mazieri, disabled this block that seems to never be used
#if 0
    /* execute separate software to build the fab tech flow table */
    if (mask_library_version_rtv(mti.db_data_dir) == 1)
    strcpy4(command, "ret_build1 ", mask_msg.key1, " ", mask_msg.key2);
    else strcpy4(command, "ret_build2 ", mask_msg.key1, " ", mask_msg.key2);
    system(command);
    log3("Just updated RET table using command ", command, ".");
#endif
    // ------------------------------------------------------------------------------------
    /* Shadow data to remote servers */
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* end mask_checkin */

void mask_port(void)
{
    int ret;
    int file_offset;
    char *dirlist[500];
    char new_delta_dir[FILE_SIZE];
    char new_data_dir[FILE_SIZE];
    char dir_name[FILE_SIZE];
    char additional_files[M2TOTAL_FILES];
    char command[80];
    char *files[M2TOTAL_FILES];
    char *db_files[M2TOTAL_FILES];
    int cnt;

     /* Create the delta directory */
    dir_create(mti.delta_dir);
    dir_create(mti.data_dir);
    /* Make the .db directory */
     dir_create(mti.db_delta_dir);
    /* Create the state file */
    if (mask_msg.req_type == CHECKIN)
    state_file_crt(NO_ACTION, NONE);
    else state_file_crt(CHECKED_OUT, NONE);
    /* Indicate ready to retrieve files from client */
    put_short(SUCCESS);
    /* First get the top level files */
    /* Read in list of local files */
    mask_filelist_read(local_m2filelist, local_m2program, 0);
    cnt = mask_files_rcv(mti.data_dir, files, 0);
    /* Remove duplicates from file list */
    mask_duplicates_remove(files, cnt);
    /* Get any directories present under mask set */
    /* Get the directory name */
    get_string(new_delta_dir);
    /* Get the files */
    /* Read in the local filelist */
    mask_filelist_read(local_db_filelist, local_db_program, 1);
    cnt = mask_files_rcv(mti.db_data_dir, db_files, 1);
    /* If the create file is not on the list, add it */
    /* The create file must be present for all versions of the data */
    mask_create_check(mti.db_data_dir, db_files);
    /* Now get the additional directories */
    strcpy2(dir_name, new_data_dir, "/");
    ret = subdir_get(dir_name, dirlist, additional_files, file_offset);
    /* Create associated version files */
    version_files_create(files, mti.delta_dir);
    /* Now create version files for .db directory */
    version_files_create(db_files, mti.db_delta_dir);
    /* Tell user port successful */
    /* Tell client mask ported to the database */
    mti.suffix_ptr = PORT;
    put_ret_code(DBYES);
    // -----------------------------------------------------------------------------------
    // May 2019, Carlos Mazieri, disabled this block that seems to never be used
#if 0
    if (mask_library_version_rtv(mti.db_data_dir) == 1)
    strcpy4(command, "ret_build1 ", mask_msg.key1, " ", mask_msg.key2);
    else strcpy4(command, "ret_build2 ", mask_msg.key1, " ", mask_msg.key2);
    system(command);
#endif
    // ----------------------------------------------------------------------------------
    /* Send new mask set to remote server */
     mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* End mask_port */

/* Save .db files in .tmpdb directory */
void mask_save(void)
{
    int index;
     char *files[M2TOTAL_FILES];

    /* Verify mask set can be saved */
    index = state_file_read4update();
    /* Build name for temporary directory */
    strcpy2(mti.tmp_data_dir, mti.data_dir, "/.tmpdb.bak");
    /* Rename the .tmpdb directory if already present */
    if (access(mti.tmpdb_data_dir, 0) == 0) dir_rename(mti.tmpdb_data_dir, mti.tmp_data_dir);
    /* Tell client ready to get files */
    put_short(SUCCESS);
     /* Get the new .tmpdb files */
    mask_files_rcv(mti.tmpdb_data_dir, files, 1);
    /* Delete old files */
    dir_delete(mti.tmp_data_dir);
    /* Update the state file with new update time */
    st.touch_time = st.upd_time = tr.upd_time;
    ++st.transaction_num;
    /* If this is template data, update the approve status */
    if (mask_msg.data_type == TEMPLATE)
    {
    if (st.version_type[0] != '-')
    st.version_type[0] = '*';
    } /* End if for template data */
    state_file_write(index);
    /* Tell user save successful */
    mti.suffix_ptr = SAVED;
    put_ret_code(DBYES);
    /* Update the common_layers tables */
/*     common_layers_update(); */
    /* Send refresh transaction to remote server */
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* end mask_save */

    /* Major revision made, and files not deleted from user's disk */
void mask_update(void)
{
    int index;

    /* Retrieve the mask files */
    index = mask_put();
    /* Delete .tmpdb directory if present */
    mask_tmpdb_dlt();
     /* State file parameters updated in mask_put */
    state_file_write(index);
    /* Tells transaction successful in put_mask */
    /* Update the common_layers tables */
/*     common_layers_update(); */
    /* Shadow data to remote servers */
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* End mask_update */

    /* Perform check in with overwrite function */
void mask_ovrwr(void)
{
    int index;

/* Retrieve the mask files */
    index = mask_put();
    /* Cancel check out and write out new state */
    st.state = NO_ACTION;
    /* Other state file parameters updated in mask_put */
    state_file_write(index);
    /* Tell user overwrite successful */
    mti.suffix_ptr = CHKIN;
    put_ret_code(DBYES);
    /* Update the common_layers tables */
/*     common_layers_update(); */
     /* Refresh updated mask set on remote servers */
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* End mask_ovrwr */

/* Retrieve mask files during check in, update and overrwrite */
int mask_put(void)
{
    int ret;
    int cnt;
    char *dirlist[500];
    char additional_files[M2TOTAL_FILES];
    char dir_name[FILE_SIZE];
    int file_offset[500];
    extern char *local_m2filelist[];
    float old_version;
    int index;
    char new_data_dir[FILE_SIZE];
    char new_db_data_dir[FILE_SIZE];
    char new_delta_dir[FILE_SIZE];
    char new_db_delta_dir[FILE_SIZE];
     char *files[M2TOTAL_FILES];
     char *db_files[M2TOTAL_FILES];
    int program[M2TOTAL_FILES];


    /* Verify mask set can be checked in */
    index = state_file_read4update();
    /* Get the new files from the client */
    /* Indicate ready to perform transaction */
    put_short(SUCCESS);
    /* First get the top level files */
    strcpy9(new_data_dir, "tmp_files/", user.login, "_", mask_msg.key1, "_", mask_msg.key2,
    "_", tr.pid_string, ".data_files");
    /* Read in list of local files */
    mask_filelist_read(local_m2filelist, local_m2program, 0);
    cnt = mask_files_rcv(new_data_dir, files, 0);
    /* Remove duplicates from file list */
    mask_duplicates_remove(files, cnt);
    /* Get any directories present under mask set */
    /* Get the directory name */
    get_string(new_delta_dir);
    /* Get the files */
    strcpy9(new_db_data_dir, "tmp_files/", user.login, "_", mask_msg.key1, "_", mask_msg.key2,
    "_", tr.pid_string, ".db_data_files");
    /* Read in the local filelist */
    mask_filelist_read(local_db_filelist, local_db_program, 1);
    cnt = mask_files_rcv(new_db_data_dir, db_files, 1);
    /* If the create file is not on the list, add it */
    /* The create file must be present for all versions of the data */
    mask_create_check(new_db_data_dir, db_files);
    /* Now get the additional directories */
    strcpy2(dir_name, new_data_dir, "/");
    ret = subdir_get(dir_name, dirlist, additional_files, file_offset);
    /* Make sure over MIN_MASK_FILES files are present */
    if ((cnt = mask_db_files_cnt(db_files, cnt)) < MIN_MASK_FILES)
    {
    rtn.len = sprintf(rtn.msg, not_enough_files, mti.prefix, cnt);
    email_send("mdsEvents@nxp.com", global_email,
    "Error checking in/updating data", rtn.msg);
    strcpy2(rtn.msg+rtn.len, DOUBLE_RET, HELP_MSG);
    shut_down(rtn.msg, NOT_ENOUGH_FILES);
    } /* End if for not enough files */
    old_version = st.vnum;
    /* If this is an update, add .001 */
    if (mask_msg.req_type == UPDATE)
    {
    mti.suffix_ptr = UPD;
    put_ret_code(DBYES);
    st.vnum += 0.001;
    } else {
    char string[4];
    sprintf(string, "%2.1f", st.vnum);
    st.vnum = atof(string)+ 0.1;
    }
    /* Version the top level files */
    /* Build the new delta directory name */
    strcpy9(new_delta_dir, "tmp_files/", user.login, "_", mask_msg.key1, "_", mask_msg.key2,
    "_", tr.pid_string, ".delta_files");
    /* Create this directory */
    dir_create(new_delta_dir);
    mask_files_version(new_data_dir, mti.data_dir, new_delta_dir, mti.delta_dir, files);
    /* Build the new db_delta directory name */
    strcpy9(new_db_delta_dir, "tmp_files/", user.login, "_", mask_msg.key1, "_", mask_msg.key2,
    "_", tr.pid_string, ".db_delta_files");
    /* Create this directory */
    dir_create(new_db_delta_dir);
    /* Version the .db files */
    if ((cnt = mask_files_version(new_db_data_dir, mti.db_data_dir, new_db_delta_dir, mti.db_delta_dir, db_files)) < MIN_MASK_FILES)
    {
    rtn.len = sprintf(rtn.msg, not_enough_versions, mti.prefix, cnt);
    email_send("mdsEvents@nxp.com", global_email,
    "Error checking in/updating data", rtn.msg);
    strcpy2(rtn.msg+rtn.len, DOUBLE_RET, HELP_MSG);
    shut_down(rtn.msg, NOT_ENOUGH_VERSIONS);
    } /* End if for not enough files */
    /* Version the other files present in the .db directory */
    other_files_version(1, mti.db_delta_dir, db_files, old_version);
    /* Scan in the mask files */
    other_files_read(files, program);
    other_files_version(0, mti.delta_dir, files, old_version);
    /* Version the additional directories */
    subdir_version(dirlist , additional_files, new_data_dir, new_delta_dir, file_offset);
    /* Delete the temporary directories no longer needed */
    if (rmdir(new_db_data_dir) != SUCCESS)
    dir_delete(new_db_data_dir);
    if (rmdir(new_data_dir) != SUCCESS)
    dir_delete(new_data_dir);
    if (rmdir(new_db_delta_dir) != SUCCESS)
    dir_delete(new_db_delta_dir);
    if (rmdir(new_delta_dir) != SUCCESS)
    dir_delete(new_delta_dir);
    /* If the .tmpdb directory present, delete it */
    if (access(mti.tmpdb_data_dir, 0) == 0) dir_delete(mti.tmpdb_data_dir);
    /* Update the update time */
    st.touch_time = st.upd_time = tr.upd_time;
    ++st.transaction_num;
    /* If this is template data, update the approve status */
    if (mask_msg.data_type == TEMPLATE)
    {
    if (st.version_type[0] != '-')
    st.version_type[0] = '*';
    } /* End if for template data */
    /* If this is the first check in for this tool, set the owner and tmp owner */
    else
    {
    if (strcmp(st.own_id, NONE) == 0) strcpy(st.own_id, user.email);
    if (strcmp(st.own_id, NONE) == 0) strcpy(st.own_id, user.email);
    } /* End else */
    return(index);
} /* End mask_put */

/* Append data to an existing mask file in the database */
void mask_append(void)
{
    int ccc;
    int index;
    char new_data_dir[FILE_SIZE];

    /* If this is Austin, just send the request to wacc */
    if (strcmp(ip.master_site, "wacc") != 0)
    m2server_send("wacc");
    /* Verify mask set is present */
    index = state_file_read4update();
    /* Get the new files from the client */
    /* Indicate ready to perform transaction */
    put_short(SUCCESS);
    /* Actually only getting one file */
    strcpy9(new_data_dir, "tmp_files/", user.login, "_", mask_msg.key1, "_", mask_msg.key3,
    "_", tr.pid_string, ".data_files");
    ccc = mask_files_append(new_data_dir, mti.data_dir);
    /* Version the top level files */
    /* Update version number for all files at current version */
    all_files_version(mti.data_dir, mti.delta_dir);
    all_files_version(mti.db_data_dir, mti.db_delta_dir);
    /* Delete the temporary directories no longer needed */
    if (rmdir(new_data_dir) != SUCCESS)
    dir_delete(new_data_dir);
    /* Update the update time */
    st.vnum += .001;
    st.touch_time = st.upd_time = tr.upd_time;
    ++st.transaction_num;
    /* If this is the first check in for this tool, set the owner and tmp owner */
    if (strcmp(st.own_id, NONE) == 0) strcpy(st.own_id, user.email);
    if (strcmp(st.last_version, NONE) == 0) strcpy(st.last_version, user.email);
    if (strcmp(st.user_id, NONE) == 0) strcpy(st.user_id, user.email);
    strcpy(st.cwd, user.cwd);
    state_file_write(index);
    if (ccc == DBYES)
    {
    sprintf(rtn.msg, "CCC results for mask set '%s' updated in the database", mask_msg.key1);
    return_success(rtn.msg, DBYES);
    email_send("mdsEvents@nxp.com", user.email, "CCC results updated", rtn.msg);
    /* Log the sending of the e-mail */
    sprintf(rtn.msg, "Email sent to %s for CCC update to%s", user.email, mti.suffix);
    data_log(tr.log_file, rtn.msg);
    } else {
    mti.suffix_ptr = " Appended.";
    put_ret_code(DBYES);
    } /* end else */
    /* Shadow data to remote servers */
    mask_refresh(0, rmt_mpd.server_num, REFRESH);
} /* End mask_append */

    /* change the owner */
void mask_mod_owner(void)
{
    int index;

    /* Read and lock the state file */
    index = state_file_read4update();
    /* Verify user authorized to perform transaction */
    if (strcmp(st.own_id, user.email) != SUCCESS)
    {
    char name[32];
    char phone[16];
    ldapfsl_get_user_name_phone(st.own_id, name, phone);
    sprintf(rtn.msg, mod_owner_err, name, phone, st.own_id);
    shut_down(rtn.msg, GRANT4ERR);
    }
    /* Change the owner */
    strcpy(st.own_id, mask_msg.par1);
    strcpy(st.last_version, mask_msg.par1);
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
    /* Tell user owner modified */
    mti.suffix_ptr = "modified";
    put_ret_code(DBYES);
     /* Send updated state file to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
} /* End mask_mod_owner */

void mask_add_usergroup(void)
{
    int list_cnt;
    int cnt;
    char *old_list[50];
    char *new_list[50];
    int index;
    char suffix[12];
    char local_key[KEY_LEN];

    /* Set up suffix and key for later use */
    if (mask_msg.data_type == MASK_SET)
    {
    strcpy(suffix, ".masks");
    strcpy(local_key, mask_msg.key1);
    } else {
    strcpy(suffix, ".templates");
    strcpy3(local_key, mask_msg.key1, " ", mask_msg.key2);
    }
    /* Read and lock the state file */
    index = state_file_read4update();
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Add the new  user groups */
    /* convert old and new usergroups to list format */
    list_cnt = string2list_convert(st.usergroup, old_list);
    /* Put list into expected format with spaces instead of commas */
    strexc(mask_msg.key3, ',', ' ');
    string2list_convert(mask_msg.key3, new_list);
    for (cnt = 0; new_list[cnt] != NULL; ++cnt)
    list_cnt += m2list_add(old_list, new_list[cnt], new_list[cnt], list_cnt);
    /* Get rid of any invalid items */
    m2list_delete(old_list, "None", list_cnt);
    m2list_delete(old_list, NONE, list_cnt);
    /* Now put back into string format */
    list2string_convert(old_list, st.usergroup);
    /* Add references to the new user groups */
    mask_usergroup_add(local_key, mask_msg.key3, suffix);
    /* Increment the transaction number */
    ++st.transaction_num;
     /* Modify the touch time */
    st.touch_time = tr.upd_time;
    /* Write out new state file */
    state_file_write(index);
    strcpy4(rtn.msg, "User groups for ", mti.suffix, " are:  ", st.usergroup);
    return_success(rtn.msg, DBYES);
     /* Send updated state file to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
} /* End mask_add_usergroup */

void mask_add_user(void)
{
    int list_cnt;
    int cnt;
    char *old_list[50];
    char *new_list[50];
    int index;
    char suffix[12];
    char local_key[KEY_LEN];

    /* Set up suffix and key for later use */
    if (mask_msg.data_type == MASK_SET)
    {
    strcpy(suffix, ".masks");
    strcpy(local_key, mask_msg.key1);
    } else {
    strcpy(suffix, ".templates");
    strcpy3(local_key, mask_msg.key1, " ", mask_msg.key2);
    }
    /* Read and lock the state file */
    index = state_file_read4update();
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Add the new  user */
    /* convert old and new users to list format */
    list_cnt = string2list_convert(st.users, old_list);
    /* Put list into expected format with spaces instead of commas */
    strexc(mask_msg.key3, ',', ' ');
    string2list_convert(mask_msg.key3, new_list);
    for (cnt = 0; new_list[cnt] != NULL; ++cnt)
    list_cnt += m2list_add(old_list, new_list[cnt], new_list[cnt], list_cnt);
    /* Get rid of any invalid items */
    m2list_delete(old_list, "None", list_cnt);
    m2list_delete(old_list, NONE, list_cnt);
    /* Now put back into string format */
    list2string_convert(old_list, st.users);
    /* Increment the transaction number */
    ++st.transaction_num;
     /* Modify the touch time */
    st.touch_time = tr.upd_time;
    /* Write out new state file */
    state_file_write(index);
    strcpy4(rtn.msg, "Users for ", mti.suffix, " are:  ", st.users);
    return_success(rtn.msg, DBYES);
     /* Send updated state file to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
} /* End mask_add_user */

void mask_mod_usergroup(void)
{
    int index;
    char usergroups[1024];
    char users[1024];
    char suffix[12];
    char local_key[KEY_LEN];

    /* Set up suffix and key for later use */
    if (mask_msg.data_type == MASK_SET)
    {
    strcpy(suffix, ".masks");
    strcpy(local_key, mask_msg.key1);
    } else {
    strcpy(suffix, ".templates");
    strcpy3(local_key, mask_msg.key1, " ", mask_msg.key2);
    }
    /* Read and lock the state file */
    index = state_file_read4update();
    /* Verify user authorized to perform transaction */
    owner_check();
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Retrieve list of user groups */
    get_string(usergroups);
    /* Get list of users */
    get_string(users);
    /* Delete references to current user groups */
    mask_usergroup_delete(local_key, st.usergroup, suffix);
    /* Change the list of user groups */
    if (strncmp(usergroups, NONE, 3) == 0)
    strcpy(st.usergroup, "None");
    else
    {
    strcpy(st.usergroup, usergroups);
    /* Change commas to spaces */
    strexc(st.usergroup, ',', ' ');
    /* Add references to the new user groups */
    mask_usergroup_add(local_key, st.usergroup, suffix);
    } /* End else */
    if (strncmp(users, NONE, 3) == 0)
    strcpy(st.users, "None");
    else
    {
    strcpy(st.users, users);
    strexc(st.users, ',', ' ');
    }
    /* Increment the transaction number */
    ++st.transaction_num;
     /* Modify the touch time */
    st.touch_time = tr.upd_time;
    /* Write out new state file */
    state_file_write(index);
    /* Modify the global mask set file */
    mask_entry_mod();
    /* Tell user owner modified */
    mti.suffix_ptr = "modified";
    put_ret_code(DBYES);
     /* Send updated state file to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
} /* End mask_mod_usergroup */

/* Grant overwrite */
void mask_grant_overwrite(void)
{
    int index;

    /* Read and lock the state file */
    index = state_file_read4update();
    /* Verify user authorized to perform transaction */
    if (strcmp(st.own_id, user.email) != SUCCESS)
    {
    char name[32];
    char phone[16];
    ldapfsl_get_user_name_phone(st.own_id, name, phone);
    sprintf(rtn.msg, grant4err, mti.prefix, name, phone, st.own_id);
    shut_down(rtn.msg, GRANT4ERR);
    }
    /* Grant overwrite */
    strcpy(st.last_version, mask_msg.par1);
    /* Increment the transaction number */
    ++st.transaction_num;
     /* Modify the touch time */
    st.touch_time = tr.upd_time;
    /* Write out new state file */
    state_file_write(index);
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Tell user overwrite granted */
    strcpy5(rtn.msg, "User '", mask_msg.par1, "' granted overwrite for ",
    mti.suffix, ".");
    return_success(rtn.msg, DBYES);
     /* Send updated state file to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
} /* End mask_grant_overwritewr */

void mask_cancheckout(void)
{
    int index;

    /* Read and lock the state file */
    index = state_file_read4update();
    /* Change the state */
    st.state = NO_ACTION;
    /* Increment the transaction number */
    ++st.transaction_num;
    /* Change the touch date */
    st.touch_time = tr.upd_time;
    /* Write out new state file */
    state_file_write(index);
    /* Delete .tmpdb directory if present */
    mask_tmpdb_dlt();
    /* Tell user check out cancelled */
    put_short(SUCCESS);
    mti.suffix_ptr = CAN;
    strcpy2(mti.prefix, "Check-out for ", mti.suffix);
    put_ret_code(DBYES);
     /* Send updated state file to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
} /* End mask_cancheckout */

/* Delete the mask set */
void mask_delete(void)
{
    int index;

    /* Make sure mask can be deleted */
    index = state_file_read4update();
    state_file_close(index);
    /* Delete the delta directory */
    dir_recursive_dlt(mti.delta_dir);
    /* Delete the data directory */
    dir_recursive_dlt(mti.data_dir);
    /* Delete entry from global mask set file */
    mask_entry_dlt();
    /* Delete reference of mask from user groups */
    mask_usergroup_delete(key, st.usergroup, ".masks");
    /* Indicate transaction successful so far */
    put_short(SUCCESS);
    /* Tell user mask set deleted */
    mti.suffix_ptr = DLT;
    put_ret_code(DBNO);
    /* Tell remote servers mask set deleted */
    mask_send_delete(0, rmt_mpd.server_num);
} /* End mask_delete */

/* Transfer mask ownership to new site */
void mask_transfer(void)
{
    int index;

    /* Make sure mask can be transferred */
    index = state_file_read4update();
    /* Make sure the new site is available */
    /* Change the site to the new site */
    strcpy(st.master_site, mask_msg.par1);
    /* Increment the transaction number */
    ++st.transaction_num;
    /* Increment the touch time */
    st.touch_time = tr.upd_time;
    /* Write out the new data */
    state_file_write(index);
    /* If this is mask data, modify mask table */
    if (mask_msg.data_type == MASK_SET) mask_entry_mod();
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Tell user data transferred */
    strcpy3(rtn.msg, " transferred to '", mask_msg.par1, "'");
    mti.suffix_ptr = rtn.msg;
    /* Send user return code */
    put_ret_code(DBYES);
} /* end mask_transfer */

/* Make sure parent directory present */
void source_tech_check()
{
    char file_name[FILE_SIZE];

    /* First verify that the fab and technology are present */
    /* build state file name */
    strcpy6(file_name, FAB_DELTA, SLASH, mask_msg.par1, SLASH,
    mask_msg.par2, "/state_file");
    if (access(file_name, 0) != 0)
    {
    strcpy5(mti.prefix, "Fab '", mask_msg.par1, "', technology '", mask_msg.par2, "'");
    close_down1f(non_err, mti.prefix, NON_ERR);
    }
    /* Now verify the data has been approved */
    strcpy6(file_name, OFF_FAB_DATA, SLASH, mask_msg.par1, "/",
    mask_msg.par2, "/.db");
    if (access(file_name, 0) != 0)
    {
    strcpy5(mti.prefix, "Approved copy of fab '", mask_msg.par1,
    "', technology '", mask_msg.par2, "'");
    close_down1f(non_err, mti.prefix, NON_ERR);
    }
} /* End source_tech_check */

/* Create new version files */
void version_files_create(char *files[], char *dir)
{
    int cnt;
     FILE *fp;
     char file_name[FILE_SIZE];
     char *file_ptr;

    /* Build path prefix for following loop */
    file_ptr = strcpy2(file_name, dir, SLASH) +file_name;
    /* Create the specified version files */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
    /* Build complete name */
    strcpy(file_ptr, files[cnt]);
    if ((fp = fopen(file_name, "w")) == NULL)
    close_down1m(wr_open_err, file_name, WR_OPEN_ERR);
    fprintf(fp, "Version 1.000 %s %s %s\n",
    user.login, req_array[mask_msg.req_type], log_date);
    fclose(fp);
    } /* End for */
} /* end version_files_create */

/* Retrieve C4mask value assigned to mask set */
/* Give error if no value assigned */

/* Set a C4 mask set value */
void mask_c4set(void)
{
    int index;

    /* Make sure value can be set */
    index = state_file_read4update();
    /* Set the value */
    strcpy(st.version_type, mask_msg.par1);
    /* Write out value */
    state_file_write(index);
    /* Tell user transaction successful */
    put_short(SUCCESS);
    mti.suffix_ptr = " set.";
    strcpy2(mti.prefix, "C4 mask set for ", mti.suffix);
    put_ret_code(DBYES);
     /* Send updated state file to remote servers */
    mask_refresh(0, rmt_mpd.server_num, MARK);
} /* End mask_c4set */

/* Send list of all defined mask sets with specified criteria */
/* Used by Mask Designer */
void mask_list(int user_type, int time_type, int time_flag)
{
    long lower_time;
    long upper_time;
    int read_done;
     int fnd = DBNO;
    char *date_ptr;
    FILE *fp;
    MASK_ENTRY entry;
    char line[260];
    char date[12];
    int len = 0;

    /* Open mask set file */
    if ((fp = fopen(GLOBAL_MASK_FILE, "r")) == NULL)
    close_down1m(rd_open_err, GLOBAL_MASK_FILE, RD_OPEN_ERR);
    /* convert times from string to long format if needed */
    if (time_type != M2NONE)
    {
    lower_time = time_convert(mask_msg.key1);
    if (time_flag == RANGE) upper_time = time_convert(mask_msg.key2);
    else if (time_flag == ON) upper_time = lower_time+SECPERDAY;
    else upper_time = 0;
    } /* end if for time conversion needed */
    /* Indicate ready to send data */
    put_short(SUCCESS);
    /* Send the file name */
    put_binary_string("mask.sets", 9);
    /* Get ack */
    get_short();
    /* Read in mask set data in a while loop */
    while (line_get(fp, line) > 0)
    {
    /* Parse line into individual fields */
    sscanf(line, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%lu",
    entry.mask, entry.wl, entry.tech, entry.flow, entry.design,
    entry.creator, entry.owner, entry.site, entry.arc_status, &entry.time);
    /* Determine if this entry should be sent */
    /* See if this mask is valid */
    if (mask_valid(entry.mask) == DBNO) continue;
    /* See if this mask was created from specified fab */
    if (strcmp(mask_msg.par1, NONE) != 0) { if (strcmp(mask_msg.par1, entry.wl) != 0) continue; }
    /* Seeif created from specified technology */
    if (strcmp(mask_msg.par2, NONE) != 0) { if (strcmp(mask_msg.par2, entry.tech) != 0) continue; }
    /* See if created from specified flow */
    if (strcmp(mask_msg.par3, NONE) != 0) { if (strcmp(mask_msg.par3, entry.flow) != 0) continue; }
    /* See if created from specified design */
    if (strcmp(mask_msg.par4, NONE) != 0) { if (strcmp(mask_msg.par4, entry.design) != 0) continue; }
    /* Indicate state file not yet read */
    read_done = DBNO;
    /* Read state file if needed */
    /* If checking creator, owner, or last user */
    if (user_type != M2NONE)
    {
        if (user_type == M2MODIFY)
        {
        state_file_list(entry.mask);
        read_done = DBYES;
        }
        else read_done = DBNO;
        if (user_check(user_type, mask_msg.par5, entry.creator, entry.owner) != SUCCESS) continue;
    } /* End if for checking some type of user */
    if (time_type == M2MODIFY)
    {
        if (read_done == DBNO) state_file_list(entry.mask);
        if (time_calc(time_flag, lower_time, upper_time, st.upd_time) != SUCCESS) continue;
    } /* End if for checking modify */
    else if (time_type == M2CREATE)
    if (time_calc(time_flag, lower_time, upper_time, entry.time) != SUCCESS) continue;
    /* If reach this point, mask meets specified criteria */
    fnd = DBYES;
    /* Build the string to write out to the mask set file */
    /* Must convert the date from a digital to a character format */
    date_ptr = ctime(&entry.time);
    /* Pull out only the month, day and year */
    /* First pull out the month and day */
    strncpy(date, date_ptr+4, 7);
    /* Now pull out the year */
    strncpy(date+7, date_ptr+20, 4);
    date[11] = '\0';
    str_pad(entry.mask, 5);
    str_pad(date, 12);
    str_pad(entry.creator, 8);
    str_pad(entry.site, 6);
    /* Output data from mask file */
    len += strcpy6(bf.buffer+len, entry.mask,  entry.creator, date,
    entry.site, entry.arc_status, "\n");
    if (len > BUFFER_SIZE-260)
    {
    put_binary_string(bf.buffer, len);
        len = 0;
    } /* End if for ready to send data */
    } /* end while */
    /* Send the last part of the data */
    if (len > 0) put_binary_string(bf.buffer, len);
    /* Close the mask file */
    fclose(fp);
    /* Indicate all data sent */
    put_short(SUCCESS);
    /* Send client return code */
    if (fnd == DBYES)
    {
    mti.suffix_ptr = RTV;
    strcpy(mti.prefix, "List of mask sets");
    put_ret_code(DBNO);
    }
    else return_warning(mask_wrn, MASK_WRN, DBNO);
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* end mask_list */
/* Send list of all defined mask sets */
/* Used by Auto Stepper Download */
void mask_listall(void)
{
    char *date_ptr;
    FILE *fp;
    MASK_ENTRY entry;
    char line[260];
    char date[12];
    int len = 0;

    /* Open mask set file */
    if ((fp = fopen(GLOBAL_MASK_FILE, "r")) == NULL)
    close_down1m(rd_open_err, GLOBAL_MASK_FILE, RD_OPEN_ERR);
    /* Indicate ready to send data */
    put_short(SUCCESS);
    /* Send the file name */
    put_binary_string("mask.sets", 9);
    /* Get ack */
    get_short();
    /* Read in mask set data in a while loop */
    while (line_get(fp, line) > 0)
    {
    /* Parse line into individual fields */
    sscanf(line, "%s %s %s %s %s %s %s %s %s %lu",
    entry.mask, entry.wl, entry.tech, entry.flow, entry.design,
    entry.creator, entry.owner, entry.site, entry.arc_status, &entry.time);
    /* Build the string to write out to the mask set file */
    /* Must convert the date from a digital to a character format */
    date_ptr = ctime(&entry.time);
    /* Pull out only the month, day and year */
    /* First pull out the month and day */
    strncpy(date, date_ptr+4, 7);
    /* Now pull out the year */
    strncpy(date+7, date_ptr+20, 4);
    date[11] = '\0';
    /* Output data from mask file */
    len += strcpy8(bf.buffer+len, entry.mask, " ", entry.wl, " ",
    entry.tech, " ", entry.flow, " ");
     len += strcpy8(bf.buffer+len, entry.design, " ", entry.owner, " ",
    date, " ", entry.arc_status, "\n");
    if (len > BUFFER_SIZE-260)
    {
    put_binary_string(bf.buffer, len);
        len = 0;
    } /* End if for ready to send data */
    } /* end while */
    /* Send the last part of the data */
    if (len > 0) put_binary_string(bf.buffer, len);
    /* Close the mask file */
    fclose(fp);
    /* Indicate all data sent */
    put_short(SUCCESS);
    /* Send client return code */
    mti.suffix_ptr = RTV;
    strcpy(mti.prefix, "List of mask sets");
    put_ret_code(DBNO);
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* end mask_listall */

/* build script to create prior version of file */
int tmp_script_file_create(char *script_file, char *delta_file)
{
    int ret;
    char fnd = DBNO;
    FILE *fi;
    FILE *fo;
    char line[260];

    if ((fi = fopen(delta_file, "r")) == NULL)
    close_down1m(rd_open_err, delta_file, RD_OPEN_ERR);
    /* Open the script file */
    if ((fo = fopen(script_file, "w")) == NULL)
    close_down1m(wr_open_err, script_file, WR_OPEN_ERR);
    /* Scan through delta file getting diff commands */
    while (line_get(fi, line) >= 0)
    {
    if ((ret = strncmp(line, "Version ", 8)) == 0)
    {
        /* See if this is the version looking for */
        if (strncmp(line+8, mask_msg.par1, 3) == 0)	    {
        fnd = DBYES;
        break;
        } /* End if for match in version */
        /* If version less than one looking for, indicate version not found */
        else if (ret < 0) break;
    } /* end if for version line found */
    else fprintf(fo, "%s\n", line);
    } /* end while */
    /* End diff commands with write command */
    fprintf(fo, "w\n");
    /* Close files through working with */
    fclose(fi);
    fclose(fo);
    if (fnd == DBYES) return(SUCCESS);
    else return(FAILURE);
} /* End script_file_create */

/* Retrieve version information about mask set */
/* Sends version number, month, day, and year version created in an array */
void mask_versions(void)
{
    char wrn_flag = SUCCESS;
    FILE *fp;
    int cnt = 0;
    char *list[1000];
    char line[260];
    struct stat buf;

    /* Verify the mask set is present */
    state_file_read();
    /* Look for autosaved version */
    if (stat(mti.tmpdb_data_dir, &buf) == 0)
    {
    list[cnt] = malloc(50);
    sprintf(list[cnt++], "Autosave %s", ctime(&(buf.st_mtime)));
    /* Get rid of trailing carriage return */
    list[0][strlen(list[0])-1] = '\0';
    } /* end if for autosave present */
    /* build name of delta create file to read */
    strcat(mti.db_delta_dir, "/create");
    /* Open this file */
    if ((fp = fopen(mti.db_delta_dir, "r")) == NULL)
    close_down1m(rd_open_err, mti.db_delta_dir, RD_OPEN_ERR);
    /* Read in lines until reach end of file */
    while (line_get(fp, line) >= 0)
    {
    /* See if this is a version line */
    if (strncmp(line, "Version ", 8) == 0)
    {
        list[cnt] = malloc(strlen(line) - 7);
    strcpy(list[cnt], line+8);
        ++cnt;
        if (cnt >=999)
        {
        wrn_flag = M2WRN;
        break;
        } /* End if for array boundary exceeded */
    } /* End if for version string found */
    } /* End for */
    /* Point last pointer at NULL */
    list[cnt] = NULL;
    /* Close file no longer needed */
    fclose(fp);
    /* If at least one version found, tell user successful */
    if (cnt > 0)
    {
    /* Indicate ready to send data */
    put_short(SUCCESS);
    /* Send the list of versions found */
    list_get(list);
    /* Decide whether sending success or warning */
    if (wrn_flag == SUCCESS)
    {
        mti.suffix_ptr = RTV;
        put_ret_code(DBYES);
    } /* End no warnings */
    else return_warning(version_limit_wrn, VERSION_LIMIT_WRN, DBYES);
    }
    else close_down1f(no_version_history, mti.suffix, NO_VERSION_HISTORY);
} /* end mask_versions */

/* Send version history, parent fab, tech, owner, etc. to user */
void mask_history(void)
{
    FILE *fp;
    int cnt = 0;
    char *list[1000];
    char line[260];

    /* Verify the mask set is present */
    state_file_read();
    /* build name of delta create file to read */
    strcat(mti.db_delta_dir, "/create");
    /* Open this file */
    if ((fp = fopen(mti.db_delta_dir, "r")) == NULL)
    close_down1m(rd_open_err, mti.db_delta_dir, RD_OPEN_ERR);
    /* Read in lines until reach end of file */
    while (line_get(fp, line) >= 0)
    {
    /* See if this is a version line */
    if (strncmp(line, "Version ", 8) == 0)
    {
        list[cnt] = malloc(strlen(line)-7);
        strcpy(list[cnt], line+8);
        ++cnt;
    } /* End if for version string found */
    } /* End while */
    /* Point last pointer at NULL */
    list[cnt] = NULL;
    /* Close file no longer needed */
    fclose(fp);
    /* If at least one version found, tell user successful */
    if (cnt > 0)
    {
    /* Build additional information needed */
    create_info_read();
    /* Indicate ready to send data */
    put_short(SUCCESS);
    /* Send the list of versions found */
    list_get(list);
    return_success(rtn.msg, DBYES);
    }
    else close_down1f(no_version_history, mti.suffix, NO_VERSION_HISTORY);
} /* end mask_history */

/* Send global data and fab data to client */
void global_files_send(char *data_dir)
{
     char *files[M2TOTAL_FILES];

    if (mask_msg.data_type == MASK_SET)
    {
    char create_file[64];
    strcpy2(create_file, data_dir, "/.db/create");
    create_read(create_file);
    strcpy4(mti.tmp_data_dir, FAB_DATA, SLASH, mask_msg.par1, FAB_DIR);
     }
    /* Build name for fab associated with template file */
    else strcpy4(mti.tmp_data_dir, FAB_DATA, SLASH, mask_msg.key1, FAB_DIR);
    /* Indicate ready to perform transaction */
    put_short(SUCCESS);
    /* Make a list of global files */
    dir_list(OFF_GLOBAL_DATA, files);
    /* Send the global files */
    files_send(OFF_GLOBAL_DATA, files);
    /* Send the fab files */
    /* If this is mask data, must read in the parent fab */
    if (access(mti.tmp_data_dir,0) == 0)
    {
    dir_list(mti.tmp_data_dir, files);
    files_send(mti.tmp_data_dir, files);
    }
    else put_short(SUCCESS);
} /* End global_files_send */

/* See if user is creator, owner, or last user for mask */
int user_check(M2TIME user_type, char *user, char *creator, char *owner)
{

    switch(user_type)
    {
    case M2MODIFY:
    {
        if (strcmp(st.user_id, user) == 0) return(SUCCESS);
        else return(FAILURE);
    }
    break;
    case M2OWN:
    {
        if (strcmp(owner, user) == 0) return(SUCCESS);
        else return(FAILURE);
    }
    break;
    case M2CREATE:
    {
        if (strcmp(creator, user) == 0) return(SUCCESS);
        else return(FAILURE);
    }
    break;
    /* Get status of mask set for Mask Designer */
    case M2STATUS: mask_status(); break;
    default: return(SUCCESS);
     } /* End switch */
    return(SUCCESS);
} /* End user_check */

/* See if mask set already present in Mask Designer */
void mask_status(void)
{
    if (access(xx, 0) != 0)
    {
    put_short(SUCCESS);
    sprintf(rtn.msg, "%s not present in the Mask Designer database.", mti.prefix);
    return_success(rtn.msg, DBNO);
     }
    else close_down1f(crt2err, mti.prefix, CRT2ERR);
} /* End mask_status */


void mask_send_delete(int start_index, int end_index)
{
    register short cnt;
    short ret;

    /* Build header to send to remote server */
    strcpy(mask_msg.key3, st.master_site);
     mask_msg.transaction_time = st.transaction_num;
    bld_header(bf.header_buffer, RMT_DELETE, mask_msg.data_type, st.transaction_num);
    /* Update each server in a for loop */
    for (cnt = start_index; cnt < end_index; ++cnt)
    {
    /* Connect to remote server */
    /* Send the header */
    /* Get back the response */
    ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
    /* Get the return code */
    get_rmt_ret_code(DBYES);
    } /* End for */
} /* End mask_send_delete */


void rmt_mask_refresh(void)
{
    int ret;

     /* Verify the mask data is present */
    ret = state_file_read4owner();
    if (ret == NON_ERR)
    {
    rmt_create_data();
    /* Add entry for mask set */
    if (mask_msg.data_type == MASK_SET) rmt_mask_entry_add(mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4);
    } /* End if */
    /* If not owner, return, message to rmt server already sent */
    else if ((mask_msg.req_type == MARK)  && (mask_msg.transaction_time == (st.transaction_num+1)))
    rmt_mark_data();
    else rmt_refresh_data();
} /* End rmt_mask_refresh */

void rmt_mask_delete(void)
{

    /* Read owning database from state file */
    /* Do not allow delete if requesting database does not own data */
    /* If requesting server does not own data, abort transaction */
    /* Following function sends error message to remote server */
    state_file_read4owner();
    /* Delete data and delta directories */
    dir_recursive_dlt(mti.data_dir);
    dir_recursive_dlt(mti.delta_dir);
    /* Delete entry from mask table */
    if (mask_msg.data_type == MASK_SET)mask_entry_dlt();
    /* Tell remote server transaction successful */
    put_short(SUCCESS);
    strcpy3(rtn.msg, mti.prefix, " deleted on ", ip.master_site);
    return_success(rtn.msg, DBNO);
} /* End rmt_mask_delete */

/* Shadow mask set and template to remote data base */
void mask_shadow(int server_num)
{
    int refresh_cnt[5] = { 0, 0, 0, 0, 0 };
    int tmp_sock = sock;
    int ret;

    /* Verify this node owns data */
    state_file_read4shadow();
    /* Put master site in key 3 */
    strcpy(mask_msg.key3, ip.master_site);
    /* Perform an audit on just this piece of data */
    ret = audit_data(refresh_cnt, server_num, server_num+1);
    /* Build message to send to client */
    /* Now send the return code */
    sock = tmp_sock;
    if (ret < 0) return_err(rtn.msg, ret);
    else
    {
    put_short(ret);
    return_success(rtn.msg, DBYES);
    } /* End if */
    log3("Closing transaction ", tr.pid_string, ".");
    exit(0);
} /* End mask_shadow */

/* Initialize mask paths needed to perform mask transactions */
void mask_paths_init(void)
{
    strcpy3(mti.data_dir, MASK_DATA, SLASH, mask_msg.key1);
    strcpy4(mti.db_data_dir, MASK_DATA, SLASH, mask_msg.key1, "/.db");
    strcpy4(mti.tmpdb_data_dir, MASK_DATA, SLASH, mask_msg.key1, "/.tmpdb");
    strcpy3(mti.delta_dir, MASK_DELTA, SLASH, mask_msg.key1);
    strcpy4(mti.db_delta_dir, MASK_DELTA, SLASH, mask_msg.key1, "/.db");
    strcpy3(mti.prefix, "Mask set '", mask_msg.key1, "'");
    strcpy3(mti.suffix, "mask set '", mask_msg.key1, "'");
    /* Build the state file name */
    strcpy2(mti.state_file, mti.delta_dir, "/state_file");
} /* End mask_paths_init */
void mask_list_members(void)
{
    char *user_list[20];
    char usergroup_list[20];
    char name[128];
    char first_name[32];
    char last_name[32];

    /* Read in data from state file */
    state_file_read();
    /* Indicate ready to send data */
    put_short(SUCCESS);
    /* Convert usergroups from string format to array format */
    if ((strncmp(st.usergroup, "None", 4) != 0)
    && (strncmp(st.usergroup, NONE, 1) != 0))
    {
    string2list_convert(st.usergroup, usergroup_list);
    /* Send the usergroup list */
    list_get(usergroup_list);
    } /* End if for usergroup present */
    else put_short(SUCCESS);
    if ((strncmp(st.users, "None", 4) != 0)
    && (strncmp(st.users, NONE, 1) != 0))
    {
    /* Convert users from string format to array_format */
    string2list_convert(st.users, user_list);
    /* Expand user list */
    expand_coreid_list_to_name_and_coreid(user_list);
    /* Send the user list */
    list_get(user_list);
     } /* End if for users present */
    else put_short(SUCCESS);
    /* find the owner's name */
       if (ldapfsl_get_user_full_name(st.own_id, name) != DBYES)
       {
           strcpy(name, "NXP User");
       }
       /* Scan in first and last name */
       sscanf(name, "%s %s", first_name, last_name);
    /* Take under scores out of first name */
    strexc(first_name, '_', ' ');
       sprintf(name, "%s, %s (%s)", last_name, first_name, st.own_id);
    /* Send owner name */
    put_string(name);
    /* Send return code */
    return_success("List of user groups retrieved", DBNO);
} /* End mask_list_members */

void mask_usergroup_delete(char *mask, char *usergroup, char *suffix)
{
    char file_name[256];
    char *list[10];
    int cnt;

    /* If no usergroup assigned, simply return */
    if (strncmp(usergroup, "None", 4) == 0) return;
    /* convert usergroup to list format */
    string2list_convert(usergroup, list);
    /* Go through the list deleting each entry */
    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
    /* Build name of usergroup list file */
    strcpy4(file_name, USER_DELTA, "/", list[cnt], suffix);
    /* Delete the entry from the user group file */
    if (access(file_name, 0) == 0)
    entry_delete(file_name, mask);
    /* Free memory no longer needed */
    free(list[cnt]);
    } /* End for */
} /* End mask_usergroup_delete */

void mask_usergroup_add(char *mask, char *usergroup, char *suffix)
{
    char file_name[256];
    char *list[10];
    int cnt;

    /* If no usergroup assigned, simply return */
    if (strncmp(usergroup, "None", 4) == 0) return;
    /* convert usergroup to list format */
    string2list_convert(usergroup, list);
    /* Go through the list deleting each entry */
    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
    /* build name of user group file */
    strcpy3(file_name, USER_DELTA, "/", list[cnt]);
    if (access(file_name, 0) != 0)
    {
        rtn.len += sprintf(rtn.msg+rtn.len, usergroup_add_wrn, list[cnt], mti.suffix);
    } else {
        strcat(file_name, suffix);
        /* Add the entry to the user group file */
        entry_add(file_name, mask, mask);
    } /* End else */
    /* Free memory no longer needed */
    free(list[cnt]);
    } /* End for */
} /* End mask_usergroup_add */

/* Check for valid mask */
int mask_valid(char *mask)
{
    int fd;
    char mars_mask[4];
    char upper_bound;

    if (mask_msg.program == ALL_MASKS) return(DBYES);
    /* Get upper bound from mask file */
    /* Open the file where the mask set name is kept */
    if ((fd = open(MASKSET_TABLE, O_RDONLY)) != EOF)
    {
    /* Read in the data from the file */
    if (read(fd, mars_mask, 4) < 4)
    upper_bound = 'z';
    else upper_bound = mars_mask[0];
    close(fd);
    } /* End if for open */
    /* Otherwise Use default character */
    else upper_bound = 'z';
    if ((mask[0] >= 'a') && (mask[0] <= upper_bound)
    && (mask[1] >= '0') && (mask[1] <= '9')
    && (mask[2] >= '0') && (mask[2] <= '9')
    && (mask[3] >= 'a') && (mask[3] <= 'z'))
    return(DBYES);
    else return(DBNO);
} /* End mask_valid */

/* See if parent template is newer than mask set */
void parent_template_check(long upd_time)
{
    long parent_upd_time;
    char create_file[64];

    /* Read in parent fab and flow */
    /* build create file name */
    strcpy2(create_file, mti.db_data_dir, "/create");
    create_read(create_file);
    /* build state file name for parent template */
    strcpy6(mti.state_file, FAB_DELTA, "/", mask_msg.par1, "/", mask_msg.par2, "/state_file");
    /* Read  state file */
    if ((parent_upd_time = template_state_file_read()) >= SUCCESS)
    {
    /* Give warning if official newer than mask */
    if (parent_upd_time > upd_time)
    {
    rtn.len += sprintf(rtn.msg+rtn.len, template_newer_wrn, mask_msg.key1, mask_msg.par1, mask_msg.par2);
    } /* End if for template newer */
    /* If reached this point, ready to return */
    return;
    } /* End if for able to read  state file */
    /* Otherwise indicate no official template present */
/* may need later
    else sprintf(rtn.msg, no_off_template_wrn, mask_msg.key1, mask_msg.par1, mask_msg.par2);
        rtn.len += sprintf(rtn.msg+rtn.len, unofficial_newer_wrn, mask_msg.key1, mask_msg.par1, mask_msg.par2);
*/
    /* If reached this point, template not present */
    rtn.len += sprintf(rtn.msg+rtn.len, no_template_data, mask_msg.key1, mask_msg.par1, mask_msg.par2);
} /* End parent _template_check */
void mask_view_users(void)
{
    state_file_read();
    put_short(SUCCESS);
    /* Put the data in the return message */
    if (strcmp(st.users, NONE) == 0)
    strcpy3(rtn.msg, "No users present for ", mti.suffix, ".");
    else strcpy4(rtn.msg, "Users for ", mti.suffix, " include:  ", st.users);
    return_success(rtn.msg, DBYES);
} /* mask_view_users */

void mask_view_usergroups(void)
{
    state_file_read();
    put_short(SUCCESS);
    /* Put the data in the return message */
    if (strcmp(st.usergroup, NONE) == 0)
    strcpy3(rtn.msg, "No usergroups present for ", mti.suffix, ".");
    else strcpy4(rtn.msg, "Usergroups for ", mti.suffix, " include:  ", st.usergroup);
    return_success(rtn.msg, DBYES);
} /* view_usergroups */

void mask_view_cwd(void)
{
    state_file_read();
    put_short(SUCCESS);
    /* Put the data in the return message */
    strcpy4(rtn.msg, "CWD for ", mti.suffix, ":", st.cwd);
    return_success(rtn.msg, DBYES);
} /* mask_view_cwd */


/* Removes dupliate names from being in a subdirectory */
void mask_duplicates_remove(char *files[], int file_num)
{
    int cnt;

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
    if (strchr(files[cnt], '/') != NULL)
    {
        /* Free duplicate entry */
        free(files[cnt]);
        /* Move last entry to vacant position */
    --file_num;
    files[cnt] = files[file_num];
        files[file_num] = NULL;
    }
    } /* End for */
}

void mask_transaction_log_rtv(void)
{

    /* Verify the data is present */
    state_file_read();
   /* Tell client ready to send data */
    put_short(SUCCESS);
    /* Now send the file */
    strcat(key, ".data_log");

    /* If this is PC data, change '/' to ':' */
    if (mask_msg.data_type == PC) strexc(key, '/', ':');
    file_get_alternate(mti.delta_dir, "data_log", key);
    /* Send return code */
    mti.suffix_ptr = RTV;
    strcpy2(mti.prefix, "Transaction log for ", mti.suffix);
    put_ret_code(DBYES);
} /* end mask_transaction_log_rtv */


/* Refresh two directory trees or two groups of files */
void mask_refresh(int start_index, int end_index, int req_type)
{
    extern TMP_BUFFER bf;
    short ret;
    int cnt;

    /* Build header to send to remote server */
    strcpy(mask_msg.key3, ip.master_site);
     mask_msg.transaction_time = st.transaction_num;
    bld_header(bf.header_buffer, req_type, mask_msg.data_type, st.transaction_num);
    /* Update each server in a for loop */
    for (cnt = start_index; cnt < end_index; ++cnt)
    {
    /* Connect to remote server */
    /* Send the header */
    /* Get back the response */
    ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
    if ((ret == REFRESH) || (ret == CREATE)) refresh_data();
    else if (ret == MARK)
    {
        /* Send the state file */
        if (mask_msg.data_type != GLOBAL)
        file_put(mti.delta_dir, "state_file");
        else
        {
        char state_file[64];
        strcpy2(state_file, mask_msg.key1, ".state");
        file_put(GLOBAL_DELTA, state_file);
        } /* End else for global data */
        /* Get the return code */
        ret = get_rmt_ret_code(DBYES);
    } /* End mark */
    else if (ret < 0)
    {
        data_log(tr.log_file, rtn.msg);
        /* Put entry in resend file */
        resend_entry_add(rmt_mpd.site[cnt], cnt);
    }
    } /* End for */
} /* End mask_refresh */

/* Check for create file on .db file list */
/* Also count the number of .db files retrieved */
/* If not present add it */
/* The create file must be present for all versions of the data */
void mask_create_check(char *new_data_dir, char *db_files[])
{
    int cnt;
    char new_file[FILE_SIZE];
    char old_file[FILE_SIZE];

    if (list_find(db_files, CREATE_FILE) == SUCCESS)
    return;
    for (cnt = 0; db_files[cnt] != NULL; ++cnt);
    /* Will now be pointing to the end of the list */
    db_files[cnt] = malloc(7);
    strcpy(db_files[cnt++], CREATE_FILE);
    db_files[cnt] = NULL;
    /* Now make a copy of the create file in the new data directory */
    strcpy3(new_file, new_data_dir, "/", CREATE_FILE);
    strcpy3(old_file, mti.data_dir, "/.db/", CREATE_FILE);
    file_copy(old_file, new_file);
} /* end mask_create_check */

/* Convert the .db files */
int mask_db_dir_convert(char *data_dir, char *db_data_dir, char *files[])
{
    char *tmp_ptr;
    char *data_ptr;
    char data_file[FILE_SIZE];
    char tmp_file[FILE_SIZE];
    int cnt;
    int library_version;

    /* No longer using Allan's convert routine */
    return(DBNO);
    /* See if files need to be converted */
    /* Calculate the library version passed in */
    if (mask_msg.num == 0) mask_msg.num = 1;
    library_version = mask_library_version_rtv(mti.db_data_dir);
     if (library_version == mask_msg.num) return(DBNO);
    else
    {
    /* Make a copy of the files in a temp directory */
    /* build name for temporary directory */
    strcpy7(data_dir, "/tmp/", key, "_", user.login, "_", tr.pid_string, "suffix");
    /* Create this directory */
    dir_create(data_dir);
    strcpy2(db_data_dir, data_dir, "/.db");
    dir_create(db_data_dir);
    data_ptr = data_file + strcpy2(data_file, mti.db_data_dir, "/");
    tmp_ptr = tmp_file + strcpy2(tmp_file, db_data_dir, "/");
    /* Already have a list of files to version */
    /* Go through list copying the file */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
    /* Copy data file into temporary directory */
    strcpy(tmp_ptr, files[cnt]);
    strcpy(data_ptr, files[cnt]);
    file_copy(data_file, tmp_file);
    } /* end inner for */
    /* Perform the conversion */
    mask_convert(data_dir, db_data_dir, files);
    return(DBYES);
    } /* End else */
} /* End mask_db_dir_convert */

void mask_convert(char *data_dir, char *db_data_dir, char *files[])
{
    extern char bin_dir[];
    char output_file[FILE_SIZE];
    char status;
    char home_dir[FILE_SIZE];
    char command[260];

    /* Call convert routine to do the conversion */
    /* Must change to directory where files are */
    /* Get current directory before changing directory */
    getwd(home_dir);
    chdir(data_dir);
    /* Build the command to execute */
    sprintf(command, "%s/convert %d >%s.convert.output 2>&1 ", bin_dir, mask_msg.num, key);
    status = WEXITSTATUS(system(command));
    /* Change back to the directory where previously running */
    chdir(home_dir);
    data_log(tr.log_file, command);
    if (status != 0)
    {
    if ((status == -12) || (status == -23))
    {
        rtn.len += sprintf(rtn.msg+rtn.len, convert_wrn, status);
    } /* End for warning present */
    /* At this point know found error */
    else
    {
        /* for override off */
    if (mask_msg.override_flag == DBNO)
    {
    sprintf(rtn.msg, convert_ovr, status);
    return_override(rtn.msg, CONVERT_OVR);
    } /* End if for override flag off */
        else rtn.len += sprintf(rtn.msg+rtn.len, convert_err, mti.prefix);
    } /* End else for error */
    } /* end if for status not equal to 0 */
    /* If status equal to 0, delete convert output */
    else
    {
    strcpy2(output_file, db_data_dir, "/convert.output");
    unlink(output_file);
    } /* End else for return of 0 */
    /* Free original list of files no longer needed */
/*
    files_free(files);
*/
    /* this is needed because convert routine might add or delete files */
    /* Make list of converted files */
    dir_list(db_data_dir, files);
} /* end mask_convert */

void mask_db_dir_send(char *data_dir, char *db_data_dir, int convert, char *files[])
{
    /* Tell client ready to send the .db directory */
    put_string(".db");
    if (convert == DBYES)
    {
    files_send(db_data_dir, files);
    /* Remove the directories no longer needed */
    dir_recursive_dlt(data_dir);
    } /* End if */
     else files_send(mti.db_data_dir, files);
} /* End mask_db_dir_send */

/* Retrieve the library version number */
int mask_library_version_rtv(char *dir)
{
    char library_file[FILE_SIZE];
    int version = 1;
    FILE *fp;

    /* Build name of library file */
    strcpy2(library_file, dir, "/.dbversion");
    if ((fp = fopen(library_file, "r")) == NULL) return(1);
    fscanf(fp, "%d", &version);
    fclose(fp);
    return(version);
} /* End mask_library_version_rtv */


/* Delete the .tmpdb directory */
void mask_tmpdb_dlt(void)
{
    if (mask_msg.data_type != MASK_SET) return;
    if (access(mti.tmpdb_data_dir, 0) != 0) return;
    dir_delete(mti.tmpdb_data_dir);
}
int mask_db_files_cnt(char *db_files[], int total_cnt)
{
    int cnt;
    int cnt1;

    if (total_cnt < MIN_MASK_FILES) return(total_cnt);
    for (cnt = 0, cnt1 = 0; db_files[cnt] != NULL; ++cnt)
    {
    if (db_files[cnt][0] != '.') ++cnt1;
    }
    return(cnt1);
}

int mask_use_autosave(void)
{
    char *tmp_files[M2TOTAL_FILES];

    /* If not a mask set, autosave not present */
    if (mask_msg.data_type != MASK_SET) return(DBNO);
    /* Check for presence of autosave dir */
    /* If not present cannot use */
else if (access(mti.tmpdb_data_dir, 0) != 0)
    return(DBNO);
    else if(mask_files_list(mti.tmpdb_data_dir, tmp_files) < MIN_MASK_FILES)
    {
    /* Delete the invalid directory */
    dir_delete(mti.tmpdb_data_dir);
    return(DBNO);
    }
    else return(DBYES);
}

int new_mask_use_autosave(void)
{
    /* If not a mask set, autosave not present */
    if (mask_msg.data_type != MASK_SET) return(DBNO);
    /* Check for presence of autosave dir */
    /* If not present cannot use */
if (access(mti.tmpdb_data_dir, 0) != 0)
    {
    if ((mask_msg.override_flag == DBYES) && (strcmp(mask_msg.par2, "1") == 0))
    {
    sprintf(rtn.msg, autosave_not_present, mti.prefix);
        return_err(rtn.msg, AUTOSAVE_NOT_PRESENT);
    } /* end if for requesting data not present */
    else return(DBNO);
    }
    /* At this point know it is present */
    if (mask_msg.override_flag == DBNO)
    return_override(autosave_present_ovr, AUTOSAVE_PRESENT);
    else if (strcmp(mask_msg.par2, "1") == 0)
    return(DBYES);
    else if (strcmp(mask_msg.par2, "2") == 0)
    return(DBNO);
    else return_override(autosave_present_ovr, AUTOSAVE_PRESENT);
} /* end mask_use_autosave */

void process_mask(void)
{

        strcpy(key, mask_msg.key1);
    /* Initialize paths needed for following transactions */
    mask_paths_init();
    switch(mask_msg.req_type)
{
    case TRANSACTION_LOG_RTV: mask_transaction_log_rtv(); break;
    /* Create a new mask set from an existing template */
    case REFRESH: rmt_mask_refresh(); break;
    case MARK: rmt_mask_refresh(); break;
    case CREATE:
    {
    pin_check();
        mask_create();
    }
    break;
    /* Create a new mask set from an existing mask set */
    case COPY:
    {
    pin_check();
         mask_copy();
    }
    break;
    case READ:
    case READ_ANY:
    {
        if (strcmp(mask_msg.par1, NONE) == 0)
        mask_read();
        else if (strcmp(mask_msg.par1, "Autosave") == 0)
        {
        strcpy(mask_msg.par1, NONE);
        strcpy(mask_msg.par2, "1");
        mask_msg.override_flag = DBYES;
        mask_read();
        } /* End if for requesting autosave */
        else mask_read_prior();
    }
    break;
    /* Check out mask set data */
    case CHECKOUT:
    {
    pin_check();
        /* If mask set not present, try reading from Mask Prep */
        if (access(mti.state_file, 0) != 0) maskprep_read();
        else if (strcmp(mask_msg.par1, NONE) == 0)
        mask_checkout();
        else if (strcmp(mask_msg.par1, "Autosave") == 0)
        {
        strcpy(mask_msg.par1, NONE);
        strcpy(mask_msg.par2, "1");
        mask_msg.override_flag = DBYES;
        mask_checkout();
        } /* End if for requesting autosave */
        else mask_read_prior();
    }
    break;
    /* Retrieve check out status */
    case CHECKOUT_STATUS_RTV: mask_checkout_status_rtv(); break;
    case COMMON_LAYERS_UPDATE:  mask_common_layers_update(); break;
    /* Cancel check-out on specified mask set */
    case CANCHECKOUT:
    {
    pin_check();
        mask_cancheckout();
        /* refresh done separately because cancheckout used for several data types */
        /* Send updated state file to remote servers */
        mask_refresh(0, rmt_mpd.server_num, MARK);
    }
    break;
    /* Check in mask set files */
    case CHECKIN:
    {
    pin_check();
        mask_checkin();
    }
    break;
    /* Update mask set in database */
    /* Major revision made, and files not deleted from user's disk */
    case UPDATE:
    {
    pin_check();
        mask_update();
    }
    break;
    /* Perform check in with overwrite function */
    case OVERWRITE:
    {
    pin_check();
        mask_ovrwr();
    }
    break;
    /* Save .db files to database with no versioning */
    case SAVE:
    {
    pin_check();
         mask_save();
    }
    break;
    case APPEND:
    {
    pin_check();
         mask_append();
    }
    break;
    /* Modify the owner of a mask set */
    /* Must be the owner to perform this transaction */
    /* The user must supply X.500 user ID of new owner */
    case MOD_OWNER:
    {
    pin_check();
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
    /* Delete n existing mask set */
    case DELETE:
    {
    pin_check();
        mask_delete();
    }
    break;
    /* Transfer ownership of mask set to new database */
    case TRANSFER:
    {
    pin_check();
        mask_transfer();
         /* Send updated state file to remote servers */
        mask_refresh(0, rmt_mpd.server_num, MARK);
    }
    break;
    /* Force shadow of mask data to all database machines */
    case SHADOW: mask_shadow(mask_msg.num); break;
    case VERSIONS:
    {
        mask_versions();
    }
    break;
    case HISTORY:
    {
        mask_history();
    }
    break;
    /* Set c4mask value */
    case C4SET:
    {
    pin_check();
         mask_c4set();
    }
    break;
    /* Get C4 mask for specified mask set */
    /* Unarchive a previously archived mask */
    case RESTORE1MASK:
    {
    pin_check();
        mask_restore(mask_msg.key1);
    }
    break;
    case M2LIST:
    {
        mask_list(mask_msg.override_flag, atoi(mask_msg.key3), mask_msg.num);
    }
    break;
    case LISTALL:
    {
        mask_listall();
    }
    break;
    case LIST_MEMBERS: mask_list_members(); break;
    case RMT_DELETE: rmt_mask_delete(); break;
    case AUDIT:
    {
    long audit_time = audit_time_set();
        mask_audit(&audit_time);
    }
    break;
    /* Audit local mask table */
    case OFF_AUDIT:
    {
        mask_table_audit();
    }
    break;
    /* Called when remote server asks to audit a single mask set */
    case RMT_AUDIT:
    {
        rmt_audit_data();
    }
    break;
    /* Audit the state file remotely */
    case RMT_AUDIT_STATE:
    {
        rmt_audit_state();
    }
    break;
    case VIEW_USERS:  mask_view_users(); break;
    case VIEW_USERGROUPS:  mask_view_usergroups(); break;
    case VIEW_CWD:  mask_view_cwd(); break;
    default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_mask */




/* Expand the list */
void expand_coreid_list_to_name_and_coreid(char *list[])
{
    int cnt;
    char *ptr;
    char name[128];
    char first_name[64];
    char last_name[64];

    DBG("Function called.");

    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
          if (ldapfsl_get_user_full_name(list[cnt], name) == DBYES)
          {
            /* Scan in first and last name */
            sscanf(name, "%s %s", first_name, last_name);

            /* Take under scores out of first name */
            strexc(first_name, '_', ' ');

            ptr = malloc(strlen(list[cnt]) + strlen(name) +5);
            sprintf(ptr, "%s, %s (%s)", last_name, first_name, list[cnt]);

            /* Free original memory and put in new memory */
            free(list[cnt]);
            list[cnt] = ptr;
          }
    } /* End for */
} /* End expand_coreid_list_to_name_and_coreid */

