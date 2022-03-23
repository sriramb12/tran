/****************************************************************************
 *
 * Function:	email_send
 *
 * Description:	Sends email to specified user.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include "dir.h"
#include "server_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities

int remote_email_send(void)
{
    DBG();

    char from_email[80];
    char to_email[80];
    char subject[80];
    char body[BUFFER_SIZE];
    FILE *fp;
    char file_name[64];
    char *file_ptr = file_name + strcpy1(file_name, "email_dir.send/");
    int cnt;
    int cnt1;
    char *files[1024];

    /* Make a list of all files present in the directory */
    dir_list("email_dir.send", files);

    /* Send email for each file */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    /* Build file name */
	    strcpy(file_ptr, files[cnt]);
	
	    /* Open the file */
	    if ((fp = fopen(file_name, "r")) == NULL) continue;
	
	    /* Scan in the from email address */
	    line_get(fp, from_email);

	    /* Scan in the to email address */
	    line_get(fp, to_email);

	    /* Scan in the subject line */
	    line_get(fp, subject);

	    /* Scan in the body */
	    cnt1 = 0;
	    while ((body[cnt1] = getc(fp)) != EOF) ++cnt1;
	    
	    /* Put null at end */
	    body[cnt1] = '\0';
	    
	    /* Send the email */
	    email_send(from_email, to_email, subject, body);
	    
	    /* Delete the file */
	    unlink(file_name);
	    
	    /* Free the memory */
	    free(files[cnt]);
    } /* end for for sending email */

    return(cnt);
} /* End remote_email_send */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
