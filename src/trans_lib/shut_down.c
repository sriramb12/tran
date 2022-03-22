/****************************************************************************
 *
 * Function:	shut_down
 *
 * Description:	Closes socket and exits during unsuccessful transactions
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <mysql.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "global_defines.h"
#include "external_intercom_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "report_log.h"
#include "trans_lib_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "rtn.h"
#include "server_lib.h"
#include "global_debug.h"


/* Sends last message, closes socket and cleans up temporary files */

/* Return nonexistent error to user */
void shut_down(
    char *msg,		/* Points to message sent to user */
    register short rtn_code)	/* Holds return code sent to user */
{
    DBG();

    extern MYSQL dt_sql;
    extern int request_logged;
    extern char global_mask[];

    //Carlos Mazieri, Nov 28 2014
    //SSL accept errors happen all the time
    //in ssl_server.c:  if (SSL_accept(ssl) <= 0), the return sometimes is zero
    //SSL return=0 SSL_get_error()=5

    if (rtn_code != SSL_ACCEPT_ERR)
    {
        FORCE_DBG("msg=%s rtn_code=%d", msg, rtn_code);
    }
    else
    {
        DBG("msg=%s rtn_code=%d", msg, rtn_code);
    }

    /* If request not logged, log it */
    if (request_logged == DBNO) request_log();
    /* Add warning if using version that will soon expire */
    expire_version_check();
    /* Send user return code and message */
    put_short(rtn_code);
    put_string(msg);
    close(sock);
     /* Log the message */
    if (log_request(LOG_REQUEST_FILE) == SUCCESS)
    {
	data_log(tr.log_file, msg);
	log3("Closing transaction ", tr.pid_string, ".");
    } /* end if */
    report_log(gu.key1, fs.real_name, global_mask, msg);
    /* Insert record in dt_transactions table */
    strcpy(fs.key, gu.key1);
    sql_dt_transaction_insert ( rtn_code, msg);
    /* If this is server set, get, or authenticate, update time stamp */
    sql_reg_timestamp_update();
    /* Close SQL connection */
    sql_edc_close();
    mysql_close(&dt_sql);
    mysql_library_end();
    /* Exit child process */
    exit(1);
} /* end shut_down */

/* Return nonexistent error to user */
void return_non_err(char *key)
{
    DBG("key=%s", key);
    /* Build message to send */
	    sprintf(rtn.msg, trans_non_err, key);
    shut_down(rtn.msg, TRANS_NON_ERR);
} /* end return_non_err */
