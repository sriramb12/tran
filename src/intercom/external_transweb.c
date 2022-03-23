/****************************************************************************
 *
 * Function:	External TransWeb functions
 *
 * Description:	External TransWeb functions
 *
 ***************************************************************************/

#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "client_lib.h"
#include "close_down.h"
#include "customer_table.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "global_report.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "mm_email_msg.h"
#include "registry_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_arrays.h"
#include "trans_defines.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_msg.h"
#include "trans_server_paths.h"
#include "reg_globals.h"
#include "rtn.h"
#include "trans_server_macros.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "rmt_server.h"
#include "strcpy.h"
#include "global_debug.h"

/* Send file keyword to sender */
void transweb_file_key_send(char *file_name, char *description, char *key, char *size, char *dlt_time, char *gmt_dlt_time, char **email_array, char **secure_key_array)
{
    extern int not_approved_cnt;
    int len;
    int cnt;
    char msg[BUFFER_SIZE];
    char *gmt_dlt_time_ptr;

    DBG();

    /* Build message to send */
    if (strcmp(gmt_dlt_time, "Pending authorization" ) == 0)
        gmt_dlt_time_ptr = "";
    else
        gmt_dlt_time_ptr = gmt_dlt_time;

    if (customer == EXTERNAL_INTERCOM_ONLY)
        len = sprintf(msg, INTERCOM_FILE_MSG, user.name, file_name, description, size, dlt_time, gmt_dlt_time_ptr);
    else
    {
	    if (not_approved_cnt == 0)
            len = sprintf(msg, TRANSWEB_FILE_MSG, user.name, file_name, description, size, key, dlt_time, gmt_dlt_time_ptr, email_array[0], key);
	    /* Here is where the new message is added */
        else
            len = sprintf(msg, TRANSWEB_UNAPPROVED_MSG, user.name, file_name, description, size, key, dlt_time,  email_array[0], key);
    }

    /* Now add the list of secure keywords */
    if (email_array[0] != NULL)
        len += strcpy1(msg+len, "The following personal secure keywords were sent to customers:\n\n");

    for (cnt = 0; email_array[cnt] != NULL; ++cnt)
    {
	    if (secure_key_array[cnt] != NULL)
	        len += sprintf(msg+len, "Keyword '%s' sent to %s\n", secure_key_array[cnt], email_array[cnt]);
    } /* end for */

    if (strchr(gu.email1, '@') == NULL) {
        strcat(gu.email1, "@nxp.com");
    }

    email_send(TRANSWEB_EMAIL, gu.email1, "Personal secure keyword for external customer", msg);
} /* end transweb_file_key_send */

void mm_file_key_send(char *file_name, char *description, char *key, char *size, char *dlt_time, char *gmt_dlt_time, char **email_array, char **secure_key_array)
{
    char local_msg[8024];
    char *msg = local_msg;
    char sr[64];
    char subject[64];
    int cnt;

    DBG();

    /* Get the SR to be used in the e-mail message */
    /* it is the second string in the description */
    sr_get(sr, description);

    /* Build message to send */
    msg += sprintf(msg, MM_FILE_MSG, sr, file_name, description, size, key, dlt_time, gmt_dlt_time, key);
    msg += strcpy1(msg, "-----------------------------------------------------------------------\n");
    msg += strcpy1(msg, "\nThe following keywords were sent to users:\n\n");

    for (cnt = 0; email_array[cnt] != NULL; ++cnt)
        msg += sprintf(msg, "Personal secure keyword '%s' sent to %s\n",

    secure_key_array[cnt], email_array[cnt]);

    /* Tag on trailing message */
    msg+= sprintf(msg, MM_CONTACT_INFO, sr, sr);

    /* Now add the list of secure keywords */
    /* MM e-mail address already present in user.email */
    /* Build subject line */
    strcpy2(subject, "NXP file keyword for SR# ", sr);

    if (strchr(gu.email1, '@') == NULL) {
        strcat(gu.email1, "@nxp.com");
    }
    email_send(TRANSWEB_EMAIL, gu.email1, subject, local_msg);
} /* end mm_file_key_send */

