#ifndef TRANS_FTP
/****************************************************************************
 *
 * Function:	rmt_dmi_reg
 *
 * Description: Look up user in local TransWeb registry
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "close_down.h"
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "user_params.h"
#include "rmt_server.h"
#include "rtn.h"
#include "registry_prototypes.h"
#include "strcpy.h"
#include "trans_lib_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "x500_defines.h"
#include "ldapfsl.h"
#include "global_debug.h"


/* Look up user in local TransWeb registry */
int transweb_reg_check(void)
{
    char *ptr;
    char local_email[260];
    char email1[260];
    char email2[260];
    char name[260];
    FILE *fp;
    char file_name[64];
    char lock_char[4];
    int len;
    char line[512];
    char company[64];
    char tmp_user_id[260];
    int  is_legacy_freescale = 0;

    DBG();

    /* Return if this is a ping request */
    if ((gu.req_type == DBPING) || (gu.req_type == DBSILENT_PING))
        return(SUCCESS);

    /* Make sure the login is in lower case */
    str_lower(user.login);

    /* For TransWeb user.login and user.email are the same */
    /* Make them the same to avoid confustion later */
    strcpy(user.email, user.login);

    if (strchr(user.email, '@') != NULL && strcasestr(user.email, "@freescale.com") != NULL)
    {
         is_legacy_freescale = 1;
    }

    /* Build name for registry file */
    len = strcpy1(file_name, "transweb_registry/site_reg.xx");
    file_name[len-2] = user.email[0];
    file_name[len-1] = user.email[1];

    /* Build the string to search for */
    strcpy(local_email, user.email);

    // if registry file does not exist create it
    if ((fp = fopen(file_name, "r")) == NULL)
    {
        return (transweb_reg_add(file_name, local_email));
    }

    while (line_get(fp, line) != EOF)
    {
	    if (sscanf(line, "%[^;]%*c%c%*c%[^;]%*c%[^;]%*c%[^;]%*c%s", tmp_user_id,  &lock_char, email1, email2, name, company) != 6)
	    {
	        /* If innvalid format found, tell database administrator */
	        sprintf(rtn.msg, invalid_reg_format, file_name, 0, line);
	        email_send(admin.err_email, admin.err_email, "TransWeb error reading registration file", rtn.msg);
	        continue;
	    } /* End if for invalid format found */

            if ( is_legacy_freescale == 0
                && (strcasecmp(local_email, email1) == 0 ||
                    strcasecmp(local_email, email2) == 0))
	    {
	        /* Copy in company for user */
	        strcpy(user.name, name);
	        strcpy(gu.email2, email2);
	        strcpy(gu.email1, email1);
	        /* Put primary e-mail in lower case */
	        str_lower(gu.email1);
	        return(SUCCESS);
	    } /* End else */
    } /* end while */

    /* If reached this point, user not found */
    fclose(fp);

    return(transweb_reg_add(file_name, user.email));
} /* End transweb_reg_check */



/* Add user to TransWeb registry */
int transweb_reg_add(char *file_name, char *user_id)
{
    char line[260];
    int ret = FAILURE;

    DBG("file_name='%s' and user_id='%s'", file_name, user_id);

    LDAP_RETURN_DATA * ldap_ret = ldap_ret = ldapfsl_lookupByCoreId(user_id);
    if (BAD_LDAP_RETURN_DATA(ldap_ret) && strchr(user_id, '@') != 0)
    {
        ldapfsl_free_return_data(ldap_ret);
        ldap_ret = ldapfsl_lookupByMail(user_id);
    }
    if (GOOD_LDAP_RETURN_DATA(ldap_ret))
    {
         ldapfsl_copy_info_to_global_user_structure(ldap_ret);
         ldapfsl_copy_info_to_global_trans_structure(ldap_ret);
          /* build entry to add from information already retrieved */
          sprintf(line, "%s;0;%s;%s;%s;NXP", user.login, gu.email1, gu.email2, user.name);
          if (add_entry(file_name, line) == 0)
          {
               ret = DBYES;
          }         
          /* Since user just added, set his lock count at 0 */
          user_lock_cnt = 0;
    }
    ldapfsl_free_return_data(ldap_ret);
    return ret;
} /* End transweb_reg_add */
#endif
