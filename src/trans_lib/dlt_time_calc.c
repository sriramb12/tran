/****************************************************************************
 *
 * Function:	trans_dlt_time_calc
 *
 * Description:	Calculates time big file will be deleted.
 *
 ***************************************************************************/

#include <strings.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#include "global_server_params.h"
#include "strcpy.h"
#include "global_debug.h"


long trans_dlt_time_calc(char *current_gmt_time, char *dlt_time_string, char *gmt_time_string, char *expiration_time_string)
{
    extern char time_zone[];
    long dlt_time;
    char *time_ptr;

    DBG();

    /* Put in expiration time */
    strcpy(expiration_time_string, "will be deleted in three working days.\n");

    /* Figure out the current GM time */
    /* Convert to string representation */
#ifdef SOLARIS
    time_ptr = asctime_r(gmtime(&tr.upd_time), current_gmt_time, 30);
#else
    time_ptr = asctime_r(gmtime(&tr.upd_time), current_gmt_time);
#endif

    strcpy(current_gmt_time+24, " GMT");

    /* Add 72 hours to current time */
    dlt_time = tr.upd_time+259200;

    /* Only count working days not calendar days */
	/* Do not make any changes for files submitted on Monday or Tuesday */
    if ((strncmp(tr.log_date, "Mon", 3) == 0) || (strncmp(tr.log_date, "Tue", 3) == 0));
	/* Extend the expiration date one day if submitted on Sunday */
	else if (strncmp(tr.log_date, "Sun", 3) == 0) dlt_time += 86400;
	/* Extend the expiration date to days for all other days */
	else dlt_time += 172800;

    /* Convert new time to character  representation to send to user */
#ifdef SOLARIS
    time_ptr = ctime_r(&dlt_time, dlt_time_string, 30);
#else
    time_ptr = ctime_r(&dlt_time, dlt_time_string);
#endif

    /* Get rid of carriage return& add time zone */
    strcpy2(dlt_time_string+24, " ", time_zone);

#ifdef SOLARIS
    time_ptr = asctime_r(gmtime(&dlt_time), gmt_time_string, 30);
#else
    time_ptr = asctime_r(gmtime(&dlt_time), gmt_time_string);
#endif

    /* Get rid of carriage return & add time zone */
    strcpy(gmt_time_string + 24, " GMT");

    /* Add the time zone for current time already calculated */
    strcpy2(tr.log_date+24, " ", time_zone);

    return(dlt_time);
} /* End trans_dlt_time_calc */


/* Set values for no file expiration */
long no_expiration_calc(char *current_gmt_time, char *dlt_time_string, char *gmt_time_string, char *expiration_time_string)
{
    extern char time_zone[];
    long dlt_time = 0;
    char *time_ptr;

    DBG();

    /* Indicate never expires */
    strcpy(expiration_time_string, " has no expiration date.\n");

    /* Figure out the current GM time */
    /* Convert to string representation */
#ifdef SOLARIS
    time_ptr = asctime_r(gmtime(&tr.upd_time), current_gmt_time, 30);
#else
    time_ptr = asctime_r(gmtime(&tr.upd_time), current_gmt_time);
#endif

    strcpy(current_gmt_time+24, " GMT");
    strcpy2(tr.log_date+24, " ", time_zone);
    strcpy(dlt_time_string, "No expiration.");
    strcpy(gmt_time_string, "No expiration.");

    return(dlt_time);
} /* end no_expiration_calc */


void start_transfertime_calc(void)
{
    extern unsigned long long start_transfertime;
    struct timeval tv;

    DBG();

    gettimeofday(&tv, NULL);
    start_transfertime = tv.tv_usec+ tv.tv_sec*1000000;
} /* end start_transfertime_calc */


/* Calculate the end transfer time */
/* Records inserted into dt_transfertimes after get & put transactions complete */
void end_transfertime_calc(void)
{
    extern unsigned long long end_transfertime;
    struct timeval tv;

    DBG();

    gettimeofday(&tv, NULL);
    end_transfertime = tv.tv_usec+ tv.tv_sec*1000000;
} /* end end_transfertime_calc */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
