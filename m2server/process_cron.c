/****************************************************************************
 *
 * Function:	process_cron
 *
 * Description:	Processes requests from Mask Designer client
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "mask_server_defines.h"
#include "m2audit_prototypes.h"
#include "m2server_globals.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "mti.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2msg.h"
#include "m2server_errs.h"
#include "m2req_types.h"
#include "m2server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"

void process_cron(void)
{
    extern char *group_email[];
    extern char *admin_email[];

    switch(mask_msg.req_type)
    {
    case RESEND_DATA:
    {
        /* First remove any traqnsient lock files */
        system("rm registry/*.lock");
        resend_data();
    }
    break;
    /* This will be a shadow of one or more files from the remote server */
    case SHADOW:
    {
        /* If this is waccdb, deny request */
        if (strcmp(ip.master_site, "wacc") == 0)
        close_down1m("not wacc", "test", -1);
        files_shadow_rcv(mask_msg.key1);
        log3("Closing transaction ", tr.pid_string, ".");
    } /* End serial refresh */
    break;
    case SEND_GROUP: audit_mail_send(group_email); break;
    case SEND_ADMIN: audit_mail_send(admin_email); break;
    case AUDIT_DIR: audit_dir(mask_msg.key1, 0, rmt_mpd.server_num); break;
    case AUDIT_FILE:    audit_file(mask_msg.key1, 0, rmt_mpd.server_num); break;
    case RMT_AUDIT_DIR: rmt_audit_dir(mask_msg.key1); break;
    case RMT_AUDIT_FILE: rmt_audit_file(mask_msg.key1); break;
    default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_cron */

void mars_send(void)
{
    char command[FILE_SIZE];
    char date[16]= "00_00_00";
    char old_file[64];
    char new_file[64];
    long time_stamp = tr.upd_time-SECPERDAY;
    char *time_stamp_string = ctime(&time_stamp);

    /* Calculate current date */
    log_convert(time_stamp_string, date);
    /* Copy log file from old directory to new directory */
    /* build name of old file */
    strcpy2(old_file, "contract_tables/mask_log/", date);
    /* If log file not present, simply exit */
    if (access(old_file, 0) != 0)
    close_down1f(non_err, old_file, NON_ERR);
    strcpy2(new_file, "mars_log/", date);
    file_copy(old_file, new_file);
    /* Run script to convert all HTML characters */
    strcpy2(command, "cat db_data/filter.html | ex - ", new_file);
    system(command);
    /* Let client know transaction successful */
     put_short(SUCCESS);
    return_success("Mars file copied", DBNO);
} /* End mars_send */

/* Send audit report generated for the week */
void audit_report_send(void)
{
    char subject[64];
    int len;

    /* Build subject line to send */
    len = strcpy1(subject, "Audit report for ");
    strncpy(subject+len, tr.log_file+4, 8);
    email_file_send("mdsEvents@nxp.com", AUDIT_MAIL_LIST,  subject, mti.audit_summary);
    /* Let client know is successful */
    put_short(SUCCESS);
    return_success("Audit report sent", DBNO);
} /* End audit_report_send */

/* Sends audit email for admin or audit group */
void audit_mail_send(char *mail_list)
{
    char subject[48];
    char file_name[48];

    /* Build the name of the file */
    strcpy2(file_name, AUDIT_PATH, tr.log_file+4);
    /* See if audit file is present */
    if (access(file_name, 0) != 0)
    {
    sprintf(rtn.msg, audit_file_err, file_name+10);
    email_send(admin.from_email, mail_list, "Audit file not present", rtn.msg);
    shut_down(rtn.msg, AUDIT_FILE_ERR);
    } /* End if for file not present */
    else
    {
    /* build subject */
    strcpy2(subject, "Audit summary for ", tr.log_file+4);
    email_file_send(admin.from_email, mail_list, subject, file_name);
    strcpy(rtn.msg, "E-mail sent for audit log.");
    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
    } /* End else for file present */
} /* end audit_mail_send */
