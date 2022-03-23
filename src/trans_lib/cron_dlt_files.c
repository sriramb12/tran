/****************************************************************************
 *
 * Function:	cron_dlt_files
 *
 * Description:	Delete big files with a cron job.
 *		        Deletes files in specified directory.
 *		        All history files are deleted later.
 *
 ***************************************************************************/

#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#include "close_down.h"
#include "edc_defines.h"
#include "edc_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "server_lib.h"
#include "trans_defines.h"
#include "trans_server_macros.h"
#include "trans_server_errs.h"
#include "trans_server_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_paths.h"
#include "user_params.h"
#include "global_debug.h"


extern TRANS_FILE_STAT  fs;


/* Read information from global file during cron dlt */
FILE *read_cron_file(char *file_name)
{
    extern char global_mask[];
    char line[4][260];
    register short cnt;
    FILE *fp;

    DBG("file_name=%s", file_name);

    /* Open global file */
    fp = fopen(file_name, "r");
    if (fp == NULL) return(NULL);

    /* Read in the data */
    /* Read in 4 lines of text */
    for (cnt = 0; cnt < 4; ++cnt)
        line_get(fp, line[cnt]);

    fclose(fp);

    /* Scan in parameters from first line */
    sscanf(line[0], ICOM_EXTEND_FORMAT,
        &fs.file_type, &fs.disk_index, &fs.dlt_time, fs.own_id, &fs.email_flag,
        &fs.extension, &fs.dlt_flag, &fs.lock_flag, &fs.mode, &fs.compression, global_mask, fs.tar_type);

    /* Pull out the real file name */
    strcpy(fs.real_name, line[1]+13);

    /* Scan in the original size */
    sscanf(line[2], "%*s %*s %lld", &fs.original_size);
    sscanf(line[3], "%*s %lld", &fs.compressed_size);

    /* Return the file pointer */
    return(fp);
} /* End read_cron_file */


void cron_dlt_all(void)
{
    extern int global_disk_usage;
    char msg[BUFFER_SIZE];
    char *list[1000];
    extern int disk_cnt;
    extern TRANS_DISK disk;
    register int disk_index;
    int cnt;
    int percent;
    int num[2];

    DBG();

    /* Make sure user is authorized to perform the transaction */
/*
    strcpy2(user_site, user.login, tr.user_site);
    if ((strcmp(user_site, admin.login1) != SUCCESS)
    && (strcmp(user_site, admin.login2)) != SUCCESS)
    shut_down(not_auth_err, NOT_AUTH_ERR);
*/
    /* Make sure the global data directory exists */
    if (access(GLOBAL_FILE_DIR, 0) != 0)
        close_down1m(non_err, GLOBAL_FILE_DIR, NON_ERR);

    /*  Reset disk usage */
    set_trans_disk_usage();
    rtn.len = strcpy1(rtn.msg, "Reset disk usage");

    /* Perform cron delete on each defined data directory */
    cnt = 0;
    disk_cnt = 10;
    for (disk_index = 0; disk_index <disk_cnt; ++disk_index)
    {
		if (strlen(disk.path[disk_index]) < 10)
		{
	        sprintf(rtn.msg, bad_disk_path, disk.path[disk_cnt], disk_cnt);
	        email_send("", admin.err_email, "Bad disk path", rtn.msg);
	        continue;
		} /* End if */

		cnt += cron_dlt_files(disk.path[disk_index], list);
    } /* End for */

    rtn.len += sprintf(rtn.msg+rtn.len, "\nDeleted %d big file(s).", cnt);
    cnt = dlt_empty_global_files("global_data");
    rtn.len += sprintf(rtn.msg+rtn.len, "\nDeleted %d empty global file(s).", cnt);

    /* Delete expired entries from keyword file */
    cnt = dlt_keyword_entries();
    rtn.len += sprintf(rtn.msg+rtn.len, "\nDeleted %d entries from keyword file.", cnt);

    /* Delete the old history files */
    dlt_history_files(num);
    rtn.len += sprintf(rtn.msg+rtn.len, "\nDeleted %d history file(s).", num[0]);
    rtn.len += sprintf(rtn.msg+rtn.len, "\nDeleted %d dirlist file(s).", num[1]);

    /* Delete expired entries from user keyword file */
    cnt = user_keys_delete();
    rtn.len += sprintf(rtn.msg+rtn.len, "\nDeleted %d user keywords.\n", cnt);

    /* Delete any transient files hanging around */
    //system("rm registry/*.lock");
    //system("rm transweb_registry/*.lock");

    cnt = files_dlt_transient("tmp_files");
    rtn.len += sprintf(rtn.msg+rtn.len, "\nDeleted %d file(s) in directory 'tmp_files'\nDeleted transient registry lock files", cnt);
/*
    reg_check();
*/
	rtn.len += strcpy1(rtn.msg+rtn.len, "\nAudited registry");
    percent = disk_usage_check(msg);
    sprintf(rtn.msg+rtn.len, "\nDisk usage at %d%%", percent);

    /* Send client return message */
    put_short(SUCCESS);
    put_success_msg(NONE);

    /* Insert disk usage into dt_disk table */
    sql_dt_disk_insert(percent);

    /* If disk usage is high send warning to administrator */
    if (percent > global_disk_usage) 
    {
        char subject[32];
        sprintf(subject, "Disk usage above %d%%\n", global_disk_usage);
        email_send("", admin.err_email, subject, msg);
    } /* End if */
} /* End cron_dlt_all */


