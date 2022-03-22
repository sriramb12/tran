#include <malloc.h>
#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_sql_lib.h"
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities

extern char dtms_db[];

#define MWR_QUERY \
"select mwr_tapeout_date, mwr_id, mwr_product_name, mwr_id_request_type from mwr_requests_v where mwr_maskset = '%s' and \
(mwr_final_signoff=1 or mwr_final_signoff=0) and (mwr_state = 4 or mwr_state = 5 or mwr_state = 7) order by mwr_id desc"

/* Indicates which database is the default */
/* Get result from an SQL query */
MYSQL_RES * result_get(char *site, char *db, char *query1, char *query2)
{
    DBG("site='%s', db='%s', query1='%s' and query2='%s'", site, db, query1, query2);

    int sql_errno;
    int ret;
    MYSQL sql;
    MYSQL_RES *result;

    mysql_init(&sql);
    sql_connect(&sql, site, db);

    data_log("query", query1);
    ret = mysql_query(&sql, query1);
    if ((query2 != NULL) && (query2[0] != '\0'))
        ret = mysql_query(&sql, query2);

    /* Retrieve all of the records */
    result = mysql_store_result(&sql);

    /* Check for errors */
    sql_errno = mysql_errno(&sql);
    if (sql_errno != 0)
    {
	    const char *err_ptr = mysql_error(&sql);
        DBG("Mysql error code=%d msg=%s, exiting...", sql_errno, err_ptr);
	    sprintf(rtn.msg, sql_err, sql_errno, query1, err_ptr);
        data_log("messages", rtn.msg);
	    shut_down(rtn.msg, SQL_ERR);
    } /* End if */

    mysql_close(&sql);
    /* mysql_library_end(); */

    return(result);
} /* end result_get */


MYSQL_RES * result1get(char *site, char *db, char *query1)
{
    DBG("site='%s', db='%s' and query1='%s'", site, db, query1);

    int sql_errno;
    int ret;
    MYSQL sql;
    MYSQL_RES *result;

    mysql_init(&sql);
    sql_connect(&sql, site, db);

    data_log("query", query1);
    ret = mysql_query(&sql, query1);

    /* Retrieve all of the records */
    result = mysql_store_result(&sql);

    /* Check for errors */
    sql_errno = mysql_errno(&sql);

    if (sql_errno != 0)
    {
	    const char *err_ptr = mysql_error(&sql);
        DBG("Mysql error code=%d msg=%s, exiting...", sql_errno, err_ptr);
	    sprintf(rtn.msg, sql_err, sql_errno, query1, err_ptr);
        data_log("messages", rtn.msg);
	    shut_down(rtn.msg, SQL_ERR);
    } /* End if */

    mysql_close(&sql);
    /* mysql_library_end(); */

    return(result);
} /* end result1get */


/* Get result from an open connection */
MYSQL_RES * connected_result_get(MYSQL *sql, char *query)
{
    DBG("query='%s'", query);

    int sql_errno;
    int ret;
    MYSQL_RES *result;

    ret = mysql_query(sql, query);
    /* Retrieve all of the records */

    result = mysql_store_result(sql);
    /* Check for errors */

    sql_errno = mysql_errno(sql);
    if (sql_errno != 0)
    {
	    const char *err_ptr = mysql_error(sql);
        DBG("Mysql error code=%d msg=%s, exiting...", sql_errno, err_ptr);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
        data_log("messages", rtn.msg);
	    shut_down(rtn.msg, SQL_ERR);
    } /* End if */

    return(result);
} /* end connected_result_get */


int insert_data(char *site, char *db, char *query, int report_err_flag)
{
    DBG("site='%s', db='%s' and query='%s'", site, db, query);

    int sql_errno;
    int ret;
    /* Extra structure present to prevent memory overwrite */
    MYSQL sql;

    /* Log for debugging purposes */
    data_log2("query", db, query);
    mysql_init(&sql);
    sql_connect(&sql, site, db);

    ret = mysql_query(&sql, query);
    sql_errno = mysql_errno(&sql);
    if (sql_errno != 0)
    {
	    const char *err_ptr = mysql_error(&sql);
        DBG("Mysql error code=%d msg=%s, will not exit, will keep running", sql_errno, err_ptr);
	    if (report_err_flag == DBYES)
	    {
	        sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
            data_log(tr.log_file, rtn.msg);
	    }
	    else data_log(tr.log_file, err_ptr);

	    ret = FAILURE;
    } /* End if */
    else ret = SUCCESS;

    mysql_close(&sql);
    /* mysql_library_end(); */

    return(ret);
} /* end insert_data */


