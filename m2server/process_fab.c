/****************************************************************************
 *
 * Function:	process_fab
 *
 * Description:	Processes fab requests 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dir.h"
#include "create.h"
#include "close_down.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "mask_server_defines.h"
#include "list.h"
#include "m2files.h"
#include "m2mask_files.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "m2state_table.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
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


void rmt_fab_rename(void)
{
    int index;

    /* Verify new fab not present or owned by requesting server */
    index = state_file_read4owner();
    /* Build name for existing fab */
    strcpy3(mti.old_data_dir, FAB_DATA, SLASH, mask_msg.key2);
    strcpy3(mti.old_off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key2);
    strcpy3(mti.old_delta_dir, FAB_DELTA, SLASH, mask_msg.key2);
    strcpy3(mti.tool_state_file, mti.old_delta_dir, FAB_DIR, "/state_file");
    /* Verify the source fab is present */
    if (access(mti.tool_state_file, 0) != 0)
    {
	strcpy3(mti.prefix, " '", mask_msg.key2, "'");
	close_down1f(non_err, mti.prefix, NON_ERR);
    }
     /* Create the 3 directories associated with the fab */
    rename(mti.old_off_data_dir,mti.off_data_dir);
    rename(mti.old_data_dir,mti.data_dir);
    rename(mti.old_delta_dir,mti.delta_dir);
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Tell user fab renamed */
    strcpy3(rtn.msg, mti.prefix, "renamed on ", ip.master_site);
     return_success(rtn.msg, DBNO);
} /* End rmt_fab_rename */

/* Delete an existing fab */
void fab_dlt(void)
{
    int index;

    /* Verify fab can be deleted */
    index = state_file_read4update();
    state_file_close(index);
     /* Delete the 3 directories associated with the fab */
    dir_recursive_dlt(mti.off_data_dir);
    dir_recursive_dlt(mti.data_dir);
    dir_recursive_dlt(mti.delta_dir);
    /* Tell user transaction successful up to this point */
    put_short(SUCCESS);
    /* Tell user fab deleted */
    mti.suffix_ptr = DLT;
    put_ret_code(DBNO);
    /* Send delete request to remote server */
    mask_send_delete(0, rmt_mpd.server_num);
} /* End fab_dlt */


/* Delete fab from remote databases */
void rmt_fab_dlt(void)
{
    int index;

    /* Verify requesting server owns the data */
    index = state_file_read4owner();
    /* Delete three directories associated with the fab */
    dir_recursive_dlt(mti.off_data_dir);
    dir_recursive_dlt(mti.data_dir);
    dir_recursive_dlt(mti.delta_dir);
    /* Tell remote server transaction successful up to this point */
    put_short(SUCCESS);
    /* build message to send to remote server */
    strcpy3(rtn.msg, mti.prefix, " deleted on ", ip.master_site);
    return_success(rtn.msg, DBNO);
} /* End rmt_fab_dlt */

/* Send user list of fabs */
void fab_list(void)
{
    char *files[1000];

    dir_list(FAB_DELTA, files);
    files_sort(files);
    /* Indicate that list of fabs will be retrieved */
    put_short(SUCCESS);
    /* Send list to client */
    list_get(files);
    /* Send user return code */
    mti.suffix_ptr = RTV;
    strcpy(mti.prefix, "List of fabs ");
    put_ret_code(DBNO);
} /* End fab_list */

void fab_audit(void)
{
    char fab_dir[FILE_SIZE];
    char *fab_ptr = fab_dir + strcpy2(fab_dir, FAB_DELTA, "/");
    int tmp_sock = sock;
    char *files[1000];
    char *fabs[50];
    int cnt;
    int cnt1;
    int ret;

    dir_list(FAB_DELTA, files);
    /* Make sure each is a valid fab */
    for (cnt = 0, cnt1 = 0; files[cnt] != NULL; ++cnt)
    {
	strcpy2(fab_ptr, files[cnt], "/.fab");
	if (access(fab_dir, 0) == 0)
	fabs[cnt1++] = files[cnt];
    } /* end for */
    /* Build header to send to remote server */
    bld_header(bf.header_buffer, RMT_AUDIT, FAB, 0);
    /* Update each server in a for loop */
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	ret = init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
    /* Send the buffer */
	list_get(files);
    /* Get the return code */
	get_rmt_ret_code(DBNO);
    } /* End for */
    /* Tell client fab audit complete */
    /* Restore original socket value */
     sock = tmp_sock;
    put_short(SUCCESS);
     return_success(rtn.msg, DBNO);
} /* End fab_audit */

