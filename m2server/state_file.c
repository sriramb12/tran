/****************************************************************************
 *
 * Function:	state_file_crt
 *
 * Description:	state_file functions
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <errno.h>
#include <unistd.h>
#include "close_down.h"
#include "entry.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "lock_file.h"
#include "mask_server_defines.h"
#include "m2globals.h"
#include "m2state_table.h"
#include "m2req_types.h"
#include "m2server_globals.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "mti.h"
#include "m2state_file.h"
#include "rtn.h"
#include "strcpy.h"
#include "user_params.h"
#include <strings.h>

void state_file_crt(int state, char *usergroup)
{
    int cnt;
    FILE *fp;
    int max_programs;

    if (mask_msg.data_type == MASK_SET) max_programs = 15;
    else max_programs = 1;

    if ((fp = fopen(mti.state_file,"w")) == NULL)
    close_down1m(wr_open_err, mti.state_file,  WR_OPEN_ERR);
    /* Set errno to default value of 0 */
    errno = 0;
    /* Put data from global variables into the state file */
    fprintf(fp, GLOBAL_STATE_OUTPUT,
    ip.master_site, "-", 0.0, tr.upd_time, 0, 1, 1.000, "None", tr.upd_time);
    /* Initialize user group and users */
    if(strcmp(usergroup, NONE) != 0)
    state_file_usergroup_add(fp, usergroup);
    else fprintf(fp, "None\nNone\n");
    fprintf(fp, TOOL_STATE_OUTPUT,
    state, user.email, user.email, user.email, user.cwd);
    /* Copy value in state to pre state for remote updates */
    st.pre_state = NO_ACTION;
     /* Write out blank data for rest of tools */
    errno = 0;
    for (cnt =1; cnt <max_programs; ++cnt)
    fprintf(fp, "4 0 0 0 0\n");
    /* If could not write to state file, abort transaction */
    if (errno != 0)
    close_down1m(write_err, mti.state_file,  WRITE_ERR);
    fclose(fp);
} /* End state_file_crt */

void pc_state_file_crt(int state, char *usergroup)
{
    FILE *fp;


    if ((fp = fopen(mti.state_file,"w")) == NULL)
    close_down1m(wr_open_err, mti.state_file,  WR_OPEN_ERR);
    /* Set errno to default value of 0 */
    errno = 0;
    /* Put data from global variables into the state file */
    fprintf(fp, GLOBAL_STATE_OUTPUT,
    ip.master_site, "-", 0.0, tr.upd_time, state, 1, 1.000, mask_msg.par2, tr.upd_time);
    if (errno != 0)
    close_down1m(write_err, mti.state_file,  WRITE_ERR);
    /* Add reference to usergroup if present */
    if(strcmp(usergroup, NONE) != 0)
    state_file_usergroup_add(fp, usergroup);
    else fprintf(fp, "None\nNone\n");
    fprintf(fp, TOOL_STATE_OUTPUT,
    state, user.email, user.email, user.email, user.cwd);
    /* Entry for history portion of data */
    fprintf(fp, TOOL_STATE_OUTPUT,
    state, user.email, user.email, user.email, user.cwd);
    /* Copy value in state to pre state for remote updates */
    st.pre_state = NO_ACTION;
    /* If could not write to state file, abort transaction */
    fclose(fp);
} /* End pc_state_file_crt */

/* Reads in values from a state file */
void state_file_read(void)
{
    char tmp_string[200];
    int ret;
    FILE *fp;
     char line[260];
    int cnt;

    if ((fp = fopen(mti.state_file, "r")) == NULL)
    {
    if  (mask_msg.req_type != CHECKOUT_STATUS_RTV)
    maskprep_read();
    else close_down1f(non_err, mti.prefix, NON_ERR);
    } /* end if */
    if ((ret =  fscanf(fp,GLOBAL_STATE_INPUT,
    st.master_site, st.version_type, &st.app_version, &st.touch_time,
    &st.app_time, &st.transaction_num, &st.vnum, st.approver, &st.upd_time)) != 9)
    if (exit == DBYES)
    close_down1m(read_err, mti.state_file, READ_ERR);
    else
    {
    strcpy(st.version_type, "-");
    return;
    }
     /* Scan past carriage return before getting lines of text */
    getc(fp);
    /* Get the user groups and list of users */
    line_get(fp, st.usergroup);
    line_get(fp, st.users);
    /* Get the tool portion of the data */
    for (cnt =0; cnt <= mask_msg.program; ++cnt)
    line_get(fp, line);
    cnt =sscanf(line, TOOL_STATE_INPUT,
    &st.state, st.user_id, st.own_id, st.last_version, st.cwd);
    fclose(fp);
    if (cnt != 5)
    {
    if (exit == DBYES)
    close_down1m(read_err, mti.state_file, READ_ERR);
    else strcpy(st.version_type, "-");
    } /* End if */
} /* End state_file_read */

