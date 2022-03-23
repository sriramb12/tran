/****************************************************************************
 *
 * Function:	send_transweb_keyword
 *
 * Description:	Builds a secure keyword and associates it with
 *		the user's Email and the original keyword thus giving
 *		the user implied authorization for accessing the
 *		original file using the secure keyword.
 *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "close_down.h"
#include "customer_table.h"
#include "dir.h"
#include "mm_email_msg.h"
#include "files.h"
#include "edc_defines.h"
#include "edc_prototypes.h"
#include "external_intercom_prototypes.h"
#include "ftp_info.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_msg.h"
#include "trans_server_paths.h"
#include "reg_globals.h"
#include "registry_prototypes.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "ldapfsl.h"
#include "global_debug.h"


/* These variables are used only in this file */
/* Return the SR by scanning it out of the description */
void sr_get(char *sr, char *description)
{
    char *ptr;

    DBG("sr='%s', description='%s'", sr, description);

    /* SR is the second string in the description */
    if (sscanf(description, "%*s %s", sr) < 1)
        strcpy(sr, "Unknown");

    /* Take trailing comma off of SR number */
    else if ((ptr = strchr(sr, ',')) != NULL)
        ptr[0] = '\0';
} /* end sr_get */


/* Build message to send, then send the Email message */
void secure_external_email_send(char *file_name, char file_type, char *description, char *size_string, char *secure_key, char *file_key, char *local_time, char *gmt_time, char *email_addr, char *msg_ptr)
{
    int len;
    char msg_body[BUFFER_SIZE];
    char *msg;
    char subject[256];
    char sr[20];

    DBG();

    msg = secure_msg_body_build(msg_body,  email_addr, secure_key, file_key, file_name, size_string, file_type, local_time, gmt_time, description);

    /* Now output the message about where to retrieve the keyword */
    msg += sprintf(msg, msg_ptr, email_addr, secure_key);
    strcpy4(subject, "Secure ", program_set(), " keyword for use only by ", email_addr);

    if (customer == EXTERNAL_MM)
    {
	    /* first get the SR # */
	    sr_get(sr, description);
	    strcpy2(subject+len, " For SR# ", sr);
    } /* end if for external MM */

	/* If the second e-mail not set, use the primary e-mail */
    /* also if this is an external customer, just use primary e-mail */
    if ((customer == EXTERNAL_TRANSWEB) || (strcmp(gu.email2, NONE) == 0) || (strncasecmp(gu.email2, "unknown", 7) == 0))
        email_send(gu.email1,  email_addr, subject, msg_body);
    else
        email_send(gu.email2,  email_addr, subject, msg_body);

    rtn.len += strcpy5(rtn.msg+rtn.len, "Sending personal secure keyword '", secure_key, "' to '", email_addr, "'.\n\n");
} /* End secure_external_email_send */


/* Build MM message to send */
void external_mm_email_send(char *file_name, char file_type, char *description, char *file_size_string, char *secure_key, char *file_keyword, char *local_time, char *gmt_time, char *email_addr, char *msg_ptr)
{
    char sr[64];
    char subject[64];
    char local_msg[8024];
    char *msg = local_msg;

    DBG("file_name='%s', file_type='%s', description='%s', file_size_string='%s', secure_key='%s', file_keyword='%s', local_time='%s', gmt_time='%s', email_addr='%s', msg_ptr='%s'",
            file_name, file_type, description, file_size_string, secure_key, file_keyword, local_time, gmt_time, email_addr, msg_ptr);

    /* Get the SR to be used in the e-mail message */
    sr_get(sr, description);

    /* Now build the message */
    msg += sprintf(msg, MM_EXTERNAL_MSG, sr, file_name, description, file_size_string, secure_key, file_keyword, local_time, gmt_time, secure_key);


/* Feb 2018, Carlos Mazieri, perhaps this code is not necessary

    // If sending to external, verify external registered in LDAP
    if ((customer == INTERNAL_MM) || (customer == EXTERNAL_MM)
    {

         if (ldapfsl_exists_account_by_email(email_addr) != DBYES)
            msg += sprintf(msg, MM_REG_MSG, email_addr);

    }
*/

    /* Tag on contact info */
    sprintf(msg, MM_CONTACT_INFO, sr, sr);

    /* MM e-mail address already present in user.email */
    /* Build subject line */
    strcpy2(subject, "NXP TransWeb keyword for SR# ", sr);
    email_send(FROM_SUPPORT_EMAIL,  email_addr, subject, local_msg);

    /* Send copy of secure keyword to support@nxp.com */
    strcpy4(subject, "TransWeb keyword sent to ", email_addr, " for SR# ", sr);
    rtn.len += strcpy5(rtn.msg+rtn.len, "Sending secure keyword '", secure_key, "' to '", email_addr, "'.\n\n");

    /* Now send the message */
    email_send(TRANSWEB_EMAIL, SUPPORT_EMAIL,  subject, local_msg);
} /* End external_mm_email_send */