/* audit the remote fabs */
void rmt_fab_audit(void)
{
    int cnt = 0;
    char *files[1000];
    char *data_ptr;
    char *delta_ptr;
    char *off_ptr;
    int off_audit_cnt = 0;
    int data_audit_cnt = 0;
    int delta_audit_cnt = 0;

    /* Tell server ready to get list of fabs */
    put_short(SUCCESS);
    /* Get the list from the remote server */
    list_put(files);
    /* Initialize pointers for following loop */
    data_ptr = mti.data_dir + strcpy2(mti.data_dir, FAB_DATA, "/");
    off_ptr = mti.off_data_dir + strcpy2(mti.off_data_dir, OFF_FAB_DATA, "/");
    delta_ptr = mti.delta_dir + strcpy2(mti.delta_dir, FAB_DELTA, "/");
    /* Make sure all fabs present */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* Build fab name */
	strcpy(data_ptr, files[cnt]);
	if (access(mti.data_dir, 0) != 0)
	{
	    mkdir(mti.data_dir, 0700);
	    log3("Just created fab data directory  ", mti.data_dir, ".");
	    ++data_audit_cnt;
	}
	strcpy(off_ptr, files[cnt]);
	if (access(mti.off_data_dir, 0) != 0)
	{
	    mkdir(mti.off_data_dir, 0700);
	    log3("Just created fab official directory  ", mti.off_data_dir, ".");
	    ++off_audit_cnt;
	}
	strcpy(delta_ptr, files[cnt]);
	if (access(mti.delta_dir, 0) != 0) 
	{
	    mkdir(mti.delta_dir, 0700);
	    log3("Just created fab delta directory  ", mti.delta_dir, ".");
	    ++delta_audit_cnt;
	} /* end if */
    } /* End for */
    /* Tell remote server audit complete */
     sprintf(rtn.msg, "Created %d official directories, %d data directories and %d delta directories on %s\n",
    off_audit_cnt, data_audit_cnt, delta_audit_cnt, ip.master_site);
    return_success(rtn.msg, DBNO);
} /* End rmt_fab_audit */

/* Create a fab remotely */
void rmt_fab_create(void)
{
    char tar_command[FILE_SIZE];

    /* First verify fab not present */
    if (access(mti.delta_dir, 0) == 0) 
    {
	sprintf(rtn.msg, "Fab already present");
    rmt_shut_down(rtn.msg, -1);
    }
    /* Tell remote server ready to retrieve data */
    put_short(SUCCESS);
    strcpy2(tar_command, "tar xf - ", mti.data_dir);
     command_put(tar_command);
    strcpy2(tar_command, "tar xf - ", mti.delta_dir);
     command_put(tar_command);
    strcpy2(tar_command, "tar xf - ", mti.off_data_dir);
     command_put(tar_command);
    /* Tell server retrieved all data */
    strcpy3(rtn.msg, mti.prefix, "created on ", ip.master_site);
    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
} /* End rmt_fab_create */

/* Shadow new fab data to remote servers */
void fab_create_send(void)
{
    int cnt;
     char tar_command[FILE_SIZE];
    /* Build header to send to remote server */
    bld_header(bf.header_buffer, RMT_CREATE, FAB, 0);
    /* Update each server in a for loop */
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	/* Connect to remote server */
	/* Send the header */
	/* Get back the response */
	init_rmt_socket(bf.header_buffer, &rmt_mpd, cnt);
	/* Send the three directories */
	/* Build tar command */
	strcpy2(tar_command, "/bin/tar cf - ", mti.data_dir);
	command_get(tar_command);
    strcpy2(tar_command, "tar cf - ", mti.delta_dir);
    command_get(tar_command);
    strcpy2(tar_command, "tar cf - ", mti.off_data_dir);
    command_get(tar_command);
	/* Indicate all data sent */
	put_short(SUCCESS);
	/* Get return code */
	get_rmt_ret_code(DBYES);
    } /* End big for */
    /* Free memory no longer needed */
    /* Indicate closing transaction and exit */
    log3("Closing transaction ", tr.pid_string, ".");
    exit(0);
} /* End fab_create_send */