/* Read state file specified by file name passed in */
void source_state_file_read(char *delta_dir)
{
    FILE *fp;
     char state_file[64];

    strcpy2(state_file, delta_dir, "/state_file");
    if ((fp = fopen(state_file, "r")) == NULL)
     close_down1m(rd_open_err, state_file, RD_OPEN_ERR);
    if ( fscanf(fp,GLOBAL_STATE_INPUT,
    st.master_site, st.version_type, &st.app_version, &st.touch_time,
    &st.app_time, &st.transaction_num, &st.vnum, st.approver, &st.upd_time) != 9)
    close_down1m(read_err, state_file, READ_ERR);
    /* Close file no longer needed */
    fclose(fp);
} /* End source_state_file_read */

/* Read template state file to get update time */
long template_state_file_read(void)
{
    int ret;
    FILE *fp;
    long parent_upd_time;

    if ((fp = fopen(mti.state_file, "r")) == NULL)
    return(FAILURE);
    ret = fscanf(fp, PARENT_STATE_INPUT, &parent_upd_time);
    /* Close file no longer needed */
    fclose(fp);
    if (ret == 1)
    return(parent_upd_time);
    else return(ret);
} /* End template_state_file_read */

/* Read data from state file needed for mask list */
void state_file_list(char *mask)
{
    FILE *fp;

    strcpy4(mti.state_file, MASK_DELTA, "/", mask, "/state_file");
    if ((fp = fopen(mti.state_file, "r")) == NULL)
    {
    st.upd_time = 0;
    strcpy(st.user_id, NONE);
return;
    }
    if ( fscanf(fp,GLOBAL_STATE_INPUT,
    st.master_site, st.version_type, &st.app_version, &st.touch_time,
    &st.app_time, &st.transaction_num, &st.vnum, st.approver, &st.upd_time) != 9)
    {
    st.upd_time = 0;
    strcpy(st.user_id, NONE);
    }
    /* Close file no longer needed */
    fclose(fp);
} /* End state_file_list */

/* Gets the state for update, read, approve, version and delete transactions */
/* Called by process_approve, process_update, and process_read */
int state_file_read4update(void)
{
    int index;
    FILE *fp;
    char line[260];
     int cnt;
     int len;

    /* Lock the file so no other process may access it */
    index = state_file_lock_set(mti.state_file);
    /* Open the state file */
    if ((st.fd = open(mti.state_file, O_RDWR)) == EOF)
    /* Check for appropriate action when state file not present */
    state_file_not_present(index);
     /* Open file with stream pointer */
    if ((fp = fopen(mti.state_file, "r")) == NULL)
    close_down1m(rd_open_err, mti.state_file, RD_OPEN_ERR);
    /* Read in the data from the file */
    /* Scan information in the state file into global variables */
    if ( fscanf(fp,GLOBAL_STATE_INPUT,
    st.master_site, st.version_type, &st.app_version, &st.touch_time,
    &st.app_time, &st.transaction_num, &st.vnum, st.approver, &st.upd_time) != 9)
    {
    fclose(fp);
    close_down1m(read_err, mti.state_file, READ_ERR);
    }
    /* If database does not own data, send request to owner */
    /* this is not done if request type is read any */
    /* Do not check for owner under following conditions */
    if ((mask_msg.data_type != USERGROUP)
    && (mask_msg.req_type != READ_ANY)
    && (strcmp(st.master_site, ip.master_site) != 0))
    {
    fclose(fp);
    lock_file_clear(index);
    m2server_send(st.master_site);
    }
    /* Get carriage return before scanning in lines */
    getc(fp);
    /* Get user group and users */
    line_get(fp, st.usergroup);
    line_get(fp, st.users);
    /* Get the tool portion of the data */
    for (cnt = 0; (len = line_get(fp, line)) > 0; ++cnt)
    {
    st.line[cnt] = malloc(len+2);
    strcpy2(st.line[cnt], line, "\n");
    } /* End for */
    st.line[cnt] = NULL;
    /* Close the stream pointer no longer needed */
    fclose(fp);
    /* Scan data for this tool into global variables */
    data_log2(tr.log_file, "ready to scan data for ", mti.state_file);
    /* If expected number of lines not present give an error */
    if (mask_msg.program >= cnt)
    close_down1m(read_err, mti.state_file, READ_ERR);
    sprintf(rtn.msg, "mask_msg.program is %d\n", mask_msg.program);
    data_log(tr.log_file, rtn.msg);
    sprintf(rtn.msg, "cnt is %d\n", cnt);
    data_log(tr.log_file, rtn.msg);
    mask_msg.program = 0;
    if (sscanf(st.line[mask_msg.program], TOOL_STATE_INPUT,
    &st.state, st.user_id, st.own_id, st.last_version,
     st.cwd) != 5)
    close_down1m(read_err, mti.state_file, READ_ERR);
    /* If doing user group request, no further checks are needed */
    if ((mask_msg.data_type == USERGROUP)
    || (mask_msg.req_type == APPEND))
    return(index);
    /* make sure user has access to the data */
    /* Do not do check if approving or unapproving PC data */
    if ((mask_msg.data_type == PC)
    && ((mask_msg.req_type == APPROVE) || (mask_msg.req_type == UNAPPROVE)));
    else owner_check();
    /* Keep track of previous state so can reset later if error occurs */
    /* Keep track of version number so can reset later if error occurs */
    pre_vnum = st.vnum;
    st.pre_state = st.state;
    /* Calculate index into state table */
    if (mask_msg.req_type <= UNARCHIVE)
    get_action();
    return(index);
} /* End state_file_read4update */

