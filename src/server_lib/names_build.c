/****************************************************************************
 *
 * Function:	names_build
 *
 * Description:	Builds list of file names to retrieve report data from
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "global_debug.h" // Mazieri's debug facilities

int names_build(char *date1, char *date2, char *files[])
{
    DBG("date1='%s' and date2='%s'", date1, date2);

    int cnt = 0;
    FILE_DATE d1;
    FILE_DATE d2;
    int month;
    int day;
    int start_month;
    int end_month;
    int start_day;
    int end_day;
    int year;
    int new_year;

    date_calc(&d1, date1);
    date_calc(&d2, date2);

    /* If ending month greater than starting month give error */
    if ((d2.month < d1.month) && (d1.year == d2.year))
    {
        sprintf(rtn.msg, month_err, d1.month, d2.month);
        shut_down(rtn.msg, MONTH_ERR);
    }

    /* If not in the same year give error */
    if (d1.year > d2.year)
        shut_down(year_err, YEAR_ERR);

    for (new_year = d1.year; new_year <= d2.year; ++new_year)
    {
        if (new_year < d2.year) end_month = 12;
        else end_month = d2.month;

        if (new_year == d1.year) start_month = d1.month;
        else start_month = 1;

        /* Calculate the cut off year */
        if (new_year == 1999) year = 99;
        else year = new_year-2000;
    
        for (month = start_month; month <= end_month; ++month)
        {
            if ((month < d2.month)  || (new_year < d2.year)) end_day = 31;
            else end_day = d2.day;
        
            if (month == d1.month) start_day = d1.day;
            else start_day = 1;
        
            /* Account for the same start and end month in different years */
            if ( (d1.year != d2.year) && (new_year == d2.year)) start_day = 1;
        
            for (day = start_day; day <= end_day; ++day)
            {
                files[cnt] = malloc(9);
                sprintf(files[cnt], "%2d_%2d_%2d", month, day, year);
                ++cnt;
            } /* End inner for */
            
            if (cnt == 999) break;
        } /* End outer for */
    } /* End year for */

    files[cnt] = NULL;

    return(cnt);
} /* End names_build */


int date_calc(FILE_DATE *d, char *date)
{
    DBG("date='%s'", date);

    char month[4];
    char day[4];
    char year[5];
    int len = strlen(date);

    /* Verify the date is in correct format */
    if ((date[2] != '/') || (date[5] != '/') || ((len != 8) && (len != 10)))
    {
        sprintf(rtn.msg, date_err);
        shut_down(rtn.msg, DATE_ERR);
    }

    strncpy(month, date, 2);
    month[2] = '\0';
    d->month = atoi(month);

    if ((d->month <1) ||( d->month > 12))
    {
        sprintf(rtn.msg, month_range_err, month);
        shut_down(rtn.msg, MONTH_RANGE_ERR);
    }

    strncpy(day, date+3, 2);
    day[2] = '\0';
    d->day = atoi(day);
    
    if ((d->day <1) || (d->day > 31))
    {
        sprintf(rtn.msg, day_range_err, day);
        shut_down(rtn.msg, DAY_RANGE_ERR);
    }

    strncpy(year, date+6, 4);
    d->year = atoi(year);
    
    if (d->year == 99) d->year = 1999;
    else if (d->year < 99) d->year += 2000;
    if (d->year < 1999)
    {
        sprintf(rtn.msg, year_range_err, year);
        shut_down(rtn.msg, YEAR_RANGE_ERR);
    }

    return(SUCCESS);
} /* end date_calc */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
