/****************************************************************************
 *
 * Name:	bld_time_stamp
 *
 * Description:	Builds time stamp consisting of current date and time
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/types.h>
#include <time.h>
#include <string.h>
#include "global_server_prototypes.h"
#include "global_debug.h" // Mazieri's debug facilities

void time_stamp_build(char *time_stamp)
{
    DBG("time_stamp='%s'", time_stamp);

    char *log_date;
    long current_time;

    /* Set up default time stamp */
    strcpy(time_stamp, "000000");

    /* Get the current time in seconds since 1970 */
    current_time = time(0);
    log_date = ctime(&current_time);
    month_convert(log_date, time_stamp+2);

    /* Copy in the day of the month from positions 8 and 9 */
    if (log_date[8] != ' ') time_stamp[4] = log_date[8];
    else time_stamp[4] = '0';

    time_stamp[5] = log_date[9];
    
    /* Copy the year */
    time_stamp[0] = log_date[22];
    time_stamp[1] = log_date[23];

    /* Copy in the hour */
    time_stamp[6] = log_date[11];
    time_stamp[7] = log_date[12];

    /* Copy in the minute */
    time_stamp[8] = log_date[14];
    time_stamp[9] = log_date[15];

    /* Copy in the seconds */
    time_stamp[10] = log_date[17];
    time_stamp[11] = log_date[18];

    /* Terminate string with null character */
    time_stamp[12] = '\0';
} /* End time_stamp_build */

/* The following allows me to use month '01' instead of 'Jan' */
/*is.files.fileuse*/
/*ts.files.fileuse*/