/* Verify external customer can access the keyword */
void transweb_keyword_verify(char *new_key, char *secure_key, char *file_key)
{
    extern char keyword_extension[];
    extern char secure_keyword_extension[];

    DBG();

    /* Verify secure and file keyword have correct suffix */
    if (secure_key[8] != secure_keyword_extension[0])
    {
	    sprintf(rtn.msg, secure_suffix_err, secure_key[8], secure_keyword_extension[0]);
	    shut_down(rtn.msg, SECURE_SUFFIX_ERR);
    } /* End if for invalid secure keyword extension */

    if (file_key[8] != keyword_extension[0])
    {
	    sprintf(rtn.msg, file_suffix_err, file_key[8], keyword_extension[0]);
	    shut_down(rtn.msg, FILE_SUFFIX_ERR);
    } /* End if for invalid file keyword extension */

    /* Look up the secure keyword */
    /* If found file keyword is returned */
    keyword_lookup(secure_key, new_key);

    /* Verify file keyword is correct */
/*
    if (strcmp(new_key, file_key) != 0)
    close_down1f(invalid_file_key, HELP_MSG, INVALID_FILE_KEY);
*/
    /* If reached this point, keyword checks successful */
} /* end external_keyword_verify */


/* Verifies request is being made between NXP & external TransWeb customer */
void external_access_verify(void)
{
    DBG();

    if (strcmp(gu.from_company, FREE_COMPANY) == 0)
    {
	    if (strcmp(gu.to_company, TRANSWEB_COMPANY) != 0)
	        shut_down(external_access_err, EXTERNAL_ACCESS_ERR);
    }
    else if (strcmp(gu.from_company, TRANSWEB_COMPANY) == 0)
    {
	    if (strcmp(gu.to_company, FREE_COMPANY) != 0)
	        shut_down(external_access_err, EXTERNAL_ACCESS_ERR);
    }
    else 
        shut_down(external_access_err, EXTERNAL_ACCESS_ERR);
} /* End external_access_check */


void cron_file_write(char *replyto, char *recipients, char *subject, char *msg, char *key)
{
    char full_file_name[32];
    FILE *fp;

    DBG();

    sprintf(full_file_name, "%s/%s", FSS_EMAIL_DIR, key);

    if ((fp = fopen(full_file_name, "w")) == NULL) return;
        fprintf(fp, "To: %s\n", recipients);

    fprintf(fp, "From:  %s\n", replyto);
    fprintf(fp, "Subject:  %s\n\n", subject);
    fprintf(fp, "%s\n", msg);

    fclose(fp);
} /* end cron_file_write */


/* Set the program */
char *program_set(void)
{
    static char msg_ptr[9];

    DBG();

    switch(customer)
    {
	    case INTERNAL_INTERCOM:
	    {
	        strcpy(msg_ptr, "InterCom");
	    }
	    break;
	    case INTERNAL_TRANSWEB:
	    case EXTERNAL_TRANSWEB:
	    case EXTERNAL_MM:
	    {
	        strcpy(msg_ptr, "transWeb");
        }
	    break;
	    case FTP_TRANS:
	    {
	        strcpy(msg_ptr, "TransFTP");
	    }
	    break;
	    default: strcpy(msg_ptr, "InterCom"); break;
    } /* end switch */

    return(msg_ptr);
} /* end function program_set */


void description_create(char *description)
{
    char *time_ptr = ctime(&tr.upd_time);
    DBG();

    strcpy2(description, "Data transfer on ", time_ptr);

    /* Lop off trailing carriage return */
    description[strlen(description)-1] = '\0';
} /* End description_create */

/*es.files.fileuse*/
/*is.files.fileuse*/
