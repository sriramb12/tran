/****************************************************************************
 *
 * Functions:	line_scan, arg_scan, string_scan
 *
 * Description:	Routines to scan in user input
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "global_client_errs.h"
#include "global_defines.h"
#include "server_params.h"
#include "strcpy.h"
#include "transapi_defines.h"


#include "global_debug.h" // Mazieri's debug facilities


/* Get an input line */
void line_scan(char *line)
{
    DBG();
    int cnt = 0;

    /* Ignore leading spaces */
    while ((line[cnt] = getchar()))
    {
	if ((line[cnt] != ' ') && (line[cnt] != '\t') && (line[cnt] != '\n'))
	{
	    ++cnt;
	    break;
	} /* End if for non white space found */
    } /* End while */
    while ((line[cnt] = getchar()) != '\n') ++cnt;
    line[cnt] = '\0';
} /* end line_scan */

/* Get an argument from the user */
/* This function allows the user to enter a carriage return */
/* and not specify any argument */
int arg_scan(char *line)
{

    DBG( "line=%s", line );

    int cnt  = 0;
    char c;

    while (((c = getchar()) != '\n') && (c != ' ') && (c != '\t'))
     line[cnt++] = c;
    if (cnt > 0)  line[cnt] = '\0';
    return(cnt);
} /* end arg_scan */

int string_scan(char *line)
{
    DBG( "line=%s", line );
    int cnt  = 0;
    char c;

    while ((c = getchar()) != '\n') 
     line[cnt++] = c;
    if (cnt > 0)  line[cnt] = '\0';
    return(cnt);
} /* end string_scan */
