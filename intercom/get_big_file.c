/****************************************************************************
 *
 * Function:	get_big_file
 *
 * Description:	Retrieves a big file from the InterCom/TransWeb database.
 *
 ***************************************************************************/

#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "customer_table.h"
#include "external_intercom_prototypes.h"
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
#include "ldapfsl.h"
#include "global_debug.h"


void not_approved_err_send(char *key)
{
    extern TRANS_FILE_STAT fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char own_email1[32];
    char own_email2[32];
    char own_name[32];
    char *ptr;

    DBG();

    /* Get the name and e-mail for the owner */
    ldapfsl_user_lookup_and_copy_global_structures(fs.own_id);

    data_log2(tr.log_file, "own name is ", user.name);

    /* Get the first name */
    /* Build the message */
    sprintf(rtn.msg, not_approved_err,  user.name, gu.email2, user.name);
    shut_down(rtn.msg, NOT_APPROVED_ERR);
} /* end not_approved_err_send */


/* Key for big file to retrieve */
void get_big_file(char *key, int reg_status)
{
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    int tar_option;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    extern TRANS_FILE_STAT fs;
    extern unsigned long long separate_configured_size;
    extern unsigned long long parallel_configured_size;


    /* Open global file */
    read4info_big_file(key, &fs, DBNO);

    DBG("reg_statu=%d fs.own_id=[%s] gu.email1=[%s]", reg_status, fs.own_id, gu.email1);
  //  DBG_WAIT_LONGER();

	/* If the customer is not registered approved, do not allow him to retrieve the data */
    /* If sender is a trusted sender or sender and retriever the same */
    /* The customer can retrieve the data */
	if ((reg_status != APPROVED) && (strcasecmp(fs.own_id, gu.email1) != 0) && (is_trusted_sender(fs.own_id) == DBNO))
	{
	    /* Add a pending request */
	    if (sql_approve_check(gu.email1) != PENDING) {
            data_log2(tr.log_file, "adding pending request for ", gu.email1);
            sql_pending_request_insert(gu.email1, user.login, lines[15]);
	    } else {
            data_log2(tr.log_file, "Not adding pending request for ", gu.email1);
        }

		not_approved_err_send(gu.key1);
	} /* end if */

    /* If this file is locked for data transfer, give error and exit */
    if (fs.lock_flag == 'l')
    {
	    long long total_size;
	    char *msg = rtn.msg + sprintf(rtn.msg, trans_err, fs.real_name);

	    /* Pass in the total size */
	    sscanf(lines[2], "%*s %*s %lld", &total_size);
	    transfer_status_big_file(key, msg, total_size);

	    shut_down(rtn.msg, TRANS_ERR);
    }
    /* If this file is locked for user retrieval, give error and exit */
    else if (fs.lock_flag == 'L')
    {
	    sprintf(rtn.msg, block_err, fs.real_name, lines[5]+13, fs.own_id);
	    shut_down(rtn.msg, BLOCK_ERR);
    }
    if ((customer = INTERNAL_INTERCOM) || (customer == EXTERNAL_INTERCOM))
    {
        /* Only send as separate blocks if specified by the user */
        if ( IS_REQUEST_GET_SEPARATED(gu.req_type) && (fs.original_size > separate_configured_size)) {
            get_separate();
        }
        /* If serial specified or this is Windows, send serial */
        /* Also send serial if not compressed or less than 5 Mb */
        else if ( IS_REQUEST_GET_SERIAL(gu.req_type)
            || (strncasecmp(gu.machine_type, "Windows", 7) == 0)
            || (strncasecmp(gu.machine_type, "AIX", 3) == 0)
            || (strncasecmp(gu.machine_type, "Web_browser", 11) == 0)
            || (strncasecmp(gu.machine_type, "Web browser", 11) == 0)
	        || (fs.original_size != fs.compressed_size)
            || (fs.original_size < 5000000)) {

            tar_option = send_file(&fs);
        }

        /* Send as parallel if specified or size > configured size */
        else
        if ((fs.original_size >= parallel_configured_size) || IS_REQUEST_GET_PARALLEL(gu.req_type) || IS_REQUEST_GET_LARGE_PARALLEL(gu.req_type))
        {
            get_parallel();
        }
        else
        {
            /* Send as serial for any other conditions */
            tar_option = send_file(&fs);
        }
    }
    else tar_option = send_file(&fs);

    /* Send the ack */
    get_big_file_ack(key);
} /* end get_big_file */


void get_big_file_ack(char *key)
{
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    extern char program_name[];
    extern char global_secure_keyword[];
    char file_size_string[32];

    DBG();

    num_gmb_calc(file_size_string, fs.original_size);

    /* See if creator needs to be sent mail */
    /* Right now it is sent even if creator and owner are the same */
    if ((fs.email_flag == DBYES) || (search_email_file(fs.own_id, GLOBAL_EMAIL_FILE) == DBYES)
        || (search_email_file(gu.from_company, COMPANY_EMAIL_FILE) == DBYES))
        send_rtv_email(fs.own_id, lines);

    /* Build message based on file type */
    if (strcmp(gu.machine_type, WEB_BROWSER) == 0)
    {
        if (fs.file_type == DIRECTORY) {
            sprintf(rtn.msg+rtn.len, dir_wrn, fs.real_name, fs.real_name);
        } else if (fs.file_type == FILES) {
	        char *ptr = strchr(fs.real_name, ' ');
	        if (ptr != NULL) ptr[0] = '\0';
	            sprintf(rtn.msg+rtn.len, files_wrn, fs.real_name, fs.real_name);
        } else
            strcpy7(rtn.msg, "File '", fs.real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
    } else {
        if (fs.file_type == DIRECTORY) {
	        rtn.len = strcpy7(rtn.msg, "Directory '", fs.real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
        } else if (fs.file_type == FILES) {
	        strexc(fs.real_name, '`', ' ');
	        strcpy7(rtn.msg, "Files '", fs.real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
	    } else {
	        /* If retrieved as a compressed file, add the .gz suffix */
	        if (gu.req_type == DBGET_ZIPPED) strcat(fs.real_name, ".gz");
	            strcpy7(rtn.msg, "File '", fs.real_name, "' (", file_size_string, ") retrieved from ", program_name, ".");
	    } /* end small else */
    } /* End big else */

    /* Copy in the to company for reporting purposes */
    sscanf(lines[12], "%*s %s", gu.to_company);

    /* Send the return code */
    put_success_msg(key);

    /* If delete option is on or user did a get-delete, delete the file */
    if ( fs.dlt_flag == DBYES || (IS_REQUEST_GET_DELETE(gu.req_type) && fs.dlt_flag != 3))
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

    /* Update SQL with retrieval date */
    if (global_secure_keyword[0] != '\0')
        sql_keyword_mapping_update();

    /* Modify the keyword to indicate file has been retrieved */
    keyword_mod();

    /* Update mask info if this is a mask shop */
    maskshop_files_update(key);
} /* End get_big_file_ack */
