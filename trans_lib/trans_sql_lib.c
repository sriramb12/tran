#include <mysql.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "close_down.h"
#include "customer_table.h"
#include "edc_defines.h"
#include "external_intercom_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_sql_lib.h"
#include "rtn.h"
#include "sql_prototypes.h"
#include "strcpy.h"
#include "trans_server_arrays.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "user_params.h"
#include "ldapfsl.h"
#include "global_debug.h"


/* Configurable edc database name */
extern char edc_database[];
extern char dt_database[];
extern MYSQL edc_sql;
extern MYSQL dt_sql;
extern int edc_init;


/* See if SQL error occurred */
int sql_err_check(char *query, MYSQL *sql, char *err_msg)
{
    DBG("query='%s' and err_msg='%s'", query, err_msg);

    int sql_errno = mysql_errno(sql);
    if (sql_errno != 0)
    {
         FORCE_DBG("FIXME: is it necessary to reset the error? \"sql_errno = 0;\"");
         FORCE_DBG("sql_errno=%d query=%s", sql_errno, query);
    }

    if (sql_errno != 0)
    {
        const char *err_ptr = mysql_error(sql);
        sprintf(err_msg, sql_err, sql_errno, query, err_ptr);
        data_log(tr.log_file, err_msg);
        FORCE_DBG("sql_errno=%d sql_error_msg=%s", sql_errno, err_msg);
        DBG("Will return FAILURE");
        return(FAILURE);

    } /* end if */
    return(SUCCESS);
} /* end sql_err_check */


/* Map status from the database to defined values */
int sql_status_map(char *status)
{
    DBG("status='%s'", status);

    if (strcmp(status, "inactive") == 0) return(INACTIVE);
    else if (strcmp(status, "active") == 0) return(ACTIVE);
    else if (strcmp(status, "approved") == 0) return(APPROVED);
    else if (strcmp(status, "pending") == 0) return(PENDING);
    else if (strcmp(status, "revoked") == 0) return(REVOKED);
    else if (strcmp(status, "rejected") == 0) return(REJECTED);
    else return(NOT_REGISTERED);
} /* end sql_status_map */

/* Map status from the database to defined values */
void sql_status_map_str(APPROVE_STATUS st, char *status)
{
    DBG("status='%d'", st);

    if (st == INACTIVE)      { sprintf(status, "%s", "INACTIVE"); }
    else if (st == ACTIVE)   { sprintf(status, "%s", "ACTIVE");   }
    else if (st == APPROVED) { sprintf(status, "%s", "APPROVED"); }
    else if (st == PENDING)  { sprintf(status, "%s", "PENDING");  }
    else if (st == REVOKED)  { sprintf(status, "%s", "REVOKED");  }
    else if (st == REJECTED) { sprintf(status, "%s", "REJECTED"); }
    else { sprintf(status, "%s", "NOT_REGISTERED"); }
} /* end sql_status_map_str */


int sql_insert_data(char *query, char *err_msg)
{
    int ret;
    /* Extra structure present to prevent memory overwrite */
    MYSQL sql1;
    extern MYSQL dt_sql;

    DBG("query=[%s]", query);

    /* Log for debugging purposes */
    data_log("query", query);

    /* Make sure the connection is still alive */
    DBG("will ping MySQL to check if still alive");

    ret = sql_connect(&dt_sql, DTMS_DB, dt_database);
    DBG("ret=%d errno=%d", ret, mysql_errno(&dt_sql));

    ret = mysql_query(&dt_sql, query);
    DBG("ret=%d errno=%d", ret, mysql_errno(&dt_sql));

    ret = sql_err_check(query, &dt_sql, err_msg);
    DBG("after sql_err_check(query, &dt_sql, err_msg); ret='%d'", ret);
    return(ret);
} /* end sql_insert_data */


