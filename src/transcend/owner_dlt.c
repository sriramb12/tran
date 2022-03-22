/****************************************************************************
 *
 * Function:	owner_dlt
 *
 * Description:	Retrieves info on big file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include "global_defines.h"
#include "global_server_params.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_macros.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_defines.h"
#include "rtn.h"
#include "server_lib.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h"

void owner_dlt(char *key)
{
    DBG("key='%s'", key);

    extern TRANS_FILE_STAT fs;
    char line[TRANS_LINE_NUM][TRANS_LINE_LEN];
    FILE *fp;
    FILE *read_big_file();

    /* read in global file */
    fp = read_big_file(key, line, &fs);

    /* Make sure the user owns the data */
    if (strcmp(user.email, fs.own_id) != 0)
    {
	    fclose(fp);
	    sprintf(rtn.msg, own_err, fs.title, fs.real_name);
	    shut_down(rtn.msg, OWN_ERR);
    }

    /* If the owner delete flag already set, tell user */
    if (fs.dlt_flag == 3)
    {
	    fclose(fp);
	    sprintf(rtn.msg, owner_dlt_err, fs.title, fs.real_name);
	    shut_down(rtn.msg, OWNER_DLT_ERR);
    }
    
    /* Set the delete time to 3 */
    fs.dlt_flag = 3;
    
    /* Write out changes to global file */
    DBG("Before call write_big_file()");
    write_big_file(fp, line, &fs);
    DBG("after call write_big_file()");
    
    /* Build message to send to user */
    /* Tell user deletion lock has been set */
    fs.title[0] -= 32;
    sprintf(rtn.msg, "Delete lock set on %s '%s'.", fs.title, fs.real_name);
    
    put_short(SUCCESS);
    
    put_success_msg(key);
} /* End owner_dlt */


void user_dlt(char *key)
{
    DBG("key='%s'", key);

    extern TRANS_FILE_STAT fs;
    char line[TRANS_LINE_NUM][TRANS_LINE_LEN];
    FILE *fp;
    FILE *read_big_file();

    /* read in global file */
    fp = read_big_file(key, line, &fs);

    /* Make sure the user owns the data */
    if (strcmp(user.email, fs.own_id) != 0)
    {
	    fclose(fp);
	    sprintf(rtn.msg, own_err, fs.title, fs.real_name);
	    shut_down(rtn.msg, OWN_ERR);
    }

    /* If delete lock set to 3, set back to DBNO */
    /* This will allow anyone to delete the data */
    if (fs.dlt_flag == 3)
        fs.dlt_flag = DBNO;
    else
    {
	    /* Close file no longer needed */
	    fclose(fp);
	
	    /* Tell user delete lock not set */
	    sprintf(rtn.msg, user_dlt_err, fs.title, fs.real_name);
	
	    shut_down(rtn.msg, USER_DLT_ERR);
    } /* End else */

    /* Write out changes to global file */
    DBG("Before call write_big_file()");
    write_big_file(fp, line, &fs);
    DBG("after call write_big_file()");

    /* Build message to send to user */
    /* Tell user data no longer locked for deletion */
    sprintf(rtn.msg, "Delete lock no longer set on %s '%s'.", fs.title, fs.real_name);

    put_short(SUCCESS);
    put_success_msg(key);
} /* End user_dlt */
