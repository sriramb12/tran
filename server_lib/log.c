/****************************************************************************
 *
 * Function:	log
 *
 * Description:	Logs database mesages.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include "close_down.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "strcpy.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "rtn.h"

#include "global_debug.h" // Mazieri's debug facilities


/* Logs current transaction */
int log(char *log_file, char *string)
{
    DBG("log_file='%s', string='%s'", log_file, string);

    extern int log_cnt;
    FILE *log_ptr;

    if(log_cnt >= 100) exit(1);
    
    ++log_cnt;
    
    if ((log_ptr = fopen(log_file, "a")) == NULL)
    {
	    printf("Could not log message '%s' in file '%s'\n", string, log_file);
	    return(FAILURE);
    }

    fprintf(log_ptr, "%s\n", string);
    fclose(log_ptr);

    return(SUCCESS);
}


int data_log(char *log_file, char *string)
{
    DBG("log_file='%s'", log_file);
    DBG("string\n%s", string);

    FILE *log_ptr;

    if ((log_ptr = fopen(log_file, "a")) == NULL)
    {
	    printf("Could not log message '%s' in file '%s'\n", string, log_file);
	    return(FAILURE);
    }

    fprintf(log_ptr, "%s\n", string);

    fflush(log_ptr);
    fclose(log_ptr);

    return(SUCCESS);
} /* End data_log */


int delta_log(char *log_file, char *string)
{
    DBG("log_file='%s' and string='%s'", log_file, string);

    FILE *log_ptr;

    if ((log_ptr = fopen(log_file, "a")) == NULL)
    {
	    printf("Could not log message '%s' in file '%s'\n", string, log_file);
	    return(FAILURE);
    }

    fprintf(log_ptr, string);
    fflush(log_ptr);
    fclose(log_ptr);

    return(SUCCESS);
} /* End delta_log */


int data_log2(char *log_file, char *string1, char *string2)
{
    DBG("log_file='%s', string1='%s' and string2='%s'", log_file, string1, string2);

    FILE *log_ptr;

    if ((log_ptr = fopen(log_file, "a")) == NULL)
    {
	    printf("Could not log message '%s%s' in file '%s'\n", string1, string2, log_file);
	    return(FAILURE);
    }

    fprintf(log_ptr, "%s%s\n", string1, string2);
    fclose(log_ptr);

    return(SUCCESS);
} /* End data_log */


/* Scan through the file offset number of lines and log new message after offset */
int data_log_offset(char *log_file, char *string, int offset)
{
    DBG("log_file='%s', string='%s' and offset='%d'", log_file, string, offset);

    char tmp_log_file[256];
    char line[260];
    int cnt = 1;
    FILE *fi;
    FILE *fo;

    if ((fi = fopen(log_file, "r")) == NULL)
    {
	    printf("Could not log message %s in file %s\n", string, log_file);
	    return(FAILURE);
    }

    strcpy2(tmp_log_file, log_file, ".b");
    if ((fo = fopen(tmp_log_file, "w")) == NULL)
    {
	    printf("Could not log message %s in file %s\n", string, log_file);
	    return(FAILURE);
    }

    /* Read lines until offset reached or end of file reached */
    while (line_get(fi, line) >= 0)
    {
	    fprintf(fo, "%s\n", line);
        ++cnt;
	    if (cnt == offset) break;
    }

    /* Write new string out to file */
    fprintf(fo, "%s\n", string);

    /* close both files */
    fclose(fi);
    fclose(fo);

    /* Write temp file over original file */
    rename(tmp_log_file, log_file);
    return(SUCCESS);
} /* data_log_offset */


void log3(char *str1, char *str2, char *str3)
{
    DBG("str1='%s', str2='%s' and str3='%s'", str1, str2, str3);

    FILE *log_ptr;

    if ((log_ptr = fopen(tr.log_file, "a")) == NULL)
        close_down1m(wr_open_err, tr.log_file, WR_OPEN_ERR);

    fprintf(log_ptr, "%s%s%s\n", str1, str2, str3);
    fclose(log_ptr);
}


