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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "dir.h"
#include "close_down.h"
#include "global_defines.h"
#include "mask_server_defines.h"
#include "m2files.h"
#include "m2mask_files.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "m2state_table.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
/*
#include "global_server_prototypes.h"
*/
#include "m2server_errs.h"
#include "m2server_paths.h"
#include "m2req_types.h"
#include "mti.h"
#include "pre_suffix.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "m2msg.h"

/* Retrieve prior version of mask set */
void mask_read_prior(void)
{
    extern char global_email[];
    char local_version[8];
    char *data_ptr;
    char *delta_ptr;
    char *dirlist[1000];
    int index;
    int cnt;
    char data_dir[FILE_SIZE];
    char db_dir[FILE_SIZE];
    char *files[M2TOTAL_FILES] = {0};
    char *db_files[M2TOTAL_FILES] = {0};


    /* Make sure mask set can be retrieved */
    if (mask_msg.req_type == CHECKOUT)
    {
        index = state_file_read4update();
    }
    else
    {
        state_file_read();
        /* Make sure transaction not in progress */
        index = state_file_lock_set(mti.state_file);
        lock_file_clear(index);
    }
     /* Create files in data directory */
    cnt = prior_files_create(mti.data_dir, mti.delta_dir, ".data_files",
    data_dir, files);
    cnt = prior_files_create(mti.db_data_dir, mti.db_delta_dir, ".data_files/.db",
    db_dir, db_files);
    if (cnt <= 0)
    shut_down("no version files present", -1);
    if (cnt < MIN_MASK_FILES)
    {
        rtn.len = sprintf(rtn.msg, not_enough_files, mti.prefix, cnt);
        email_send("mdsEvents@nxp.com", global_email,
                   "Error reading prior version", rtn.msg);
        strcpy2(rtn.msg+rtn.len, DOUBLE_RET, HELP_MSG);
        shut_down(rtn.msg, NOT_ENOUGH_FILES);
    }
    /* See if data needs to be converted */
    prior_mask_db_dir_convert(data_dir, db_dir, db_files);
    /* Send global and fab files to user */
    /* Save version to temp buffer */
    strcpy(local_version, mask_msg.par1);
    global_files_send(mti.data_dir);
    /* Restore version */
    strcpy(mask_msg.par1, local_version);
    /* Send the data files */
    files_send(data_dir, files);
    /* Send the .db files */
    put_string(".db");
    files_send(db_dir, db_files);
    /* Make a list of other directories for this version */
    subdir_prior_version_list(mti.delta_dir, dirlist, mask_msg.par1);
    /* Send the other directories if present */
    data_ptr = mti.data_dir + strlen(mti.data_dir);
    data_ptr += strcpy1(data_ptr, "/.dir/");
    delta_ptr = mti.delta_dir + strlen(mti.delta_dir);
    delta_ptr += strcpy1(delta_ptr, "/.dir/");
    for (cnt = 0; dirlist[cnt] != 0; ++cnt)
    {
        put_string(dirlist[cnt]);
        /* Build full directory path */
        strcpy2(data_ptr, "/", dirlist[cnt]);
        strcpy2(delta_ptr, "/", dirlist[cnt]);
        /* Create the files for the prior version */
        prior_files_create(mti.data_dir, mti.delta_dir, ".dir", data_dir, files);
        /* Send the files */
        files_send(data_dir, files);
    }
    /* Indicate all directories sent */
    put_short(SUCCESS);
    /* If this is a check out transaction, update the state file */
    /* Restore value for delta dir */
    delta_ptr[0] = '\0';
    if (mask_msg.req_type == CHECKOUT)
    {
        st.state = CHECKED_OUT;
        ++st.transaction_num;
        st.touch_time = tr.upd_time;
        strcpy(st.user_id, user.email);
        if (mask_msg.req_type == CHECKOUT)
            state_file_write(index);
    } /* end if for doing check out */
    /* Send the return code */
    if (rtn.len > 0)
    {
        return_warning(rtn.msg, CONVERT_WRN, DBYES);
    }
    else
    {
        mti.suffix_ptr = RTV;
        put_ret_code(DBYES);
    } /* End else */
    /* Delete temporary directories no longer needed */
    files_dlt(files);
    files_dlt(db_files);
    /* Delete directories no longer needed */
    if (rmdir(db_dir) != 0) dir_delete(db_dir);
    if (rmdir(data_dir) != 0) dir_delete(data_dir);
} /* End mask_read_prior */

