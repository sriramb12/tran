#ifndef TRANS_FTP
/****************************************************************************
 *
 * Function:	send_secure_keyword
 *
 * Description:	Builds an encrypted keyword and associates it with
 *		        the user's Email and the original keyword thus giving
 *		        the user implied authorization for accessing the
 *		        original file using the encrypted keyword.
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>

#include "customer_table.h"
#include "edc_defines.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "lock_file.h"
#include "mm_email_msg.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_msg.h"
#include "trans_server_paths.h"
#include "user_params.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "ldapfsl.h"
#include "global_debug.h"


void secure_email_send(char *file_name, char file_type, char *description, char *file_size_string, char *user_key, char *local_time, char *gmt_time, char *email_addr, char *msg_ptr)
{
    extern int customer_type;
    extern char keyword_link_extension[];
    char file_type_string[16];
    char subject[FILE_SIZE];
    extern char program_name[];
	char *msg = bf.msg;

    DBG();

    /*msg += strcpy3(msg, "**IMPORTANT NOTE**\nThe personal secure keyword listed below can be used only by " , email_addr, "\n\n");*/

	if (file_type == DIRECTORY)
	{
        strcpy(file_type_string, "directory ");
		msg += strcpy5(msg, "User '", user.name, "' has placed a directory in ", program_name, " for you.\n");
		msg += strcpy2(msg,	"\nDirectory: ", file_name);
    }
	else if (file_type == FILES)
    {
        strcpy(file_type_string, "files ");
		msg += strcpy5(msg, "User '", user.name, "' has placed files in ", program_name, " for you.\n");
		msg += strcpy2(msg,	"\nFiles: ", file_name);
	} else {
        strcpy(file_type_string, "file ");
		msg += strcpy5(msg, "User '", user.name, "' has placed a file in ", program_name, " for you.\n");
		msg += strcpy2(msg,	"\nFile name: ", file_name);
	}

	if (description[0] != '\0')
	    msg += strcpy2(msg, "\nDescription: ", description);

	msg += strcpy2(msg, "\nFile size: ", file_size_string);
	msg += strcpy5(msg, "\n\nPersonal secure keyword:  ", user_key, "  (for use only by ", email_addr, ")");
	msg += strcpy4(msg, "\n\nTransWeb URL: https://nww.transweb.nxp.com/index", keyword_link_extension, "?go=KEYWORD&KEYWORD=", user_key);
	msg += strcpy5(msg, "\nExpires: ", local_time, "  [ ", gmt_time, " ]");

	if (customer_type == INTERNAL_CUSTOMER)
	    sprintf(msg, FREE_REL_MSG, email_addr, user_key);

	rtn.len += strcpy5(rtn.msg+rtn.len, "Sending personal secure keyword '", user_key, "' to '", email_addr, "'.\n\n");
	strcpy2(subject,  "Personal secure keyword for use only by ", email_addr);

    email_send(gu.email1, email_addr, subject, bf.msg);


    DBG("end secure_email_send()");
} /* end secure_email_send */


/* Build MM message to send */
void internal_mm_email_send(char *file_name, char file_type, char *description, char *file_size_string, char *user_key, char *local_time, char *gmt_time, char *email_addr, char *msg_ptr)
{
    char sr[64];
    char subject[256];
    char local_msg[8024];
    char *msg = local_msg;
    char *ptr;

    DBG();

    /* Get the SR to be used in the e-mail message */
    /* it is the second string in the description */
    if (sscanf(description, "%*s %s", sr) == 0)
        strcpy(sr, "Unknown");
    /* Take trailing comma off of SR number */
    else if ((ptr = strchr(sr, ',')) != NULL)
        ptr[0] = '\0';

    /* Build the message to send */
    msg += sprintf(msg, MM_INTERNAL_MSG, sr, file_name, description, file_size_string, user_key, local_time, gmt_time, user_key);

    /* for MM customer, tack on end of message */
    sprintf(msg, MM_CONTACT_INFO, sr, sr);

    /* MM e-mail address already present in user.email */
    /* Build subject line */
    strcpy4(subject, "Personal secure keyword for SR# ", sr, "for use only by ", email_addr);
    email_send(FROM_SUPPORT_EMAIL,  email_addr, subject, local_msg);

    /* Send copy of secure keyword to NXP Support */
    /* build new subject line */
    strcpy4(subject, "Personal secure keyword sent to ", email_addr, " for SR# ", sr);
    email_send(TRANSWEB_EMAIL, SUPPORT_EMAIL, subject, local_msg);
    rtn.len += strcpy5(rtn.msg+rtn.len, "Sending personal secure keyword '", user_key, "' to '", email_addr, "'.\n\n");

    DBG("End internal_mm_email_send()");
} /* End internal_mm_email_send */