MYSQL_RES * sql_result_get(char *query, int *errcode)
{
    extern MYSQL dt_sql;
    char err_msg[BUFFER_SIZE];
    int ret;
    MYSQL sql;
    MYSQL sql1;
    MYSQL_RES *result = NULL;

    DBG("query='%s' and errcode='%d'", query, errcode);

    sql_connect(&dt_sql, EXTERNAL_DTMS_DB, dt_database);

    data_log("query", query);
    ret = mysql_query(&dt_sql, query);
    DBG("mysql returned ret='%d' for query `%s`", ret, query);

    if (ret == SUCCESS)
    {
        /* Retrieve all of the records */
        result = mysql_store_result(&dt_sql);

        /* Check for errors */
        if((*errcode = sql_err_check(query, &dt_sql, err_msg)) != SUCCESS)
        {
            mysql_free_result(result);
            result = NULL;
        }
    }
    return(result);
} /* end sql_result_get */



int sql_generic_columm_retrieve(char *query, MYSQL_COLUMN_RETRIEVE *fields_info)
{
    MYSQL_ROW row;
    int num_fields;
    int num_rows;
    int errno;
    int counter;
    MYSQL_RES *result = NULL;

    DBG("query='%s'  number of fields = %d", query, fields_info->fields_counter);

    /* Perform the query */
    result = sql_result_get( query, &errno);

    /* If no result, indicate person not approved */
    if (result == NULL)
    {
        DBG("No results or query error");
        return(FAILURE);
    }

    /* Count number of fields */
    num_fields = mysql_num_fields(result);
    num_rows = mysql_num_rows(result);

    if ((num_fields < fields_info->fields_counter) || (num_rows < 1))
    {
        mysql_free_result(result);
        DBG("No results or query error");
        return(FAILURE);
    }

    row = mysql_fetch_row(result);
    for (counter = 0; counter < fields_info->fields_counter; counter++)
    {
        if (row[counter] != NULL)
        {
            strcpy(fields_info->field[counter], row[counter]);
        }
    }

    mysql_free_result(result);
    return(SUCCESS);
}


/* Retrieve a column from an SQL table */
int sql_column_rtv(char *query, char *column)
{
    MYSQL_COLUMN_RETRIEVE query_info;
    query_info.fields_counter = 1;
    query_info.field[0]       = column;
    return sql_generic_columm_retrieve(query, &query_info);
} /* end sql_column_rtv */


int sql2column_rtv(char *query, char *column1, char *column2)
{
    MYSQL_COLUMN_RETRIEVE query_info;
    query_info.fields_counter = 2;
    query_info.field[0]       = column1;
    query_info.field[1]       = column2;
    return sql_generic_columm_retrieve(query, &query_info);
}



int sql4column_rtv(char *query, char *column1, char *column2, char *column3, char *column4)
{
    MYSQL_COLUMN_RETRIEVE query_info;
    query_info.fields_counter = 4;
    query_info.field[0]       = column1;
    query_info.field[1]       = column2;
    query_info.field[2]       = column3;
    query_info.field[3]       = column4;
    return sql_generic_columm_retrieve(query, &query_info);
}



