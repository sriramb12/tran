/****************************************************************************
 *
 * Function:	reg_lookup
 *
 * Description:	Verifies user is a registered user
 *		by lookin him up in the DMI registry.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <string.h>
#include "global_defines.h"
#include <stdio.h>
#include "global_server_errs.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "rtn.h"
#include "strcpy.h"
#include "user_params.h"

#include "global_debug.h" // Mazieri's debug facilities

int reg_lookup(void)
{
#ifdef NXP
    char *ptr;
#endif
    int lock_cnt;
    int line_cnt = 0;
    char line[260];
    extern TMP_BUFFER bf;
    FILE *fp;
    char fnd = DBNO;
    char reg_site[32];

    DBG();

    /* Check to see if user is in registration file */
    /* Build the name for the registration file */
    reg_name_build(bf.file_name, user.email);

    /* Open the registration file */
    if ((fp = fopen(bf.file_name, "r")) == NULL)
        return(DBNO);

    while (line_get(fp, line) > 0)
    {
	    if (sscanf(line, "%s %d %s %*s", reg_site, &lock_cnt, tr.user_email) != 3)
	    {
	        /* If innvalid format found, tell database administrator */
	        sprintf(rtn.msg, invalid_reg_format, bf.file_name, line_cnt, line);
	        email_send(admin.err_email, admin.err_email, "OPCtrans error reading registration file", rtn.msg);
	    } /* End if for invalid format found */

	    if (strcmp(tr.user_site, reg_site)== 0)
	    {
#ifdef OPCTRANS
	        if (lock_cnt == 0) fnd = 3;
	        else fnd = DBYES;
#else
	        fnd = DBYES;
#endif

#ifdef ONSEMI
	        str_strip(tr.user_email, "@onsemi.com");
#endif

	        break;
	    } /* End if for entry found */

        ++line_cnt;
    } /* End while */

    /* Close the file */
    fclose(fp);

    return(fnd);
} /* end reg_lookup */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