/* Verifies data can be shadowed from remote server */
/* Verifies this site does not own the data */
void state_file_read4shadow(void)
{
    FILE *fp;
     int ret;

    /* Open the state file */
     /* Open file with stream pointer */
    /* If file not present, indicate data not present */
    if ((fp = fopen(mti.state_file, "r")) == NULL)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Scan information in the state file into global variables */
    ret =  fscanf(fp,"%s",  st.master_site);
    /* Close file no longer needed */
    fclose(fp);
    /* If no data present, allow shadowing */
    if (ret != 1) return;
     /* If database does not own data, do not allow shadowing */
    if (strcmp(st.master_site, ip.master_site) != 0)
     m2server_send(st.master_site);
} /* End state_file_read4shadow */

int state_file_read4owner(void)
{
    FILE *fp;
     int ret;

    /* Open the state file */
     /* Open file with stream pointer */
    /* If file not present, indicate data not present */
    if ((fp = fopen(mti.state_file, "r")) == NULL)
    return(NON_ERR);
    /* Scan information in the state file into global variables */
    ret =  fscanf(fp,GLOBAL_STATE_INPUT,
    st.master_site, st.version_type, &st.app_version, &st.touch_time,
    &st.app_time, &st.transaction_num, &st.vnum, st.approver, &st.upd_time);
    /* Close file no longer needed */
    fclose(fp);
    /* If no data present, allow shadowing */
    if (ret != 9) return(SUCCESS);
    /* If requesting database does not own data, do not allow shadowing */
    if (strcmp(mask_msg.key3, st.master_site) != 0)
    {
        email_send("", "mdsEvents@nxp.com", "Database error", rtn.msg);
    sprintf(rtn.msg, not_server, mti.prefix, st.master_site);
    /* this function exits software */
    rmt_shut_down(rtn.msg, NOT_SERVER);
     } /* End if */
    else return(SUCCESS);
} /* End state_file_read4owner */

/* Writes out changes made to an already open state file */
void state_file_write(int index)
{
    register int len;
    int cnt;

    /* Point to the beginning of the file */
    /* If unable to do this, just close the file and open it again */
    if (lseek(st.fd, 0, SEEK_SET) != SUCCESS)
    {
    close(st.fd);
    if ((st.fd = open(mti.state_file, O_WRONLY)) == EOF)
    /* Check for appropriate action when state file not present */
    state_file_not_present(index);
    } /* end if */
    /* Write the data back out to the state file */
    sprintf(bf.buffer, GLOBAL_STATE_OUTPUT,
    st.master_site, st.version_type, st.app_version, st.touch_time,
    st.app_time, st.transaction_num, st.vnum, st.approver, st.upd_time);
    len = strlen(bf.buffer);
    len += strcpy4(bf.buffer+len, st.usergroup, "\n", st.users, "\n");
    if ( write(st.fd, bf.buffer, len) != len)
    close_down1m(write_err, mti.state_file, WRITE_ERR);
    /* Write out changed line */
    sprintf(bf.buffer, TOOL_STATE_OUTPUT,
    st.state, st.user_id, st.own_id, st.last_version, st.cwd) ;
    /* Free line no longer needed */
    free(st.line[mask_msg.program]);
    st.line[mask_msg.program] = bf.buffer;
    for (cnt = 0; st.line[cnt] != NULL; ++cnt)
    {
    len = strlen(st.line[cnt]);
    if ( write(st.fd, st.line[cnt], len) != len)
    close_down1m(write_err, mti.state_file, WRITE_ERR);
    } /* End for */
    /* Unlock the file so other processes may access it */
    lock_file_clear(index);
    close(st.fd);
    st.fd = 0;
} /* End state_file_write */

