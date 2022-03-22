/****************************************************************************
 *
 * Function:	close_down
 *
 * Description:	Routines to print error messages and exit.
 *      This file contains different close down routines
 *      These routines are used by DB server and Mask Shop server
 *
 ***************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "strcpy.h"
#include "user_params.h"
#include "global_debug.h"


/* Builds help message from info from the global_params file */
void help_msg_print(char *help_msg)
{
    DBG("help_msg='%s'", help_msg);

    sprintf(help_msg, "For assistance contact %s administrator:\n\n\
    Name:  %s %s\n\
    Phone:  %s\n\
    Email:  %s\n\n",
    admin.program, admin.name1, admin.name2, admin.phone, admin.from_email);
}


/* Prints error num, error message, bad value, and help message */
/* Used with bad_req_type, bad_data_type, bad_version_flag */
void close_down1d(char *err_msg, char bad_val, register short err_no)
{

    DBG("err_msg='%s', bad_val='%d'", err_msg, bad_val);

    sprintf(bf.msg, "%s:  '%d'. %s%s", err_msg, bad_val, DOUBLE_RET, help_msg);

    /* Send return code and message to user and shut_down */
    shut_down(bf.msg, err_no);
} /* End close_down1d */


/* Prints error code and one string */
void close_down2d(char *err_msg, int bad_val, char *string, register short err_no)
{
    DBG("err_msg='%s', bad_val='%d', string='%s'", err_msg, bad_val, string);

    sprintf(bf.msg, err_msg, bad_val, string);

    /* Send return code and message to user and shut_down */
    shut_down(bf.msg, err_no);
} /* End close_down2d */


/* Prints error code and one two strings */
void close_down3d(char *err_msg, int bad_val, char *string1, char *string2, register short err_no)
{
    DBG("err_msg='%s', bad_val='%d', string='%s', string2='%s'", err_msg, bad_val, string1, string2);

    sprintf(bf.msg, err_msg, bad_val, string1, string2);

    /* Send return code and message to user and shut_down */
    shut_down(bf.msg, err_no);
} /* End close_down3d */


/* Sends formatted message with one parameter and no help message */
void close_down1f(char *format, char *par1, register short err_no)
{
    DBG("format='%s', par1='%s'", format, par1);

    sprintf(bf.msg, format, par1);

    shut_down(bf.msg, err_no);
} /* End close_down1f */


/* Sends formatted string with two parameters */
void close_down2f(char *err_msg, char *par1, char *par2, register short err_no)
{
    DBG("err_msg='%s', par1='%s', par2='%s'", err_msg, par1, par2);

    sprintf(bf.msg, err_msg, par1, par2);

    /* Send return code and message to user and shut_down */
    shut_down(bf.msg, err_no);
} /* End close_down2f */


/* Sends a formatted message with 3 parameters and no help message */
void close_down3f(char *format, char *par1, char *par2, char *par3, register short err_no)
{
    DBG("format='%s', par1='%s', par2='%s', par3='%s'", format, par1, par2, par3);

    sprintf(bf.msg, format, par1, par2, par3);
    shut_down(bf.msg, err_no);
} /* End close_down3f */


/* Sends a formatted message with 4 parameters and no help message */
void close_down4f(char *format, char *par1, char *par2, char *par3, char *par4, register short err_no)
{
    DBG("format='%s', par1='%s', par2='%s', par3='%s', par4='%s'", format, par1, par2, par3, par4);

    sprintf(bf.msg, format, par1, par2, par3, par4);
    shut_down(bf.msg, err_no);
} /* End close_down4f */


/* Sends a formatted message with 5 parameters and no help message */
void close_down5f(char *format, char *par1, char *par2, char *par3, char *par4, char *par5, register short err_no)
{
    DBG("format='%s', par1='%s', par2='%s', par3='%s', par4='%s', par5='%s'", format, par1, par2, par3, par4, par5);

    sprintf(bf.msg, format, par1, par2, par3, par4, par5);
    shut_down(bf.msg, err_no);
} /* End close_down5f */


/* Sends formatted message with one parameters and a help message */
void close_down1p(char *format, char *par1, register short err_no)
{
    DBG("format='%s', par1='%s'", format, par1);

    sprintf(bf.msg, format, par1, DOUBLE_RET, help_msg);
    shut_down(bf.msg, err_no);
} /* End close_down1p */


/* Sends formatted message with two parameters and a help message */
void close_down2p(char *format, char *par1, char *par2, register short err_no)
{
    DBG("format='%s', par1='%s', par2='%s'", format, par1, par2);

    sprintf(bf.msg, format, par1, par2, DOUBLE_RET, help_msg);
    shut_down(bf.msg, err_no);
} /* End close_down2p */


void close_down3p(char *format, char *par1, char *par2, char *par3, register short err_no)
{
    DBG("format='%s', par1='%s', par2='%s', par3='%s'", format, par1, par2, par3);

    sprintf(bf.msg, format, par1, par2, par3, DOUBLE_RET, help_msg);
    shut_down(bf.msg, err_no);
} /* End close_down2p */