char * customer_msg_set(char *email_addr, int reg_status)
{
    DBG("email_addr='%s', reg_status='%d'", email_addr, reg_status);

    int len = strlen(email_addr);
    /* If sending to internal user point to message for internal user */
    /* Else point to message for external customer */
    /* If the e-mail is for a NXP customer or the sender is external  */
    /* TransWeb, send the to NXP message */

    if ( (len > 8) && (strcasecmp(email_addr+len-8, "@nxp.com") == 0) || (customer_type == EXTERNAL_CUSTOMER) )
        return(TO_NXP_REL_MSG);
    else if (customer_type == INTERNAL_CUSTOMER)
    {
        /* If the customer is approved send the approved message */
	    if (reg_status == APPROVED)
	        return(TO_APPROVED_EXTERNAL_REL_MSG);
        else
            return(TO_UNAPPROVED_EXTERNAL_REL_MSG);
    } /* end if */

    else if (customer == EXTERNAL_INTERCOM_ONLY)
        return(INTERCOM_ONLY_MSG);
    else
        data_log(tr.log_file, "No e-mail message selected");
} /* End customer_msg_set */


void send_external_keyword(
    char *file_keyword,	/* File keyword to be encrypted */
    char *email_addr,	/* Email address used as encryption key */
    char *file_name,	/* Name of file in Transcend */
    char *file_size_string,	/* Original size of file */
    char file_type,
    long dlt_time,
    char *local_time,
    char *gmt_time,
    char *description[])
{
    int ret;
    char secure_key[32];
	char *msg;
    int len = 555;


    DBG();
    ret = create_secure_keyword(file_keyword, email_addr, description, len, secure_key);

	if (ret != SUCCESS)
	{
	    rtn.len += strcpy4(rtn.msg+rtn.len, RET_TAB, "Secure keyword not sent to '", email_addr, "'\n\n");
	}
	else
	{
	    rtn.len += strcpy5(rtn.msg+rtn.len, "Sending personal secure keyword '", secure_key, "' to '", email_addr, "'.\n\n");
	} /* End if for successful insert */

    /**
     *   Aug 1st 2018 Carlos Mazieri comment this piece ofcode
     *
     * It looks lke wrong
     *  // If this is not compuGraphics, send the e-mail message
         if (is_compugraphics(email_addr) == DBYES) return;
    */

	/* Send the Email message, build message to send */
    msg = bf.msg;
    if (customer == EXTERNAL_TRANSWEB)
		msg += strcpy5(msg, "The ", program_set(), " system has generated a secure keyword for you.\n",
            "You can use this keyword in conjunction with the file keyword to view \n",
            "information about the data and modify file attributes.\n");
	else
	    msg += strcpy5(msg, "The ", program_set(), " system has generated a secure keyword for you.\n",
            "You can use this keyword to retrieve the data ,  view \n",
            "information about the data, and modify file attributes.\n");

		msg += strcpy2(msg,	"\n    File name:     ", file_name);

	    if (description[0] != '\0')
		    msg += strcpy2(msg, "\n    Description:   ", description);

	  msg += strcpy2(msg,   "\n    File size:     ", file_size_string);
	  msg += strcpy5(msg,   "\n    Personal secure keyword:  ", secure_key, "  (for use only by ", email_addr, ")");
	  msg += strcpy4(msg,   "\n    Expires:       ", local_time,
					        "\n                     ", gmt_time);

	  email_send(TRANSWEB_EMAIL,  email_addr, "Personal secure keyword", bf.msg);
} /* End send_external_keyword */


/* This function is called when the -m option is used */
void transweb_keyword_send(void)
{
    char email[BUFFER_SIZE];
    char *email_addr[1000];
    char *secure_key_array[1000];
    FILE *fp;
    extern TRANS_FILE_STAT fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char description[160];

    DBG();

    /* Read in information about the file */
    fp = read_big_file(gu.key1, lines, &fs);

    /* Let client know successful up to this point */
    put_short(SUCCESS);

    /* Get the description */
    description[0] = '\0';
    get_string(description);
    get_string(email);

    /* Convert commas to spaces */
    strexc(email, ',', ' ');

    /* Convert semicolons to spaces */
    strexc(email, ';', ' ');
    string2list_convert(email, email_addr);
    send_transweb_keyword(gu.key1, email_addr,  fs.real_name, lines[2]+13, fs.file_type, fs.dlt_time, lines[8]+13, lines[9]+13, lines[13]+13, description);

    /* Send the sender the file keyword and info about secure keywords */
	transweb_file_key_send(fs.real_name, description, gu.key1, lines[2]+13, lines[8]+13, lines[9]+13, email_addr, secure_key_array);

    /* Tell the user the send was successful */
    /* Return message already initialized */
    /* Send the return message */
    put_success_msg(gu.key1);

    /* If description not already present in file add it */
    if (lines[TRANS_LINE_NUM-1][0] == '\0')
    {
	    strcpy2(lines[TRANS_LINE_NUM-1], "Description: ", description);
        DBG("Before call write_big_file()");
	    write_big_file(fp, lines, &fs);
        DBG("after call write_big_file()");
    } /* End if for description not set */
} /* End transweb_keyword_send */

