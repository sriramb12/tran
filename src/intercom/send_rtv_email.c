/****************************************************************************
 *
 * Function:	send_rtv_email
 *
 * Description: Sends mail to user letting him know who pulled his file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "customer_table.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_server_globals.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "user_params.h"
#include "reg_globals.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h"

#define MAX_LENGTH 1024

void send_rtv_email(char *to_email,  char lines[TRANS_LINE_NUM][TRANS_LINE_LEN])
{
    char subject[FILE_SIZE];
    extern char secure_keyword_extension[];
    extern char global_secure_keyword[];
    time_t end_time = time(0);
    char mask[5];
    extern TRANS_FILE_STAT fs;

    DBG();

    char *rtv_time = ctime(&end_time);
    strncpy(mask, lines[1]+13, 4);
    mask[4] = '\0';
    /* Build the subject line */
    strcpy2(subject, "Retrieval of ", lines[1]+13);
    rtn.len = strcpy2(rtn.msg, user.name,
    " has retrieved data from InterCom.\n\n    ");
        rtn.len += strcpy5(rtn.msg+rtn.len, lines[1],
    "\n    File size:   ", lines[2]+13,
    "\nDate retrieved:  ", rtv_time);
    if (strcmp(global_secure_keyword+8, secure_keyword_extension) == 0)
    rtn.len += strcpy3(rtn.msg+rtn.len, "    Secure key:  ", global_secure_keyword, "\n");
    if (strlen(lines[14]) > 14) 
    strcpy3(rtn.msg+rtn.len, "    ", lines[14], "\n\n");
    /* Call function to send the e-mail */
    email_send(gu.email1, to_email, subject, rtn.msg); 
    /* Log message to indicate the Email was sent */
    sprintf(rtn.msg, "Sent Email notification to %s\n", fs.own_id);
    data_log(tr.log_file, rtn.msg);
} /* End send_rtv_email */

void cron_rtv_email_send(char *to_email, char *from_email, char *msg)
{
    FILE *fp;
    char file_name[32];

    DBG();

    sprintf(file_name, "%s/%s.%ld", FSS_EMAIL_DIR, fs.key, tr.upd_time);
    if ((fp = fopen(file_name, "w")) == NULL) return;
    fprintf(fp, "To:  %s\n", to_email);
    fprintf(fp, "From:  %s\n", from_email);
    fprintf(fp, "Subject:  Intercom data retrieval\n\n");
    fprintf(fp, msg);
    fclose(fp);
}
void send_disk_email(char *msg_body, int percent)
{
    short len;
    char msg[BUFFER_SIZE];

    DBG();

    /* Build message to send */
    /* First line has from and to email addresses */
    /* Second line has subject line */
    len = strcpy5(msg, admin.err_email, " ", admin.err_email, "\nDisk usage above 90%\n\n", msg_body);
    /* Send server indication that email will be sent */
    put_short(DBEMAIL);
    put_binary_string(msg, len);
    /* Indicate email was sent */
    rtn.len += strcpy2(rtn.msg+rtn.len, "\nSentemail notification to ", admin.err_email);
} /* End send_disk_email */
/*es.files.fileuse*/
/*is.files.fileuse*/