/* Initialize a connection in preparation for multiple queries */
int sql_connect(MYSQL *sql, char *host, char *db_name)
{
    int ret = -1;
    DBG("sql='%p', db_name='%s' and host='%s'", sql, db_name, host);

    if (sql != 0)
    {
        if (sql->db != 0 && mysql_ping(sql) == 0)
        {
            DBG("already connected");
            ret = SUCCESS; // already connected
        }
        else
        if (mysql_init(sql) && mysql_real_connect(sql, host, "rxfh80", "DR2dr33m", db_name, 0, 0, 0))
        {
            DBG("connected");
            ret = SUCCESS; // connection OK
        }
        else
        {
            char err_msg [512];
            const char *err_ptr = mysql_error(sql);
            sprintf(err_msg, "ERROR: could not connect to Mysql host=%s errno=%d MySqlMessage=%s on %d:%d",
                      host, mysql_errno(sql), err_ptr, __FILE__, __LINE__);
            data_log(tr.log_file, err_msg);
            FORCE_DBG("%s", err_msg);
            exit(1);
        }
    }
    DBG("ret=%d" , ret);
    return ret;
} /* end sql_connect */


/* Perform a query to an open connection */
int sql_query(MYSQL *sql, char *query)
{
    DBG("sql='%s' and query='%s'", sql, query);

    int sql_errno;
    mysql_query(sql, query);

    /* Check for errors */
    sql_errno = mysql_errno(sql);
    if (sql_errno != 0)
    {
	    const char *err_ptr = mysql_error(sql);
	    sprintf(rtn.msg, sql_err, sql_errno, query, err_ptr);
        data_log("messages", rtn.msg);
    } /* End if */
} /* end sql_query */


/* Add in the date */
void mwr_date_copy(char *project, char *date)
{
    DBG("project='%s' and date='%s'", project, date);

    /* copy in the month */
    strncpy(project, date+5, 2);

    /* copy in the day */
    strncpy(project+3, date+8, 2);

    /* copy in the year */
    strncpy(project+6, date+2, 2);
} /* End mwr_date_copy */


/* Find out if DTMS is required for this MWR */
int is_dtms_required(char *mwr_num)
{
    DBG("mwr_num='%s'", mwr_num);

    char query[1024];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int num_rows;

    sprintf(query, "select mwr_dtms_required from mwr_dtms where mwr_dtms_mwr_num = \"%s\"", mwr_num);
    data_log("query", query);
    result = result_get(DTMS_DB, "mwr", query, NULL);

    /* Check for result from query */
    if (result!= NULL)
    {
	    /* Get the number of rows */
        num_rows = mysql_num_rows(result);

	    /* If there is one row read in the value */
        if (num_rows >= 1)
        {
	        row = mysql_fetch_row(result);
	        /* See if value set to no */
	        if (strcasecmp(row[0], "no") == 0) return(DBNO);
	        else return(DBYES);
	    } /* End if for one row */
    } /* End if for result found */

    /* If reached this point do not know result so assume DTMS needed */
    return(DBYES);
} /* End dtms_required */


int is_mwr_complete(char *mask)
{
    DBG("mask='%s'", mask);

    char query[1024];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int num_rows;

    sprintf(query, MWR_QUERY, mask);
    data_log("query", query);
    result = result_get(DTMS_DB, "mwr", query, NULL);

    /* Check for result from query */
    if (result!= NULL)
    {
	    /* Get the number of rows */
        num_rows = mysql_num_rows(result);

	    /* If there is one row read in the value */
        if (num_rows >= 1)
        {
	        row = mysql_fetch_row(result);

	        /* See if value set to no */
	        if (strcasecmp(row[0], "no") == 0) return(DBNO);
	        else return(DBYES);
	    } /* End if for one row */
    } /* End if for result found */

    /* If reached this point do not know result so assume DTMS needed */
    return(DBYES);
} /* End is_mwr_complete */


