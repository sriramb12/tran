#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	dbhist_big_file
 *
 * Description:	Retrieve history of a big file from the 
 *		Transcend/InterCom repository.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "transrc.h"
#include "transapi_defines.h"

#include "trans_prototypes.h"
#include "client_lib.h"

int dbhist_big_file(char *secure_key, char *file_key)		/* Key needed to retrieve the big file */
{
    char file_name[FILE_SIZE];
    char *file_ptr;
    short ret;

    if (is_keyword(DBHIST_DATA, secure_key) < 0) return(-1);
    /* Connect to the Transcend database */
    if ((ret = init_trans_socket(DBHIST_DATA, secure_key, NONE, NONE, 0, 0, 
    UNSPECIFIED, file_key, NONE, NONE, NONE, NONE, NONE)) < SUCCESS) return(-517);
   /* Put the path in the file name */
#ifdef WINDOWS_NT4
    file_ptr = file_name;
#else
    file_ptr = file_name + strcpy2(file_name, transrc.historyDir, "/");
#endif
    /* Get the file name */
    ret = get_string(file_ptr);
    /* If transaction failed,  return mesage is initialized */
    if (ret < 0)
	return(ret);
    /* do not pull down the history file if user does not want it */
#ifdef TRANSCEND_GUI
    /* Do not want to get the history file for Transcend GUI */
    transrc.historyDir[0] = '\0';
#endif
    if (transrc.historyDir[0] == '\0')
    {
	put_short(DBNO);
	ret = get_ret_code();
    } else {
	/* If file already present, delete it */
    	if (access(file_name, 0) == 0) 
	unlink(file_name);
	/* Get the file */
	ret = get_file(DBHIST_DATA, file_name, 33206, ASCII);
	/* Return message retrieved in get_file */
	/* If got file in home directory, let user know */
     if((strcmp(transrc.historyDir, user.cwd) != 0)
     &&(strcmp(transrc.historyDir, "None") != 0))
	strcat(rtn.msg, "\nThis file is in your home directory.");
    }
    return(ret);
} /* end dbhist_big_file */
#endif /* For not TRANSAPI */
