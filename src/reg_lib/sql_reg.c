/****************************************************************************
 *
 * Function:	sql_dt_registry_insert
 *
 * Description:	Insert dt_transaction into dt_transactions
 *
 ***************************************************************************/

#include "ldapfsl.h"
#include <mysql.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#include "customer_table.h"
#include "dbreg_errs.h"
#include "edc_defines.h"
#include "external_intercom_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "sql_prototypes.h"
#include "strcpy.h"
#include "trans_server_arrays.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "user_params.h"


#include "global_debug.h" // Mazieri's debug facilities

extern char dt_database[];

static int perform_auto_register()
{
    DBG();
    if (gu.req_type == DBSITE_CHECK)
    {
        DBG("returning REG_VERIFY_ERR=%d", REG_VERIFY_ERR);
        return(REG_VERIFY_ERR);
    }

    /* otherwise try to perform auto registration
     *
     * It asks user and password and once it authenticates, saves the record
     */
    return(sql_autoreg_user());
}



/* Insert entry in dt_registry table */
int sql_dt_registry_insert( char *login, char *core_id, char *friendly_email, char *user_name)
{
    char query[1024];
    char err_msg[1024];
    char site[16];
    int ret = 0;

// Dec 2019, Carlos Mazieri,  saving always '0', it is internal people only
#if 0
    if (strcasecmp(core_id, login) == 0)
        strcpy(site, "0");
    else strcpy(site, tr.user_site);
#else
    strcpy(site, "0");
#endif

    sprintf(query, "insert into data_transfer.dt_registry(login, core_id, friendly_email, user_name, site, account_status) \
        values ('%s', '%s', '%s', '%s', '%s', '%s') on duplicate key update account_status = 'active', friendly_email = '%s', user_name = '%s'",
        login, core_id, friendly_email, user_name, site, "active", friendly_email, user_name);

    DBG("query size = %d, query=%s", strlen(query), query);
    if (sql_insert_data(query, err_msg) == SUCCESS)
        ret = ACTIVE;
    else
        ret = NOT_REGISTERED;

    DBG("ret=%d login='%s', core_id='%s', friendly_email='%s' and user_name='%s'", ret, login, core_id, friendly_email, user_name);
    return ret;
} /* end sql_dt_registry_insert */


/* Verify user is present in the registry */
int sql_dt_registry_lookup(char *login, char *site)
{
    MYSQL_RES *result;
    char query[1024];
    int errno;

    DBG("login='%s', and site='%s'", login, site);

    // Dec 2019, Carlos Mazieri,  one row as active is enough, so 'site' is no longer used

    sprintf(query, "select core_id, friendly_email, user_name, site, account_status from data_transfer.dt_registry where login = '%s' and account_status = 'active'", login);

    DBG("will query for `%s`", query);

    /* Get the record or die due to SQL error */
    result = sql_result_get(query, &errno);
    if (errno == 0 && result != NULL &&  mysql_num_rows(result) >= 1)
    {
        return ACTIVE;
    }

    return perform_auto_register();
} /* end sql_dt_registry_lookup */


/* Query sites for specified user */
int sql_dt_registry_sites_lookup(char *core_id)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[1024];
    int num_fields;
    int num_rows;
    int cnt;
    int errno;

    DBG("core_id='%s'", core_id);

    /* Add extra enter needed whether data is found */
    rtn.len += strcpy1(rtn.msg+rtn.len, "\n");
    sprintf(query, "select login, site from data_transfer.dt_registry where core_id = '%s'", core_id);

    /* Get the record or die due to SQL error */
    result = sql_result_get(query, &errno);
    if (errno != 0) return(FAILURE);

    /* If no result person not registered */
    if (result == NULL) return(FAILURE);

    /* Count number of fields */
    num_fields = mysql_num_fields(result);
    num_rows = mysql_num_rows(result);

    if ((num_fields < 2) || (num_rows < 1))
        return(FAILURE);

    row = mysql_fetch_row(result);
    rtn.len += sprintf(rtn.msg+rtn.len, "\nSite:   %s - %s\n", row[0], row[1]);

    for (cnt =1; cnt < num_rows; ++cnt)
    {
	    if ((row = mysql_fetch_row(result)) != NULL)
	    rtn.len += sprintf(rtn.msg+rtn.len, "        %s - %s\n", row[0], row[1]);
    } /* end for */

    return(0);
} /* end sql_dt_registry_sites_lookup */


