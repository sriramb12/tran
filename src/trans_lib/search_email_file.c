/****************************************************************************
 *
 * Function:	search_email_file
 *
 * Description:	Sees if owner needs to be sent Email.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "external_intercom_prototypes.h"
#include "trans_server_paths.h"
#include "trans_server_errs.h"
#include "global_debug.h"

int search_email_file(char *owner, char *email_file)
{
    int len = strlen(owner);
    int ret;
    FILE *fp;
    char user[EMAIL_LEN];

    DBG();

    /* Open global file */
    if ((fp = fopen(email_file, "r")) == NULL)
    return(DBNO);
    /* Search for the user */
    while (line_get(fp, user) != EOF)
    {
	if ((ret = strncmp(owner, user, len)) == 0) 
	{
	    fclose(fp);
	    return(DBYES);
	}
	if (ret < 0) break;
    }
    fclose(fp);
    return(DBNO);
} /* End search_email_file */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
