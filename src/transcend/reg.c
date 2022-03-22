/****************************************************************************
 *
 * Function:	x500authenticate
 *
 * Description:	Verifies user is registered to use Transcend.
 *
 * Returns:	Returns 'DBYES' if Email address is found.
 *		    Sends error message to client and exits if
 *		    Email address is not found.
 *		    The primary Email address is returned in parameter
 *		    'user_email1' if it is found. Secondary Email address
 *		    is returned in 'user_email2'.
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "close_down.h"
#include "edc_defines.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_server_prototypes.h"
#include "reg_globals.h"
#include "user_params.h"
#include "rmt_server.h"
#include "rtn.h"
#include "server_lib.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"
#include "ldapfsl.h"


/* Registration is not done with the authentication */
void x500authenticate(char *user_id, char *user_pwd)
{
    char *ptr;
    int i[32];
    char ascii_pwd[20];
    int ret;
    char decrypt_pwd[20];
    char crypted_userlogin[128];
    int cnt;

    DBG("user_id='%s', user_pwd='%s'", user_id, user_pwd);

    extern char cert7db[];
    ldapfsl_setCaCertFileName(cert7db);
    LDAP_RETURN_DATA * ldap_ret = NULL;

    request_log();

    /* Password is in integer representation of ASCII characters */
     /* Scan into integer array */
    ret = sscanf(user_pwd, "%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.",
        &i[0], &i[1], &i[2], &i[3], &i[4], &i[5], &i[6], &i[7], &i[8], &i[9], &i[10], &i[11], &i[12], &i[13], &i[14], &i[15],
        &i[16], &i[17], &i[18], &i[19], &i[20], &i[21], &i[22], &i[23], &i[24], &i[25], &i[26],  &i[27], &i[28], &i[29], &i[30], &i[31]);

    /* Build string with ASCII values */
    for (cnt = 0; cnt <ret; ++cnt) {
        ascii_pwd[cnt] =  i[cnt];
    }

    /* Terminate string with null character */
    ascii_pwd[ret] = '\0';

    /* Now decrypt the password */
    password_encrypt(decrypt_pwd, ascii_pwd, ret);


    /*ret = company_authenticate(user_id, decrypt_pwd, gu.email1, gu.email2, user.name); should fill gu.email1, gu.email2, user.name */
    ldap_ret = ldapfsl_authenticate_user_id(user_id, decrypt_pwd);

    if (BAD_LDAP_RETURN_DATA(ldap_ret))
    {
        ldapfsl_free_return_data(ldap_ret);
        ldap_ret = 0;
        shut_down(authen_err, AUTHEN_ERR);
    }

    strcpy3(rtn.msg, ldap_ret->info[0]->friendly_email, ";", ldap_ret->info[0]->name);

#ifdef TRANSCEND
    /* Must do copy here because not previously done if this is Transcend */
    strcpy(user.email, ldap_ret->info[0]->friendly_email);
#endif

    ret = transweb_reg_check();

	/* Now tell user authentication successful */
    put_short(SUCCESS);

    strcpy (crypted_userlogin, user.login);
    /* Encrypt user ID and send to user */
    if (is_gui() == DBYES)
    {        
        core_id_crypt(crypted_userlogin, ENCRYPT);
    } /* End if */

    strcpy2(rtn.msg+strlen(rtn.msg), "\n", crypted_userlogin);

    data_log(tr.log_file, rtn.msg);
    put_success_msg(NONE);
    ldapfsl_free_return_data(ldap_ret);

    exit(0);
} /* End x500authenticate */


void x500lookup(char *user_id)
{
    DBG("user_id='%s'", user_id);

    /* Log the request */
    request_log();

    /* Look up user in data transfer registry */
    if (sql_dt_registry_lookup((user_id, "0") >= 0))
    {
	    strcpy3(rtn.msg, gu.email2, ";", user.name);

	    /* Tell user authentication successful */
	    put_short(SUCCESS);
        put_success_msg(NONE);

        exit(0);
    } /* End if for lookup successful */
    /* If return not successful, give failure */
    else
    {
	    sprintf(rtn.msg, reg_verify_err, user_id);
	    shut_down(rtn.msg, REG_VERIFY_ERR);
    } /* end else */
} /* End x500lookup */
