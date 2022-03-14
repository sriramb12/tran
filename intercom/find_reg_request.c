/****************************************************************************
 *
 * Function:	find_reg_request
 *
 * Description:	Finds previous registration request inn site registration file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "trans_server_paths.h"
#include "global_debug.h"

int find_reg_request(user_site)
    char *user_site;	/* site of user looking for */
{
    char site[48];
    FILE *fp;

    DBG();

    /* Open the mail registration file */
    /* If file not present, site is not present */
    if ((fp = fopen(SITE_REG_REQUEST, "r")) == NULL)
    return(DBNO);
    /* Pull in lines of text until find match or reach end of file */
    while (fscanf(fp, "%s", site) != EOF)
    {
	/* See if a match is found */
	if (strcmp(user_site, site) == 0)
	{
	    /* Close the file and return matching status */
	    fclose(fp);
	    return(DBYES);
	} /* end if */
    } /* end while */
    /* If reach this point, entry was not found */
    /* Close the mail registration file */
    fclose(fp);
    return(DBNO);
} /* End find_reg_request */