/* Delete the data file */
/* If on DTMS, this is actually a directory */
void data_unlink(char *data_file)
{
    DBG();

    if (strcasecmp(server.site, DTMS) == 0)
        dir_recursive_dlt(data_file);
    else 
        unlink(data_file);
} /* end dir_recursive_dlt */


int cron_dlt_files(char *file_path, char *list[])
{
    char  history_file[FILE_SIZE];
    int len;
    register int num = 0;
    register int file_cnt = 0;
    FILE *fp;
    DIR *dp;
    struct dirent *dir, *readdir();
    char *delta_ptr;
    char *data_ptr;
    char *history_ptr;
    char data_file[FILE_SIZE];
    char delta_file[FILE_SIZE];

    DBG();

    if ((dp = opendir(file_path)) == NULL)
        close_down2d(open_dir_err, errno, file_path, OPEN_DIR_ERR);

    /* Initialize first part of file paths */
    data_ptr = strcpy1(data_file, file_path) + data_file;
    delta_ptr = strcpy1(delta_file, GLOBAL_FILE_PATH) + delta_file;
    history_ptr = strcpy1(history_file, HISTORY_FILE_PATH) + history_file;

    while ((dir = readdir(dp)) != NULL)
    {
        ++file_cnt;
	    /* Ignore the '.' and '..' files */
	    if ((dir->d_name[1] == '\0') || ((dir->d_name[1] == '.') && (dir->d_name[2] == '\0')))
	        continue;

	    /* strip off the suffix */
	    len = strcpy1(delta_ptr, dir->d_name);

	    if (strcmp(delta_ptr+len-3, ".gz") == 0) delta_ptr[len-3] = '\0';
	    else if (strncmp(delta_ptr+len-7, ".gz.pgp", 7) == 0) delta_ptr[len-7] = '\0';

	    if ((fp = read_cron_file(delta_file)) == NULL)
	    {
	        /* If the global file is not present, delete the actual file */
	        /* Build name for the actual big file */
	        strcpy(data_ptr, dir->d_name);
	        data_unlink(data_file);

	        /* If history file exists, log to it */
	        strcpy3(history_file, HISTORY_FILE_PATH, delta_ptr, ".history");
	        if (access(history_file, 0) == 0)
	        {
	            /* Log cron delete in history file */
	            log_history_file(delta_ptr);
#ifdef TRANSCEND
	            sql_trans_history_update(dir->d_name);
#endif
	        } /* end if for history file present */

	        /* Indicate one more big file deleted */
	        ++num;
	        continue;
	    }
	
	    /* If the expiration date has expired */
        /* If does not expire, dlt_time is set to 0 */
	    if ((tr.upd_time > fs.dlt_time) && (fs.dlt_time != 0))
	    {
	        /* Delete file with global info */
	        unlink(delta_file);
	    
	        /* Build name for the actual big file */
	        len = strcpy1(data_ptr, dir->d_name);
	        data_unlink(data_file);
	    
	        /* If history file exists, log to it */
	        /* Log cron delete in history file */
	        strcpy3(history_file, HISTORY_FILE_PATH, delta_ptr, ".history");

	        if (access(history_file, 0) == 0)
	            log_history_file(delta_ptr);
	    
	        /* Indicate one more big file deleted */
	        ++num;

#ifdef TRANSCEND
	        sql_trans_history_update(dir->d_name);
#endif
	    } /* End if */
    } /* end while */

    closedir(dp);
    return(num);
} /* End cron_dlt_files */

/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
