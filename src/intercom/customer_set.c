#include <strings.h>
#include <string.h>
#include "customer_table.h"
#include "global_server_params.h"
#include "strcpy.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "user_params.h"
#include "global_debug.h"

#ifdef TRANS_FTP
/* Set FTP_TRAnS settings */
void trans_ftp_set(char *remote_dir)
{
    /* Copy the remote directory from gu.email2 before registration checked */
    strcpy(remote_dir, gu.email2);

    if (strcmp(remote_dir, NONE) == 0) {
        /* If no value for remote directory was given, */
        name_build(remote_dir);
    }
} /* end trans_ftp_set */
#endif


static int isExtenalClient()
{
    int ret = strncasecmp(gu.from_company, "External", 8) == 0 ? 1 : 0;
    DBG("ret=%d gu.to_company=%s gu.from_company=%s XIO_COMPANY=%s FREE_COMPANY=%s", ret, gu.to_company, gu.from_company, XIO_COMPANY, FREE_COMPANY);
    return ret;
}



/* Set customer type */
int customer_set(char *remote_dir)
{
    extern TRANS_PARAMS gu;
    char *ptr;
    int customer;

    DBG();

#ifdef TRANS_FTP
    customer = TRANS_FTP;
    /* Call function to set TransFTP settings */
    trans_ftp_set(remote_dir);
    return(customer);
#endif

#ifdef INTERCOM
    DBG("#ifdef INTERCOM");   
    if (isExtenalClient())
    {
         customer = EXTERNAL_INTERCOM;
    }
    else
    {
        customer = INTERNAL_INTERCOM;
    }
    return(customer);
#endif

#ifdef TRANSWEB
    /* Must set the Mass Market Customer first */
    if ((strlen(gu.to_company) == 29) && (strcmp(gu.to_company, "External Mass Market Customer") == 0))
    {
	    /* If not authorized to send data give error */
	    if (entry_find(SUPPORT_EMAIL_LIST, user.login) != SUCCESS)
	        shut_down(support_auth_err, SUPPORT_AUTH_ERR);

	    customer = INTERNAL_MM;
	    strcpy(gu.to_company, TRANSWEB_COMPANY);
	    strcpy(user.name, "NXP Support");
	    strcpy(user.email, SUPPORT_EMAIL);
    }
    /* If coming from internal site, set to internal customer */
    else if (strcmp(gu.from_company,  FREE_COMPANY) == 0)
    {
	    customer = INTERNAL_TRANSWEB;
	    /* If friendly e-mail not initialized, set it */
        /* 	if (strcmp(gu.email2, NONE) == 0) transweb_internal_email_map(); */
    } /* End if */
    /* If NXP customer coming from external site, make this an internal customer */
    else if (((strlen(gu.email1) > 8) && (strcasecmp(gu.email1+strlen(gu.email1)-8, "@nxp.com") == 0)) || (strchr(gu.email1, '@') == NULL))
    {
	    customer = INTERNAL_TRANSWEB;

	    /* If wrong company set, modify it */
	    if (strcmp(gu.from_company, FREE_COMPANY) != 0)
	    {
	        strcpy(gu.to_company, gu.from_company);
	        strcpy(gu.from_company, FREE_COMPANY);
	    } /* end if for company not set correctly */

	    /* If friendly e-mail not initialized, set it */
        /* 	if (strcmp(gu.email2, NONE) == 0) transweb_internal_email_map(); */
    } /* end if */
    /* External customer coming from external site */
    else if (strcmp(gu.from_company, TRANSWEB_COMPANY) == 0)
        customer = EXTERNAL_TRANSWEB;
#endif

    return(customer);
} /* End customer_set */


/* See if customer registered in external LDAP */
int customer_ldap_check(void)
{
    DBG();
    return(DBYES);
} /* end customer_ldap_check */


void customer_email_log(char *email1, char *email2)
{
    char email_log_file[48];
    char log_string[120];

    DBG();

    strcpy2(email_log_file, "email_", tr.log_file);
    strcpy3(log_string, email1, ";", email2);

    data_log(email_log_file, log_string);
} /* End customer_email_log */

/*es.files.fileuse*/
/*is.files.fileuse*/
