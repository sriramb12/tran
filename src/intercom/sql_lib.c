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
#include "strcpy.h"
#include "trans_server_arrays.h"
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


/* Add entry to intercom_history table */
void sql_intercom_history_insert( char *keyword, char *file_name, char *core_id, 
    unsigned long long original_size, unsigned long long compressed_size)
{
    char err_msg[BUFFER_SIZE];
    char *ptr;
    char query[8000];
    FILE *fp;
    char history_file[32];

    DBG();

    /* Transactions logged only for TransFTP */
    strcpy3(history_file, "history_data/", keyword, ".history");
    fp = fopen(history_file, "r");
    line_get(fp, rtn.msg);
    history_info_get(fp);

    /* Strip off trailing e-mail address if present */
    if ((ptr = strchr(core_id, '@')) != NULL)
    {
        LDAP_RETURN_DATA *ret = ldapfsl_lookupByMail(core_id);
        if (BAD_LDAP_RETURN_DATA(ret))
        {
            ldapfsl_free_return_data(ret);
            ret = ldapfsl_lookupByCoreId(core_id);
        }
        if (GOOD_LDAP_RETURN_DATA(ret))
        {
            core_id = ret->info[0]->coreId;
        }
        ldapfsl_free_return_data(ret);
    }
    /* Build query to execute */
    sprintf(query, "insert into intercom_history\
(keyword, core_id, file_name, original_size, compressed_size, history)\
 values('%s', '%s', '%s', %llu, %llu, '%s')",
keyword, core_id, file_name, original_size, compressed_size, rtn.msg);
    if (sql_insert_data(query, err_msg) != SUCCESS) 
    data_log("query", rtn.msg);
} /* End sql_intercom_history_insert */

/* Update existing entry in intercom_history table */
void sql_intercom_history_update(char *keyword)
{
    char err_msg[BUFFER_SIZE];
    char query[10000];
    FILE *fp;
    char history_file[32];

    DBG();

   /* Transactions logged only for TransFTP */
    strcpy3(history_file, "history_data/", keyword, ".history");
    fp = fopen(history_file, "r");
    line_get(fp, rtn.msg);
    history_info_get(fp);
    /* Build query to execute */
    strcpy5(query, "update intercom_history set history = '",
    rtn.msg, "' where keyword = '", keyword, "'");
    if (sql_insert_data(query, err_msg) != SUCCESS) 
    data_log("query", rtn.msg);
} /* End sql_intercom_history_update */

void sql_gdb_data_rtv(char *extra_data, int gdb_id)
{
    int num_fields;
    int len = 0;
    char query[1024];
    MYSQL_ROW row;

    DBG();

    /* Indicate no data found yet */
    extra_data[0] = '\0';
sprintf(query, "select gdb_file from repository_dessync where id = %d", gdb_id);
    num_fields = gdb_data_get(query, &row);
    if (num_fields == 1)
    len += sprintf(extra_data+len, "File name:  %s\n", row[0]);
sprintf(query, "select gdbsize, gdbsizegz from repository_metadata_results where repository_uid = %d", gdb_id);
    num_fields = gdb_data_get(query, &row);
    if (num_fields == 2)
	        len += sprintf(extra_data+len, "Uncompressed GDS size:  %s bytes\nCompressed GDS size:  %s bytes\n",
	    row[0], row[1]);
sprintf(query, "select name from repository_topcell where job_uid = %d", gdb_id);
    num_fields = gdb_data_get(query, &row);
    if (num_fields == 1)
	    len += sprintf(extra_data+len, "Top cell:  %s\n",
	    row[0]);
sprintf(query, "select extents_x1, extents_y1, extents_x2, extents_y2 from repository_metadata_results where repository_uid = %d", gdb_id);
    num_fields = gdb_data_get(query, &row);
    if (num_fields == 4)
	    len += sprintf(extra_data+len, "Die extents:  %s, %s, %s, %s\n",
	    row[0], row[1], row[2], row[3]);
    data_log("data_log", extra_data);
} /* end sql_gdb_data_rtv */

int gdb_data_get(char *query, MYSQL_ROW *row)
{
    int num_rows;
    int num_fields = 0;
    MYSQL_RES *result;

    DBG();

    /* Set row to NULL in case it is not found */
    *row = NULL;
    result = result1get(EXTERNAL_DTMS_DB, "repository", query);
    if (result != NULL)
    {
    num_rows = mysql_num_rows(result);
	if (num_rows >= 1)
	{
	num_fields = mysql_num_fields(result);
	*row = mysql_fetch_row(result);
	return(num_fields);
	} /* end if for one or more rows */
    } /* end if for result found */
    return(num_fields);
} /* end gdb_get_data */

