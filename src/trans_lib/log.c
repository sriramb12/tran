/****************************************************************************
 *
 * Function:    log_history_file
 *
 * Description:    Logs transaction in history file
 *
 * Author:    Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_server_arrays.h"
#include "trans_server_paths.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "trans_lib_prototypes.h"
#include "user_params.h"


#include "global_debug.h"

void log_history_file(char *key)
{
    char file_size_string[32];
    char time_string[48];
     extern char time_zone[];

     DBG();

    /* Log transaction for all types of gets, puts, deletes, */
    /* info, set mail, cancel email etc. */
    if ((gu.req_type == DB1STEP_UPLOAD)
    || (gu.req_type == DBGET_DATA)
    || (gu.req_type == 	DBGETDLT_DATA)
    || (gu.req_type == DBGET_PARALLEL)
    || (gu.req_type == DBGET_RANDOM_PARALLEL)
    || (gu.req_type == DBGET_SERIAL)
    || (gu.req_type == DBGET_ZIPPED)
    || (gu.req_type == DBGETDLT_ZIPPED)
    || (gu.req_type == DBPUT_PARALLEL)
    || (gu.req_type == DBPUT_RANDOM_PARALLEL)
    || (gu.req_type == DBPUT_DATA)
    || (gu.req_type == DBPUT_TAR_FILE)
    || (gu.req_type == 	DBPUTDLT_DATA)
    || (gu.req_type == 	DBPUTDLT_TAR_FILE)
    || (gu.req_type == DBPUT_FILES)
    || (gu.req_type == DBPUT_VERIFY_FILES)
    || (gu.req_type == DBPUTDLT_VERIFY_FILES)
    || (gu.req_type == DBPUTDLT_FILES)
    || (gu.req_type == DBPUT_ACK)
    || (gu.req_type == DBGET_ACK)
    || (gu.req_type == TRANSDIRLIST_RTV))
    {
	/* Build name of file log */
	strcpy3(bf.file_name, HISTORY_FILE_PATH, key, ".history");
	/* If history file not present, put in new time stamp */
	if (access(bf.file_name, 0) != 0)
	{
	    sprintf(bf.msg, "%ld", time(0) + TWO_WEEKS);
	    data_log(bf.file_name, bf.msg);
	} /* End if for history file not present */
	/* build message to log */
	/* Strip carriage return off of log date */
	tr.log_date[24] = '\0';
	/* Calculate the transaction time */
	hms_calc(time(0)-tr.upd_time, time_string);
	rtn.len = strcpy3(rtn.msg, req_array[gu.req_type], " performed by ", user.name);
    num_gmb_calc(file_size_string, fs.original_size);
	strcpy9(rtn.msg+rtn.len, 
	" on ", tr.log_date, " ", time_zone, 
    ".\n    Transfer time: ", time_string,
    "  Size:  ", file_size_string, ".");
	data_log(bf.file_name, rtn.msg);
    }
    else if ((gu.req_type == DBDLT_DATA)
    || (gu.req_type == DBCRON_DLT)
    || (gu.req_type == DBINFO_DATA)
    || (gu.req_type == DBHIST_DATA)
    || (gu.req_type == DBEXTEND_DATA)
    || (gu.req_type == DBSET_FILE_EMAIL)
    || (gu.req_type == DBCAN_FILE_EMAIL)
    || (gu.req_type == DBSEND_KEYWORD	)
    || (gu.req_type == DBSIZE_CHECK)
    || (gu.req_type == DBLOCK_DATA)
    || (gu.req_type == DBUNLOCK_DATA)
    || (gu.req_type == DBSET_ASCII)
    || (gu.req_type == DBSET_BINARY)
    || (gu.req_type == DBOWNER_DLT)
    || (gu.req_type == DBUSER_DLT))
    {
	/* Build name of file log */
	strcpy3(bf.file_name, HISTORY_FILE_PATH, key, ".history");
	/* If history file not present, put in new time stamp */
	if (access(bf.file_name, 0) != 0)
	{
	    sprintf(bf.msg, "%ld", time(0) + TWO_WEEKS);
	    data_log(bf.file_name, bf.msg);
	} /* End if for history file not present */
	/* build message to log */
	/* Strip carriage return off of log date */
	tr.log_date[24] = '\0';
	strcpy8(rtn.msg, req_array[gu.req_type], " performed by ", user.name,
	" on ", tr.log_date, " ", time_zone, ".");
	data_log(bf.file_name, rtn.msg);
    } /* End if for logging mesage in file log */
} /* End log_history_file */


/* Decide if request should be logged */
int log_request(char *file_name)
{
    FILE *fp;
    char line[260];
    int req_type;
    int log_status;

     DBG();

    /* If log file not present go ahead and log request */
    if ((fp = fopen(LOG_REQUEST_FILE, "r")) == NULL)
        return(SUCCESS);

    /* Scan in lines from the file until end of file reached */
    while (line_get(fp, line) != EOF)
    {
	    /* Parse line output */
	    /* If comment line, skip it */
	    if (line[0] == '#') continue;
        if (sscanf(line, "%d %*s %d", &req_type, &log_status) != 2) continue;
	    if (req_type == gu.req_type)
	    {
	        fclose(fp);
	        /* If found request type, return the status */
	        return(log_status);
	    } /* End if for request type found */
    } /* End while */

    fclose(fp);

    /* If reached this point, request type not found */
    /* Log request if not found in file */
    return(SUCCESS);
} /* End log_request */


void hms_calc(long seconds, char *string)
{

    int hours = seconds/3600;
    int min = (seconds%3600)/60;
    int sec = (seconds%3600)%60;
   int len = 0;

     DBG();
    if (hours > 0)
    {
	len = sprintf(string, " %d hr.", hours);
    }
    if (min > 0)
    {
	len += sprintf(string+len, " %d min.", min);
    }
    if (sec > 0)
    {
    len += sprintf(string+len, " %d sec.", sec);
    }
    if (len == 0) strcpy(string, " 1 sec.");
} /* end hms_calc */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