/* Add entry to dt_transactions table */
void sql_dt_transaction_insert (int rtn_status, char *rtn_msg)
{    
    extern METADATA md;
    extern char global_mask[];
    extern char global_secure_keyword[];
    char err_msg[1024];
    char *ptr;
    long currenttime = time(0);
    int transfertime = currenttime-tr.upd_time;
    float transferspeed;
    char query[8192];
    char starttime_string[48];
    char dlt_time_string[48];
    char transferspeed_string[32];
    int  length_deb = 0;

    if ( strcmp(req_array[gu.req_type],  "Company table rtv") == 0)
    {
        return;
    }

    DBG("rtn_msg='%s' and rtn_status=%d", rtn_msg, rtn_status);

    /* Convert start and end times to SQL format */
    sql_time_convert(starttime_string, tr.upd_time, NULL);
    sql_time_convert(dlt_time_string, fs.dlt_time, NULL);   

    /* Calculate transfer speed */
    if ((transfertime > 0) && (fs.original_size > 0))
        transferspeed = fs.original_size/(1024*1024*transfertime);
    else
        transferspeed = 0;

    sprintf(transferspeed_string, "%2.2f mbps", transferspeed);

    /* Get rid of trailing : and spaces */
    if ((ptr = strchr(fs.title, ':')) != NULL)
        ptr[0] = '\0';
    if ((ptr = strchr(so.program, ' ')) != NULL)
        ptr[0] = '\0';

    /* Take quotes out of return message */
    strexc(rtn_msg, '"', ' ');
    strexc(rtn.msg, '\'', ' ');

    /* Convert web browser to standard value */
    if (strncasecmp(gu.machine_type, "Web", 3) == 0)
        strcpy(gu.machine_type, "Web browser");

    length_deb = sprintf(query, " insert into dt_transactions \
    (file_keyword	, secure_keyword	, requester_email	, requester_name	, requester_site,  \
    request_type	, file_name	, description	, serversite	, destination_site	,  \
    tool	, tool_version	, platform	, ip_node_address	, cwd	,  \
    original_size	, compressed_size	, transfertime	, transferspeed	, icap_class,	  \
	    datatype, creationdate, startdate, expirationdate, \
 mask_id, mask_status, status_code, status_msg \
 ) values ( \
 \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \
 \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \
 \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \
    %llu, %llu, %d, \"%s\", \"%s\", \
    \"%s\", \"%s\", \"%s\", \"%s\", \
    \"%s\", \"%s\", %d, \"%s\")",
    fs.key, global_secure_keyword, gu.email1, user.name, user.site,
    req_array[gu.req_type],  fs.real_name,  md.description, gu.from_company, gu.to_company,
        so.program	, user.client_version	, gu.machine_type	, user.mach	, user.cwd,
        fs.original_size	, fs.compressed_size	, transfertime	, transferspeed_string	, gu.icap_class,
    fs.title, starttime_string, starttime_string, dlt_time_string,
    global_mask, "na", rtn_status, rtn_msg);

    DBG("%d bytes copied into query buffer sizeof(query)=%d", length_deb, sizeof(query));
    if (length_deb >= sizeof(query))
    {
        char msg[1024];
        sprintf(msg, "buffer overflow on %s:%d\n", __FILE__, __LINE__);
        fprintf(stderr, "%s\n", msg);
        DBG("%s" , msg);
        close_down(msg, 0);
    }

    /* Run command to put record in the SQL database */
    sql_insert_data(query, err_msg);
} /* End sql_dt_transaction_insert */


/* Insert disk usage stats */
void sql_dt_disk_insert (int percent)
{
    char err_msg[1024];
    char query[4096];
    char starttime_string[48];

    DBG();

    /* Convert start and end times to SQL format */
    sql_time_convert(starttime_string, tr.upd_time, NULL);

    sprintf(query, " insert into dt_disk \
        (serversite,  percent, startdate, starttime) values ('%s', %d, '%s', '%s')",
        server.site, percent, starttime_string, starttime_string);

    /* Run command to put record in the SQL database */
    sql_insert_data(query, err_msg);
} /* end dt_disk_insert */


/* Update timestamp in registry */
void sql_reg_timestamp_update(void)
{
    char query[2048];
    char err_msg[1024];
    DBG();

    /* If this is server set, get or authenticate, update */
    if ((gu.req_type != DBSERVER_SET) && (gu.req_type != DBX500AUTHEN) && (gu.req_type != DBGET_DATA))
        return;

    sprintf(query, "update dt_registry set account_status = 'active' , timestamp = now() where login = '%s' and ( site = '0' or site = '%s')", user.login, tr.user_site);
    sql_insert_data(query, err_msg);
} /* end sql_reg_timestamp_update */


/* Close edc sql connection */
void sql_edc_close(void)
{
    DBG();
    if (edc_init == DBYES)
    {
	    edc_init = DBNO;
	    /* Close SQL connection */
	    mysql_close(&edc_sql);
    } /* end if */
} /* end sql_edc_close */


/* Initialize structure for SQL queries */
void sql_edc_init(void)
{
    edc_init = DBNO;
    if (sql_connect(&edc_sql, EXTERNAL_DTMS_DB, edc_database) == SUCCESS)
    {
        edc_init = DBYES;
    }
    DBG("edc_init=%d", edc_init);
} /* end sql_edc_init */


