/****************************************************************************
 *
 * Function:	lookup_release_keyword
 *
 * Description:	Looks up file keyword for release keyword.
 *		Typical release keywords are 'sunos_trans',
 *		hpux_trans, etc.  Returns file keyword to calling program.
 *
 * Parameters:
 *      char *release_key   -   keyword that describves the release
 *      char *file_key  -   Associated file keyword returned to calling function
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

void lookup_release_keyword(char *release_key, char *file_key)
{
    DBG("release_key='%s', file_key='%s'", release_key, file_key);

    FILE *fp;
    char new_key[16];	/* Holds release key from file */

    if ((fp = fopen(RELEASE_KEYWORD_FILE, "r")) == NULL)
        close_down1m(rd_open_err, RELEASE_KEYWORD_FILE, rd_open_err);

    while (fscanf(fp, "%s %s", new_key, file_key) != EOF)
    {
	    if (strcmp(new_key, release_key) == 0)
	    {
	        fclose(fp);
	        return(SUCCESS);
	    } /* End if for keyword found */
    } /* End while */

    /* If reached this point keyword was not found */
    /* Close file and return failure */
    fclose(fp);
    close_down1f(non_err, release_key, NON_ERR);
} /* End lookup_release_keyword */