/* Sends user error code, error message, quoted string, and help message */
void close_down1q(char *err_msg, char *file, register short err_no)
{
    DBG("err_msg='%s', file='%s'", err_msg, file);

    strcpy7(bf.msg, err_msg, RET2TAB, "'", file, "'",  DOUBLE_RET, help_msg);
    shut_down(bf.msg, err_no);
} /* End close_down1q */


void close_down1m(char *err_msg, char *file, register short err_no)
{
    char subject[80];
    char machine_msg[48];

    DBG("err_msg='%s', file='%s'", err_msg, file);

    sprintf(machine_msg, "Error%d@%s", err_no, so.program);
    strcpy5(bf.msg, err_msg, RET2TAB, "'", file, "'");
    email_cnt = 0;

    if(email_cnt == 0)
    {
	    /* Indicate that e-mail message already sent from this process */
	    email_cnt = 1;
	    sprintf(subject, "Error %d from %s", err_no, so.program);
	    email_send(user.email, admin.err_email, subject, bf.msg);
    }

    shut_down(bf.msg, err_no);
} /* End close_down1m */


/* Gives an error and send e-mail to admin */
void close_down_mail(char *err_msg, register short err_no)
{
    char subject[80];
    char email_address[80];

    DBG("err_msg='%s'", err_msg);

    sprintf(email_address, "Error%d@%s", err_no, so.program);
    sprintf(subject, "Error %d from %s", err_no, so.program);

    email_send( email_address, admin.err_email, subject, err_msg);
    shut_down(bf.msg, err_no);
} /* End close_down_mail */


/* Sends email to admin and user */
void close_down1t(char *err_msg, char *file, register short err_no, char *user_email)
{
    char local_email[48];
    char subject[80];

    DBG("err_msg='%s', file='%s'", err_msg, file);

    strcpy5(bf.msg, err_msg, RET2TAB, "'", file, "'");
    sprintf(subject, "Error %d from %s", err_no, so.program);
	email_send( "", admin.err_email, subject, bf.msg);

    strcpy(local_email, user_email);
	email_send( "", local_email, subject, bf.msg);

    shut_down(bf.msg, err_no);
} /* End close_down1t */


/* Prints one error message, 2 quoted strings and help message */
void close_down2q(char *err_msg, char *file1, char *file2, register short err_no)
{
    DBG("err_msg='%s', file1='%s', file2='%s'", err_msg, file1, file2);

    strcpy7(bf.msg, err_msg, RET2TAB, "'", file1, RET_TO, file2, "'");
    shut_down(bf.msg, err_no);
} /* end close_down2q */


/* Sends one message and help message */
void close_down(char *err_msg, register short err_no)
{
    DBG("err_msg='%s'", err_msg);

    strcpy3(bf.msg, err_msg, DOUBLE_RET, help_msg);

    /* Send return code and message to user and shut_down */
    shut_down(bf.msg, err_no);
} /* End close_down */


/* Sends user error number, one message and help message */
void close_down1s(char *err_msg, register short err_no)
{
    DBG("err_msg='%s'", err_msg);

    sprintf(bf.msg, "Error %d:  %s%s%s", err_no, err_msg, DOUBLE_RET, help_msg);

    /* Send return code and message to user and shut_down */
    shut_down(bf.msg, err_no);
} /* End close_down1s */


/* Sends error number and 2 messages on the same line */
void close_down2s(char *msg1, char *msg2, register short err_no)
{
    DBG("msg1='%s', msg2='%s'", msg1, msg2);

    sprintf(bf.msg, "Error %d:  %s%s", err_no, msg1, msg2);
    /* Send return code and message to user and shut_down */
    shut_down(bf.msg, err_no);
}

/* Sends error code, 3 messages, and help message */
void close_down3s(char *err_msg, char *file, char *help, register short err_no)
{

    DBG("err_msg='%s', file='%s', help='%s'", err_msg, file, help);

    sprintf(bf.msg, "Error %d:  %s%s'%s'%s%s%s%s", err_no, err_msg, RET2TAB, file, RET2TAB, help, DOUBLE_RET, help_msg);
    shut_down(bf.msg, err_no);
}


/* Prints one error message, 2 quoted strings and help message */
/* Also sends the database administrator E-mail concerning the problem */
void close_down2m(char *err_msg, char *file1, char *file2, register short err_no)
{
    char tag[8];
    char subject[80];

    DBG("err_msg='%s', file1='%s', file2='%s'", err_msg, file2, file2);

    sprintf(subject, "Error from %s", so.program);
    sprintf(tag, "tag%d", err_no);

    strcpy8(bf.msg, err_msg, RET2TAB, "'", file1, RET_TO, file2, "'", tag);

    email_send( "", admin.err_email, subject, bf.msg);
    shut_down(bf.msg, err_no);
} /* end close_down2m */