/* Insert entry in the EDC database */
int sql_edc_insert_data(char *query, char *err_msg)
{
    int ret;
    /* Extra structure present to prevent memory overwrite */
    MYSQL sql1;
    extern MYSQL edc_sql;

    DBG("query='%s' and err_msg='%s'", query, err_msg);

    /* Log for debugging purposes */
    data_log("query", query);

    /* If connection not yet made, set it up */
    if (edc_init == DBNO)
        sql_edc_init();
    /* Make sure the connection is still alive */
    else if (mysql_ping(&edc_sql) != 0)
        sql_edc_init();

    ret = mysql_query(&edc_sql, query);
    sql_err_check(query, &edc_sql, err_msg);

    return(ret);
} /* end sql_edc_insert_data */


MYSQL_RES * sql_edc_result_get(char *query)
{
    extern MYSQL edc_sql;
    char err_msg[BUFFER_SIZE];
    int ret;
    MYSQL sql;
    MYSQL sql1;
    MYSQL_RES *result = NULL;

    DBG("query='%s'", query);

    /* Set up the EDC connection if not alredy present */
    if (edc_init == DBNO)
        sql_edc_init();
    /* Make sure the connection is still alive */
    else if (mysql_ping(&edc_sql) != 0)
        sql_edc_init();

    data_log("query", query);
    ret = mysql_query(&edc_sql, query);
    DBG("ret=%d errno=%d", ret, mysql_errno(&edc_sql));

    if (ret == SUCCESS)
    {
        /* Retrieve all of the records */
        result = mysql_store_result(&edc_sql);
        DBG("result=%p", result);

        /* Check for errors */
        if( sql_err_check(query, &edc_sql, err_msg) != SUCCESS)
        {
            mysql_free_result(result);
            result = NULL;
        }
        else
        {
            int rows =  mysql_num_rows(result);
            DBG("rows = %d", rows);
        }
    }
    return(result);
} /* end sql_edc_result_get */


/* Retrieve a column from an SQL table */
int sql_edc_column_rtv(char *query, char *column)
{
    MYSQL_ROW row;
    int num_fields;
    int num_rows;
    MYSQL_RES *result;

    DBG("query='%s' and column='%s'", query, column);

    /* Perform the query */
    result = sql_edc_result_get(query);

    /* If no result, indicate person not approved */
    if (result == NULL)
        return(FAILURE);

    /* Count number of fields */
    num_fields = mysql_num_fields(result);
    num_rows = mysql_num_rows(result);

    if ((num_fields < 1) || (num_rows < 1))
    {
        mysql_free_result(result);
        return(FAILURE);
    }

    row = mysql_fetch_row(result);
    strcpy(column, row[0]);
    mysql_free_result(result);
    return(SUCCESS);
} /* end sql_edc_column_rtv */


/* Check to see if this e-mail address is a NXP address */
int is_nxp(char *email_addr)
{
    int len;

    DBG("email_addr='%s'", email_addr);

    /* If @ suffix not present, will automatically be NXP */
    if (strchr(email_addr, '@') == NULL)
        return(DBYES);

    /* If length less than 8, cannot be e-mail nxp.com address */
    len = strlen(email_addr);

    if (len < 8)
        return(DBNO);
    else if (strcasecmp(email_addr + len-8, "@nxp.com") == 0)
        return(DBYES);
    else
        return(DBNO);
} /* end is_nxp */


/* Get manager's ID */
void sql_manager_rtv(char *manager, char *core_id)
{
    char query[2048];
    const char *unknown_manager  = "unknown";

    DBG("core_id='%s'", core_id);

    /* Initialize default value for manager */
    strcpy(manager, unknown_manager);

    sprintf(query, "select distinct mngid from edc.requests where coreid = '%s' and transtype = 'email'", core_id);

    if (sql_edc_column_rtv(query, manager) != SUCCESS || strcasecmp(manager, unknown_manager) == 0)
    {
        // work around while NXO extranet does not have manager field
#if defined (DMZ) && (defined(DEBUG) || defined(DEVEL))
      ldapfsl_setDevelopmentLdapServer();
#endif
        /* Look up user in LDAP */
        LDAP_RETURN_DATA * ldap_ret = ldapfsl_getManagerOfCoreId(core_id);
        if (GOOD_LDAP_RETURN_DATA(ldap_ret))
        {
            strcpy(manager, ldap_ret->info[0]->coreId);
        }
        ldapfsl_free_return_data(ldap_ret);
    }
#if defined (DMZ) && (defined(DEBUG) || defined(DEVEL))
      ldapfsl_setProductionLdapServer();
#endif
    DBG("manager='%s'", manager);
} /* end sql_manager_rtv */


