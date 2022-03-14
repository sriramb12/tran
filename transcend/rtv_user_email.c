#ifdef garbage
/****************************************************************************
 *
 * Function:	rtv_user_email
 *
 * Description:	Retrieves the user's 
 *		Email address from the Email registry file
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_server_arrays.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "user_params.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

int rtv_user_email(char *user_email)
{
    DBG("user_email='%s'", user_email);

    int len;
    FILE *fp;

    /* Build the name of the site file to search */
    len = strcpy1(bf.file_name, SITE_REG_PATH);
    bf.file_name[len++] = user.email[0];
    bf.file_name[len++] = user.email[1];
    bf.file_name[len] = '\0';

    /* Open this file for reading */
    if ((fp = fopen(bf.file_name, "r")) == NULL)
        close_down1m(rd_open_err, bf.file_name, RD_OPEN_ERR);

    /* Search through the file for the site */
    while (fscanf(fp, "%s %*d %s", bf.buffer, user_email) == 3)
    {
	    /* See if this is the user's tr.user_site */
	    if (strncmp(bf.buffer, tr.user_site, len) == 0)
	    {
	        /* Close the file */
	        fclose(fp);
	        return(SUCCESS);
	    } /* End if for user's site found */
    } /* End while */

    /* If reached this point user's Email address was not found */
    /* Close the file and return failure */
    fclose(fp);

    return(FAILURE);
} /* End rtv_user_email */
#endif