void fablist_build(FILE *fp, char *fab_dir)
{
    int fab_len;
    int fab_cnt;
    int tech_cnt;
    char tech_dir[FILE_SIZE];
    char *fab[M2TOTAL_FILES];
     char *tech[M2TOTAL_FILES];

    /* Make list of fab directories */
    dir_list(fab_dir, fab);
    /* For each fab, make list of associated technologies */
    for (fab_cnt = 0; fab[fab_cnt] != NULL; ++fab_cnt)
    {
	/* If this is an invalid fab, skip it */
	if (name_valid_check(fab[fab_cnt], "fab_tech.files/invalid_fabs") == DBNO) continue;
	fab_len = strlen(fab[fab_cnt]);
	/* Build name of directory to list */
	strcpy3(tech_dir, fab_dir, "/", fab[fab_cnt]);
	mask_files_list(tech_dir, tech);
	/* Get the technology for eacvh fab */
	for (tech_cnt = 0; tech[tech_cnt] != NULL; ++tech_cnt)
	{
	    /* If this is an invalid technology, skip it */
	    if (name_valid_check(tech[tech_cnt], "fab_tech.files/invalid_techs") == DBNO) continue;
	    /* If doing this for Mask Prep, put fab in upper case */
	    /* Now write out the line to the output file */
	    fprintf(fp, "%s;%s\n", fab[fab_cnt], tech[tech_cnt]);
	    /* Free this tech no longer needed */
	    free(tech[tech_cnt]);
	} /* End for for going through list of techs */
	free(fab[fab_cnt]);
    } /* end for for going through list of fabs */
} /* End fablist_build */

/* This build the fab and tech list for MARS */
/* this is built once a week by the audits */
/* If this list is different than normal list, it is used */
/* If flows are needed, this list will no longer be built */
void tech_list_build(void)
{
    char tmp_file[FILE_SIZE];
    int fab_len;
    int fab_cnt;
    int tech_cnt;
    char tech_dir[FILE_SIZE];
    char *fab[M2TOTAL_FILES];
     char *tech[M2TOTAL_FILES];
    FILE *fp;

    if ((fp = fopen(tmp_file, "w")) == NULL)
    close_down1m(wr_open_err, tmp_file, WR_OPEN_ERR);
    /* Make list of fab directories */
    dir_list(FAB_DELTA, fab);
    /* For each fab, make list of associated technologies */
    for (fab_cnt = 0; fab[fab_cnt] != NULL; ++fab_cnt)
    {
	/* If bthis is the empty fab, skip it */
	if (strcmp(fab[fab_cnt], "empty") == 0) continue;
	fab_len = strlen(fab[fab_cnt]);
	/* Build name of directory to list */
	strcpy3(tech_dir, FAB_DELTA, "/", fab[fab_cnt]);
	mask_files_list(tech_dir, tech);
	    /* Write out the fab tech list */
	for (tech_cnt = 0; tech[tech_cnt] != NULL; ++tech_cnt)
	{
	    /* If this is the empty technology, skip it */
	    if (strcmp(tech[tech_cnt], "empty") == 0) continue;
	    /* Output fab and tech to the output file */
	    fprintf(fp, "%s;%s\n", fab[fab_cnt], tech[tech_cnt]);
	    /* Free this tech no longer needed */
	    free(tech[tech_cnt]);
	} /* End for for going through list of techs */
	free(fab[fab_cnt]);
    } /* end for for going through list of fabs */
    /* Close the output file */
    fclose(fp);
    /* Indicate transaction successful */
    put_short(SUCCESS);
    return_success("Fab-tech table retrieved", DBNO);
} /* End tech_list_build */

