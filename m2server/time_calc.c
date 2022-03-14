#include <stdio.h>
#include <time.h>
#include "global_defines.h"
#include "m2server_globals.h"
#include "m2globals.h"
#include "m2server_errs.h"
#include "rtn.h"
#include "strcpy.h"

/* Change month from form 'Jan' to '01' */
int month_convert2int(char *date)
{
    int month;

    /* Fill in the month characters based on the date value */
	switch(date[0])
    {
	/* Fill in numeric values for January, June, or July */
	case 'J':
	{
	    if (date[1] == 'a') month = 0;
	    else
	    {
		if (date[2] == 'n') month = 5;
		else month = 6;
	    } /* end if */
	}
	break;

	case 'A':
	{
	    if (date[1] == 'p') month = 3;
	    else month = 7;
	}
	break;

	/* Fill in numeric value for March and May */
	case 'M':
	{
	    if (date[2] == 'r') month = 2;
	    else month = 4;
	}
	break;
	/* Fill in numeric value for February */
	case 'F': month = 1; break;
	/* Fill in numeric value for September */
	case 'S': month = 8; break;
	/* October */
	case 'O': month = 9; break;
	/* November */
	case 'N': month = 10; break; 
	/* December */
	case 'D': month = 11; break; 
    default: month = 0;
    } /* end switch statement */
    return(month);
}

long time_convert(char *time_string)
{
    char month[10];
    int day;
    int year;
    struct tm ts;

    /* Scan out month, day and year */
    strexc(time_string, ',', ' ');
    sscanf(time_string, "%s %d %d", month, &day, &year);
    if ((year < 1900) || (year > 2037))
    {
	sprintf(rtn.msg, invalid_year, year);
	return_err( rtn.msg, INVALID_YEAR);
    }
    ts.tm_sec = 0;
    ts.tm_min = 0;
    ts.tm_hour = 0;
    ts.tm_mday = day;
    ts.tm_mon = month_convert2int(month);
    if (year < 72) ts.tm_year = year+100;
    else if (year >1900) ts.tm_year = year-1900;
    else ts.tm_year = year;
    ts.tm_wday = 0;
    ts.tm_yday = 0;
    ts.tm_isdst = 0;
    return(mktime(&ts));
} /* End time_convert */

int time_calc(int time_flag, long lower_time, long upper_time, long mask_time)
{

    /* See if mask_time meets specified criteria */
    switch (time_flag)
    {
	case PRIOR: 
	{
	    if (mask_time < lower_time) return(SUCCESS);
	    else return(FAILURE);
	}
	break;
	case ON:
	{
	    if ((mask_time >= lower_time) && (mask_time <= upper_time)) return(SUCCESS);
	    else return(FAILURE);
	}
	break;
	case SINCE: 
	{
	    if (mask_time > lower_time) return(SUCCESS);
	    else return(FAILURE);
	}
	break;
	case RANGE:
	{
	    if ((mask_time >= lower_time) && (mask_time <= upper_time)) return(SUCCESS);
	    else return(FAILURE);
	}
	break;
	default: return(SUCCESS);
    } /* end switch */
} /* End time_calc */
