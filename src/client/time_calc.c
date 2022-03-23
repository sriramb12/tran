/****************************************************************************
 *
 * Function:	time_calc
 *
 * Description:	Calculates estimated time for transaction to complete
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities


#define TWO_DAYS 86400

char *time_left_calc(unsigned long long total_size, unsigned long long transferred_size, int elapsed_time)
{
    DBG();
    char *time_ptr;
    char hours[32];
    static char time_string[100];
    char *time_string_ptr = time_string + strcpy1(time_string, "Estimated completion time:  ");
long new_time ;
    long expected_time;
    int len= 0;
    unsigned long time_left;

    /* Calculate the time left */
    if (transferred_size <= 0) return(0);
time_left =  (total_size-transferred_size)*(elapsed_time)/transferred_size;
    /* If less than 5 seconds or greater than two days, print nothing */
    if ((time_left < 5) || (time_left > TWO_DAYS)
    || (elapsed_time == 0))
    {
	time_string[0] = '\0';
	return(time_string);
    } /* end if */
time_left =  (total_size-transferred_size)*(elapsed_time)/transferred_size;
    /* Calculate the number of hours */
    new_time = time_left/3600;
    if (new_time > 0)
    {
        len += sprintf(time_string_ptr, "%ld hour", new_time);
        if (new_time > 1) time_string_ptr[len++] = 's';
        time_string_ptr[len++] = ' ';
    }
    new_time = time_left/60%60;
    if (new_time > 0)
    {
        len += sprintf(time_string_ptr+len, "%ld minutes", new_time);
        if (new_time == 1) time_string_ptr[--len] = '\0';
        time_string_ptr[len++] = ' ';
    }
    new_time = time_left%3600%60;
    if (new_time > 0)
    {
        len += sprintf(time_string_ptr+len, "%ld seconds", new_time);
        if (new_time == 1)
	{
	   time_string_ptr[len-1] = '\0';
	    --len;
	} /* end if */
    }
    /* Add the current time */
    expected_time = time(0)+time_left;
    time_ptr = ctime(&expected_time);
    if (len == 0) strcpy1(time_string_ptr, "0 seconds");
    /* If calculated time present, add current time */
    if (len > 0)
    {
	strncpy(hours, time_ptr+11, 8);
	hours[8] = '\0';
	len += strcpy2(time_string_ptr+len, " from now, at ", hours);
    }
    /* Add a carriage return */
    strcat(time_string_ptr, "\n");
    /* Return time calculated */
    return(time_string);
} /* End time_left_calc */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
