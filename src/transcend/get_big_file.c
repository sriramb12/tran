/****************************************************************************
 *
 * Function:	get_big_file
 *
 * Description:	Retrieves a big file from the Transcend database.
 *
 ***************************************************************************/

#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_defines.h"
#include "trans_server_macros.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "rtn.h"
#include "strcpy.h"
#include "user_params.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


/* Parameter: Key for big file to retrieve */
void get_big_file(char *key)
{
    DBG("key='%s'", key);

    extern TRANS_FILE_STAT fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    extern unsigned long long parallel_configured_size;
    extern unsigned long long separate_configured_size;
    extern char program_name[];
    int dtms;
    int parallel_type = atoi(gu.par5);
    int tar_option = DBNO;
    char real_name[FILE_SIZE];
    char file_size_string[32];
    FILE *fp;

     /* Open global file */
    fp = read_big_file(key, lines, &fs);
    fclose(fp);

    /* If this file is locked for data transfer, give error and exit */
    lock_status_check();

    /* If data needs to be deleted, set delete flag */
    if (IS_REQUEST_GET_DELETE(gu.req_type) && fs.dlt_flag != 3)
    {
        fs.dlt_flag = DBYES;
    }

    /* Determine if this is DTMS */
    if (strcmp(server.site, DTMS) == 0) dtms = DBYES;
    else dtms = DBNO;

#ifdef DTMS_TEST
    /* If this is DTMS and retrieving part of a file, see if it is present */
    if ((dtms == DBYES) && (strcmp(gu.par2, NONE) != 0))
    {
	    /* Build name of file to get */
        strcpy4(bf.file_name, disk.path[fs.disk_index], key, ".gz/", gu.par2);
	    if (access(bf.file_name, 0) != 0)
	        return_non_err( bf.file_name);
    }

    /* Send the file */
    if ((dtms == DBYES) && (fs.file_type == DIRECTORY))
    {
	    command_get_compressed_dir();
    }
    else
#endif
    /* If the file is over 20 Mb, and is zipped send it as a parallel file */
    /* Also must be using version 7.0.3 or later of Transcend */
#ifdef TRANSCEND
    /* Only send as separate blocks if specified by the user */
    if ((gu.req_type == DBGET_SEPARATE) && (fs.compressed_size > separate_configured_size))
        get_separate();
    else if ((fs.original_size > parallel_configured_size) && !IS_REQUEST_GET_SERIAL(gu.req_type)	&& (fs.original_size == fs.compressed_size) && (machine_supports_parallel() == DBYES) && (strcmp(user.client_version, "7.0.5") >= 0))
        get_parallel();
    else if (IS_REQUEST_GET_PARALLEL(parallel_type) && !IS_REQUEST_GET_SERIAL(gu.req_type) && (machine_supports_parallel() == DBYES) && (strcmp(user.client_version, "7.0.5") >= 0))
        get_parallel();
    /* At this point DTMS will never db DBYES */
    else if ((dtms == DBYES) && (fs.original_size == fs.compressed_size) && (strcmp(fs.real_name +strlen(fs.real_name)-3, ".gz") == 0) && (strcmp(user.client_version, "7.0.3") <= 0))
        command_get();
    else
#endif
    tar_option = send_file(&fs, key, dtms);
    
    /* If retrieving as a compressed file, calculate compressed size */
    if (IS_REQUEST_GET_ZIPPED(gu.req_type))
        num_gmb_calc(file_size_string, fs.compressed_size);

    /* Otherwise calculate the original size */
    else num_gmb_calc(file_size_string, fs.original_size);

    /* Build message based on file type */
    /* Put file name back to original name */
    /* or use alternate name if specified */
    if (strcmp(gu.par3, NONE) != 0)
        strcpy(real_name, gu.par3);
    else 
        strcpy_ex(real_name, fs.real_name, ' ', '`');

    if (fs.file_type == DIRECTORY)
    {
	    /* If retrieving as a compressed file, add file suffix */
        if (IS_REQUEST_GET_ZIPPED(gu.req_type))
	        strcat(real_name, ".tar.gz");

	    rtn.len = strcpy7(rtn.msg, "Directory '", real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
	    if (tar_option == DBYES)
	        strcpy2(rtn.msg+rtn.len , "\n\nThis directory was retrieved as a tar file.\n", "Use the Stuff It utility to unbundle the directory.");
    }
    else if (fs.file_type == FILES)
        rtn.len = strcpy7(rtn.msg, "Files '", real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
    else 
    {
        if (IS_REQUEST_GET_ZIPPED(gu.req_type) && (is_zipped_file(real_name) == DBNO))
	        strcat(real_name, ".gz");

	strcpy7(rtn.msg, "File '", real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
    } /* End else */
#ifdef TRANSWEB
    /* If this is not the Transcend GUI, print a warning */
    if (strcmp(gu.machine_type, "TransGUI") != 0)
    {
	    if (fs.file_type == DIRECTORY)
	        sprintf(rtn.msg+rtn.len, dir_wrn, real_name, real_name);
	    else if (fs.file_type == FILES)
	    {
	        char *ptr = strchr(real_name, ' ');
	        if (ptr != NULL) ptr[0] = '\0';
	            sprintf(rtn.msg+rtn.len, files_wrn, real_name, real_name);
	    }
    } /* end if for not Transcend GUI */
#endif

    /* Send the return code */
    put_success_msg(key);

    /* If delete option is on or user did a get-delete, delete the file */
    if ( fs.dlt_flag == DBYES || (IS_REQUEST_GET_DELETE(gu.req_type) && fs.dlt_flag != 3))
    {
	    /* Delete global file */
	    /* Build name for global file */
	    strcpy2(bf.file_name, GLOBAL_FILE_PATH, key);
	    unlink(bf.file_name);
        
        /* Build name for real file */
        strcpy3(bf.file_name, disk.path[fs.disk_index], key, ".gz");
        
        /* Delete real file */
        unlink(bf.file_name);
	    /* Log history in database */
    } /* End if for deleting file */

    /* See if creator needs to be sent mail */
	/* If email flag turned on for this file or global email notification */
	/* set, send email to creator */
	if ((fs.email_flag == DBYES) || (search_email_file(fs.own_id, GLOBAL_EMAIL_FILE) == DBYES))
	    send_rtv_email(key, gu.email1, fs.own_id, fs.real_name, fs.file_type);
} /* End get_big_file */


void get_big_file_ack(char *key)
{
    DBG("key='%s'", key);
    
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    extern char program_name[];
    char file_size_string[32];

    num_gmb_calc(file_size_string, fs.original_size);
    /* See if creator needs to be sent mail */
    /* Do not send email if creator and owner are the same */
    /*
    if (strcmp(gu.email1, fs.own_id) != 0)
*/
    {
	    if ((fs.email_flag == DBYES) || (search_email_file(fs.own_id, GLOBAL_EMAIL_FILE) == DBYES) || (search_email_file(gu.from_company, COMPANY_EMAIL_FILE) == DBYES))
	        send_rtv_email(key, gu.email1, fs.own_id, fs.real_name, fs.file_type);
    }
    
    /* Build message based on file type */
    if (strcmp(gu.machine_type, WEB_BROWSER) == 0)
    {
        if (fs.file_type == DIRECTORY)
        {
            sprintf(rtn.msg+rtn.len, dir_wrn, fs.real_name, fs.real_name);
        }
        else if (fs.file_type == FILES)
        {
	        char *ptr = strchr(fs.real_name, ' ');
	        if (ptr != NULL) ptr[0] = '\0';
	        sprintf(rtn.msg+rtn.len, files_wrn, fs.real_name, fs.real_name);
        }
        else
        {
            strcpy7(rtn.msg, "File '", fs.real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
        }
    }
    else
    {
        if (fs.file_type == DIRECTORY)
	        rtn.len = strcpy7(rtn.msg, "Directory '", fs.real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
        else if (fs.file_type == FILES) 
	    {
	        strexc(fs.real_name, '`', ' ');
	        strcpy7(rtn.msg, "Files '", fs.real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
	    } /* end if */
        else 
	    {
	        /* If retrieved as a compressed file, add the .gz suffix */
            if (IS_REQUEST_GET_ZIPPED(gu.req_type))
            {
                    strcat(fs.real_name, ".gz");
            }
            strcpy7(rtn.msg, "File '", fs.real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
	    } /* end small else */
    } /* End big else */
    
    /* Copy in the to company for reporting purposes */
    sscanf(lines[12], "%*s %s", gu.to_company);
    
    /* Send the return code */
    put_success_msg(key);
    
    /* If delete option is on or user did a get-delete, delete the file */
    if (((fs.dlt_flag == DBYES) || IS_REQUEST_GET_DELETE(gu.req_type)) && fs.dlt_flag != 3)
    {
        data_log(tr.log_file, "deleting data no longer needed");
	    /* Delete global file */
	    /* Build name for global file */
	    strcpy2(bf.file_name, GLOBAL_FILE_PATH, key);
	    unlink(bf.file_name);
    
        /* Delete the actual file */
        /* Build name for real file */
        strcpy3(bf.file_name, disk.path[fs.disk_index], key, ".gz");
        
        /* Delete real file */
        unlink(bf.file_name);
    } /* End if for deleting file */

    /* Modify the keyword to indicate file has been retrieved */
    keyword_mod();
#ifdef INTERCOM
    /* Update mask info if this is a mask shop */
    mask_info_update(key);
#endif
} /* End get_big_file_ack */
/*ts.files.fileuse*/
