/****************************************************************************
 *
 * Function:	keyword_lookup
 *
 * Description:	Looks up file keyword in keyword file
 *		The keyword file holds mappings between
 *		secure user keywords and the
 *		associated file keywords.
 *		Takes as input the secure keyword and
 *		returns the decrypted keyword.
 *
 ***************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "intercom_server_prototypes.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_lib_prototypes.h"
#include "user_params.h"
#include "global_debug.h"
#include "customer_table.h"



void format_secure_keyword_authorization_error(char *owner_email, char *secure_key )
{
     extern int customer_type;
     if (customer_type == EXTERNAL_CUSTOMER)
     {
         sprintf(rtn.msg, external_encryption_auth_err, secure_key, owner_email, server.site, user.email);
     }
     else
     {
         sprintf(rtn.msg, internal_encryption_auth_err, secure_key, owner_email, server.site, user.login, user.email);
     }
}

/* returns  SUCCESS user no able to look at keyword, otherwise returns -1 */
int is_user_allowed_to_look_secure_keyword(char *owner_email)
{

    int ret = -1;
    if (    gu.req_type == DBPUT_ACK
        ||  maillist_auth_check(owner_email) == SUCCESS
        ||  support_auth_check(owner_email)  == SUCCESS
        ||  mica_auth_check(owner_email)     == SUCCESS
        ||  keyword_auth_check(owner_email)  == SUCCESS
        ||  service_account_check()          == SUCCESS)
    {

        ret = SUCCESS;
    }
    return ret;
}


int keyword_lookup(char *old_key, char *new_key)
{   
    char line[1024];
    char tmp_key[64];
    FILE *fp;
    char secure_keyword[64];
    char reg_email[128];

    DBG("old_key='%s', new_key='%s'", old_key, new_key);
    DBG("the keyword file is '%s'", KEYWORD_FILE);

    if ((fp = fopen(KEYWORD_FILE, "r")) != NULL)
    {               
        while (long_line_get(fp, line) != EOF)
        {
            DBG("line='%s'", line);
            sscanf(line, "%s %s %s", secure_keyword, tmp_key, reg_email);
            DBG("secure_keyword='%s', tmp_key='%s', reg_email='%s'", secure_keyword, tmp_key, reg_email);
            if (strcmp(secure_keyword, old_key) == SUCCESS)
            {
                DBG("Entering IF strcmp(secure_keyword, old_key) == SUCCESS");
                fclose(fp);
                // if that is secure keyword only the owner can see it
                if (is_user_allowed_to_look_secure_keyword(reg_email) == SUCCESS)
                {
                    DBG("Positive check reg_email='%s' against a terrific logic", reg_email);
                    strcpy(new_key, tmp_key);
                    return SUCCESS;
                } /* End if */
                else
                {
                    DBG("Failed to check reg_email='%s' against a terrific logic", reg_email);
                    format_secure_keyword_authorization_error(reg_email, old_key);
                    return ENCRYPTION_AUTH_ERR;
                }
            } /* End if for keyword found */
        } /* End while */

        /* If reached this point keyword was not found */
        /* Close file and return error to user */
        fclose(fp);
    }
    DBG("no key found returning NO_ENCRYPTION_KEY");
    return  NO_ENCRYPTION_KEY;
} /* End keyword_lookup */


/* Lookup the keyword in the database*/
int  keyword_database_lookup(
    char *secure_key,
    char *file_key)	/* Holds decrypted key if found */

{
    char keyword_file[32];
    char tmp_key[64] = {0};
    FILE *fp;
    char new_secure_key[64];
    char reg_email[128];
    int  ret = SUCCESS;

    DBG();

    /* build name of keyword file */
    strcpy2(keyword_file, KEYWORD_DIR, secure_key);
    if ((fp = fopen(keyword_file, "r")) == NULL)
    {
        ret = keyword_mapping_lookup(secure_key, tmp_key, reg_email);
    } /* End if for unable to open keyword file */
    else
    {
        fscanf(fp, "%s %s %s", new_secure_key, tmp_key, reg_email);
        fclose(fp);
    }
    if (ret == SUCCESS && tmp_key[0] != '\0')
    {
        if (is_user_allowed_to_look_secure_keyword(reg_email) == SUCCESS)
        {
            DBG("Positive check reg_email='%s' against a terrific logic", reg_email);
            strcpy(file_key, tmp_key);
            return SUCCESS;
        } /* End if */
        else
        {
            DBG("Failed to check reg_email='%s' against a terrific logic", reg_email);
            format_secure_keyword_authorization_error(reg_email, secure_key);
            return ENCRYPTION_AUTH_ERR;
        }
    }
    DBG("no key found returning NO_ENCRYPTION_KEY");
    return  NO_ENCRYPTION_KEY;
} /* End keyword_database_lookup */
