/****************************************************************************
 *
 * Function:	extend_big_file
 *
 * Description:	Retrieves info on big file.
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_macros.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_defines.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


void history_timestamp_update(char *key, long dlt_time)
{
    char line[260];
    long history_dlt_time = dlt_time + TWO_WEEKS;
    char new_history_file[64];
    char history_file[64];
    FILE *fi;
    FILE *fo;

    DBG();

    strcpy3(new_history_file, "history_data/", key, ".new_history");
    strcpy3(history_file, "history_data/", key, ".history");

    if ((fo = fopen(new_history_file, "w")) == NULL)
        close_down1m(wr_open_err, new_history_file, WR_OPEN_ERR);

    /* Write out the new time */
    fprintf(fo, "%ld\n", history_dlt_time);

    /* Now append existing history file */
    if ((fi = fopen(history_file, "r")) == NULL) 
    {
	    fclose(fo);
	    return;
    } /* end if */

    /* Ignore the first line where the current time stamp is */
    line_get(fi, line);

    /* Now read in other lines in a while loop */
    while (line_get(fi, line) != EOF)
        fprintf(fo, "%s\n", line);

    /* close both files */
    fclose(fi);
    fclose(fo);

    /* Delete old file and rename new file */
    unlink(history_file);
    file_rename(new_history_file, history_file);
} /* End history_timestamp_update */


void extend_big_file(char *key, int days)
{
    extern int timeout_extension;
    extern char time_zone[];
    extern TRANS_FILE_STAT fs;
    char *asctime();
    char *time_ptr;
    struct tm *gm_time;
    struct tm *gmtime();
    char line[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char dlt_day[30];
    char new_dlt_time[30];
    char new_gm_time[30];
    int extra_extension;
    FILE *fp;

    DBG();

    /* read in global file */
    fp = read_big_file(key, line, &fs);

    /* Make sure the file has not already been extended three working days */
    if ((fs.extension >= timeout_extension) && (strcmp(tr.user_email, "b36283") != 0))
    {
	    /* Close the open global file */
	    fclose(fp);
	    sprintf(rtn.msg, extend_err, fs.title, fs.real_name);
	    shut_down(rtn.msg, EXTEND_ERR);
    }

    /* Make sure the user isn't trying to extend more than 6 days */
    if ((fs.extension+days > timeout_extension) && (strcmp(tr.user_email, "b36283") != 0))
    {
	    /* Close the open global file */
	    fclose(fp);
	
	    if ((3-fs.extension) == 1)
	        sprintf(rtn.msg,total1extend_err, fs.title, fs.real_name);
	    else 
	        sprintf(rtn.msg, total2extend_err, fs.title, fs.real_name);
	
	    shut_down(rtn.msg, TOTAL_EXTEND_ERR);
    }

    /* If the data has no expiration date, give error */
    if(fs.dlt_time == 0)
        shut_down(expiration_extend_err, EXPIRATION_EXTEND_ERR);

    /* Read end the current deletion date */
    sscanf(line[8], "%*s %s", dlt_day);

    /* If expires on Friday, extend two extra days */
    if (strcmp(dlt_day, "Fri") == 0)
        extra_extension = 2;
    /* If Expires on Thursday and extending more than one day, */ 
    /* extend two extra days */
    else if ((strcmp(dlt_day, "Thu") == 0) && (days > 1))  
        extra_extension = 2;
    /* If expires on Wednesday and extending more than 2 days */ 
    /* extend two additional days */
    else if ((strcmp(dlt_day, "Wed") == 0) && (days > 2)) 
        extra_extension = 2;
    /* Do not extend extra days for any other situations */
    else 
        extra_extension = 0;

    /* Calculate the new time */
    fs.dlt_time += (days+extra_extension)*86400;

    /* Calculate the new date */
    time_ptr = ctime(&fs.dlt_time);
    time_ptr[24] = '\0';

    /* Save new delete time so it is not overwritten */
    strcpy(new_dlt_time, time_ptr);

    /* First and last two lines must be modified */
    /* Update the file extension */
    fs.extension += days;

    /* Now put new expiration date in ninth line */
    sprintf(line[8], "Expires:     %s %s", new_dlt_time, time_zone);

    /* Now calculate the expiration time in Greenwich Mean Time  */
    gm_time = gmtime(&fs.dlt_time);
    time_ptr = asctime(gm_time);

    /* Get rid of extra carriage return */
    time_ptr[24] = '\0';
    strcpy(new_gm_time, time_ptr);
    strcpy3(line[9], "             ", time_ptr, " GMT");

    /* Write out changes to big file */
    DBG("Before call write_big_file()");
    write_big_file(fp, line, &fs);
    DBG("after call write_big_file()");

    /* Build message to send to user */
    if (days == fs.extension)
    sprintf(rtn.msg,
"The expiration date on %s '%s' has been extended %d working day(s).\n\nLocal Expiration Time:  %s\nGreenwich Mean Time:    %s",
    fs.title, fs.real_name, days, new_dlt_time, new_gm_time);
    else sprintf(rtn.msg,
"The expiration date on %s '%s' has been extended %d working day(s).\nTotal days extended:  %d.\n\nLocal Expiration Time:  %s\nGreenwich Mean Time:    %s",
    fs.title, fs.real_name, days, fs.extension, new_dlt_time, new_gm_time);
    
    /* Tell the user the date has been extended */
    put_short(SUCCESS);
    put_success_msg(key);
} /* End extend_big_file */

