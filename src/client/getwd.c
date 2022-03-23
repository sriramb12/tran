#ifndef TRANSAPI
/****************************************************************************
 *
 * NXP Semiconductor N.V. Confidential and Proprietary Information.
 *
 * Function:	getwd
 *
 * Description:	Retrieve current working directory.
 *
 * Original Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <sys/param.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>

#include "global_debug.h"



char * getwd(name)
char * name;
/* Function: places full pathname of current working directory into name and
 * returns name on success, NULL on failure.
 * getwd is an adaptation of pwd. May not return to the current directory on
 * failure.
 * LIMITATION:  You the end user of this function must assure that you provide
 *              enough allocated space in the calling argument to hold the 
 *              value returned.  This value is defined to be MAXPATHLEN defined
 *              by the system you are on!!!
 */
{
    DBG();
    int len;
	FILE    *po;

    po = popen("pwd","r");
    if (po) 
    {
	if (fgets(name,MAXPATHLEN,po)) 
	{
	pclose(po);
    len = strlen(name)-1;
	    if (name[len] == '\n') name[len] = '\0';
	    return(name);
	}
	else
	{
	pclose(po);
	    name[0] = '\0';
	    return(NULL);
	} /* End else */
    } /* end if */
    else
    {
	name[0] = '\0';
	return(NULL);
    } /* End else */
} /* End getwd */
#endif /* For not TRANSAPI */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
