/****************************************************************************
 *
 * Function:	check_reg
 *
 * Description:	Verifies user is registered at this internal site
 *
 * Returns:	Returns 'DBYES' if Email address is found.
 *		    Sends error message to client and exits if Email address is
 *		    not found. The primary Email address is returned in parameter
 *		    'gu.email1' if it is found.
 *		    Secondary Email address is returned in 'gu.email2'.
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "close_down.h"
#include "customer_table.h"
#include "edc_defines.h"
#include "edc_prototypes.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "rmt_server.h"
#include "trans_server_globals.h"
#include "trans_server_errs.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "reg_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_errs.h"
#include "x500_defines.h"
#include "global_debug.h"


/* Decrypt user's login name if using GUI */
void login_decrypt (void)
{
    //int core_id_crypt();
    int ret;
    extern int core_id_timeout;

    DBG();

    if (gu.req_type != DBX500AUTHEN)
    {
        /* Check for encrypted Core ID */
        if ((strlen(user.login) > 35) && ((strcmp(gu.machine_type, "InterComGUI") == 0)
            || (strcmp(gu.machine_type, "Linux GUI") == 0) || (strcmp(gu.machine_type, "Windows GUI") == 0)
            || (strcmp(gu.machine_type, "TransGUI") == 0)))
        {
	        /* Decrypt the Core ID */
	        ret = core_id_crypt(user.login, DECRYPT) ;

	        if (ret > core_id_timeout)
                data_log(tr.log_file, expired_core_id);
	    } /* end if for GUI */

	    /* copy decrypted core ID into email1 */
	    strcpy(gu.email1, user.login);
    } /* End if for not X500 authentication */
} /* end login_decrypt */


void x500authenticate(char *user_id, char *user_pwd, int edc_type)
{
    int ret = -1;
    char decrypt_pwd[64];

    DBG();

    /* Password is in integer representation of ASCII characters */
    /* Scan into integer array and decrypt  */
    decrypt_ascii_pwd(user_pwd, decrypt_pwd);
    ret = ldapfsl_user_authenticate_and_copy_global_structures(user_id, decrypt_pwd);

    if (ret == DBYES)
    {
        /* build entry to send back to client */
        rtn.len = strcpy3(rtn.msg, user.email, ";", user.name);
        DBG("rtn.msg=%s user.email=%s user.name=%s user.login=%s" , rtn.msg, user.email, user.name, user.login);

	    /* If this is the Intercom GUI send back an encrypted form of the Core ID */
	    if ((strcmp(gu.machine_type, "InterComGUI") == 0) || (strcmp(gu.machine_type, "TransGUI") == 0)
	        || (strcmp(gu.machine_type, "Linux GUI") == 0) || (strcmp(gu.machine_type, "Windows GUI") == 0))
	    {
            char encrypted_core_id [sizeof(user.login)];
            strcpy(encrypted_core_id, user.login);
            ret = core_id_crypt(encrypted_core_id, ENCRYPT);

	        /* Add the encrypted Core ID to the message */
            strcpy2(rtn.msg+rtn.len, "\n", encrypted_core_id);
	    } /* end if */

	    /* Now tell user authentication successful */
	    put_short(SUCCESS);
        put_success_msg("");
        DBG("tell user authentication successful");

#if !defined(DEVEL_USING_SINGLE_PROCESS)
        exit(0);  // production
#else
        return;
#endif
    } /* End if for authentication successful */

    if (edc_type == EXTERNAL_CUSTOMER && ldapfsl_exists_account_by_coreid(user_id) == DBYES)
    {
        /* March 2018, Carlos Mazieri
         *
         * extern icom client production version 7.0  using .icomrc does not bring the password, current servers should have a work around
         * to let the authentication work,  compare client request buffer:
         *
         * with .icomrc bf.buffer="~gtAy3g^g 37 39.;carlos.mazieri@gmail.com;lvd7005;10.29.240.39;/home/b35201;7.0;Linux64;1519487805 0 0 0"
         * no   .icomrc bf.buffer="~gtAy3g^g 37 83.126.118.65.30.59.37.81.;carlos.mazieri@gmail.com;lvd7005;10.29.240.39;/home/b35201;7.0;Linux64;1520599780 0 0 0~
         *
         *
         *              no .icomrc                     |  with .icomrc
         *              -----------------------------  | -----------------
         * gu.key1       83.126.118.65.30.59.37.81.    |  39.
         *
         *
         * work around for external customer authentication using .icomrc?
         *
         *   version <= 7.0 && user_pwd like "39." && registred
         */

        int is_password_encrypted = 1;
        int counter = 0;
        for (; user_pwd != 0 && *user_pwd != '\0'; ++counter, ++user_pwd)
        {
            if ( isdigit(*user_pwd) == 0 && *user_pwd != '.')
            {
                is_password_encrypted = 0;
            }
        }
        if (strcmp(user.client_version, "7.0") <= 0 && is_password_encrypted && (counter > 0 && counter < 6))
        {
            if (sql_external_reg_check(user_id) == APPROVED)
            {
                put_short(SUCCESS);
                put_success_msg("");
                DBG("authentication bypassed for old icom clients using .icomrc");
#if !defined(DEVEL_USING_SINGLE_PROCESS)
        exit(0);  // production
#else
        return;
#endif
            }
        }
        shut_down(registration_url_err, REGISTRATION_URL_ERR);
    }
	else
	{
	    char msg[256];

	    strcpy3(msg, authen_err, "\n\nError message: ", rtn.msg);
	    shut_down(msg, AUTHEN_ERR);
	} /* End else */
} /* End x500authenticate */
