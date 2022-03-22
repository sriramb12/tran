#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	dbquery_user
 *
 * Description:	Retrieve info about user(s) from Transcend registry
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "transapi_defines.h"
#include "trans_prototypes.h"
#include "client_lib.h"

int dbquery_user(int req_type, char *key)		/* Key needed to retrieve the big file */
{
    char file_name[FILE_SIZE];
    char *file_ptr;
    short ret;

    /* Connect to the Transcend database */
    if ((ret = init_trans_socket(req_type, key, NONE, NONE, 0, 0, 
    UNSPECIFIED, NONE_STRING)) < SUCCESS) return(ret);
   /* Put the path in the file name */
#ifdef WINDOWS_NT4
    file_ptr = file_name;
#else
    file_ptr = file_name + strcpy2(file_name, user.cwd, "/");
#endif
    /* Get the file name */
    ret = get_string(file_ptr);
    /* If transaction failed,  return mesage is initialized */
    if (ret < 0)
	return(ret);
    if (access(file_name, 0) == 0) 
    unlink(file_name);
    /* Get the file */
    ret = get_file(DBQUERY_USER, file_name, 33206, ASCII);
    /* Return message retrieved in get_file */
    return(ret);
} /* end dbquery_user */
#endif /* For not TRANSAPI */