/* Create version of files from prior release */
int prior_files_create(char *data_dir, char *delta_dir, char *suffix, char *tmp_dir, char *new_files[])
{
     char *files[M2TOTAL_FILES];
    char *delta_ptr;
    char *data_ptr;
    char *tmp_ptr;
    char delta_file[FILE_SIZE];
    char data_file[FILE_SIZE];
    char tmp_file[FILE_SIZE];
    char script_file[FILE_SIZE];
    char command[FILE_SIZE];
     int cnt;
    int new_cnt = 0;

    /* build name for temporary directory */
    strcpy7(tmp_dir, "/tmp/", key, "_", user.login, "_", tr.pid_string, suffix);
    /* Create this directory */
    dir_create(tmp_dir);
    /* build name for script file */
    strcpy2(script_file, tmp_dir, "/script_file");
    delta_ptr = delta_file + strcpy2(delta_file, delta_dir, "/");
    data_ptr = data_file + strcpy2(data_file, data_dir, "/");
    tmp_ptr = tmp_file + strcpy2(tmp_file, tmp_dir, "/");
    /* Make a list of files to version */
    mask_files_list(data_dir, files);
    /* Go through list creating prior version of each file */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
    /* Build complete delta file name */
    strcpy(delta_ptr, files[cnt]);
    /* Build script file to create prior version */
    /* If this is the rb_out.gdsii file, simply copy it */
    if (strcmp(files[cnt], "rb_out.gdsii") == 0)
    {
        strcpy(tmp_ptr, files[cnt]);
        strcpy(data_ptr, files[cnt]);
        file_copy(data_file, tmp_file);
        new_files[new_cnt++] = files[cnt];
        continue;
    } /* End if for rb_out.gdsii file */
    if (script_file_create(script_file, delta_file) == SUCCESS)
    {
    /* Copy data file into temporary directory */
        strcpy(tmp_ptr, files[cnt]);
        strcpy(data_ptr, files[cnt]);
        file_copy(data_file, tmp_file);
        /* build command */
    if (strcmp(files[cnt], "text_mark") == 0)
        strcpy4(command, "cat ", script_file, " | ed - ", tmp_file);
        else strcpy4(command, "cat ", script_file, " | ex - ", tmp_file);
        /* Execute the command */
        system(command);
        /* Copy file name to list to send */
        new_files[new_cnt++] = files[cnt];
    }
    /* If did not find this file, take it off the list */
    else free(files[cnt]);
    /* Delete script file no longer needed */
/*
    unlink(script_file);
*/
    } /* End for */
    /* Terminate list with NULL */
    new_files[new_cnt] = NULL;
    /* Terminate metadir list with NULL */
    return(new_cnt);
} /* End prior_files_create */

