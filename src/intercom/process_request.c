/****************************************************************************
 *
 * Function:	process_request
 *
 * Description:	Main routine called to process database requests
 *		from the database client.  Calls other functions
 *		to perform specific transactions.
 *
 ***************************************************************************/

#include <mysql.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include "close_down.h"
#include "customer_table.h"
#include "edc_defines.h"
#include "edc_prototypes.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "mm_email_msg.h"
#include "trans_lib_prototypes.h"
#include "trans_server_arrays.h"
#include "trans_server_msg.h"
#include "trans_defines.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_paths.h"
#include "reg_globals.h"
#include "trans_server_macros.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "x500_defines.h"
#include "ldapfsl.h"
#include "global_debug.h"

void process_request(int socket)
{
    extern MYSQL dt_sql;
    extern char dt_database[];
    extern int edc_init;
    extern int request_logged;
    extern int global_disk_usage;
    extern int disk_cnt;
    extern char global_secure_keyword[];
    int reg_status =  APPROVED;
    char remote_dir[FILE_SIZE] = NONE;
    int ret;
    int len;
    extern int email_cnt;
    extern int log_cnt;
    char decrypt_pwd[32];

    DBG();

    ldapfsl_setProductionLdapServer();

    /* Indicate no email sent for the current transaction */
    /* Indicate no messages logged for the current transaction */
    /* Indicate no warnings or errors or global files found */
    rtn.len = log_cnt = email_cnt = 0;

    /* Indicate transaction not logged */
    request_logged = DBNO;

    /* Copy the socket value into the global variable */
    sock = socket;

    request_read();

    /* Initialize file name in case it is not used */
    strcpy(fs.real_name, NONE);

    /* Get the PID of the current process */
    tr.pid = getpid();
    sprintf(tr.pid_string, "%d", tr.pid);

    /* Get the current time and Build the log file name */
    bld_log_name();

#ifndef TRANSWEB
    /* Initialize the mask set name */
    /* do this only if data being sent to the mask shop */
    if (external_verify(gu.to_company) == SUCCESS)
    mask_name_generate(gu.key1);
#endif

    /* Build the user's site for future use */
    len = strcpy1(tr.user_site, user.login);
    get_site(user.addr, tr.user_site);

    /* Set secure keyword */
    global_secure_keyword[0] = '\0';
    disk_cnt = 10;

    /* Determine the type of customer */
    customer = customer_set(remote_dir);
    customer_type = customer_table[CUSTOMER_TYPE][customer];
    DBG("customer_type=%d", customer_type);

    /* If this is the GUI, decrypt user login */
    login_decrypt();

    /* Log the request */
    request_log();
    request_logged = DBYES;

    sql_connect(&dt_sql,EXTERNAL_DTMS_DB, dt_database);
    edc_init = DBNO;


    /* See if this is an authentication request */
    /* Authentication takes care of checking registry also */
    /* Following requests done before registration check because they also check registration */
    if (gu.req_type == DBX500AUTHEN)
    {
        x500authenticate(gu.email1, gu.key1, customer_type);
#if !defined(DEVEL_USING_SINGLE_PROCESS)
	    exit(0);
#else
    return;
#endif
    } else if (gu.req_type == DBX500LOOKUP)
    {
	    x500lookup(gu.key1);
#if !defined(DEVEL_USING_SINGLE_PROCESS)
	    exit(0);
#else
    return;
#endif
    } else if (gu.req_type == DBSITE_CHECK)
    {
	    site_check();
#if !defined(DEVEL_USING_SINGLE_PROCESS)
	    exit(0);
#else
    return;
#endif
    } /* end if */


    ldapfsl_user_lookup_and_copy_global_structures(user.login);
    DBG("user.login=%s user.email=%s gu.email1=%s gu.email2=%s", user.login, user.login, gu.email1, gu.email2);

    // if it is a single block from a parallel transfers
    // the Registry was already made by the main process (the parent)
    if (IS_SINGLE_BLOCK_FROM_PARALLEL_TRANSFER(gu.req_type))
    {
          reg_status = APPROVED;
    }
    else
    {
        /* If this is an external customer, initialize e-mail address */
        /* also check external registration */
        if (customer_type == EXTERNAL_CUSTOMER)
        {
            strcpy(gu.email1, user.login);

            /* Approve status is checked for external customers */
            reg_status = sql_external_reg_check(gu.email1);
        }
        else
        {
            /* See if registration for internal customer needs to be checked */
            /* Registration is checked for direct requests
             *   client needs to be user.client_version >=  "7.0"
             */
            if (strcmp(gu.par6, "indirect") != 0)
            {
                // client version less than 7.0 NOT check
                int not_check = user.client_version[1] == '.' && user.client_version[0] < '7';
                DBG("gu.req_type=%d user.client_version=%s not_check=%d", gu.req_type, user.client_version, not_check);
                if (not_check == 0) // do the Register check
                {
                    reg_status = sql_dt_registry_lookup(user.login, tr.user_site);
                }
            } /* end if for direct request */
            /* All internal customers are automatically approved */
            reg_status = APPROVED;
        } /* end else */
    }

    /* Initialize value for global mask */
    global_mask_set();
    lock_files_init();

    DBG("gu.req_type=%d", gu.req_type);

    switch (gu.req_type)
    {
        case DBSERVER_SET: server_set(); break;
	    /* External LDAP authentication */
	    case DBX500AUTHEN:
	    {
	        x500authenticate(gu.email1, gu.key1, customer_type);
	    }
	    break;

	    /* Send email that is present in the fss_email directory */
	    /* A cron job executes this command once a minute */
	    case DBPING:
	    case DBSILENT_PING:
	    {
	        put_short(SUCCESS);
	        sprintf(rtn.msg, "Ping to %s successful", server.site);
	        put_success_msg("");
	    }
	    break;

	    case DBGET_ZIPPED:
	    case DBGET_DATA:
	    case DBGETDLT_DATA:
	    case DBGET_SERIAL:
	    case DBGET_SEPARATE:
	    case DBGET_PARALLEL:
        case DBGET_LARGE_PARALLEL:
        case DBGETDLT_LARGE_PARALLEL:
        case DBGETDLT_SERIAL:
        case DBGET_SERIAL_ZIPPED:
        case DBGETDLT_SERIAL_ZIPPED:
        case DBGETDLT_SEPARATE:
	    {
	        get_big_file(gu.key1, reg_status);
	    }
	    break;

	    /* Put a big file in InterCom */
	    case DBPUT_DATA:
	    case DBPUTDLT_DATA:
	    case DBPUT_PARALLEL:
        case DBPUTDLT_PARALLEL:
	    case DBPUT_TAR_FILE:
	    case DBPUTDLT_TAR_FILE:
	    case DBPUT_FILES:
	    case DBPUTDLT_FILES:
	    case DBPUT_DUPLICATE_FILES:
	    case DBPUT_SEPARATE:
	    case DBPUT_LARGE_PARALLEL:
	    case DBPUTDLT_LARGE_PARALLEL:        
	    {
	        maskshop_version_check();
	    put_big_file(remote_dir, gu.key1, gu.original_size);
	    }
	    break;

	    case DBPUT_VERIFY_FILES:
	    case DBPUTDLT_VERIFY_FILES:
	    {
	        /* Verify version later than 6.4 being used */
	        maskshop_version_check();
	        mask_filelist_check_put(remote_dir, gu.key1, gu.original_size);
	    }
	    break;

	    case DBPUT_ACK: put_ack(gu.key1,  atoi(gu.par2)); break;
	    case DBGET_ACK: get_ack(gu.key1,  atoi(gu.par2)); break;

	    /* Perform 1 step upload with web interface */
	    /* This is not supported for TransFTP */
#ifndef TRANS_FTP
	    case DB1STEP_UPLOAD: trans1step_upload(gu.key1); break;

	    /* Initialize 1 step download */
	    case DB1STEP_DOWNLOAD: trans1step_download(gu.key1); break;

	    /* complete 1 step download */
	    case DB1STEP_DOWNLOAD_COMPLETE: trans1step_download_complete(gu.key1); break;

	    /* zip file after uploaded with 1 step upload */
	    case DB1STEP_ZIP: trans1step_zip(gu.key1); break;
#endif

	    /* Transfer parallel block of data */
	    case DBBLOCK_PUT: block_put(gu.key1, gu.original_size, atoi(gu.par2), gu.file_mode); break;
	    case DBSEPARATE_BLOCK_PUT: separate_block_put(gu.key1, gu.original_size, atoi(gu.par2), gu.file_mode); break;
	    case DBBLOCK_GET:  block_get(gu.key1, gu.original_size, gu.file_mode, atoi(gu.par1), atoi(gu.par2));  break;
	    case DBSEPARATE_BLOCK_GET: separate_block_get(gu.key1, gu.original_size, gu.file_mode, atoi(gu.par1), atoi(gu.par2));  break;

	    /* Delete a big file already present in InterCom */
	    case DBDLT_DATA: dlt_big_file(gu.key1); break;
	    case DBINFO_DATA: info_big_file(gu.key1); break;
	    case DBFILENAME_RTV: filename_rtv(gu.key1); break;
	    case DBHIST_DATA: history_big_file(gu.key1); break;
	    case TRANSWEB_HISTORY:  transweb_history(gu.key1); break;
	    case DBSTATUS_DATA: status_big_file(gu.key1); break;

	    /* Retrieve list of file keywords for this user */
	    case DBUSR_KEYS:  user_keys_rtv(); break;

	    /* Retrieve a list of secure keywords this user has received */
	    case DBREAD_KEYWORD:
	    case DBSECURE_KEYS:
	    case DBQUERY_KEYWORD:
	    {
	        external_secure_keys_rtv();
	    }
	    break;

	    /* Retrieve long listing of secure keywords */
	    case DBLONG_SECURE_KEYS_RTV: long_external_secure_keys_rtv(); break;

	    /* Retrieve long listing of file keywords */
	    case DBLONG_FILE_KEYS_RTV: long_file_keys_rtv(); break;
	    case DBEXTEND_DATA: extend_big_file(gu.key1, (int)gu.original_size); break;
	    case DBOWNER_DLT: owner_dlt(gu.key1); break;
	    case DBUSER_DLT: user_dlt(gu.key1); break;

	    /* Send specified user(s) encrypted file keyword */
	    /* 'gu.key1' holds the file keyword */
	    case DBSEND_KEYWORD: transweb_keyword_send(); break;

	    /* Perform auto registration */
	    /* Auto registration performed by the user */
	    case DBAUTO_REG:
	    {
	        /* For external customers do not allow auto registration */
	        if (customer_type == EXTERNAL_CUSTOMER)
	            shut_down(extern_autoreg_err, EXTERN_AUTOREG_ERR);

	        user_autoreg();
	    }
	    break;

	    /* Retrieves list of secure keywords sent to user */
	    case DBSET_USER_EMAIL:
	    {
	        /* Add new entry */
	        if (((ret = add_entry(GLOBAL_EMAIL_FILE, gu.email1)) != SUCCESS) && (ret != DUP_ENTRY))
	            shut_down(rtn.msg, ret);

    	    strcpy(rtn.msg, "You will receive Email when users retrieve files you have transferred.");
	        put_short(SUCCESS);
	        put_success_msg("");
	    }
	    break;

	    /* Cancel Email reception */
	    case DBCAN_USER_EMAIL:
	    {
	        /* Delete existing entry */
	        dlt_entry(GLOBAL_EMAIL_FILE, gu.email1);
    	    strcpy(rtn.msg, "You will not receive Email when users retrieve files you have transferred.");
	        put_short(SUCCESS);
	        put_success_msg("");
	    }
	    break;

	    /* Set Email reception on a file */
	    case DBSET_FILE_EMAIL:

	    /* Cancel Email reception on a file */
	    case DBCAN_FILE_EMAIL:
	    {
	        mod_email_big_file(gu.key1);
	    }
	    break;

	    /* Lock the file from retrieval */
	    case DBLOCK_DATA: lock_big_file(gu.key1); break;
	    case DBUNLOCK_DATA: unlock_big_file(gu.key1); break;

	    /* Delete user keys */
        case USER_KEYS_DELETE: user_keywords_delete(); break;

	    /* Delete expired files and check disk usage */
	    case DBCRON_DLT:  cron_dlt_all(); break;
	    case DBSIZE_CHECK:  size_check_big_file(gu.key1); break;
        case DBTRANSWEB_COMPANY_RTV: transweb_company_rtv(); break;
        case DBTRANSWEB_REPOSITORY_RTV: transweb_repository_rtv(); break;

	    /* Retrieve list of defined InterCom companies */
	    case DBCOMPANY_TABLE_RTV:  company_table_rtv(); break;
        /* case DBPUT_LIST_ALL: report_put_list(gu.par1, gu.par2); break; */

	    case DBTRANSWEB_LOG_RTV:
	    {
	        char full_file_name[64];

            /* If file name not initialized, use log from previous day */
            if (strcmp(gu.key1, NONE) == 0)
                bld_log_name4previousday(gu.key1);

	        /* Build full file name */
	        strcpy2(full_file_name, "report_log/", gu.key1);

	        get_intercom_file(full_file_name, gu.key1);
            sprintf(rtn.msg, "file '%s' retrieved.", full_file_name);
            DBG("returning DBNO");
	        return_success(rtn.msg, DBNO);
	    }
	    break;

	    /* Get the dir list for the files present */
	    case TRANSDIRLIST_RTV:
	    case TRANSDIRLIST_FILE_RTV:
	    {
	        transdirlist_rtv(gu.key1);
	    }
	    break;

	    /* Uploaded dirlist for file being put */
	    case TRANSDIRLIST_PUT: transdirlist_put(gu.key1); break;

	    /* Retrieve Intercom release, supported for both internal and external */
	    case DBGET_RELEASE: get_release(gu.key1); break;

	    /* Retrieve release information stored in the releases file */
	    case DBGET_RELEASE_INFO: release_info_rtv("releases/releases"); break;
	    case DBLIST_EXTENSIONS_RTV: list_extensions_rtv(); break;

#ifdef TRANS_FTP
	    /* Send encrypted file via FTP to TSMC over internet */
	    case DBFTP_RESEND: ftp_resend(remote_dir, gu.key1); break;
	    case DBFTP_GET: ftp_get(gu.key1, remote_dir); break;
	    case ENCRYPT_ITAR_DATA: encrypt_itar_data(); break;
#endif
	    case 555:
	    {
	        mask_filelist_check_put(remote_dir, gu.key1, gu.original_size);
	    }
	    break;

	    case DBMASK_FILELIST_SEND:
	    {
	        /* Some of the clients put the key in par1 and others in par2 */
	        if (strcmp(gu.par1, NONE) != 0)
	            mask_filelist_send(gu.key1, gu.par1);
	        else
	            mask_filelist_send(gu.key1, gu.par2);
	    }
	    break;

	    case DBMASK_HISTORY: maskshop_history_get(gu.key1); break;

	    /* Approve new user */
	    /* Trust a user */
	    case DBTRUST_USER:
	    {
	        if (strcmp(gu.par1, "revoke") == 0)
	            edc_trusted_sender_revoke(gu.key1, gu.par1, gu.par2);
	        else
	            edc_trust_sender(gu.key1, gu.par1, gu.par2);
	    }
	    break;

	    case EDC_COMMANDLINE_APPROVE:
	    {
	        /* Do not allow external customer to perform this request */
	        if (customer_type == EXTERNAL_CUSTOMER)
	            shut_down(unsupported_transaction_err, UNSUPPORTED_TRANSACTION_ERR);

	        sql_commandline_approve(gu.key1);
	    }
	    break;

	    case DBREQUEST_APPROVAL:
	    {
	        /* Do not allow external customer to perform this request */
	        if (customer_type == EXTERNAL_CUSTOMER)
	            shut_down(unsupported_transaction_err, UNSUPPORTED_TRANSACTION_ERR);

            if ((ret = sql_pending_request_insert(gu.key1, user.login, gu.par1)) != SUCCESS)
	            shut_down(rtn.msg, ret);

	        sprintf(rtn.msg, APPROVAL_REQUEST_MSG, gu.key1);
	        put_short(SUCCESS);
            DBG("returning DBNO");
	        return_success(rtn.msg, DBNO);
	    }
	    break;

	    case DBREQUEST_STATUS:
	    {
	        /* Do not allow external customer to perform this request */
	        if (customer_type == EXTERNAL_CUSTOMER)
	            shut_down(unsupported_transaction_err, UNSUPPORTED_TRANSACTION_ERR);

	        ret = sql_approve_check(gu.key1);
            	char map_msg[30] = "";
		sql_status_map_str(ret, map_msg);
                DBG("status msg=%s", map_msg);

	        sprintf(rtn.msg, STATUS_REQUEST_MSG, gu.key1, map_msg);
                DBG("formatted status msg=%s", rtn.msg);
	        put_short(SUCCESS);
                DBG("returning DBNO");
	        return_success(rtn.msg, DBNO);
	    }
	    break;

	    /* Do report list */
	    case DBEXTRACT_REGISTRY: sql_registry_extract(); break;
	    case DBREPORT_USAGE: report_usage(1, gu.par1, gu.par2,  gu.key1); break;
	    case DBREPORT_SUMMARY: report_usage(2, gu.par1, gu.par2, gu.key1); break;

	    /* Send email not previously sent */
	    case TRANSWEB_EMAIL_SEND:
	    {
	        email_dir_rtv();
	    }
	    break;

	    /* Retrieve FSS log files */
	    case FSS_RTV_LOGS: fss_rtv_logs(); break;
	    case DBSUPPORT_ADD: support_add(gu.key1); break;
	    case DBSUPPORT_DELETE: support_delete(gu.key1); break;

	    case DBSUPPORT_VIEW:
	    {
	        support_check(gu.key1);
            put_short(SUCCESS);
	        get_release_file(gu.key1, gu.key1+9);
	        /* Add 9 so db_admin path not printed */
	        sprintf(rtn.msg, SUPPORT_MSG, gu.key1+9);
            DBG("returning DBNO");
	        return_success(rtn.msg, DBNO);
	    }
	    break;

	    /* Retrieve name, phone, e-mail, etc. about user */
	    /* Also display sites where he is registered */
	    case DBQUERY_USER: query_user(DBQUERY_USER, gu.key1); break;

	    /* Retrieve name, phone, e-mail, etc. about user */
	    case DBFAST_QUERY_USER: query_user(DBFAST_QUERY_USER, gu.key1);  break;

	    /* Send registry URL to user */
	    case DBREG_URL:
	    {
	        put_short(SUCCESS);
            DBG("returning DBNO");
	        return_success(REGISTRATION_URL_MSG, DBNO);
	    }
	    break;

	    /* Verify user registered at this site */
	    case DBSITE_CHECK: site_check(); break;

	    /* Look up user in LDAP */
	    case DBX500LOOKUP: x500lookup(gu.key1); break;

	    /* Log subnet for encrypted password */
	    case DBENCRYPT_PASSWORD_LOG:
	    {
	        /* Decrypt the password */
	        decrypt_ascii_pwd(gu.key1, decrypt_pwd);
	        /* Make sure correct password entered */
                ret = ldapfsl_user_authenticate_and_copy_global_structures(gu.email1, decrypt_pwd);
	        if (ret != DBYES)
	        {
	    	    char msg[260];
	    	    strcpy3(msg, authen_err, RET2TAB, rtn.msg);
	    	    shut_down(msg, -1);
	        } /* end if */

	        put_short(SUCCESS);
            DBG("returning DBNO");
	        return_success(decrypt_pwd, DBNO);
	    }
	    break;

	    default:
	    {
	        /* Return failure code to client */
            DBG("returning BAD_REQ_TYPE");
	        close_down1d(bad_req_type, gu.req_type, BAD_REQ_TYPE);
	    } /* end default */

     } /* End switch */

    /* Exit child process */
#if !defined(DEVEL_USING_SINGLE_PROCESS)
    exit(0);
#endif
} /* End process_request */

#ifndef TRANSWEB
#ifndef TRANS_FTP
#ifndef INTERCOM
void rmt_server_read() { }
#endif
#endif
#endif

ftp_put_continuous() { }
long long ftp_get_parallel_size(char *remote_dir, int total_blocks, char *hms){ }
#ifdef TRANS_FTP
int core_id_crypt() { return(0);  }
int edc_trusted_sender_revoke() { return(0); }
int edc_trust_sender() { return(0); }
int is_trusted_sender() { return(0); }
#endif
void ftp_get(char *file_name, char *remote_dir) { }
/*es.files.fileuse*/
/*is.files.fileuse*/