void fab_tech_rtv(void)
{
    struct stat buf;
    time_t mtime;

    /* First get the last modification date for the file */
    if (stat(GLOBAL_TECH_FILE, &buf) != 0)
    close_down1m(rd_open_err, GLOBAL_TECH_FILE, RD_OPEN_ERR);
    mtime = atol(mask_msg.key1);
    if (mtime >= buf.st_mtime)
    put_short(SUCCESS);
    else
    {
	put_short(DBYES);
	file_put("fab_tecxh.files", "fab_tech.tbl");
	return_success("Fab table received", DBNO);
    } /* End else */
    /* Send the return code */
    return_success("Latest copy of fab table already present", DBNO);
} /* End fab_tech_rtv */

/* Verify this is a valid fab */
int name_valid_check(char *fab, char *file_name)
{
    char line[260];
    FILE *fp;

    /* If file not present, assume fab is valid */
    if ((fp = fopen(file_name, "r")) == NULL) return(DBYES);
    /* Read through list looking for invalid fabs */
    while (line_get(fp, line) != EOF)
    {
	/* If this is a blank line skip it */
	if (strlen(line) == 0) continue;
	/* If fab in this file return no */
	if (strcasecmp(fab, line) == 0) 
	{
	    fclose(fp);
	return(DBNO);
	} /* End if */
    } /* End while */
    /* If reached this point, fab not found */
    fclose(fp);
    return(DBYES);
} /* End name_valid_check */

void fab_tech_list_send(void)
{
    char tmp_file[FILE_SIZE];
    char command[1024];
    FILE *fp;

    strcpy5(tmp_file, "fab_tech.files/tmp_files/", user.login, "_", tr.pid_string, ".fab_tech_tbl");
    if ((fp = fopen(tmp_file, "w")) == NULL)
    close_down1m(wr_open_err, tmp_file, WR_OPEN_ERR);
    /* Make list of fab directories */
    fablist_build(fp, FAB_DELTA);
    /* Make list of fabs from Mask Prep */
    fablist_build(fp, "old_fab_dlta");
    /* Close the output file */
    fclose(fp);
    /* Now append any valid fabs not present in the list */
    if (access(NEW_FABS, 0) == 0)
    file_append(NEW_FABS, tmp_file);
    /* Indicate ready to send data */
    put_short(SUCCESS);
    /* Sort the file and put in final location */
    strcpy4(command, "cat ", tmp_file, 
    " | tr abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ",
    " | sort -u >fab_tech.files/fab_tech.tbl");
    system(command);
    file_put("fab_tech.files", "fab_tech.tbl");
    /* Indicate transaction successful */
    return_success("Fab list retrieved", DBNO);
    /* Delete temporary file no longer needed */
    unlink(tmp_file);
} /* End fab_tech_list_send */