/* Temporary copy of mask_projects_rtv */
int sql_mask_projects_rtv(char *mask, char *projects[])
{
    DBG("mask='%s'", mask);

	projects[0] = malloc(16);
	strcpy(projects[0], "DTMS not needed");

    return(1);
} /* End temporary function */



/* Get DTMS status for a project */
int sql_mwr_status_rtv(char *project_name, char *mwr_num, char *project_date)
{
    DBG("project_name='%s', mwr_num='%s' and project_date='%s'", project_name, mwr_num, project_date);

    int cnt;
    int num_rows;
    char query[1024];
    MYSQL_ROW row;
    MYSQL_RES *result;

    /* If the project is set to 'DTMS Not needed', no action is needed */
    if (strcmp(project_name, "DTMS not needed") == 0) return(DTMS_NOT_NEEDED);

    /* Initialize default value */
    strcpy(project_date, NONE);

    /* Extract MWR number */
    sscanf(project_name, "%*s %s", mwr_num);
    data_log("query", mwr_num);
    strcpy3(query, " select mwr_final_signoff from mwr_requests where mwr_id = '", mwr_num, "'");
    data_log("query", query);
    result = result_get(DTMS_DB, "mwr", query, NULL);

    /* If no result, indicate no projects found */
    if (result == NULL) return(FAILURE);

    /* There should be one or more rows */
    num_rows = mysql_num_rows(result);
    if (num_rows < 1) return(FAILURE);

    /* final tapeout date set to 1 if signoff has been done */
    /* project was marked ready to tapeout.  If null it's not ready. */
    for (cnt = 0; cnt < num_rows; ++cnt)
    {
	    row = mysql_fetch_row(result);
        if ((row[0] != NULL) && (strcmp(row[0], "1") == 0))
	    {
	        strcpy(project_date, "");
	        return(SUCCESS);
	    } /* End if */
    } /* End for */

    /* If reached this point, no  valid project */
    return(FAILURE);
} /* end sql_mwr_status_rtv */


/* convert to SQL date */
void sql_time_convert(char *time_string, long time_value, char *time_ptr)
{
    DBG("time_string='%s' and time_ptr='%s'", time_string, time_ptr);

    char *ptr;

    /* Set the default date */
    strcpy(time_string, "00/00/00 ");

    /* If no date given, simply return */
    if ((time_value == 0) && (time_ptr == NULL)) return;

    if (time_value != 0) ptr = ctime(&time_value);
    else ptr = time_ptr;

    /* Put in the year */
    strncpy(time_string, ptr+22, 2);

    /* Put in the day of the month */
    strncpy(time_string+6, ptr+8, 2);

    /* Get rid of any spaces present in the day of the month */
    if (time_string[6] == ' ') time_string[6] = '0';

    /* copy in the hour and minutes */
    strncpy(time_string+9, ptr+11, 8);

    /* Last of all put in the month */
    if (strncmp(ptr+4, "Jan", 3) == 0) strncpy(time_string+3, "01", 2);
    else if (strncmp(ptr+4, "Feb", 3) == 0) strncpy(time_string+3, "02", 2);
    else if (strncmp(ptr+4, "Mar", 3) == 0) strncpy(time_string+3, "03", 2);
    else if (strncmp(ptr+4, "Apr", 3) == 0) strncpy(time_string+3, "04", 2);
    else if (strncmp(ptr+4, "May", 3) == 0) strncpy(time_string+3, "05", 2);
    else if (strncmp(ptr+4, "Jun", 3) == 0) strncpy(time_string+3, "06", 2);
    else if (strncmp(ptr+4, "Jul", 3) == 0) strncpy(time_string+3, "07", 2);
    else if (strncmp(ptr+4, "Aug", 3) == 0) strncpy(time_string+3, "08", 2);
    else if (strncmp(ptr+4, "Sep", 3) == 0) strncpy(time_string+3, "09", 2);
    else if (strncmp(ptr+4, "Oct", 3) == 0) strncpy(time_string+3, "10", 2);
    else if (strncmp(ptr+4, "Nov", 3) == 0) strncpy(time_string+3, "11", 2);
    else if (strncmp(ptr+4, "Dec", 3) == 0) strncpy(time_string+3, "12", 2);
    else strncpy(time_string+3, "01", 2);

    /* Terminate date field */
    time_string[17] = '\0';
}