/* build script to create prior version of file */
int script_file_create(char *script_file, char *delta_file)
{
    int len = strlen(mask_msg.par1);
    int ret;
    char fnd = DBNO;
    FILE *fi;
    FILE *fo;
    char line[25001];

    if (access(delta_file, 0) != 0) return(FAILURE);
    if ((fi = fopen(delta_file, "r")) == NULL)
    close_down1m(rd_open_err, delta_file, RD_OPEN_ERR);
    /* Open the script file */
    if ((fo = fopen(script_file, "w")) == NULL)
    close_down1m(wr_open_err, script_file, WR_OPEN_ERR);
    /* Scan through delta file getting diff commands */
    while (line_get_fp(fi, line, 25000) >= 0)
    {
    if ((ret = strncmp(line, "Version ", 8)) == 0)
    {
        /* See if this is the version looking for */
        if (strncmp(line+8, mask_msg.par1, len) == 0)	    {
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
/* Convert the .db files */
void prior_mask_db_dir_convert(char *data_dir, char *db_data_dir, char *files[])
{
    int library_version;

    /* See if files need to be converted */
    /* Calculate the library version passed in */
    if (mask_msg.num == 0) mask_msg.num = 1;
    library_version = mask_library_version_rtv(db_data_dir);
     if (library_version != mask_msg.num)
    /* Call convert routine to do the conversion */
    mask_convert(data_dir, db_data_dir, files);
} /* End prior_mask_db_dir_convert */

void maskprep_read(void)
{
    FILE *fp;
    char data_dir[FILE_SIZE];
    char db_data_dir[FILE_SIZE];
    char command[260];
    char *files[M2TOTAL_FILES];
    char maskprep_file[FILE_SIZE];

    /* If this is a read and override not on, give override condition */
    if ((mask_msg.override_flag == DBNO)
    && (mask_msg.req_type == CHECKOUT))
    {
    sprintf(rtn.msg, maskprep_convert_ovr, mti.prefix);
    return_override(rtn.msg, MASKPREP_CONVERT_OVR);
    }
    /* Try to retrieve the mask set from the Mask Prep database */
    /* First check to see if it is present */
    strcpy5(maskprep_file, "maskprep_dlta/", key, "/", key, ".state");
    /* build directory to access */
    /* If directory not present give error */
    if ((fp = fopen(maskprep_file, "r")) == NULL)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Now read the mask set */
    /* build and execute the command */
    if (mask_msg.req_type == CHECKOUT)
    {
    int len;

    /* Read in the PIN */
    fscanf(fp, "%*s %*s %*s %*s %*s %*s %*s %s", user.pin);
    fclose(fp);
    len = strcpy7(command, "echo '1 ", user.pin, "' | mp_client m O ", key, " ",key, tr.pid_string);
    strcpy7(command+len, " ", user.login, " ", user.addr, " ", user.mach, " >out");
    data_log(tr.log_file, command);
    }
    else strcpy6(command, "echo '1' | mp_client m r ", key, " ",key, tr.pid_string, " >out");
    /* build directory names for later use */
    strcpy2(data_dir, key, tr.pid_string);
    strcpy2(db_data_dir, data_dir, "/.db");
    system(command);
    /* Verify data was retrieved */
    strcpy4(maskprep_file, data_dir, "/", key,  ".create");
    if (access(maskprep_file, 0) != 0)
    close_down1f(non_err, mti.suffix, NON_ERR);
    /* If the Fab is present in Mask Designer, copy in files */
    mask_fab_files_copy(maskprep_file, data_dir);
    /* Now run convert on the mask set and send the data */
    /* Must first run the Mask Prep convert */
    /* build the convert command */
    strcpy3(command, "convert_0_1 ", key, " MASK >out");
    chdir(data_dir);
    system(command);
    /* Go back to top level directory */
    chdir("..");
    /* Verify that the Mask Prep Convert was successful */
    strcpy2(maskprep_file, db_data_dir,  "/maskset");
    if (access(maskprep_file, 0) != 0)
    close_down1f(maskprep_convert_err, mti.suffix, MASKPREP_CONVERT_ERR);
    /* build the create file for Mask 2000 */
    maskprep_create_convert(key, data_dir);
    mask_convert(data_dir, db_data_dir, files);
    /* Now send the data */
    /* Send the global files */
    /* Must alter the key becvause it is used in global_send */
    strcat(key, tr.pid_string);
    global_files_send(data_dir);
    /* Send the data files */
    mask_files_list(data_dir, files);
    files_send(data_dir, files);
    /* Build list of files  in the .db directory */
    mask_files_list(db_data_dir, files);
    /* Send the .db directory and delete directories no longer needed */
    mask_db_dir_send(data_dir, db_data_dir, DBYES, files);
    /* Tell client all directories sent */
    put_short(SUCCESS);
    /* Send the return code */
    mti.suffix_ptr = RTV;
    /* Warn user converted from Mask Prep database */
    if (mask_msg.req_type == CHECKOUT)
    rtn.len += sprintf(rtn.msg+rtn.len, maskprep_convert_chkout_wrn, mti.prefix);
    else rtn.len += sprintf(rtn.msg+rtn.len, maskprep_convert_read_wrn, mti.prefix);
    put_ret_code(DBNO);
    /* Delete directories no longer needed */
    dir_delete(db_data_dir);
    dir_delete(data_dir);
} /* End maskprep_read */

int maskprep_create_convert(char *mask, char *data_dir)
{
    char fab[32];
    char flow[64];
    char design[64];
    FILE *fp;
    char file_name[FILE_SIZE];

    /* Build name for create file */
    strcpy4(file_name, data_dir, "/", mask, ".create");
    if ((fp = fopen(file_name, "r")) == NULL)
    close_down1f(rd_open_err, file_name, RD_OPEN_ERR);
    /* Now scan in data */
    fscanf(fp, "%[^/] %*c %s %s", fab, flow, design);
    fclose(fp);
    /* Now build name for new create file */
    strcpy2(file_name, data_dir, "/.db/create");
    if ((fp = fopen(file_name, "w")) == NULL)
    close_down1f(wr_open_err, file_name, WR_OPEN_ERR);
    /* Write out the data */
    fprintf(fp, "%s;%s;%s;%s;%s;%d\n",
    fab, flow, "none", design, user.email, time(0));
    fclose(fp);
    return(M2SUCCESS);
} /* End maskprep_create_convert */

void mask_fab_files_copy(char *maskprep_create_file, char *data_dir)
{
    char *files[M2TOTAL_FILES];
    char *off_fab_ptr;
    char *local_fab_ptr;
    char off_fab_dir[PATH_SIZE];
    char local_fab_dir[PATH_SIZE];
    char fab[FILE_SIZE];
    int cnt;
    FILE *fp;

    /* Read create file to get the fab */
    if ((fp = fopen(maskprep_create_file, "r")) == NULL) return;
    if (fscanf(fp, "%s", fab) != 1) return;
    /* Strip off trailing technology */
    if ((off_fab_ptr = strchr(fab, '/')) != NULL)
    off_fab_ptr[0] = '\0';
    /* Change the fab to upper case */
    str_upper(fab);
    /* Build the official. .fab directory name */
    off_fab_ptr = off_fab_dir + strcpy3(off_fab_dir, "off_fab_data/", fab, "/.fab");
    /* See if the .fab directory is present */
    if (access(off_fab_dir, 0) != 0) return;
    /* Make list of files in the fab directory */
    dir_list(off_fab_dir, files);
    /* Make the .fab directory */
    local_fab_ptr = local_fab_dir + strcpy2(local_fab_dir, data_dir, "/.fab");
    if (mkdir(local_fab_dir, 0700) != 0) return;
    /* copy files into the .fab directory */
    /* Initialize pointers for for loop */
    local_fab_ptr += strcpy1(local_fab_ptr, "/");
    off_fab_ptr += strcpy1(off_fab_ptr, "/");
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
    /* Build complete file names */
    strcpy(off_fab_ptr, files[cnt]);
    strcpy(local_fab_ptr, files[cnt]);
    file_copy(off_fab_dir, local_fab_dir);
    } /* end for */
} /* end mask_fab_files_copy */
