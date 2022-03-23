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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "server_lib.h"
#include "close_down.h"
#include "edc_defines.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_report.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "lock_file.h"
#include "mm_email_msg.h"
#include "registry_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_arrays.h"
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
#include "rmt_server.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "x500_defines.h"
#include "ldapfsl.h"
#include "global_debug.h"


/* Check for unsupported version */
void unsupported_version_check(void)
{
    DBG();

    extern char unsupported_version[10][8];
    int cnt;

    for (cnt = 0; unsupported_version[cnt][0] != '\0'; ++cnt)
    {
	    if (strcmp(user.client_version, unsupported_version[cnt]) == 0)
        {
	        strcpy3(rtn.msg, version_err, "\n\n", HELP_MSG);
	        shut_down(rtn.msg, VERSION_ERR);
	    } /* End if for unsupported version found */
    } /* end for */
} /* end unsupported_version_check */


/* check for Mass Market Request */
void mm_req_check(void)
{
    DBG();

    if (strcmp(gu.to_company, "Internal Mass Market Customer") == 0)
    {
	    strcpy(user.name, "NXP Support");
	    strcpy(user.email, SUPPORT_EMAIL);
    } /* end if */
} /* End mm_req_check */


void process_request(int socket)
{
    DBG("socket='%d'", socket);

    extern MYSQL dt_sql;
    extern char dt_database[];
    extern edc_init;
    extern char transfertype[];
    extern int customer_type;
    extern int request_logged;
    extern int global_disk_usage;
    int reg_status = ACTIVE;
    int fd;
    int ret;
    int fnd;
    int isgui;
    int len;
    extern int email_cnt ;
    extern int log_cnt;

    extern char cert7db[];
    ldapfsl_setCaCertFileName(cert7db);

    /* Indicate request not yet logged */
    request_logged = DBNO;

    /* Indicate no email sent for the current transaction */
    /* Indicate no messages logged for the current transaction */
    /* Indicate no warnings or errors or global files found */
    customer_type = TRANSCEND_CUSTOMER;
    rtn.len = email_cnt = log_cnt = 0;
    edc_init = DBNO;

    ldapfsl_setProductionLdapServer();

    transfertype[0] = '\0';

    /* Copy the socket value into the global variable */
    sock = socket;

    /* Read the socket for the string */
    request_read();

    /* Get the PID of the current process */
    tr.pid = getpid();

    /* Put PID in string format */
    sprintf(tr.pid_string, "%d", tr.pid);

    /* Get the current time and Build the log file name */
    bld_log_name();

    /* Check for MM put request */
    mm_req_check();

    /* Check for encrypted Core ID */
    if ((gu.req_type != DBX500AUTHEN) && (strcmp(user.client_version, "8.0") >= 0) && ((isgui = is_gui()) == DBYES))
    {
	    /* Decrypt the Core ID */
	    /* Only decrypt it if already encrypted */
	    if (strlen(user.login) > 7)
	        core_id_crypt(user.login, DECRYPT);

        /* Look up the user's email address */
        LDAP_RETURN_DATA * ldap_ret = NULL;
        ldap_ret = ldapfsl_lookupByCoreId(user.login);

        if (ldap_ret->result != LDAP_SUCCESS) {
            DBG("Failed to retrieve user data from LDAP after the login, shutting down the transaction");
            shut_down(rtn.msg, ldap_ret->errorMessage);
        }

        strcpy(gu.email1, ldap_ret->info[0]->friendly_email);
        strcpy(gu.email2, ldap_ret->info[0]->friendly_email);
        strcpy(user.name, ldap_ret->info[0]->name);

        /* Copy login into email for future use */
        strcpy(user.email, user.login);
    }

    /* Build the user's site for future use */
    get_site(user.addr, tr.user_site);

    sql_connect(&dt_sql, DTMS_DB, dt_database);

    /* Check registration */
    /* Must first see if this is an authentication request */
    /* if this is an X500 authentication request, do authentication */
    /* Authentication takes care of checking registry also */
    /* Following requests done before registration check because they also check registration */
    if (gu.req_type == DBX500AUTHEN)
    {
	    x500authenticate(gu.email1, gu.key1);
	    exit(0);
    } /* end if */
    else if (gu.req_type == DBX500LOOKUP)
    {
	    x500lookup(gu.key1);
	    exit(0);
    } /* end if */
    else if (gu.req_type == DBSITE_CHECK)
    {
	    site_check();
	    exit(0);
    }

    ldapfsl_user_lookup_and_copy_global_structures(user.login);

    /* Transcend registration also checks the TransWeb registration file */
#ifdef TRANSCEND
    // if it is a single block from a parallel transfers
    // the Registry was already made by the main process (the parent)
    if (IS_SINGLE_BLOCK_FROM_PARALLEL_TRANSFER(gu.req_type))
    {
          reg_status = ACTIVE;
    }
    else
    {
        /* Registration check not required for several types of requests */
        /* Registry check not required for requests where registration already checked */
        /* Also not required for server set, retrieve release, etc. */
        if ((isgui == DBYES) || (reg_check_required() == DBNO))
        {
            reg_status = ACTIVE;
        }
        else
        {
            reg_status = sql_dt_registry_lookup(user.login, tr.user_site);

            /* If this user is inactive, mark him active */
            if (reg_status == INACTIVE)
            {
                reg_status = ACTIVE;
            }
        }
    }
#endif
#ifdef TRANSWEB
    if ((reg_status = sql_dt_registry_lookup(user.login, "0")) < 0)
        shut_down(rtn.msg, reg_status);
    else reg_status = ACTIVE;

    /* Authentication required for TransWeb so entry can be added if not present */
#endif
    if (reg_status != ACTIVE)
        close_down1f(reg_verify_err, user.login, REG_VERIFY_ERR);

    /* Put the login name in lower case */
    str_lower(user.login);

    /* Now that core ID decrypted and registration checked, ready to log request */
    request_log();

    /* Initialize defaults for logging purposes */
    strcpy(fs.real_name, "0");
    fs.original_size = fs.compressed_size = 0;

    lock_files_init();

    /* Give error if this version of Transcend not supported */
    unsupported_version_check();

    switch (gu.req_type)
    {
	case DBSERVER_SET: server_set(); break;
	/* Retrieve list of companies */
	case DBCOMPANY_TABLE_RTV:  company_table_rtv(); break;
	case DBTRANSWEB_COMPANY_RTV: transweb_company_rtv(); break;
	case DBTRANSWEB_REPOSITORY_RTV: transweb_repository_rtv(); break;
	case DBGET_DATA:
	case DBGET_ZIPPED:
	case DBGET_SERIAL:
	case DBGET_SEPARATE:
	case DBGET_PARALLEL:
	case DBGETDLT_ZIPPED:
	case DBGETDLT_DATA:
    case DBGET_LARGE_PARALLEL:
    case DBGETDLT_LARGE_PARALLEL:
    case DBGETDLT_SERIAL:
    case DBGET_SERIAL_ZIPPED:
    case DBGETDLT_SERIAL_ZIPPED:
    case DBGETDLT_SEPARATE:
	{
	    get_big_file(gu.key1);
	}
	break;
	/* Get big file ack */
	case DBGET_ACK: get_ack(gu.key1,  atoi(gu.par2)); break;
	/* Verify user registered at this site */
	case DBSITE_CHECK: site_check(); break;
	/* Retrieve the daily log file from external server */
	case DBTRANSWEB_LOG_RTV:
	{
	    char full_file_name[64];
	    /* Build full file name */
	    strcpy2(full_file_name, "report_log/", gu.key1);
	    get_intercom_file(full_file_name, gu.key1);
	    sprintf(rtn.msg, "file '%s' retrieved.", full_file_name);
	    return_success(rtn.msg, DBNO);
	}
	break;
	case DBGET_RELEASE:
	{
	     get_release(gu.key1);
	}
	break;
	/* Perform 1 step upload with web interface */
	case DB1STEP_UPLOAD: trans1step_upload(gu.key1); break;
	/* Initialize 1 step download */
	case DB1STEP_DOWNLOAD: trans1step_download(gu.key1); break;
	/* complete 1 step download */
	case DB1STEP_DOWNLOAD_COMPLETE: trans1step_download_complete(gu.key1); break;
	/* zip file after uploaded with 1 step upload */
	case DB1STEP_ZIP: trans1step_zip(gu.key1); break;
	/* Transfer parallel block of data */
	case DBBLOCK_PUT:
    {
        DBG("before block_put(), gu.original_size='%d'", gu.original_size);
        block_put(gu.key1, gu.original_size, atoi(gu.par2), gu.file_mode); break;
        DBG("after block_put(), gu.original_size='%d'", gu.original_size);
    }
	case DBPUT_ACK: put_ack(gu.key1,  atoi(gu.par2)); break;
	case DBSEPARATE_BLOCK_PUT: separate_block_put(gu.key1, gu.original_size, atoi(gu.par2), gu.file_mode); break;
	case DBBLOCK_GET:
	case DBRANDOM_BLOCK_GET:
	{
        DBG("before block_get(), gu.original_size='%d'", gu.original_size);
	    block_get(gu.key1, gu.original_size, gu.file_mode, atoi(gu.par1), atoi(gu.par2));
        DBG("after block_get(), gu.original_size='%d'", gu.original_size);
	}
	break;
	case DBSEPARATE_BLOCK_GET:
    {
        DBG("before separate_block_get(), gu.original_size='%d'", gu.original_size);
        separate_block_get(gu.key1, gu.original_size, gu.file_mode, atoi(gu.par1), atoi(gu.par2));
        DBG("after separate_block_get(), gu.original_size='%d'", gu.original_size);
    }
    break;
	case DBRANDOM_BLOCK_PUT: random_block_put(gu.key1, atoi(gu.par2), gu.file_mode); break;
	case DBGET_ICOM_RELEASE:
	{
	     get_icom_release(gu.key1);
	}
	break;
	/* Put a big file in Transcend */
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
            DBG("before put_big_file(), gu.original_size='%d'", gu.original_size);
            put_big_file(gu.key1, gu.original_size);
            DBG("after put_big_file(), gu.original_size='%d'", gu.original_size);
	}
	break;
	/* Delete a big file already present in Transcend */
	case DBDLT_DATA: dlt_big_file(gu.key1); break;
	case DBINFO_DATA: info_big_file(gu.key1); break;
	case DBFILENAME_RTV: filename_rtv(gu.key1); break;
	case DBMODE_DATA: mode_big_file(gu.key1); break;
	case DBHIST_DATA: history_big_file(gu.key1); break;
	/* Retrieve list of DTMS test files */
	case DBLIST_DTMS_TEST_FILES: list_dtms_test_files(); break;
	/* Get the dir list for the files present */
	case TRANSWEB_HISTORY:  transweb_history(gu.key1); break;
	/* Get the dir list for the files present */
	case TRANSDIRLIST_RTV:
	case TRANSDIRLIST_FILE_RTV:
	{
	    transdirlist_rtv(gu.key1);
	}
	break;
	case TRANSDIRLIST_PUT: transdirlist_put(gu.key1); break;
	case DBEXTEND_DATA:
	{
        DBG("before extend_big_file(), gu.original_size='%d'", gu.original_size);
	    extend_big_file(gu.key1, (int)gu.original_size);
        DBG("after extend_big_file(), gu.original_size='%d'", gu.original_size);
	}
	break;
	/* Allow only the owner to delete the file */
	case DBOWNER_DLT: owner_dlt(gu.key1); break;
	/* Allow everyone to delete the file */
	case DBUSER_DLT: user_dlt(gu.key1); break;
	/* Send specified user(s) encrypted file keyword */
	/* 'gu.key1' holds the file keyword */
	/* the user's Email address */
	case DBSEND_KEYWORD:
	{
	    DBG("will call keyword_send()");
	    keyword_send();
	    DBG("after call keyword_send()");
    }
    break;
	/* Retrieve user file  keywords */
	case DBUSR_KEYS: user_keys_rtv(); break;
	/* The following 3 options retrieve a short list of secure keywords */
	case DBSECURE_KEYS:
	case DBREAD_KEYWORD:
	case DBQUERY_KEYWORD:
	{
	    secure_keys_rtv();
	}
	break;
	/* Retrieve long listing of secure keywords */
	case DBLONG_SECURE_KEYS_RTV: long_secure_keys_rtv(); break;
	/* Retrieve long listing of file keywords */
	case DBLONG_FILE_KEYS_RTV: long_file_keys_rtv(); break;
	case DBSET_USER_EMAIL:
	{
	    /* Add new entry */
	    if (((ret = add_file_entry("db_admin/", GLOBAL_EMAIL_FILE, user.email)) != SUCCESS) && (ret != DUP_ENTRY))
	        shut_down(rtn.msg, ret);

    	strcpy(rtn.msg, "You will receive Email when users retrieve files you have transcended.");
	    put_short(SUCCESS);
	    put_success_msg(NONE);
	}
	break;
	/* Cancel Email reception */
	case DBCAN_USER_EMAIL:
	{
	    /* Delete existing entry */
	    dlt_entry(GLOBAL_EMAIL_FILE, user.email);
    	    strcpy(rtn.msg, "You will not receive Email when users retrieve files you have transcended.");
	    put_short(SUCCESS);
	    put_success_msg(NONE);
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
	/* Verify user authorized to perform admin transactions */
	case DBADMIN_AUTH:
	{
	    char user_site[SITE_LEN];
	    strcpy2(user_site, user.login, tr.user_site);

	    /* Make sure user is authorized to perform the transaction */
    	if ((strcmp(user_site, admin.login1) != SUCCESS) && (strcmp(user_site, admin.login2)) != SUCCESS)
	        shut_down(not_auth_err, NOT_AUTH_ERR);

	    /* Otherwise indicate already authorized */
	    put_short(SUCCESS);
	    strcpy(rtn.msg, "You are authorized to perform administrative functions.");
	    put_success_msg(NONE);
	}
	break;
	/* Cleanup shared memory no longer needed */
	case SHARED_MEMORY_CLEANUP: shared_memory_cleanup(); break;
#ifdef TRANSCEND
	/* Delete old entries from the registry */
	case DBDLT_OLD_SITES:
	{
	    char *site_array[100000];

	    /* Make sure user is authorized to perform the transaction */
	    if ((strcmp(tr.user_site, admin.login1) != SUCCESS) && (strcmp(tr.user_site, admin.login2)) != SUCCESS)
	        shut_down(not_auth_err, NOT_AUTH_ERR);
	    /* Call function to build site array */
	    /*site_array_build(site_array, gu.key1);*/
	    /* Call function to delete entries */
	    /* Updated data put in tmp_registry */
	    /*dlt_old_sites(site_array);*/
	}
	break;
#endif
	/* Send user info about specified user(s) */
	case DBQUERY_USER: query_user(DBQUERY_USER, gu.key1); break;
	case DBFAST_QUERY_USER: query_user(DBFAST_QUERY_USER, gu.key1);  break;
    /* Registration for NXP */
	case DBAUTO_REG: user_autoreg(); break;
	/* Register a server site */
	case DBSERVER_REG:
	{
	    /* Make sure user is authorized to perform the transaction */
	    if ((strcmp(tr.user_site, admin.login1) != SUCCESS)
	    && (strcmp(tr.user_site, admin.login2)) != SUCCESS)
	    shut_down(not_auth_err, NOT_AUTH_ERR);
	    if ((fnd = add_server_site_entry(gu.key1, gu.email1, gu.email2))
	    == DBYES)
	    {
		char *msg = bf.buffer;
		strcpy3(rtn.msg, "Server registration performed for '", gu.key1,
		"'.\nEmail notification is being sent to the user.");
		put_short(SUCCESS);
		put_success_msg(NONE);
		msg += strcpy1(msg, "Transcend server  registration performed for:\n\n");
		if (strcmp(gu.email1, gu.email2) != 0)
		{
		    msg += strcpy3(msg, "    Server Site:        ", gu.key1, "\n");
		    msg += strcpy3(msg, "    Primary Email:    ", gu.email1, "\n");
		    msg += strcpy3(msg, "    Secondary Email:  ", gu.email2, "\n");
		} else {
		    msg += strcpy3 (msg, "    Server site:   ", gu.key1, "\n");
		    msg += strcpy3(msg, "    Email:  ", gu.email1,"\n");
		} /* End else */
		email_send ("", gu.email1,
		"Transcend Server Registration", bf.buffer);
	    } /* End if for successful */
	    else if (fnd == DUP_ENTRY)
	    {
		strcpy3(rtn.msg, "User '", gu.key1, "' re-registered.");
		shut_down(rtn.msg, DUP_ENTRY);
	    } /* End else */
	    else shut_down(rtn.msg, -1);
	    exit(0);
	}
	break;
	case DBREG_QUERY: reg_query(); break;
	/* Delete expired files and check disk usage */
	case DBCRON_DLT: cron_dlt_all(); break;
	case DBSIZE_CHECK:  size_check_big_file(gu.key1); break;
	    /* Retrieves release information stored in the releases file */
	case DBGET_RELEASE_INFO: release_info_rtv("releases/releases"); break;
	case DBGET_ICOM_RELEASE_INFO:
	{
	    /* Open release file */
	    if ((fd = open("intercom/releases", O_RDONLY)) == EOF)
	    close_down1m(rd_open_err, "releases/releases", RD_OPEN_ERR);
	    if ((len = read(fd, rtn.msg, BLOCK_SIZE)) < 1)
	    close_down1m(read_err, "releases/releases", READ_ERR);
	    rtn.msg[len] = '\0';
	    /* Close the file */
	    close(fd);
	    /* Tell client data will be sent */
	    put_short(SUCCESS);
	    /* Send return code */
	    put_success_msg(NONE);
	}
	break;
/*
	case DBDLT_X500: dlt_x500(); break;
*/
	/* Retrieve information for file extensions, */
	/* used to estimate file size ratios */
	case DBLIST_EXTENSIONS_RTV: list_extensions_rtv(); break;
	/* Get the file status */
	case DBSTATUS_DATA: status_big_file(gu.key1); break;
	case DBPING:
	case DBSILENT_PING:
	{
	    put_short(SUCCESS);
	    sprintf(rtn.msg, "Ping to %s successful", server.site);
	    put_success_msg(NONE);
	}
	break;
    case USER_KEYS_DELETE: user_keywords_delete(); break;
	/* Log that a password has been encrypted */
	case DBENCRYPT_PASSWORD_LOG:
	{
	    char decrypt_pwd[32];
        LDAP_RETURN_DATA * ldap_ret2 = NULL;

	    /* Decrypt the password */
	    decrypt_ascii_pwd(gu.key1, decrypt_pwd);
        ldap_ret2 = ldapfsl_authenticate_user_id(gu.email1, decrypt_pwd);

	    /* Make sure correct password entered, doing authentication */
	    if (ldap_ret2->result != LDAP_SUCCESS)
	    {
		    char msg[260];
		    strcpy3(msg, authen_err, RET2TAB, rtn.msg);
		    shut_down(msg, -1);
	    } /* end if */

        strcpy(gu.email1, ldap_ret2->info[0]->friendly_email);
        strcpy(gu.email2, ldap_ret2->info[0]->friendly_email);

	    put_short(SUCCESS);
	    return_success("password encrypted", DBNO);
	}
	break;
	/* Extract a copy of the registry */
	case DBEXTRACT_REGISTRY:  sql_registry_extract(gu.key1); break;
	/* Request EDC approval for external customer */
	case DBREQUEST_APPROVAL:
	{
	    if ((ret = sql_pending_request_insert(gu.key1, user.login, gu.par1)) != SUCCESS)
	    shut_down(rtn.msg, ret);
	    sprintf(rtn.msg, APPROVAL_REQUEST_MSG, gu.key1);
	    put_short(SUCCESS);
	    return_success(rtn.msg, DBNO);
	}
	break;
	case DBREQUEST_STATUS:
	{
	    ret = sql_approve_check(gu.key1);
            char map_msg[30] = "";
	    sql_status_map_str(ret, map_msg);
            DBG("status msg=%s", map_msg);

	    sprintf(rtn.msg, STATUS_REQUEST_MSG, gu.key1, map_msg);
            DBG("formatted status msg=%s", rtn.msg);
	    put_short(SUCCESS);
	    return_success(rtn.msg, DBNO);
	}
	break;
	/* Perform command line approval */
	case EDC_COMMANDLINE_APPROVE:
	{
	sql_commandline_approve(gu.key1);
	}
	break;
	case DBRTV_NAME_PHONE: close_down1d(bad_req_type, gu.req_type, BAD_REQ_TYPE); break;
	case DBSUPPORT_ADD: support_add(gu.key1); break;
	case DBSUPPORT_DELETE: support_delete(gu.key1); break;
	case DBSUPPORT_VIEW:
	{
	    support_check(gu.key1);
	    get_release_file(gu.key1, gu.key1+9);
	    /* Add 9 so db_admin path not printed */
	    sprintf(rtn.msg, SUPPORT_MSG, gu.key1+9);
	    return_success(rtn.msg, DBNO);
	}
	break;
/*
	case DBPUT_LIST_ALL: report_put_list(gu.par1, gu.par2); break;
*/
	/* Generate any type of list report */
	case DBREPORT_USAGE: report_usage(1, gu.par1, gu.par2, gu.key1); break;
	case DBREPORT_SUMMARY: data_log(tr.log_file, "calling summary"); report_usage(2, gu.par1, gu.par2, gu.key1); break;
	default:
	{
	    /* Return failure code to client */
	    close_down1d(bad_req_type, gu.req_type, BAD_REQ_TYPE);
	} /* end default */

  } /* End switch */

#if !defined(DEVEL_USING_SINGLE_PROCESS)
     DBG("will exit process_request()");
    exit(0);
#endif

} /* End process_request */

int fss_report_log() { }

/*ts.files.fileuse*/
