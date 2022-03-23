/****************************************************************************
 *
 * Function:	info_big_file
 *
 * Description:	Retrieves info on big file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include "global_defines.h"
#include <strings.h>
#include <string.h>
#include <time.h>
#include "global_server_errs.h"
#include "global_server_params.h"
#include "server_lib.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_server_macros.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

/* Get info on big file */
void info_big_file(char *key)
{
    DBG("key='%s'", key);

    extern int secure_keyword_fnd;
    FILE *fp;
    extern TRANS_FILE_STAT fs;
     int cnt;
    char *msg = rtn.msg;
    char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char tmp_key[10];
    long long total_size;

    strcpy(tmp_key, key);
    
    /* If the key is found, it is returned in variable 'key' */
    /* Returns status indicating if key sent in is a secure keyword */
    /* Looks up the file keyword if this is a secure keyword */
    /* Read meta data into line buffer */
    fp = read_big_file(key, lines, &fs);
    fclose(fp);
    
    /* If directory update whether transferred with tar or gtar */
    if ((fs.file_type == DIRECTORY) && (strlen(fs.tar_type) > 1))
        strcpy3(lines[10]+strlen(lines[10]), " (Transferred with ", fs.tar_type, ")");

    /* Copy first four lines into buffer to send to user */
    for (cnt = 1; cnt < 4; ++cnt)
        msg += strcpy2(msg, lines[cnt], "\n");

    /* If secure keyword present, display both file and secure keyword */
    if (secure_keyword_fnd == DBYES) 
    {
	    msg += strcpy3(msg, "Personal secure Key:  ", tmp_key, "\n");
	    strncpy(lines[4], "File key:", 9);
    }

    /* Now copy in the rest of the meta data into the buffer */
    for (cnt = 4; cnt < TRANS_LINE_NUM; ++cnt)
        msg += strcpy2(msg, lines[cnt], "\n");

    /* Get the lock status of the file */
    /* If data still being transferred, show user information about the transfer */
    if (fs.lock_flag == 'l')
    {
	    /* Parse out the total site */
	    sscanf(lines[2], "%*s %*s %llu", &total_size);
	    
	    /* Add transfer status to message */
	    transfer_status_big_file(key, msg, total_size);
    } /* End if for transfer still in progress */
    /* If the file is locked for retrieval, let user know */
    else if (fs.lock_flag == 'L')
        sprintf(msg, "Status:      Lock for retrieval\n");

    /* No one can retrieve the data if it is locked for retrieval by the user */
    /* Indicate data will be sent */
    put_short(SUCCESS);
    
    /* Send the return code */
    put_success_msg(key);
} /* End info_big_file */


/* Get the file name and file mode */
void mode_big_file(char *key)
{
    DBG("key='%s'", key);

    extern int secure_keyword_fnd;
    FILE *fp;
    extern TRANS_FILE_STAT fs;
    char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char tmp_key[10];

    strcpy(tmp_key, key);
    
    /* If the key is found, it is returned in variable 'key' */
    /* Returns status indicating if key sent in is a secure keyword */
    /* Looks up the file keyword if this is a secure keyword */
    /* Read meta data into line buffer */
    fp = read_big_file(key, lines, &fs);
    fclose(fp);

    /* Put the mode and file name in the return message */
    sprintf(rtn.msg, "%d %s", fs.mode, fs.real_name);
    if (fs.file_type == DIRECTORY)
        strcat(rtn.msg, ".tar");
    
    /* Indicate data will be sent */
    put_short(SUCCESS);
    
    /* Send the return code */
    put_success_msg(key);
} /* End mode_big_file */
/*ts.files.fileuse*/
