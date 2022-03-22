/****************************************************************************
 *
 * Name:	bld_time_stamp
 *
 * Description:	Builds tickler stamp consisting of current date and time
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <time.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "global_defines.h"
#include "strcpy.h"
#include "global_debug.h"

/* convert Letter form of month to digital form */
void time_stamp_month_convert(
    char *log_date,
    char *time_stamp)
{
    DBG();

    /* Change month from form 'Jan' to '01' */
    /* Fill in the month characters based on the date value */
    switch (log_date[4])
    {
	/* Fill in numeric values for January, June, or July */
	case 'J':
	{
	    time_stamp[5] = '0';
	    if (log_date[5] == 'a') time_stamp[6] = '1';
	    else
	    {
		if (log_date[6] == 'n') time_stamp[6] = '6';
		else time_stamp[6] = '7';
	    } /* end if */
	}
	break;

	case 'A':
	{
	    time_stamp[5] = '0';
	    if (log_date[5] == 'p') time_stamp[6] = '4';
	    else time_stamp[6] = '8';
	}
	break;

	/* Fill in numeric value for March and May */
	case 'M':
	{
	    time_stamp[5] = '0';
	    if (log_date[6] == 'r') time_stamp[6] = '3';
	    else time_stamp[6] = '5';
	}
	break;
	/* Fill in numeric value for February */
	case 'F':  
	{
	    time_stamp[5] = '0';
	    time_stamp[6] = '2'; 
	}
	break;
	/* Fill in numeric value for September */
	case 'S':
	{
	    time_stamp[5] = '0';
	      time_stamp[6] = '9'; 
	}
	break;
	/* October */
	case 'O':
	{
	      time_stamp[5] = '1'; 
	      time_stamp[6] = '0'; 
	}
	break;
	/* November */
	case 'N': 
	{
	    time_stamp[5] = '1';
	    time_stamp[6] = '1';
	}
	break;
	/* December */
	case 'D': 
	{
	    time_stamp[5] = '1';
	    time_stamp[6] = '2';
	}
	break;
    } /* end switch statement */
} /* End time_stamp_month_convert*/

void time_stamp_bld(char *time_stamp)
{
    char *log_date;
    char *ctime();
    long current_time;

    DBG();

    /* Get the current time in seconds since 1970 */
    /* Currently not being used */
    time_stamp[0] = '\0';
    current_time = time(0);
    log_date = ctime(&current_time);
    /* Build the template which will be overwritten */
    strcpy(time_stamp, "2000-00-00-000000");
    /* Convert month to correct format */
    time_stamp_month_convert(log_date, time_stamp);
    /* Copy in the day of the month from positions 8 and 9 */
    if (log_date[8] != ' ')
    time_stamp[8] = log_date[8];
    else time_stamp[8] = '0';
    time_stamp[9] = log_date[9];
    /* Copy the year */
    time_stamp[2] = log_date[22];
    time_stamp[3] = log_date[23];
    /* Copy in the hour */
    time_stamp[11] = log_date[11];
    time_stamp[12] = log_date[12];
    /* Copy in the minute */
    time_stamp[13] = log_date[14];
    time_stamp[14] = log_date[15];
    /* Copy in the seconds */
    time_stamp[15] = log_date[17];
    time_stamp[16] = log_date[18];
    /* Terminate string with null character */
    time_stamp[18] = '\0';
} /* End bld_time_stamp */

/* Insert the time stamp into the remote file name */
void time_stamp_insert(char *remote_file, char *time_stamp)
{
    int fnd = DBNO;
    char tmp_string[FILE_SIZE];
    char *ptr;
    char *old_ptr = remote_file;

    DBG();

    /* Currently not being used */
    return;
    /* Look for . in loop until a known suffix is found */
    while (1)
    {
	if ((ptr = strchr(old_ptr, '.')) == NULL) break;
	/* Now see if valid suffix is found */
	if ((strncmp(ptr, ".gds", 4) == 0)
	|| (strncmp(ptr, ".gz", 3) == 0)
	|| (strncmp(ptr, ".tar", 4) == 0)
	|| (strncmp(ptr, ".zip", 4) == 0)
	|| (strncmp(ptr, ".gpg", 4) == 0))
	{
	    fnd = DBYES;
	    strcpy(tmp_string, ptr);
	    strcpy2(ptr, time_stamp, tmp_string);
	    break;
	} /* End if */
	else old_ptr = ptr+1;
    } /* End while */
    /* If suffix not found, just append the time stamp */
    if (fnd == DBNO)
    strcat(remote_file, time_stamp);
} /* End time_stamp_insert */
