/****************************************************************************
 *
 * Function:	mod_email_big_file
 *
 * Description:	Modifies the Email status of a big file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "client_lib.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_server_macros.h"
#include "trans_defines.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h"

void mod_email_big_file(char *key)
{
    extern TRANS_FILE_STAT fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    FILE *fp;
    char *ptr;

    DBG();

    /* Read in data from global file */
    fp = read_big_file(key, lines, &fs);

    if (strcmp(gu.email1, fs.own_id) != 0)
    {
        sprintf(rtn.msg, own_err, fs.title, fs.real_name);
        shut_down(rtn.msg, OWN_ERR);
    }
    /* Alter the Email flag */
    if (gu.req_type == DBSET_FILE_EMAIL)
    {
	fs.email_flag = DBYES;
	strcpy5(rtn.msg, "You will now receive Email when users retrieve ", 
	fs.title, " '", fs.real_name, "'");
    }
    else
    {
	fs.email_flag = DBNO;
	strcpy5(rtn.msg, "You will no longer receive Email when users retrieve ", 
	fs.title, " '", fs.real_name, "'");
    }
    /* Write out changes to big file */
    write_big_file(fp, lines, &fs);
    /* Tell user Email flag has been changed */
    put_short(SUCCESS);
    put_success_msg(key);
} /* End mod_email_big_file */

void lock_big_file(char *key)
{
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    extern TRANS_FILE_STAT fs;
    FILE *fp;

    DBG();

    /* Read in data from global file */
    fp = read_big_file(key, lines, &fs);
    /* If the file is not locked, lock it */
    if (fs.lock_flag == 'u')
    {
	 fs.lock_flag = 'L';
	/* Write out changes to big file */
	write_big_file(fp, lines, &fs);
	/* Tell user transaction successful */
	put_short(SUCCESS);
	sprintf(rtn.msg, "Retrieval lock set on %s '%s.",
	fs.title, fs.real_name);
	put_success_msg(key);
    } /* End if */
    else
    {
	/* Close the open global file */
	fclose(fp);
	/* Build error message to send */
	strcpy3(rtn.msg, alr_lock_err, fs.real_name, "'");
    	shut_down(rtn.msg, ALR_LOCK_ERR);
    } /* End else */
} /* End lock_big_file */

/* Unlock previously locked big file */
void unlock_big_file(char *key)
{
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    extern TRANS_FILE_STAT fs;
    FILE *fp;

    DBG();

    /* Read in data from global file */
    fp = read_big_file(key, lines, &fs);
    /* If the file is locked, unlock it */
    if (fs.lock_flag == 'L')
    {
	 fs.lock_flag = 'u';
	/* Write out changes to big file */
	write_big_file(fp, lines, &fs);
	/* Tell user transaction successful */
	put_short(SUCCESS);
	sprintf(rtn.msg, "Retrieval lock no longer set on %s '%s.",
	fs.title, fs.real_name);
	put_success_msg(key);
    } /* End if */
    else
    {
	/* Close the open global file */
	/* Build error message to send */
	strcpy3(rtn.msg, not_lock_err, fs.real_name, "'");
    	shut_down(rtn.msg, NOT_LOCK_ERR);
    } /* End else */
} /* End unlock_big_file */


