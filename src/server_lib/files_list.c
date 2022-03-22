/****************************************************************************
 *
 * Function:	files_list
 *
 * Description:	Lists files in specified directory putting names in file array 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "files.h"
#include "global_debug.h" // Mazieri's debug facilities

/* The directory name passed in must not have an ending slash */
int files_list(DB_FILES *file)
{
    DBG();

    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    if ((dp = opendir(file->dir)) == NULL)
        close_down2d(open_dir_err, errno, file->dir,  OPEN_DIR_ERR);

    for (dir=readdir(dp), file->num = 0; dir != NULL; dir=readdir(dp)) 
    {
	    /* If file name begins with '.', do not put it on the list */
	    if (dir->d_name[0] == '.') continue;
	
	    strcpy(file->name[file->num++], dir->d_name);
    } /* end for */

    closedir(dp);

    /* Return the number of files found */
    return(file->num);
} /* end files_list */
