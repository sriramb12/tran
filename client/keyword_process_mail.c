#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	keyword_process_mail
 *
 * Description:	Maps encrypted keyword to real keyword
 *		name and  machine name as keywords and the  time 
 *		stamp as the seed for the encryption.
 *		Builds the doubly encrypted keyword using the
 *		receiving user's Email address as the encryption key.
 *		Sends the encrypted keyword to the receiving user.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "global_defines.h"
#include "server_params.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"


char *keyword_process_mail(char *mail_ptr, char *description, char file_type, char *mail_buffer)
{
    char *asctime();
     char user_key[10];
    struct tm *gmt_time;	/* Holds GM time when file will be deleted */
     char *time_ptr;
    char mst_time_string[32];
    char gmt_time_string[32];
    char email_addr[64];
    char file_name[80];
    char size_string[8];
     char size[16];
     long dlt_time;
    char *user_msg = mail_buffer;
    char msg[BUFFER_SIZE];
    char *mail_msg = msg;
    char line[260];
    char subject[160];
    int len;
    len = strcpy1(subject, "Intercom Keyword");
     while ((len = str_line_get(mail_ptr, line)) > 0)
    {
	if (sscanf(line, "%s %s %s %s %s %ld",
	user_key, email_addr, file_name, size, size_string, &dlt_time) != 6)
	{
	    user_msg += strcpy1(user_msg, "Unable to parse output from external software.\n");
	    mail_ptr += len+1;
	    continue;
	}
	/* build message to send */
	else
	{
	    strexc(file_name, '`', ' ');
		/* Build message to send */
	    mail_msg = msg;
	    if (file_type == DIRECTORY) 
	    {
		mail_msg += strcpy3(mail_msg, "User '", user.name,
				     "' has placed a directory in InterCom for you:\n");
		mail_msg += strcpy2(mail_msg,	"\n    Directory:       ", file_name);
	    }
	    else if (file_type == FILES) 
	    {
		mail_msg += strcpy3(mail_msg, "User '", user.name,
				     "' has placed files in InterCom for you:\n");
		mail_msg += strcpy2(mail_msg,	"\n    Files:           ", file_name);
	    } else {
		mail_msg += strcpy3(mail_msg, "User '", user.name,
				     "' has placed data in InterCom for you:\n");
    if (strchr(file_name, ' ') == NULL)
			mail_msg += strcpy2(mail_msg,	"\n    File name:       ", file_name);
			else mail_msg += strcpy2(mail_msg,	"\n    Files:           ", file_name);
	    } /* End else */
	    if (description[0] != '\0')
		mail_msg += strcpy2(mail_msg, "\n    Description:     ", description);
	    
	    mail_msg += strcpy4(mail_msg,   "\n    File size:       ", size, " ", size_string);
	    mail_msg += strcpy3(mail_msg,   "\n    Secure keyword:  ", user_key, "\n");
#ifdef MULTIPLE_INTERNAL_SERVERS
	    mail_msg += strcpy3(mail_msg,   "    Company site:    ", server.site, "\n");
#endif
    /* Figure out the current GM time */
    gmt_time = gmtime(&dlt_time);
    /* Convert to string representation */
    time_ptr = asctime(gmt_time);
    strcpy(gmt_time_string, time_ptr);
    strcpy(gmt_time_string+24, " GMT\n");
    /* Calculate mountain standard time */
    time_ptr = ctime(&dlt_time);
    strcpy(mst_time_string, time_ptr);
    strcpy(mst_time_string+24, " mst\n");
	    mail_msg += strcpy4(mail_msg,   "    Expires:         ", mst_time_string,
					        "                     ", gmt_time_string);
	    user_msg+= strcpy5(user_msg,"Sending secure keyword '", 
	    user_key, "' to '", email_addr, "'\n\n");
        email_send("transbug@nxp.com",  email_addr, subject, msg);
	} /* End else */
	mail_ptr += len+1;
    } /* End while */
    return(mail_msg);
} /* End keyword_process_mail*/
#endif /* For not TRANSAPI */
