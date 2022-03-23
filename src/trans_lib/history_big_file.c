/****************************************************************************
 *
 * Function:	history_big_file
 *
 * Description:	Retrieves history of a big file from the InterCom/TransWeb database.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include "close_down.h"
#include <unistd.h>
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_server_prototypes.h"
#include "trans_defines.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "line.h"
#include "global_debug.h"

/* Count number of lines in history file */
int history_space_left(char *key)
{
    FILE *po;
    char command[64];
    int cnt;

    DBG();

    strcpy3(command, "wc -l history_data/", key, ".history");
    /* If this is not a history transaction, indicate space left */
    if (gu.req_type != DBHIST_DATA) return(DBYES);
    if ((po = popen(command, "r")) == NULL)
    return(DBYES);
    fscanf(po, "%d", &cnt);
    pclose(po);
    if (cnt > 30) return(DBNO);
    else return(DBYES);
} /* end history_space_left */

int     history_info_get(FILE *fp)
{
    int len = 1;
    int cnt = 0;
    rtn.len = 0;

    DBG();

    while ((rtn.len < 3900) && (len >= 0)) 
    {
    len = line_get(fp, rtn.msg+rtn.len);
	rtn.len += len;
	/* Add the carriage return that was stripped off */
	rtn.msg[rtn.len++] = '\n';
	++cnt;
    } /* End while */
    return(cnt);
} /* end history_info_get */

void history_big_file(char *keyword)
{
    extern char program_name[12];
    extern char global_secure_keyword[];
    int ret;
    int cnt;
    char fnd = DBNO;
    int is_secure_key;
    FILE *fp;

    DBG();

    strcpy(global_secure_keyword, keyword);
     is_secure_key  = keyword_find(global_secure_keyword, keyword);
    /* Send the history file to the user */
    /* Build complete name of file */
    strcpy3(bf.file_name, HISTORY_FILE_PATH, keyword, ".history");
    /* Obtain a file descriptor to file */
    fp = fopen(bf.file_name, "r");
    if (fp == NULL)
    return_non_err(keyword);
    /* Scan first the first lines, it contains the delete date */
    if (line_get(fp, bf.buffer) <= 0)
    close_down1m(read_err, bf.file_name, READ_ERR);
    /* Indicate that file wil be sent */
    put_short(SUCCESS);
    /* Send name of file */
    /* Pass root name only without full path */
    put_string(bf.file_name+HISTORY_FILE_PATH_LEN);
    /* Get acknowledgement */
    /* If user does not want the file, skip the next part */
    if ((ret = get_short()) == 0)
    {
    /* Grab the first 1024 characters for the return message */
	history_info_get(fp);
    put_binary_string(rtn.msg, rtn.len);
    /* Read additional text from the file and write them to the socket */
    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), 1024, fp)) > 0) 
    {
	fnd = DBYES;
	put_binary_string(bf.buffer, ret);
    }
    /* Tell client all data for this file sent */
    put_short(SUCCESS);
    /* Close the file just sent */
    fclose(fp);
    if (fnd == DBYES)
    {
	sprintf(rtn.msg+rtn.len,
	" \nFirst %d transactions performed shown above.  Addi-\ntional transactions saved in file '%s'.",
	cnt, bf.file_name+HISTORY_FILE_PATH_LEN);
    }
    else
    {
    strcpy5(rtn.msg+rtn.len, "\nHistory of ", program_name, " transactions performed saved in file '",
    bf.file_name+HISTORY_FILE_PATH_LEN, "'.");
    }
    /* Send the return code */
    put_success_msg(keyword);
    } /* End if for client did want the file */
    else
    {
	/* Get information out of the history file */
	history_info_get(fp);
	fclose(fp);
    put_success_msg(keyword);
    } /* end else */
} /* End history_big_file */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
