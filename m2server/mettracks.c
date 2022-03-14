/****************************************************************************
 *
 * Function:	mettracks_send
 *
 * Description:	sends log for previous day for Intercom, MARS, and Mask Designer
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "reg_globals.h"
#include "rtn.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "server_lib.h"
#include "strcpy.h"
#include "x500_defines.h"

/* Send the Intercom log to the client */
void mettracks_send(char *log_prefix)
{
    int len;
    char file_name[64];
    int fd;
    long log_time;
    char *log_date;
    char log_file[24];
    char buffer[BUFFER_SIZE];

    /*Calculate the time for the Mettracks log */
    /* It is 24 hours befor */
    log_time = tr.upd_time-SECPERDAY;
    /* Convert to string representation */
    log_date = ctime(&log_time);
    /* Build the template for the log name */
    strcpy(log_file, "00_00_00");
    /* Build the name for the log file */
    log_convert(log_date, log_file);
    strcpy3(file_name, log_prefix, "/", log_file);
    /* Try opening the file */
    if ((fd = open(file_name, O_RDONLY)) < 0)
    close_down1f(non_err, log_file, NON_ERR);
    /* Tell client ready to send data */
    put_short(SUCCESS);
    /* Send name of file */
    put_string(log_file);
    /* Get the acknoledgement from the client */
    if (get_short() != SUCCESS)
    shut_down(bad_ack, BAD_ACK);
    /* Send data until everything has been sent */
    while((len = read(fd, buffer, BLOCK_SIZE)) > 0)
    put_binary_string(buffer, len);
    /* Indicate all data sent */
    put_short(SUCCESS);
    /* Build message to send to client */
    strcpy3(rtn.msg, "Log retrieved for ", log_file, ".");
    return_success(rtn.msg, DBNO);
    /* Close log file no longer needed */
    close(fd);
} /* End mettracks_send */