/* Write approval to requests table */
void sql_commandline_approve(char *customer_email)
{
    char approver[16];
    char query[2048];
    char requester_email[64];
    char err_msg[BUFFER_SIZE];
    char *ptr;
    int sql_ret = 0;

    DBG("customer_email='%s'", customer_email);

    /* See if this domain is already approved */
    if (sql_domain_check(customer_email) == APPROVED)
    {
	    /* Point to the domain */
	    ptr = strchr(customer_email, '@');
	    ++ptr;
        close_down1p(domain_approve_err, ptr, DOMAIN_APPROVE_ERR);
    } /* End if for domain already approved */

    /* Strip off @ if present */
    strcpy(requester_email, gu.email1);
	if ((ptr = strchr(requester_email, '@')) != NULL)
    {
        if (strcasecmp(gu.email1, user.email) == 0)
        {
            strcpy(requester_email, user.login);
        }
	}

    /* Get the core ID for the manager from the pending approval */
    sprintf(query, "select distinct mngid from edc.requests where value = '%s' and transtype = 'email'", customer_email);

    /* Get the result */
    if(sql_edc_column_rtv(query, approver) != SUCCESS)
        close_down1p(pending_request_err, customer_email, PENDING_REQUEST_ERR);

    /* Make sure the approver is the same person performing the transaction */
    if ((strcasecmp(requester_email, approver) != 0) && (strcasecmp(requester_email, "rxfh80") != 0))
        close_down2p(approver_mismatch_err, approver, requester_email, APPROVER_MISMATCH_ERR);

    /* Get the customer's name */
    /* Build the string to log */
    sprintf(query, "update edc.requests set status='approved', term= '1_year', queue = 1, startdate = now(), enddate=date_add(NOW(), interval 12 month) where value = '%s'", customer_email);
    sql_ret = sql_edc_insert_data(query, err_msg);
    sql_edc_close();
    if (sql_ret != SUCCESS)
    {
        shut_down(err_msg, -1);
    }

    /* Tell submitter request submitted */
    put_short(SUCCESS);
    strcpy2(rtn.msg, customer_email, " approved to retrieve data from NXP.");

    return_success(rtn.msg, DBNO);
} /* end sql_commandline_approve */


/* Put in a pending approval sent from a command line request */
int sql_pending_request_insert(char *customer_email, char *own_id, char *description)
{
    char err_msg[BUFFER_SIZE];
    char query[4096];
    char customer_name[32];
    char requester_id[32];
    char manager[32];
    int sql_ret = 0;

    DBG("customer_email='%s', own_id='%s' and description='%s'", customer_email, own_id, description);

    /* If customer or domain already approved, let customer know */
    if (sql_approve_check(customer_email) == APPROVED)
    {
	    sprintf(rtn.msg, already_approved_err, customer_email);
	    return(ALREADY_APPROVED_ERR);
    }

    /* get the external customer's name from LDAP */
    strcpy(customer_name, "External Customer");

    /* Take @ off of e-mail address */
    strcpy(requester_id, own_id);

    /* Get the core ID for the manager */
    sql_manager_rtv(manager, requester_id);

    sprintf(query,
            "insert into edc.requests (transtype, name, value, justification, status, term, coreid, mngid, startdate, enddate, queue) values('%s', '%s', '%s', '%s', 'pending', '1_year', '%s', '%s', '0', '0', 1) on duplicate key update coreid = '%s', mngid = '%s', queue = 1", "email",
            customer_name, customer_email, description,
            requester_id, manager, requester_id, manager);

    sql_ret = sql_edc_insert_data(query, err_msg);
    sql_edc_close();
    return sql_ret;
} /* end sql_pending_request_insert */


/* Check customer registration */
int sql_external_reg_check(char *email_addr)
{
    DBG("email_addr='%s'", email_addr);

    /*if ( extranet_name_lookup(email_addr, user.name) != SUCCESS)*/
    /*     strcpy(user.name, "External Customer"); */

    /* If this is an authentication request, check will be done else where */
    if (gu.req_type == DBX500AUTHEN)
        return(APPROVED);

    /* Otherwise check approve status */
    else return(sql_approve_check(email_addr));
} /* end sql_reg_check */