/* build message body for sending secure keywords */
char * secure_msg_body_build(char *msg_body,  char *email_addr, char *secure_key, char *file_key, char *file_name, char *size_string, char file_type, char *local_time, char *gmt_time, char *description)
{
    char *program_type = program_set();
    char *msg = msg_body;

    DBG();

    msg += strcpy3(msg, "IMPORTANT NOTE:  The personal secure keyword listed below can be used only by " , email_addr, ".\n\n");

    if (file_type == DIRECTORY)
    {
	    msg += strcpy4(msg, user.name, " has placed a directory in ", program_type, " for you.\n");
	    msg += strcpy2(msg,	"\n    Directory:     ", file_name);
    } /* End if */
    else if (file_type == FILES)
    {
	    msg += strcpy4(msg, user.name, " has placed files in ", program_type, " for you.\n");
	    msg += strcpy2(msg,	"\n    Files:         ", file_name);
    } else {
	    msg += strcpy4(msg, user.name, " has placed a file in ", program_type, " for you.\n");
	    msg += strcpy2(msg,	"\n    File name:     ", file_name);
    } /* end else */
    if (description[0] != '\0')
	    msg += strcpy2(msg, "\n    Description:   ", description);

    msg += strcpy2(msg,   "\n    File size:     ", size_string);
    msg += strcpy5(msg,   "\n    Personal secure keyword:  ", secure_key, "  (for use only by ", email_addr, ")");

    if (customer != EXTERNAL_INTERCOM_ONLY)
        msg += strcpy3(msg,   "\n    File keyword:  ", file_key, "");

    /* If this is data going to a remote company, specify the site the data is going to */
    if (strcmp(gu.from_company, "NXP Semiconductor") == 0)
        msg += sprintf(msg, "\n    Company site:  %s", gu.to_company);

    msg += strcpy2(msg,   "\n    Expires:       ", local_time);

    if (strcmp(gmt_time, "Pending authorization") != 0)
	    msg += strcpy2(msg, "\n                     ", gmt_time);

    return(msg);
}