/* dlt_time parameter not used, but present for compatibility with InterCom */
void send_secure_keyword(
    char *file_keyword,	/* File keyword to be encrypted */
    char *email_addr[],	/* Email address used as encryption key */
    char *file_name,	/* Name of file in Transcend */
    char *file_size_string,	/* Original size of file */
    char file_type,
    long dlt_time,
    char *local_time,
    char *gmt_time,
    char *description[])
{
    extern char global_secure_keyword[];
    extern METADATA md;
    char *ptr;
    int len;
    char friendly_email[64] = "";
    char user_name[32] = "";
    char new_email[64];
    extern char secure_keyword_extension[];
    char *index();
    int lock_index;
    int ret;
    char line[256];
    register int cnt;
    int entry_cnt;

    extern char cert7db[];

     DBG("gu.email1='%s' AND gu.par4='%s'", gu.email1, gu.par4);

     DBG("here, user.login is '%s'", user.login);

    LDAP_RETURN_DATA * ret0 = ldapfsl_lookupByCoreId(user.login);
    if (GOOD_LDAP_RETURN_DATA(ret0))
    {
          strcpy(gu.email1, ret0->info[0]->friendly_email);
          ldapfsl_copy_info_to_global_user_structure(ret0);         
    }
    ldapfsl_free_return_data(ret0);

	DBG("after LDAP query, gu.email1='%s'", gu.email1);

    /* First see if alternate sender is specified */
    /* first check the global param for it */
    if (strcmp(gu.par4, "0") == 0)
    {
        for (entry_cnt = 0; email_addr[entry_cnt] != NULL; ++entry_cnt)
        {
	        if (strcmp(email_addr[entry_cnt], "-f") == 0)
	        {
	            /* do not use the -f option as an e-mail address */
	            email_addr[cnt] = NULL;

	            /* If alternate e-mail address not specified after the -f option, nothing to use */
	            if (email_addr[cnt+1] == NULL) break;

	            /* Now copy in the alternate e-mail address */
	            strcpy(gu.par4, email_addr[cnt+1]);
	            break;
	        } /* end if */

            /* If e-mail address too short, indicate it will not be used */
	        else if (strlen(email_addr[entry_cnt]) <= 2)
	            strcpy(email_addr[entry_cnt], NONE);
        } /* end for */
    } /* end if */

    /* Now check again for par4 */
    if (strcmp(gu.par4, "0") != 0)
    {
	    strcpy(gu.email1, gu.par4);
	    DBG("gu.email1='%s' AND gu.par4='%s'", gu.email1, gu.par4);

        /* Otherwise add to the e-mail address */
        LDAP_RETURN_DATA * ret =  ldapfsl_lookupByCoreId(gu.email1);
        if (BAD_LDAP_RETURN_DATA(ret))
        {
            ldapfsl_free_return_data(ret);
            ret = ldapfsl_lookupByMail(gu.email1);
        }
        if (ret != 0 && ret->result == LDAP_SUCCESS)
        {
            strcpy(gu.email1, ret->info[0]->friendly_email);
        }
        ldapfsl_free_return_data(ret);

        if ((strcmp(user_name, "") != 0) && (strncasecmp(user_name, "Unknown", 7) != 0))
            strcpy(user.name, user_name);
        else
            strcpy(user.name, gu.par4);
    } /* end if for alternate e-mail found */

    /* Process each Email address separately */
    for (cnt = 0; cnt < entry_cnt; ++cnt)
    {
	    /* If invalid e-mail present skip it */
	    if (strcmp(email_addr[cnt], NONE) == 0)
	        continue;

	    /* Put email into correct format if needed */
        len = strcpy1(new_email, email_addr[cnt]);

        /* If just the user ID given, add the NXP part */
        if (strchr(new_email, '@') == NULL)
        {
            /* We run a LDAP query to load their respective email if coreid present */
            LDAP_RETURN_DATA * ret2 = NULL;
            ret2 = ldapfsl_lookupByCoreId(new_email);

            if (ret2->result == LDAP_SUCCESS)
            {
                DBG("will switch '%s' to the respective friendly email '%s'", new_email, ret2->info[0]->friendly_email);
                strcpy(new_email, ret2->info[0]->friendly_email);
            }
            else if (ret2->result != LDAP_SUCCESS)
            {
                DBG("Will add EMAIL_SUFFIX due to LDAP error: %d %s\n", ret2->result, ret2->errorMessage);
                strcpy(new_email+len, EMAIL_SUFFIX);
            }
            ldapfsl_free_return_data(ret2);
        } else if (customer_type == EXTERNAL_CUSTOMER) {
	        /* Verify that email address ends in '@nxp.com' */
	        if ((strlen(new_email) < 9) || (strcasecmp(new_email+strlen(new_email)-8, "@nxp.com") != 0))
	        {
	            rtn.len += sprintf(rtn.msg+rtn.len, "This site is for data transfer within NXP. Personal secure keyword not sent to '%s'.\n\n", new_email);
	            continue;
	        }
	    } /* end if for external customer */
	    else if (customer_type == TRANSCEND_CUSTOMER)
	    {
	        /* If this is Transcend & is an external customer, do not send it */
	        if ((strlen(new_email) < 9) || (strcasecmp(new_email+strlen(new_email)-8, "@nxp.com") != 0))
	        {
	            rtn.len += sprintf(rtn.msg+rtn.len, "This site is for data transfer within NXP. Personal secure keyword not sent to '%s'.\n\n", new_email);
	            continue;
	        }
	    } /* End if for Transcend */

        if (md.description[0] == '\0')
        {
            strcpy(md.description, "put by transcend");
        }

        ret = create_secure_keyword(file_keyword, email_addr[cnt], description, cnt+1, global_secure_keyword);


#if 0 // no longer saves the secure keyword in KEYWORD_FILE 'db_admin/keyword_file'
        /* Put an entry in the keyword file for the intermediate keyword */
	    DBG("before call lock_file_set()");
        lock_index = lock_file_set(KEYWORD_FILE);
	    DBG("After call lock_file_set()");

	    DBG("before call add_line()");
	    ret = add_line(KEYWORD_FILE, line);
	    DBG("after call add_line()");

	    DBG("before call lock_file_clear()");
        lock_file_clear(lock_index);
	    DBG("After call lock_file_clear()");
#endif
	    if (ret != SUCCESS)
	    {
            DBG("ret is != of SUCCESS");
	        rtn.len += strcpy4(rtn.msg+rtn.len, RET_TAB, "Encrypted keyword not sent to '", email_addr[cnt], "'\n\n");
            DBG("rtn.msg='%s'", rtn.msg);
	        continue;
	    }

	    /* Send the Email message */
	    /* Build message to send */
	    DBG("Before check customer_type");
	    if ((customer_type == INTERNAL_CUSTOMER) && (strcmp(user.email, SUPPORT_EMAIL) == 0)) {
            internal_mm_email_send(file_name, file_type, description, file_size_string, global_secure_keyword, local_time, gmt_time, new_email, NULL);
        } else {
            secure_email_send(file_name, file_type, description, file_size_string, global_secure_keyword, local_time, gmt_time, new_email, NULL);
        }
	    DBG("After check customer_type");
    } /* End for */

    DBG("End send_secure_keyword()");
} /* End send_secure_keyword */
#endif /* for not  TRANS_FTP */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