/* Build the log file name */
void bld_log_name(void)
{
    char *log_date_ptr;

    DBG();

    /* Get the current time in seconds since 1970 */
    tr.upd_time = time(0);

    /* Convert to string representation */
    sprintf(tr.upd_time_string, "%ld", tr.upd_time);
    log_date_ptr = ctime(&tr.upd_time);

    /* Copy the value into a string */
    strcpy(tr.log_date, log_date_ptr);

    /* Build the log file name */
    /* Copy in the log directory name */
    strcpy(tr.log_file, "log/00_00_00");
    log_convert(tr.log_date, tr.log_file+4);
} /* End bld_log_file */


void bld_log_name4previousday(char *log_file)
{
    DBG("log_file='%s'", log_file);

    char *log_date_ptr;
    char log_date[32];
    long upd_time;

    /* Get the current time in seconds since 1970 */
    upd_time = time(0)-86400;

    /* Convert to string representation */
    log_date_ptr = ctime(&upd_time);

    /* Copy the value into a string */
    strcpy(log_date, log_date_ptr);

    /* Build the log file name */
    /* Copy in the log directory name */
    strcpy(log_file, "00_00_00");
    log_convert(log_date, log_file);
} /* End bld_log_file4previousday */


/* The following allows me to use month '01' instead of 'Jan' */
/* Convert string representation of date to log format i.e. mm_dd_yy */
/* Result returned in time_stamp */
void log_convert(char *log_date, char *time_stamp)
{
    DBG("log_date='%s' and time_stamp='%s'", log_date, time_stamp);

    /* Change month from form 'Jan' to '01' */
    /* Fill in the month characters based on the date value */
    month_convert(log_date, time_stamp);

    /* Copy in the day of the month from positions 8 and 9 */
    if (log_date[8] != ' ')
        time_stamp[3] = log_date[8];

    time_stamp[4] = log_date[9];
    
    /* Copy the year from positions 20-21 */
    time_stamp[6] = log_date[22];
    time_stamp[7] = log_date[23];
} /* end log_convert */


void log_err(int ret, char *calling_function)
{
    DBG("ret='%i' and calling_function='%s'", ret, calling_function);

    sprintf(rtn.msg, "Return is %d for %d from %s", ret, tr.pid, calling_function);
    data_log("log/errors", rtn.msg);
} /* end log_err */


/* converts months i.e. convert Jan to 01 */
void month_convert(char *log_date, char *time_stamp)
{
    DBG("log_date='%s' and time_stamp='%s'", log_date, time_stamp);

    switch (log_date[4])
    {
	    /* Fill in numeric values for January, June, or July */
	    case 'J':
	    {
	        if (log_date[5] == 'a') time_stamp[1] = '1';
	        else
	        {
		        if (log_date[6] == 'n') time_stamp[1] = '6';
		        else time_stamp[1] = '7';
	        } /* end if */
	    }
	    break;
	    case 'A':
	    {
	        if (log_date[5] == 'p') time_stamp[1] = '4';
	        else time_stamp[1] = '8';
	    }
	    break;
	    /* Fill in numeric value for March and May */
	    case 'M':
	    {
	        if (log_date[6] == 'r') time_stamp[1] = '3';
	        else time_stamp[1] = '5';
	    }
	    break;
	    /* Fill in numeric value for February */
    	case 'F':  time_stamp[1] = '2'; break;
    	/* Fill in numeric value for September */
    	case 'S':  time_stamp[1] = '9'; break;
    	/* October */
    	case 'O':  time_stamp[0] = '1'; break;
    	/* November */
    	case 'N': 
    	{
    	    time_stamp[0] = '1';
    	    time_stamp[1] = '1';
    	}
    	break;
    	/* December */
    	case 'D': 
    	{
    	    time_stamp[0] = '1';
    	    time_stamp[1] = '2';
    	}
    	break;
        } /* end switch statement */
} /* End month_convert */