/* parameters local_time and gmt_time not used, but present for Transcend compatibility */
/* This function sends secure keywords to external customers */
/* It is called by put_big_file */
void send_transweb_keyword(
    char *file_keyword,	/* File keyword to be encrypted */
    char *email_addr[],	/* Email address used as encryption key */
    char *file_name,	/* Name of file in repository */
    char *file_size_string,	/* Original size of file */
    char file_type,
    long dlt_time,
    char *local_time,
    char *gmt_time,
    char *to_company,
    char *description)
{
    extern char *secure_keyword_array[];
    extern char global_secure_keyword[];
    extern int customer_type;
    int status;
    int ret;
    char mail_buffer[8024];
    char name[32];
    char *not_approved[10];
    extern int not_approved_cnt;
    char *ptr;
    char *msg_ptr;
    int len;
    char line[512];
    register int cnt;
    int trusted_sender;

    DBG();

    if ((customer_type == INTERNAL_CUSTOMER) && (customer != FTP_TRANS))
        trusted_sender = is_trusted_sender(user.email);
    else
        trusted_sender = DBYES;

    /* Indicate no unapproved customers found */
    not_approved_cnt = 0;

    /* Indicate nothing ready to send yet */
    mail_buffer[0] = '\0';
    ptr = mail_buffer;

    /* If from e-mail message is support@nxp.com, e-mail going to MM customer */
    /* Process each Email address separately */
    for (cnt = 0; email_addr[cnt] != NULL; ++cnt)
    {
	    /* If reached last entry, fall out of the loop */
	    /* For TransWeb will point at NULL pointer */
	    if (strcmp(email_addr[cnt], NONE) == 0)
	        continue;

	    /* Indicate a secure keyword not yet generated */
	    secure_keyword_array[cnt] = NULL;

	    if (strlen(email_addr[cnt]) <= 1) continue;

	    /* Put the Email address in lower case */
	    str_lower(email_addr[cnt]);

	    /* If the '@' construct not present, add it */
	    if (strchr(email_addr[cnt], '@') == NULL)
	    {
            LDAP_RETURN_DATA * ldap_ret =  ldapfsl_lookupByCoreId(email_addr[cnt]);
            if (GOOD_LDAP_RETURN_DATA(ldap_ret))
            {
                 email_addr[cnt] = realloc(email_addr[cnt], strlen(ldap_ret->info[0]->friendly_email) + 1);
                 strcpy(email_addr[cnt], ldap_ret->info[0]->friendly_email);
            }
            ldapfsl_free_return_data(ldap_ret);

	    } /* End if */

	    /* If enclosed in quotes or brackets, strip them */
	    str_quotes_strip(email_addr[cnt]);

	    /* If this is an external TransWeb customer, verify NXP address entered */
	    if (customer_type == EXTERNAL_CUSTOMER)
	    {
	        if (strcasecmp(email_addr[cnt]+strlen(email_addr[cnt])-8, "@nxp.com") != 0)
	        {
	            rtn.len += strcpy3(rtn.msg+rtn.len,	"Personal secure keyword not sent for invalid e-mail address of '", email_addr[cnt],
	                    "'.\nE-mail addresses must end in the suffix '@nxp.com'.\n\n");
	            continue;
	        } /* end if for invalid Email for external customer */
	    } /* end if for external customer */
	    /* Atthis point know this is an internal customer */
	    /* Do not need to make check for TransFTP */
	    else if ((customer != FTP_TRANS) && (trusted_sender == DBNO))
	    {
	        /* Verify external customer is authorized to retrieve data */
	        status = sql_approve_check(email_addr[cnt]);
            DBG("AFTER sql_approve_check(%s) status=%d",email_addr[cnt], status);
	        if (status != APPROVED)
	        {
	            /* Add pending request to SQL table */
	            if (status != PENDING)
	            {
	                data_log2(tr.log_file, "Adding pending request for ", email_addr[cnt]);
                    ret = sql_pending_request_insert(email_addr[cnt], user.login, description);
	            } else {
	                /* Otherwise indicate insert successful */
	                ret = SUCCESS;
	                data_log2(tr.log_file, "Not adding pending request for ", email_addr[cnt]);
	            } /* end else */

	            if (ret == SUCCESS)
	            {
	                not_approved[not_approved_cnt] = malloc(strlen(email_addr[cnt]) + 1);
	                strcpy(not_approved[not_approved_cnt++], email_addr[cnt]);
	                rtn.len += strcpy3(rtn.msg+rtn.len, "Approval request for '", email_addr[cnt], "' sent to your supervisor.\n");
	            } /* end if */
	            else
	                rtn.len += strcpy2(rtn.msg+rtn.len, email_addr[cnt], " is not approved to retrieve data.\n");
	        }
	    } /* end if for not TransFTP */

        ret = create_secure_keyword(file_keyword, email_addr[cnt], description, cnt+55, global_secure_keyword);

	    if (ret != SUCCESS)
	    {
	        rtn.len += strcpy3(rtn.msg+rtn.len, "Personal secure keyword not sent to '", email_addr[cnt], "'\n\n");
	        continue;
	    }

	    /* Add secure keyword to list being created for calling program */
	    secure_keyword_array[cnt] = malloc(10);
	    strcpy(secure_keyword_array[cnt], global_secure_keyword);

	    /* Point to appropriate message based on customer type */
	    msg_ptr = customer_msg_set(email_addr[cnt], status);

	    /* Build string to send to user */
        if (customer == INTERNAL_MM)
    	    external_mm_email_send(file_name, file_type, description, file_size_string, global_secure_keyword, file_keyword, local_time, gmt_time, email_addr[cnt], msg_ptr);
    	else
    	    secure_external_email_send(file_name, file_type, description, file_size_string, global_secure_keyword, file_keyword, local_time, gmt_time, email_addr[cnt], msg_ptr);
    } /* End for */

    /* If unapproved customers found, send sender e-mail */
    if (not_approved_cnt > 0)
    {
        len = strcpy1(mail_buffer,  PENDING_APPROVAL_MSG);

        for (cnt = 0; cnt < not_approved_cnt; ++cnt)
        {
	        len += strcpy2(mail_buffer+len, "	", not_approved[cnt]);
            LDAP_RETURN_DATA * ldap_ret =  ldapfsl_lookupByMail(not_approved[cnt]);
            if (GOOD_LDAP_RETURN_DATA(ldap_ret))
            {
                len += strcpy3(mail_buffer+len, " (", ldap_ret->info[0]->name, ")\n");
            }
            else
            {
                 len += strcpy1(mail_buffer+len, "\n");
            }
            ldapfsl_free_return_data(ldap_ret);
        } /* end for */

        strcpy(mail_buffer+len, "Please feel free to forward a copy of this e-mail message to your manager.\n\n");
	    email_send(TRANSWEB_EMAIL, gu.email1, "Pending approvals", mail_buffer);
    } /* end if for approval requests present */
} /* End send_transweb_keyword */
/*es.files.fileuse*/
/*is.files.fileuse*/
