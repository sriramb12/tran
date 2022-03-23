/****************************************************************************
 *
 * Function:	keyword_find
 *
 * Description:	Adds new groups for special Transcend transactions
 *
 ***************************************************************************/

#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "keyword.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_prototypes.h"
#include "reg_globals.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_lib_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"
#include "ldapfsl.h"

#include "global_debug.h"


#define POWER_SERVICE_ACCOUNTS "db_admin/power_accounts"


void support_add(char *email_file)
{
    char *users[100];
    int cnt;

    DBG();

    /* Make sure user authorized to perform this transaction */
    support_check(email_file);

    /* Tell client ready to get data */
    put_short(SUCCESS);

    /* Get list of elements to add */
    cnt = list_put(users);

    for (cnt = 0; users[cnt] != '\0'; ++cnt)
    {
	    /* Put the entry in lower case */
	    str_lower(users[cnt]);
	    add_entry(email_file, users[cnt]);
	    free(users[cnt]);
    } /* End for */

    /* Indicate users added */
    return_success("Specified customer(s) added.", DBNO);
} /* end support_add */


void support_delete(char *email_file)
{
    char *users[100];
    int cnt;

    DBG();

    /* Make sure user authorized to perform this transaction */
    support_check(email_file);

    /* Tell client ready to get data */
    put_short(SUCCESS);

    /* Get list of elements to add */
    cnt = list_put(users);
    for (cnt = 0; users[cnt] != '\0'; ++cnt)
    {
	    /* Put user ID in lower case */
	    str_lower(users[cnt]);
	    dlt_entry(email_file, users[cnt]);
	    free(users[cnt]);
    } /* End for */

    /* Indicate users deleted */
    return_success("Specified customer(s) deleted.", DBNO);
} /* end support_delete */

int service_account_check(void)
{
    char user_id[32];
    char auth_file[48];
    int ret = FAILURE;

    /* Build name of file */
    strcpy(auth_file, POWER_SERVICE_ACCOUNTS);

    /* Build user ID */
    strcpy(user_id, user.login);

    /* Put user ID in lower case */
    str_lower(user_id);

    if ((ret = entry_find(auth_file, user_id)) != SUCCESS)
    {
        ret = FAILURE;
    }

    DBG("ret=%d", ret);
    return ret;
}

int support_check(char *email_file)
{
    char user_id[32];   
    char auth_file[48];
    int ret = FAILURE;

    /* Build name of file */
    strcpy2(auth_file, email_file, ".auth");

    /* Build user ID */
    strcpy(user_id, user.login);

    /* Put user ID in lower case */
    str_lower(user_id);

    if ((ret = entry_find(auth_file, user_id)) != SUCCESS)
    {
        ret = FAILURE;
    }

    DBG("ret=%d", ret);
    return ret;
} /* End support_check */


/* Verify authorized to retrieve keywords sent to support@nxp.com */
int support_auth_check(char *email_addr)
{
    DBG("email_addr='%s'", email_addr);

    /* This list is checked only against e-mail sent to support@nxp.com */
    if (strcasecmp(email_addr, SUPPORT_EMAIL) != SUCCESS)
    {
        DBG("will return FAILURE");
        return(FAILURE);
    }

    if (entry_find(SUPPORT_EMAIL_LIST, user.login) == SUCCESS)
    {
        DBG("will return SUCCESS");
        return(SUCCESS);
    }
    return FAILURE;
} /* End support_auth_check */


int maillist_auth_check(char *auth_email)
{
    FILE *fp;
    int ret = FAILURE;
    char line[260];
    char local_email_list[60];
    char local_email_addr[60];

    DBG("auth_email='%s'", auth_email);

    /* Open the mail list authrozation file */
    if ((fp = fopen(MAILLIST_AUTH_FILE, "r")) == NULL)
        DBG("will return FAILURE");
        return(FAILURE);

    /* Now check for match on each line */
    while (line_get(fp, line) != EOF)
    {
	    if (sscanf(line, "%s %s", local_email_list, local_email_addr) != 2) continue;
	
	    /* If match found, check to see if authorized person is the same as person making call */
	    if (strcasecmp(local_email_list, auth_email) != 0) continue;

	    ret = keyword_auth_check(local_email_addr);
	    if (ret == SUCCESS) break;
    } /* End while */

    /* Close the file */
    fclose(fp);
    DBG("will return ret='%d'", ret);
    return(ret);
}


/* Verify authorized to retrieve keywords sent to mica@nxp.com */
int mica_auth_check(char *email_addr)
{
    DBG("email_addr='%s'", email_addr);

    /* This list is checked only against e-mail sent to mica@nxp.com */
    if (strcasecmp(email_addr, MICA_EMAIL) != SUCCESS) 
    {
        DBG("Will return FAILURE");
        return(FAILURE);
    }

    if (entry_find(MICA_EMAIL_LIST, user.login) == SUCCESS)
    {
        DBG("Will return SUCCESS");
        return(SUCCESS);
    }

    return(FAILURE);
} /* End mica_auth_check */


/* Check to see if user authorized to access this keyword */
int keyword_auth_check(char *email)
{
    char private_email[128] = {0};
    int ret  = SUCCESS;
    if (strchr(email, '@') == NULL)
    {
        LDAP_RETURN_DATA* user_info    = ldapfsl_lookupByCoreId(email);
        if (GOOD_LDAP_RETURN_DATA(user_info))
        {
            strcpy(private_email, user_info->info[0]->friendly_email);
            email = private_email;
        }
        ldapfsl_free_return_data(user_info);
    }


       /* first try easy comparison */
       if ((strcasecmp(user.email, email) != 0) && (strcasecmp(user.login, email) != 0)) {
           DBG("easy comparison was NOT success");
           ret = FAILURE;
       }

       /*
        *   do not want to do chopping off for Photronics addresses

       if (strncmp(email, "dpmot.dis@", 10) == 0) {
           DBG("do not want to do chopping off for Photronics addresses");
           ret = FAILURE;
       }
       */

   DBG("email='%s' ret=%d", email, ret);
   return(ret);
} /* End keyword_auth_check */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