/* Unlock and close a state file */
void state_file_close(int index)
{

    /* Unlock the file so other processes may access it */
     lock_file_clear(index);
    close(st.fd);
    st.fd = 0;
} /* End state_file_close */

/* Add reference to user group file to state file */
void state_file_usergroup_add(FILE *fp, char *usergroup)
{
    char usergroup_path[FILE_SIZE];

    /* User can pass in a user group */
    /* GUI passes in no user group as N/A */
    if ((strcmp(usergroup, "N/A") == 0)
    || (strcmp(usergroup, NONE) == 0))
    fprintf(fp, "None\nNone\n");
    else
    {
    strcpy3(usergroup_path, USER_DELTA,  "/", usergroup);
    if (access (usergroup_path, 0) == 0)
    {
        fprintf(fp, "%s\nNone\n", usergroup);
        /* Add reference to mask set to usergroup file */
        strcat(usergroup, ".masks");
        entry_add(usergroup, key, key);
    } /* End if */
    else
    {
         fprintf(fp, "None\nNone\n");
        sprintf(rtn.msg, usergroup_add_wrn, usergroup, mti.suffix);
        rtn.len = strlen(rtn.msg);
    } /* End else */
    } /* End else */
} /* End state_file_usergroup_add */


extern char *lock_files[TOTAL_LOCK_FILES];

/* Lock specified file */
int state_file_lock_set(char *file_name)
{
    int index;

    /* If the file does not exist, do not bother locking it */
    if (access(file_name, 0) != 0) return(-1);
    /* look for first open slot in array */
    for (index = 0; index < TOTAL_LOCK_FILES; ++index)
    if (lock_files[index] == NULL) break;
    /* Allocate space for lock file */
    lock_files[index] = malloc(strlen(file_name) + 6);
    if (lock_files[index] == NULL)
    close_down1m(malloc_err, file_name, MALLOC_ERR);
    /* build lock file name */
    strcpy2(lock_files[index], file_name, ".lock");
     /* Try to lock the file up to ten times */
    if (lock_file_wait(lock_files[index], 10) == SUCCESS) return(index);
    /* if reach this point, unable to lock file */
    /* If override flag not setask the user if he would like to wait for transaction to complete */
    if (mask_msg.override_flag == DBNO)
    {
    free(lock_files[index]);
    lock_files[index] = NULL;
    sprintf(rtn.msg, lock_override_err, mti.suffix);
    return_override(rtn.msg, LOCK_OVERRIDE_ERR);
    }
    /* If override flag on, wait for transaction to complete for up to a minute */
    lock_file_wait(lock_files[index], 60);
    /* At this point either locked the file or assume other transaction completed */
    return(index);
} /* End state_file_lock_set */

/* Called when state file cannot be opened */
void state_file_not_present(int index)
    {
/* Allow transaction to continue if approving machine tables */
if ((mask_msg.data_type == MACHINE) && (mask_msg.req_type == APPROVE)) return;
/* Also allow check in of mask set */
    /* If working on Mask Set, check for valid transactions */
else if (mask_msg.data_type == MASK_SET)
{
if  ((mask_msg.req_type == CHECKIN)
    && (strcmp(mask_msg.key2, "port") == 0))
mask_port();
else close_down1f(non_err, mti.prefix, NON_ERR);
    } /* end if for Mask Set data */
/* Inform user data does not exist */
else close_down1f(non_err, mti.prefix, NON_ERR);
} /* end state_file_not_present */
int autosave_present(void)
{

    char autosave_dir[FILE_SIZE];

strcpy2(autosave_dir, mti.data_dir, ".tmpdb");
    if (access(autosave_dir, 0) == 0)
    return(AUTOSAVE_PRESENT);
    else return(AUTOSAVE_NOT_PRESENT);
} /* end autosave_present */