/* Verify authorized to perform get transaction */
int sql_approve_check(char *email_addr)
{
    int email_status  = REJECTED;
    int domain_status = REJECTED;

    /* If this is a NXP customer, return approved */
    if ((strlen(email_addr ) > 8) && (strcasecmp(email_addr+strlen(email_addr)-8, "@nxp.com") == 0))
    {
        email_status = APPROVED;
    }
    else
    {
        /* At this point know this is an external customer */
        email_status  = sql_email_check(email_addr);
        domain_status = sql_domain_check(email_addr);
    }

    /* If either domain or email approved, customer is approved */
    if ((email_status == APPROVED) || (domain_status == APPROVED))
    {
        email_status = APPROVED;
    }

    DBG("email_addr='%s' ret=%d", email_addr, email_status);
    return(email_status);
} /* end sql_approve_check */


/* Verify user is approved to retrieve data */
int sql_email_check(char *email_addr)
{
    char query[2048];
    MYSQL_ROW row;
    int num_fields;
    int num_rows;
    MYSQL_RES *result;
    int ret = REJECTED;

    DBG("email_addr='%s'", email_addr);

    sprintf(query, "select status from edc.requests where value = '%s' and transtype = 'email'", email_addr);
    DBG("Will query `%s`", query);

    /* Get the result */
    result = sql_edc_result_get(query);
    DBG("result=%p", result);

    /* If no result, indicate person not approved */
    if (result == NULL)
    {
        ret = NOT_REGISTERED;
    }
    else
    {
	    /* Count number of fields */
	    num_fields = mysql_num_fields(result);
	    num_rows = mysql_num_rows(result);

	    if ((num_fields == 1) && (num_rows >= 1))
	    {
	        row = mysql_fetch_row(result);
            ret = sql_status_map(row[0]);
	    } /* end if */
        else
        {
            ret = NOT_REGISTERED;
        }
        mysql_free_result(result);
    }
    DBG("ret=%d", ret);
    return ret;
} /* End sql_email_check */


/* Verify user registered with specified domain */
int sql_domain_check(char *email_addr)
{
    char *ptr;
    char query[2048];
    MYSQL_ROW row;
    int num_rows;
    int num_fields;
    MYSQL_RES *result;
    int ret = REJECTED;

    DBG("email_addr='%s'", email_addr);

    /* convert e-mail to domain */
    if ((ptr = strchr(email_addr, '@')) == NULL) {
    	/* treat 'email_addr' as a domain name */
        ptr = email_addr;
    }
    else {
        /* get only domain part of email */
    	++ptr;
    }

    sprintf(query, "select status from edc.requests where value = '%s' and transtype = 'domain'", ptr);
    DBG("Will query `%s`", query);

    /* Get the result */
    result = sql_edc_result_get(query);
    DBG("result=%p", result);

    /* If no result, indicate person not approved */
    if (result == NULL)
    {
        ret = NOT_REGISTERED;
    }
    else
    {
        /* Count number of rows */
        num_rows = mysql_num_rows(result);
        num_fields = mysql_num_fields(result);
        if ((num_fields == 1) && (num_rows >= 1))
        {
            row = mysql_fetch_row(result);
            ret = sql_status_map(row[0]);
        } /* end if */
        else
        {
            ret = NOT_REGISTERED;
        }
        mysql_free_result(result);
    }
    DBG("ret=%d", ret);
    return ret;
} /* End sql_domain_checkd */


