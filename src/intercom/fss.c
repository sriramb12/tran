/****************************************************************************
 *
 * Function:	files_get
 *
 * Description:	Basic function to send one or more files to the database client.
 *		Files to send are passed in a file array with one or more elements.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <unistd.h>
#include "dir.h"
#include "close_down.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "report_log.h"
#include "rtn.h"
#include "strcpy.h"
#include "server_lib.h"
#include "trans_server_arrays.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "global_debug.h"

void fss_rtv_logs(void)
{
    char log_file[32];
    char alternate_file[32];
    long log_time;
    char *log_date;

    DBG();

    /*Calculate the time for the FSS log */
    /* It is 24 hours befor */
    log_time = tr.upd_time-SECPERDAY;
    /* Convert to string representation */
    log_date = ctime(&log_time);
    /* Build the template for the log name */
    strcpy(log_file, "00_00_00");
    /* Build the name for the log file */
    log_convert(log_date, log_file);
    /* Tell user ready to transfer files */
    put_short(SUCCESS);
    strcpy2(alternate_file, log_file, ".log");
    file_get_fss_log("fss_log", log_file, alternate_file);
    strcpy2(alternate_file, log_file, ".errs");
    file_get_fss_log("fss_errs", log_file, alternate_file);
    /* Send return code */
    return_success("Logs retrieved", DBNO);
}

/* Send file to client with alternate name */
void file_get_fss_log(char *dir, char *file_name, char *alternate_file)
{
    short ret;
    char full_file_name[FILE_SIZE];
    int fd;

    DBG();

    /* Build complete file name to be opened */
    strcpy3(full_file_name, dir, "/",  file_name);
    /* Send name of file */
    /* Pass root name only without full path */
    put_string(alternate_file);
    /* Get acknowledgement */
    if ((ret = get_short()) != SUCCESS)
    /* If unsuccessful, log failure, clean-up and exit */
    shut_down(bad_ack, BAD_ACK);
    fd = open(full_file_name, O_RDONLY);
    if (fd == EOF)
    {
	/* Send a blank file */
	put_short(SUCCESS);
	return;
    }
    /* Read text from the file and write them to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0)
    put_binary_string(bf.buffer, ret);
    /* Tell client all data for this file sent */
    put_short(SUCCESS);
    /* Close the file just sent */
    close(fd);
} /* End file_get_fss_log */

/* Logs transaction in FSS report log */
void fss_report_log(char *keyword, char *file_name, char *mask)
{
    char status[4];
    extern TRANS_FILE_STAT fs;
    long current_time = time(0);
    FILE *fp;
    char report_file[32];

    DBG();

    strcpy(status, "NA");
    /* Build log file name */
    strcpy2(report_file, "fss_", tr.log_file);
    if ((fp = fopen(report_file, "a")) == NULL) return;
    fprintf(fp, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%lld;%lld;%ld;%ld;%ld;%s\n",
     /* User ID platform, IP node address */
    NONE, user.email, user.name, gu.machine_type, user.addr,
     user.client_version, gu.from_company, 
	gu.to_company,
    /* Request type, file name, mask, original size */
    req_array[gu.req_type],  file_name, mask, fs.original_size,
     fs.compressed_size, tr.upd_time, current_time, current_time-tr.upd_time, status);
    fclose(fp);
} /* End fss_report_log */


void fss_err_log(char *msg)
{
    FILE *fp;
    char report_file[32];
    char *ptr;

    DBG();

    /* Build log file name */
    strcpy2(report_file, "fss_errs", tr.log_file+3);
    /* Open the log file */
    if ((fp = fopen(report_file, "a")) == NULL) return;
    /* Do not log more than one line of the error message */
    if ((ptr = strchr(msg, '\n')) != NULL) ptr[0] = '\0';
    /* Log user ID, name, from and to company, transaction and error */
    fprintf(fp, "%s;%s;%s;%s;%s;%s\n",
    gu.email1, user.name, gu.from_company, gu.to_company, req_array[gu.req_type], msg);
    fclose(fp);
}

int is_fss_customer(void)
{
    DBG();
    if ((strcmp(gu.from_company, FSS_COMPANY) == 0)
    || (strcmp(gu.from_company, TRANSWEB_COMPANY) == 0)
    || (strcmp(gu.from_company, "TransNet Customer") == 0)
    || (strcmp(gu.from_company, "AMD") == 0))
    return(0);
    else return(-1);
} /* end is _fss_company */
/*es.files.fileuse*/
/*is.files.fileuse*/