int sql_autoreg_user(void)
{
    int ret;
    char tmp_email[64];
    char user_pwd[32];
    char user_name1[64];
    char user_name2[64];
    char user_name3[64];

 /* it was decided to keep this block but disabled, one day it can tbe turned on */
#if 0
    extern int customer_type;

    /* Sep 18 2018, Carlos Mazieri
     *
     * Do auto registry without asking for a user and password, if the user exists in LDAP
     */
    if (customer_type != EXTERNAL_CUSTOMER && user.ldap_ok == DBYES)
    {
        DBG("internal user already checked by LDAP, so registry without asking the password");
        return sql_dt_registry_insert(user.login, user.login, user.email, user.name);
    }
#endif

    DBG();

    /* Indicate will be registered in this transaction */
    /* Tell client need to get user's x500 user ID and password */
	put_short(DBEMAIL);
	get_string(tmp_email);
    sscanf(tmp_email, "%s %s", gu.email1, user_pwd);

	/* Copy into secondary Email address also */
	strcpy(gu.email2, gu.email1);
	strcpy(user.email, gu.email1);

	/* Break the name into first, middle and last name */
	sscanf(user.name, "%s %s %s", user_name1, user_name2, user_name3);

	/* If user found, return success. Otherwise, return authentication error */
    if (( ret = dt_reg_user(user.login, tr.user_site, gu.email1, gu.email2,	user_name1, user_name2, user_name3, user_pwd)) >= 0)
        return(ret);
    else
        shut_down(authen_err, AUTHEN_ERR);
} /* End sql_autoreg_user */

int dt_reg_user(
    char *login,
    char *site,
    char *user_email1,	/* User's primary Email address */
    char *user_email2,	/* User's secondary Email address */
    char *user_name1,
    char *user_name2,
    char *user_name3,
    char *user_pwd)
{
    int use_secondary_email;
    int x500ret = 0;
    char local_email1[64];
    char *ptr;

    DBG("login='%s', site='%s' and user_pwd='%s'", login, site, user_pwd);

    /* Determine if secondary email address is a complete internet address */
    /* If so register user with it instead of the friendly email address */
    if ((strchr(user_email2, '@') != NULL) && (strcmp(user_email1, user_email2) != 0)) {
        DBG("Will use secondary email address '%s' for registration", user_email2);
        use_secondary_email = DBYES;
    } else {
        DBG("Will use primary email address '%s' for registration", user_email1);
        use_secondary_email = DBNO;
    }

    /* Indicate error message not initialized */
    rtn.len = 0;

    /* If length of Email address too short, inform user */
    if ((strlen(user_email1) <3) && (strlen(user_email2) <3))
    {
	    rtn.len = strcpy1(rtn.msg, bad_email_len);
	    return(FAILURE);
    } /* End if */

    /* Copy to local variable so it is not changed */
    strcpy(local_email1, user_email1);
    DBG("after strcpy(%s, %s)", local_email1, user_email1);

    /* Change to lower case */
    str_lower(local_email1);

    /* First search for user in x500 registry */
    /* Only do the search if valid password present */
    if (user_pwd[0] != '\0')
    {
        LDAP_RETURN_DATA * ldap_ret = ldapfsl_authenticate_user_id(login, user_pwd);
        if (BAD_LDAP_RETURN_DATA(ldap_ret))
        {
            ldapfsl_free_return_data(ldap_ret);
            ldap_ret = ldapfsl_authenticate_user_id(local_email1, user_pwd);
        }
        if (GOOD_LDAP_RETURN_DATA(ldap_ret))
        {
             /* Get the user's name */
               DBG("Login success");
               ldapfsl_copy_info_to_global_user_structure(ldap_ret);
               strcpy(login,       ldap_ret->info[0]->coreId);
               strcpy(user_email1, ldap_ret->info[0]->friendly_email);
               strcpy(user_email2, ldap_ret->info[0]->friendly_email);
               str_lower(login);
               ldapfsl_free_return_data(ldap_ret);
               return(sql_dt_registry_insert(login, login, user_email2, user.name));
        }
        ldapfsl_free_return_data(ldap_ret);
    } /* End if for valid OneIT password present */

    rtn.len = strcpy1(rtn.msg, authen_err);
    return(AUTHEN_ERR);
} /* End dt_reg_user */