void process_fab(void)
{
	    strcpy(key, mask_msg.key1);
    strcpy3(mti.data_dir, FAB_DATA, SLASH, mask_msg.key1);
    strcpy3(mti.off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1);
    strcpy3(mti.delta_dir, FAB_DELTA, SLASH, mask_msg.key1);
    strcpy3(mti.prefix, "fab '", mask_msg.key1, "'");
    /* Build the state file name */
    strcpy3(mti.state_file,  mti.delta_dir, FAB_DIR, "/state_file");
    switch(mask_msg.req_type)
{
	/* Send user list of fabs */
	case M2LIST:  fab_list(); break;
	/* Create a new fab, only directories created */
	case CREATE:  fab_create(); break;
	case RENAME:  fab_rename(); break;
	case DELETE:  fab_dlt(); break;
	case LIST_MEMBERS:  fab_tech_list_send(); break;
	case RMT_DELETE:  rmt_fab_dlt(); break;
	case RMT_CREATE:  rmt_fab_create(); break;
	case RMT_RENAME:  rmt_fab_rename(); break;
	case AUDIT: fab_audit(); break;
	case RMT_AUDIT: rmt_fab_audit(); break;
	case RETRIEVE: fab_tech_rtv(); break;
	case UPDATE: tech_list_build(); break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_fab */

void fab_create(void)
{
    char new_data_dir[FILE_SIZE];
    char new_off_data_dir[FILE_SIZE];
    char new_delta_dir[FILE_SIZE];
    char old_data_dir[FILE_SIZE];
    char old_delta_dir[FILE_SIZE];
    char *old_data_ptr;
    char *old_delta_ptr;
    char *new_data_ptr;
    char *new_delta_ptr;
    char *new_off_data_ptr;
    char *files[M2TOTAL_FILES];


    /* Verify fab not already present */
    if (access(mti.state_file, 0) == 0)
    close_down1f(crt_err, mti.prefix, CRT_ERR);
    /* Build name for existing fab */
    strcpy3(mti.old_delta_dir, FAB_DELTA, "/", mask_msg.key2);
    strcpy3(mti.tool_state_file, mti.old_delta_dir, FAB_DIR, "/state_file");
    /* Verify the source fab is present */
    if (access(mti.tool_state_file, 0) != 0)
    {
	strcpy3(mti.prefix, " '", mask_msg.key2, "'");
	close_down1f(non_err, mti.prefix, NON_ERR);
    }
     /* Create the 3 directories associated with the fab */
    dir_create(mti.off_data_dir);
    dir_create(mti.data_dir);
    dir_create(mti.delta_dir);
    /* Make list of directories in old fab */
    /* Copy all data directories in old fab to new fab */
    /* Initialize the pointers */
    old_data_ptr = strcpy4(old_data_dir, FAB_DATA, "/", mask_msg.key2, "/") + old_data_dir;
    old_delta_ptr = strcpy4(old_delta_dir, FAB_DELTA, "/", mask_msg.key2, "/") + old_delta_dir;
    new_data_ptr = strcpy2(new_data_dir, mti.data_dir, "/") + new_data_dir;
    new_off_data_ptr = strcpy2(new_off_data_dir, mti.off_data_dir, "/") + new_off_data_dir;
    new_delta_ptr = strcpy2(new_delta_dir, mti.delta_dir, "/") + new_delta_dir;
    strcpy(new_data_ptr, ".fab");
    strcpy(old_delta_ptr, ".fab");
    strcpy(old_data_ptr, ".fab");
    /* Get version number for source fab and technology */
    source_state_file_read(old_delta_dir);
    strcpy(new_delta_ptr, ".fab");
    mask_dir_copy(new_data_dir, old_data_dir, old_delta_dir, files, DBYES);
    /* Create the associated delta directory */
    dir_create(new_delta_dir);
    /* Create the associated version files */
    version_files_create(files, new_delta_dir);
    /* Build the state file name */
    strcpy2(mti.state_file, new_delta_dir, "/state_file");
    /* Create the state file */
    state_file_crt(NO_ACTION, mask_msg.key3);
    /* Create the associated official data directory */
    strcpy(new_off_data_ptr, ".fab");
    dir_create(new_off_data_dir);
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Tell user fab created */
    mti.suffix_ptr = CRT;
    put_ret_code(DBNO);
    /* Send update to remote servers */
    fab_create_send();
} /* End fab_create */

void fab_rename(void)
{

    /* Verify fab not already present */
    if (access(mti.state_file, 0) == 0)
    close_down1f(crt_err, mti.prefix, CRT_ERR);
    /* Build name for existing fab */
    strcpy3(mti.old_data_dir, FAB_DATA, SLASH, mask_msg.key2);
    strcpy3(mti.old_off_data_dir, OFF_FAB_DATA, SLASH, mask_msg.key2);
    strcpy3(mti.old_delta_dir, FAB_DELTA, SLASH, mask_msg.key2);
    strcpy3(mti.tool_state_file, mti.old_delta_dir, FAB_DIR, "/state_file");
    /* Verify the source fab is present */
    if (access(mti.tool_state_file, 0) != 0)
    {
	strcpy3(mti.prefix, " '", mask_msg.key2, "'");
	close_down1f(non_err, mti.prefix, NON_ERR);
    }
     /* Create the 3 directories associated with the fab */
    rename(mti.old_off_data_dir,mti.off_data_dir);
    rename(mti.old_data_dir,mti.data_dir);
    rename(mti.old_delta_dir,mti.delta_dir);
    /* Tell user transaction successful so far */
    put_short(SUCCESS);
    /* Tell user fab renamed */
    mti.suffix_ptr = REN;
    put_ret_code(DBNO);
    /* Send rename request to remote server */
     rmt_request_send(RMT_RENAME);
} /* End fab_rename */
