#include <string.h>
#include <stdio.h>
#include <time.h>
#include "global_server_params.h"
#include "strcpy.h"
#include "global_debug.h"

long day_dlt_time_calc(int days, char *current_gmt_time, char *dlt_time_string, char *gmt_time_string)
{
    extern char time_zone[];
    long dlt_time;
    char *time_ptr;
    struct tm *tmp_time;

    DBG();

    /* Figure out the current GM time */
    /* Convert to string representation */
    tmp_time = gmtime(&tr.upd_time);
#ifdef SOLARIS
    time_ptr = asctime_r(tmp_time, current_gmt_time, 26);
#else
    time_ptr = asctime_r(tmp_time, current_gmt_time);
#endif
    strcpy(current_gmt_time+24, " GMT");
    /* Add five days to current time */
    dlt_time = tr.upd_time+ 86400*days;
    /* Convert new time to character  representation to send to user */
#ifdef SOLARIS
    time_ptr = ctime_r(&dlt_time, dlt_time_string, 26);
#else
    time_ptr = ctime_r(&dlt_time, dlt_time_string);
#endif
    /* Get rid of carriage return& add time zone */
    strcpy2(dlt_time_string+24, " ", time_zone);
    /* Now calculate the equivalent GMT time */
    tmp_time = gmtime(&dlt_time);
#ifdef SOLARIS
    time_ptr = asctime_r(tmp_time, gmt_time_string, 26);
#else
    time_ptr = asctime_r(tmp_time, gmt_time_string);
#endif
    /* Get rid of carriage return & add time zone */
    strcpy(gmt_time_string + 24, " GMT");
    /* Add the time zone for current time already calculated */
    strcpy2(tr.log_date+24, " ", time_zone);
    return(dlt_time);
} /* End day_dlt_time_calc */