/* Verifies that user is registered in the data_transfer.dt_registry */
void site_check(void)
{
    int ret;

    DBG();

    /* Log request so will know where message came from */
    request_log();

    /* Verify user is registered, done with the -o option */
    ret = sql_dt_registry_lookup(user.login, "0");

    if (ret == ACTIVE)
    {
	    strcpy(rtn.msg, "User registered.");
	    put_short(SUCCESS);
	    put_success_msg(NONE);
    } /* end if */
    else close_down1f(reg_verify_err, user.login, REG_VERIFY_ERR);
} /* end site_check */


/* Extract the registry from SQL */
void sql_registry_extract(void)
{
    FILE *fp;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[] = "select * from data_transfer.dt_registry order by login, site";
    char file_name[32];
    char old_dir[48] = "registry.old";
    char current_dir[48] = "registry";
    char new_dir[48] = "registry.new";
    int num_fields;
    int num_rows;
    int errno;
    int cnt;

    DBG();

    /* Create the directory for the registry */
    dir_create(new_dir);

    /* build base file name */
    strcpy(file_name, "registry.new/site_reg.a0");
    if ((fp = fopen(file_name, "w")) == NULL) close_down1m(wr_open_err, file_name, WR_OPEN_ERR);

    /* Get the record */
    result = sql_result_get(query, &errno);

    /* If no result, give error */
    if (result == NULL) shut_down(extract_err, EXTRACT_ERR);

    /* Count number of fields */
    num_fields = mysql_num_fields(result);
    num_rows = mysql_num_rows(result);

    if ((num_fields < 8) || (num_rows < 1))
        shut_down(extract_err, EXTRACT_ERR);

    for (cnt = 0; cnt < num_rows; ++cnt)
    {
	    row = mysql_fetch_row(result);

	    /* See if new file needs to be opened */
	    if (strncmp(row[1], file_name+22, 2) != 0)
	    {
	        strncpy(file_name+22, row[1], 2);
	        /* Verify this is a valid file name */
	        if (str_is_valid_filename(file_name+22) == -1)
	        {
	            data_log2(tr.log_file, "invalid file:  ", file_name);
	            continue;
	        } /* end if for invalid file name */

	        /* close the old registry file */
	        fclose(fp);
	        if ((fp = fopen(file_name, "w")) == NULL)
	            close_down1m(wr_open_err, file_name, WR_OPEN_ERR);
	    } /* end if */

	    fprintf(fp, "%s;%s;%s;%s;%s;%s;%s\n", row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
    } /* end for */

    /* Close the file */
    fclose(fp);

    /* Move new registration directory to final location */
    /* first delete old registry */
    if (access(old_dir, 0) == 0) dir_remove(old_dir);

    dir_rename(current_dir, old_dir);
    dir_rename(new_dir, current_dir);

    put_short(SUCCESS);

    sprintf(rtn.msg, "Extracted %d records from dt_registry.", num_rows);
    return_success(rtn.msg, DBNO);
} /* end registry_extract */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
