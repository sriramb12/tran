/****************************************************************************
 *
 * Function:	file_rename
 *
 * Move specified file to new location.
 *		If the rename command fails because of trying to move 
 *		across a partition, do a copy instead.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_params.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "rtn.h"
#include "global_debug.h" // Mazieri's debug facilities

int file_rename(char *old_file, char *new_file)
{
    DBG("old_file='%s' and new_file='%s'", old_file, new_file);

    char *file_copy();

    /* If rename fails, try a copy */
    if (rename(old_file, new_file) != SUCCESS)
    {
	    /* If file_copy fails, it will exit */
	    file_copy(old_file, new_file);
	    unlink(old_file);
    } /* End if */

    return(SUCCESS);
} /* End file_rename */
