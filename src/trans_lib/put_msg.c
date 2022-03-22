/****************************************************************************
 *
 * Function:    put_msg
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
#include <string.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "server_lib.h"
#include "sql_prototypes.h"
#include "report_log.h"
#include "strcpy.h"
#include "trans_lib_prototypes.h"
#include "trans_server_arrays.h"
#include "trans_server_paths.h"
#include "trans_defines.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


void release_msg_add(char *msg)
{
    int fd;
    int len;
    int ret;

    DBG();
    /* Different release file defined for Transcend and TransWeb */
    if ((fd = open(RELEASE_FILE, O_RDONLY)) != -1)
    {
	len = strlen(msg);
	len += strcpy1(msg+len, "\n\n");
	ret = read(fd, msg+len, BUFFER_SIZE);
	if (ret > 0) msg[len+ret] = '\0';
	close(fd);
    } /* End if for able to reead release file */
} /* End release_msg_add */

void put_msg(char *key, char *log_string)
{
    extern MYSQL dt_sql;
    extern char global_mask[];
    char file_name[128];

    DBG();

    /* Check to see if expiration warning needs to be printed */
    expire_version_check();
    /* Send user SUCCESS */
    put_short(SUCCESS);
    /* Log the message in the daily log file */
    data_log(tr.log_file, rtn.msg);
	release_msg_add(rtn.msg);
    put_string(rtn.msg);
    put_string(log_string);
    /* Close the socket */
    close(sock);
    /* Log transaction in file log */
    /* Transactions logged for get, put, info, and history requests */
    log_history_file(key);
    /* Put entry in keyword directory */

    format_user_keys_file_name(file_name);

    data_log(file_name, key);
    /* Put entry in report log */
    report_log(key, gu.key1, global_mask, rtn.msg);
    /* Add record to dt_transactions table */
sql_dt_transaction_insert (SUCCESS, rtn.msg);
    /* If this is server set, get, or authenticate, update time stamp */
    sql_reg_timestamp_update();
    /* Put entry in dt_transfertimes */
    trans_transfertime_insert();
#ifdef TRANSCEND
    /* Save history if this is Transcend */
    sql_trans_history_insert(key, gu.key1, gu.email1, gu.original_size, fs.compressed_size);
#endif
#ifdef garbage
    /* Insert entry in SQL database for traceability */
    sql_intercom_history_insert(key, gu.key1, gu.email1, gu.original_size, fs.compressed_size);
#endif
    mysql_close(&dt_sql);
    sql_edc_close();
    mysql_library_end();
    /* Indicate closing transaction */
    log3("Closing transaction ", tr.pid_string, ".");
} /* End put_msg */

