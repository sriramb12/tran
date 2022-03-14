/****************************************************************************
 *
 * Function:	Mars functions
 *
 * Description:	Performs MARS functions
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "mti.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2msg.h"
#include "m2server_errs.h"
#include "m2server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"

void mars_send_weekly(void)
{
    FILE *fo;
    char *time_ptr;
    char log_file[36] = "contract_tables/mask_log/00_00_00";
    int cnt;
    long weeklytime;

    /* Open output file */
    if ((fo = fopen("weekly_mars_log", "w")) == NULL) return;
    /* Get current time and subtract seven days */
    weeklytime = time(0) - SECPERDAY * 7;
    for (cnt = 0; cnt < 7; ++cnt)
    {
    /* Convert time to character representation */
    time_ptr = ctime(&weeklytime);
    /* Build log name */
    log_convert(time_ptr, log_file+25);
    /* Grab data from file */
    file_info_get(log_file, fo);
    /* Calculate time for next day */
    weeklytime += SECPERDAY;
    }
    /* Put extra carriage return at end of output */
    fprintf(fo, "\n");
    /* Close output file */
    fclose(fo);
    /* send email to specified user */
    email_file_send("", "mdsEvents@nxp.com", "Weekly log of mask numbers issued", "weekly_mars_log");
    /* Delete weekly file no longer needed */
    unlink("weekly_mars_log");
    /* Tell client transaction successful */
/*
    put_short(SUCCESS);
    return_success("Weekly log of mask sets issued sent", DBNO);
*/
    exit(0);
} /* End mars_send_weekly */

void file_info_get(char *log_file, FILE *fo)
{
    char date[16];
    FILE *fi;
    char line[260];

    if ((fi = fopen(log_file, "r")) == NULL) return;
    /* Create date field */
    strcpy(date, log_file+25);
    /* Change '_' to '/' */
    strexc(date, '_', '/');
    /* Get input from file and write to output file */
    while (line_get(fi, line) != EOF)
    fprintf(fo, "%s %s\n", date, line);
    /* Close input file */
    fclose(fi);
}

