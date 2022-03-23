#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	trans_release_get
 *
 * Description:	Retrieve a new release from the Transcend/InterCom database.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "ssl_client_lib.h"
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "trans_client_msg.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_msg.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include <string.h>
#include "ssl_client_lib.h"

#include "global_debug.h"

/* Print commands to retrieve available releases */
int trans_release_print(char **argv)
{
    DBG();

    char *ptr = root_find(argv[0], '/');

    sprintf(rtn.msg, TRANS_RELEASE_MSG, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr);

    return(SUCCESS);
} /* end release_print */


int trans_release_get(int index)
{
    DBG();

     char c_string[8];
    char *file_ptr;
    short ret;

    /* Connect to the Transcend database */
    if ((ret = trans_socket_init(index)) < SUCCESS)
    return(ret);
    /* Get the size and file mode */
    /* The size is simply ignored */
    /* It is present to make the '-g' option compatible */
    /* with the '-r' option */
trans[index]->original_size =     get_ssl_long();
trans[index]->file_mode =     get_ssl_long();
    trans[index]->file_type = UNSPECIFIED;
   /* Put the path in the file name */
    file_ptr = trans[index]->file_name + strcpy2(trans[index]->file_name, user.cwd, "/");
    /* Get the file name */
    ret = get_ssl_string(file_ptr);
    /* At this point return mesage is initialized */
    if (ret < 0)
	return(ret);
    /* If file present in this directory, warn them it will be overwritten */
    if (access(trans[index]->file_name, 0) == 0) 
    {
    if (trans[index]->overwrite_files == DBYES)
    unlink(trans[index]->file_name);
else
    {
	printf("File '%s' already exists.  Overwrite this file, y/n?  ", file_ptr);
	scanf("%s", c_string);
	printf("\n");
	if (( c_string[0] == 'y') || (c_string[0] == 'Y')) 
	{
	    /* Delete the file and indicate files have been overwritten */
	    unlink(trans[index]->file_name);
	} /* End if */
	else
	{
	    /* Tell server unable to create file */
	    put_ssl_short(BAD_CREATE);
    ret = get_ssl_short();
     get_ssl_string(rtn.msg);
	    close(sock);
	    return(ret);
	}
    }
    } /* end if for file present */
     /* Determine if binary or ASCII file */
#ifdef garbage
    if ((strcmp(file_ptr+ret-6, "_trans") == 0)
    || (strcmp(file_ptr+ret-4, ".tar") == 0)
    || (strcmp(file_ptr+ret-4, ".dll") == 0)
    || (strcmp(file_ptr+ret-4, ".exe") == 0))
    trans[index]->file_type = BINARY;
    else trans[index]->file_type = ASCII;
#endif
    /* Get the file */
    return(custom_file_get(index));
} /* end trans_release_get */

#endif
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