/* Calculate the end transfer time */
/* Calculate transfer time and insert into dt_transfertimes */
/* Records are inserted after get and put transactions complete */
void trans_transfertime_insert(void)
{
    extern unsigned long long start_transfertime;
    extern unsigned long long end_transfertime;
    extern char transfertype[];
    extern TRANS_FILE_STAT fs;
    char err_msg[1024];
    char query[2048];   
    float transfertime;
    float local_transfertime;
    float transferspeed;

    DBG();

    /* Determine if this is a transaction that needs to be logged */
    if (transfertype[0] == '\0') return;

    /* Calculate transfertime */
    local_transfertime = (end_transfertime-start_transfertime)/1000000.000;
    transfertime = (end_transfertime-start_transfertime);

    /* Calculate transferspeed */
    transferspeed = fs.original_size/local_transfertime/1024/1024;

    /* Source and destination sites switched for gets and puts */
    /* Following if accounts for this condition */
    if (strcmp(transfertype+strlen(transfertype)-3, "put") == 0)
        sprintf(query, "insert into data_transfer.dt_transfertimes (file_keyword, transfertype, source_ip, destination_ip, source_site, destination_site, original_size, compressed_size, starttime, endtime, transfertime, transferspeed) values('%s', '%s', '%s', '%s', '%s', '%s', %llu, %llu, %llu, %llu, %6.3f, %6.3f)",
            fs.key, transfertype, user.mach, server.address, user.site, server.site_name,
            fs.original_size, fs.compressed_size, start_transfertime, end_transfertime, transfertime, transferspeed);
    else
        sprintf(query, "insert into dt_transfertimes (file_keyword, transfertype, source_ip, destination_ip, source_site, destination_site, original_size, compressed_size, starttime, endtime, transfertime, transferspeed) values('%s', '%s', '%s', '%s', '%s', '%s', %llu, %llu, %llu, %llu, %6.3f, %6.3f)",
            fs.key, transfertype, server.address, user.mach, server.site_name, user.site,
            fs.original_size, fs.compressed_size, start_transfertime, end_transfertime, transfertime, transferspeed);
            sql_insert_data(query, err_msg);
} /* end trans_transfertime_insert */



int sql_generic_columm_retrieve_into_structure_array(char *query,
                                                     MYSQL_COLUMN_RETRIEVE *fields_info,
                                                     size_t structure_size,
                                                     int    structure_elements)
{
    MYSQL_ROW row;
    int num_fields;
    int num_rows;
    int errno;
    int counter;
    int row_counter = 0;
    MYSQL_RES *result = NULL;

    DBG("query='%s'  number of fields = %d", query, fields_info->fields_counter);

    /* Perform the query */
    result = sql_result_get( query, &errno);

    /* If no result, indicate person not approved */
    if (result == NULL)
    {
        DBG("No results or query error");
        return(FAILURE);
    }

    /* Count number of fields */
    num_fields = mysql_num_fields(result);
    num_rows = mysql_num_rows(result);

    if ((num_fields < fields_info->fields_counter) || (num_rows < 1))
    {
        mysql_free_result(result);
        DBG("No results or query error");
        return(FAILURE);
    }

    while ((row = mysql_fetch_row(result)) && ++row_counter <= structure_elements)
    {
        for (counter = 0; counter < fields_info->fields_counter; counter++)
        {
            if (row[counter] != NULL)
            {
                strcpy(fields_info->field[counter], row[counter]);
            }
            else
            {
                fields_info->field[counter][0] = '\0';
            }
            // set the address for the next element in the structure
            fields_info->field[counter] += structure_size;
        }
    }
    mysql_free_result(result);
    return(row_counter);
}



MYSQL * connect_to_data_transfer()
{
    extern MYSQL dt_sql;

    mysql_init(&dt_sql);
    if (sql_connect(&dt_sql, DTMS_DB,  "data_transfer") == SUCCESS)
    {
        return &dt_sql;
    }
    return NULL;
}


char * sql_search_domain(char * domain)
{
    static char abbreviation[8];
    char query[1024];
    MYSQL_COLUMN_RETRIEVE result;
    abbreviation[0] = 0;
    result.fields_counter = 1;
    result.field[0] = abbreviation;
    sprintf(query, "select abbreviation from data_transfer.dt_server_by_domain where network_domain = '%s'", domain);
    if (sql_generic_columm_retrieve(query, &result) == FAILURE)
    {
        // domain does not exist on the table,  insert it as abbreviation = NULL
        sprintf(query, "insert into data_transfer.dt_server_by_domain(network_domain) values('%s')", domain);
        sql_insert_data(query, "ERRON on saving data dt_server_by_domain");
    }
    return abbreviation;
}
