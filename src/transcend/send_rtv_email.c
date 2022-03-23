/****************************************************************************
 *
 * Function:	send_rtv_email
 *
 * Description: Sends mail to user letting him know who pulled his file.
 *
 ***************************************************************************/

#include <stdio.h>
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_params.h"
#include "trans_server_globals.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "user_params.h"
#include "reg_globals.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


void send_rtv_email(char *user_key, char *from_email, char *to_email, char *real_name, char file_type)
{

    DBG("user_key='%s', from_email='%s', to_email='%s', real_name='%s', file_type='%s'", user_key, from_email, to_email, real_name, file_type);

    extern char program_name[];
    char subject[FILE_SIZE];
	char body[BUFFER_SIZE];
	char *msg = body;

    if (file_type == DIRECTORY)
    {
	    msg += strcpy5(msg, "User '", user.name, "' has retrieved a directory in ", program_name, " from you.\n");
	    msg += strcpy2(msg, "\n    Directory:       ", real_name);
    } /* End if */
    else if (file_type == FILES)
    {
	    msg += strcpy5(msg, "User '", user.name, "' has retrieved files in ", program_name, " from you.\n");
	    strexc(real_name, '`', ' ');
	    msg += strcpy2(msg, "\n    Files:           ", real_name);
    } /* End if */
    else
    {
	    msg += strcpy5(msg, "User '", user.name, "' has retrieved a file in ", program_name, " from you.\n");
	    msg += strcpy2(msg, "\n    File name:       ", real_name);
    } /* end else */

    msg += strcpy2(msg,   "\n    Date retrieved:  ", tr.log_date);

    if (*(msg-1) == '\n') *(msg-1) = 0;

    /* Create message to send to user */
    /* Build the subject line */
    sprintf(subject, "%s Data Retrieval", program_name);

      /* Send the email */
    email_send(from_email,  to_email, subject, body);

    /* Log message to indicate the Email was sent */
    sprintf(rtn.msg, "Sent Email notification to %s\n", to_email);
    data_log(tr.log_file, rtn.msg);
} /* End send_rtv_email */

/*ts.files.fileuse*/
