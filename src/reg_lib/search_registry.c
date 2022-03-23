#ifdef NXP
/****************************************************************************
 *
 * Function:	load_registry
 *
 * Description:	Loads user registry into memory.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *user_email1
 *  char *user_email2
 *  char *registry - Name of the registry file
 *  REG_PARAMS *reg - Holds user's name and Email address if found
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "line.h"

#include "global_debug.h" // Mazieri's debug facilities

int bsearch_registry(char *user_email1, char *user_email2, char *registry, REG_PARAMS *reg)
{
    int email1_len;
    int email2_len;
    FILE *fp;

    DBG("user_email1='%s', user_email2='%s', registry='%s'", user_email1, user_email2, registry);

    /* Open the registry */
    if ((fp = fopen(registry, "r")) == NULL) return(DBNO);

    /* calculate length of email addresses */
    email1_len = strlen(user_email1);
    email2_len = strlen(user_email2);

    while (line_get(fp, bf.buffer) > 0)
    {
	    /* Pull out the Email addresses  */
	    sscanf(bf.buffer, " %s %s",	reg->email1, reg->email2);

	    /* Compare with existing email addresses */
	    if ((strncmp(user_email1, reg->email1, email1_len) == 0)
	        || (strncmp(user_email1, reg->email2, email1_len) == 0)
	        || (strncmp(user_email2, reg->email1, email2_len) == 0)
	        || (strncmp(user_email2, reg->email2, email2_len) == 0))
	    {
	        /* Scan out the names */
	        sscanf(bf.buffer, "%*s %*s %s %s %*s %*s %*s %s", reg->name1, reg->name2, reg->phone);

	        /* Convert email address to lower case */
	        str_lower(reg->email1);
	        strcat(reg->email1, EMAIL_SUFFIX);

            DBG("Email '%s' found in the registry, name1='%s' name2='%s' and phone='%s'", reg->email1, reg->name1, reg->name2, reg->phone);

	        /* Close file no longer needed, return from here since email was found */
	        fclose(fp);
	        return(DBYES);
	    } /* End if for Email address found */
    } /* End while */

    /* Close the registry file */
    fclose(fp);
    
    /* If reached this point, Email was not found */
    /* If return message not already initialized */
    /* Put in message telling user entry in core directory not present */
    if (rtn.len <= 0)
        rtn.len = strcpy1(rtn.msg, reg_core_err);


    DBG("Cannot find parameters user_email1='%s', user_email2='%s' at registry='%s'", user_email1, user_email2, registry);
    return(DBNO);
} /* End search_registry */
#endif
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
