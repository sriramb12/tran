/****************************************************************************
 *
 * Function:	reg_query
 *
 * Description:	Checks for a registration entry for specified user
 *		If the entry is found, it is sent to the calling program.
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
#include "global_server_params.h"
#include "global_server_params.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


void reg_query(void)
{
    FILE *fp;
    int ret;
    int len;
    register int cnt;

    DBG();

    /* key holds entry looking for */
    /*Check for user in site registration table */
    /* Build name of registration file */
    len = strcpy1(bf.file_name, SITE_REG_PATH);

    /* key1 contains the login of the user being requested */
    /* Copy the first two initials into the file name */
    bf.file_name[len++] = gu.key1[0];
    bf.file_name[len++] = gu.key1[1];
    bf.file_name[len] = '\0';

    /* Open the registration file */
    if ((fp = fopen(bf.file_name, "r")) == NULL)
        close_down1m(rd_open_err, bf.file_name, RD_OPEN_ERR);

    /* Get length of site for use in following loop */
    len = strlen(gu.key1);
    
    /* Search alphabeticly for the name */
    while (1)
    {
	    /* Pull in a line of text */
	    /* Point to the first character of the line */
	    cnt = 0;

	    while (((rtn.msg[cnt] = getc(fp)) != '\n')  && (rtn.msg[cnt] != EOF) && (cnt < 256)) ++cnt;
	        /* If no text found, leave loop */
	        if (cnt == 0) break;

	    /* Put null at end of line */
	    rtn.msg[cnt] = '\0';

	    /* See if the right site was found */
	    if ((ret = strncmp(gu.key1, rtn.msg, len)) == 0)
	    {
	        /* Close the file */
	        fclose(fp);

	        /* Send the line to the client */
	        put_short(SUCCESS);

	        put_success_msg(NONE);
	    } /* End if for entry found */

        /* If entry pulled in greater than site, leave loop  */
	    if (ret < 0) break;
    } /* end while */

    /* If reached this point, entry was not found */
    fclose(fp);
    strcpy(rtn.msg, "entry not found");
    
    put_short(SUCCESS);
    put_success_msg(DBNO);
} /* end reg_query */
/*ts.files.fileuse*/
