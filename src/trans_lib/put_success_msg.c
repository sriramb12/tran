/****************************************************************************
 *
 * Function:    put_success_msg
 *
 * Description:    Sends user successful return code.  A warning
 *    	message may be sent with the return code.
 *
 * Author:    Renee Carter
 *
 ***************************************************************************/

#include <mysql.h>
#include <sys/file.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "report_log.h"
#include "rtn.h"
#include "server_lib.h"
#include "sql_prototypes.h"
#include "trans_defines.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "trans_server_paths.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

/* Holds list of expired version */
extern char expire_version[10][8];

/* Close SQL connection */

static void stop_mysql()
{
    extern MYSQL dt_sql;
    sql_edc_close();
    mysql_close(&dt_sql);
    mysql_library_end();
}

/* Check for expiring version */
void expire_version_check(void)
{
    extern char expire_version[10][8];
    extern char expire_date[];
    int cnt;

    DBG();

    for (cnt = 0; expire_version[cnt][0] != '\0'; ++cnt)
    {
        if (strcmp(user.client_version, expire_version[cnt]) == 0)
        {
            sprintf(rtn.msg+strlen(rtn.msg), version_wrn, expire_date);
            return;
        } /* End if for expiring version found */
    } /* end for */
} /* end expire_version_check */

void put_success_msg(char *key)
{
    extern TRANS_FILE_STAT fs;
    extern char global_mask[];

    DBG();

    /* Check to see if expiration warning needs to be printed */
    expire_version_check();
    /* Send user SUCCESS */
    put_short(SUCCESS);
    put_string(rtn.msg);
    /* Close the socket */
    close(sock);
    /* Log the message in the daily log file */
    if (log_request(LOG_REQUEST_FILE) == SUCCESS)
    data_log(tr.log_file, rtn.msg);
    if ((key[0] != '\0')
    && (strcmp(key, NONE) != 0)
#ifdef TRANS_FTP
    /* Send2Foundry performs info every 5 minutes while transfer is in process */
    /* so info transactions are not logged */
    && (gu.req_type != DBINFO_DATA)
#endif
    /* History logged only if less than 30 lines present */
    && (history_space_left(key) == DBYES))
    /* Log transaction in history file */
    {
	log_history_file(key);
	/* Update history in trans_history table */
#ifdef TRANSCEND
	sql_trans_history_update(key);
#endif
#ifdef TRANS_FTP
	/* Insert entry in SQL database for traceability */
/*
	sql_intercom_history_update(key);
*/
#endif
    } /* end if */
    /* Make entry in the report log */
    report_log(key, fs.real_name, global_mask, rtn.msg);
sql_dt_transaction_insert (SUCCESS, rtn.msg);
    /* If this is server set, get, or authenticate, update time stamp */
    sql_reg_timestamp_update();
    /* If this was a put or get, update transfertimes */
    trans_transfertime_insert();
    stop_mysql();
    /* Indicate closing transaction */
    log3("Closing transaction ", tr.pid_string, ".");
} /* End put_success_msg */

void return_success(char *msg, int log)
{
    DBG("msg=%s", msg);
    strcpy(rtn.msg, msg);
    put_success_msg(NONE);
}



stop_server_quietly()
{
    close(sock);
    stop_mysql();
    rtn.msg[0] = '\0';
}
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
