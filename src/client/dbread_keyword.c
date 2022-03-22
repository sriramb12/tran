#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	dbread_keyword
 *
 * Description:	Retrieve history of a big file from the 
 *		Transcend/InterCom repository.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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

int dbread_keyword(int req_type)
{
    char file_name[FILE_SIZE];
    char *file_ptr;
    short ret;

    /* Connect to the Transcend database */
    if ((ret = init_trans_socket(req_type, NONE, NONE, NONE, 0, 0, 
    UNSPECIFIED, NONE_STRING)) < SUCCESS) return(ret);
   /* Put the path in the file name */
    file_ptr = file_name + strcpy2(file_name, user.cwd, "/");
    /* Get the file name */
    ret = get_string(file_ptr);
    /* If transaction failed,  return mesage is initialized */
    if (ret < 0)
	return(ret);
    if (access(file_name, 0) == 0) 
    unlink(file_name);
    /* Get the file */
    ret = get_file(DBREAD_KEYWORD, file_name, 33206, ASCII);
#ifndef TRANSCEND_GUI
    if (ret >= 0)
    {
	char command[FILE_SIZE];
	strcpy2(command, "more ", file_name);
	system(command);
	printf("List of secure keywords saved in file %s.\n\n", file_ptr);
	exit(0);
    } /* End if */
#endif
    /* Return message retrieved in get_file */
    return(ret);
} /* end dbread_keyword */

#endif /* For not TRANSAPI */