/* Update the send2foundry_results table */
void sql_send2foundry_results_update(char *job_number, char *keyword)
{
    char err_msg[BUFFER_SIZE];
    char query[1024];

    DBG();

    if (job_number && *job_number >= '0' && *job_number <= '9')
    {
        sprintf(query, "update repository.repository_send2foundry_results set keyword = '%s' where jobnumber = %s", keyword, job_number);
        /* Nov 14 2014, Carlos Mazieri
         *
         * This if must be wrong since sql_insert_data() only accepts 2 parameters
         *
         * decision: comment the code
         */
        // if (sql_insert_data(EXTERNAL_DTMS_DB, "repository", query, err_msg) != SUCCESS)
        if (sql_insert_data(query, err_msg) != SUCCESS)
        {
            data_log("query", rtn.msg);
        }
    }
} /* End sql_send2foundry_results_update */

/* Retrieve the mask, design and technology for a specified UID */
int sql_mtd_rtv(char *id, char *gdb_file, char *tag_name, char *mask, char *technology, char *design, char *gdb_extension)
{
    char db[20] = "repository";
    int num_rows;
    int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[1024];
    char technology_uid  [12];
    char artwork_type_uid[12];

    DBG();

    strcpy2(query, " select gdb_file, tag_name, maskid, design_name, technology, artwork_type from repository_dessync where id = ", id);
    result = result1get(EXTERNAL_DTMS_DB, db, query);
    /* Parse out needed information */
    /* Count the number of rows */
    /* There should be one row */
    num_rows = mysql_num_rows(result);
    num_fields = mysql_num_fields(result);
    if ((num_fields != 6) || (num_rows < 1))
    close_down1f("bad_dessync_id", id, -1);
    /* Get the row */
    row = mysql_fetch_row(result);
    strcpy(gdb_file, row[0]);
    strcpy(tag_name, row[1]);
    strcpy(mask, row[2]);
    strcpy(design, row[3]);
    strcpy(technology_uid, row[4]);
    strcpy(artwork_type_uid, row[5]);

    /* now get the extension of the GDB */
    sprintf(query, "select extension from repository_gdb_artwork_type"
                   "   where uid = %s and active", artwork_type_uid);
    data_log("query", query);
    result = result1get(EXTERNAL_DTMS_DB, db, query);
    num_rows = mysql_num_rows(result);
    num_fields = mysql_num_fields(result);
    if ((num_fields != 1) || (num_rows < 1))
    {
        strcpy(gdb_extension, "gds");
    }
    else
    {
        row = mysql_fetch_row(result);
        strcpy(gdb_extension, row[0]);
    }

    /* Now get the technology name */
    sprintf(query, "select common_full_technology from common_technologies where common_id_technology = %s and common_active = 1", technology_uid);
    data_log("query", query);
    strcpy(db, "common");
    result = result1get(EXTERNAL_DTMS_DB, db, query);
    /* Parse out needed information */
    /* Count the number of rows */
    /* There should be one row */
    num_rows = mysql_num_rows(result);
    num_fields = mysql_num_fields(result);
    if ((num_fields != 1) || (num_rows < 1))
    {
        close_down1f("bad_tech_id", row[4], -1);
    }
    /* Get the row */
    row = mysql_fetch_row(result);
    strcpy(technology, row[0]);
} /* End sql_mtd_rtv */

/* Check approve status of customers */
/* Array of e-mail addresses passed in */
/* The purpose for this routine is to see if any approval requests are pending */
int new_sql_pre_auth_check(char *email_addr[], int auth_status[])
{
    int total_auth_status = APPROVED;
    int cnt;

    DBG();

    /* Go through the array checking status */
    for (cnt = 0; email_addr[cnt] != '\0'; ++cnt)
    {
	    /* First verify this is not a NXP customer */
	    if (is_nxp(email_addr[cnt]) == DBYES)
	        auth_status[cnt] = APPROVED;
	    else if ((auth_status[cnt] = sql_approve_check(email_addr[cnt])) != APPROVED)
	        total_auth_status = PENDING;
	} /* end for */

    return(total_auth_status);
} /* sql_pre_auth_check */

/* Check approve status of customers */
/* Array of e-mail addresses passed in */
/* The purpose for this routine is to see if any approval requests are pending */
int sql_pre_auth_check(char *email_addr[])
{
    int reg_status;
    int cnt;

    DBG();

    /* Go through the array checking status */
    for (cnt = 0; email_addr[cnt] != '\0'; ++cnt)
    {
	    /* First verify this is not a NXP customer */
	    if (is_nxp(email_addr[cnt]) == DBYES)  continue;

	    reg_status = sql_approve_check(email_addr[cnt]);
        if ((reg_status == NOT_REGISTERED) || (reg_status == PENDING) || (reg_status == REVOKED))
	        return(PENDING);
	} /* end for */

    /* If reached this point, all users are approved or rejected */
    return(APPROVED);
} /* sql_pre_auth_check */
/*es.files.fileuse*/
/*is.files.fileuse*/
