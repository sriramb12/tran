/****************************************************************************
 *
 * Function:	files_copy
 *
 * Description:	Copies files in specified directory putting names in file array 
 *		into a new directory.
 *
* Returns:	void
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "close_down.h"
#include "dir.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "files.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities


/* The directory name passed in must not have an ending slash */
void files_copy(DB_FILES *file, char *old_dir)
{
    DBG();

    char *old_ptr;
    char *new_ptr;
    char new_file[FILE_SIZE];
    char old_file[FILE_SIZE];
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    if ((dp = opendir(old_dir)) == NULL)
        close_down2d(open_dir_err, errno, file->dir, OPEN_DIR_ERR);

    /* Create the new directory */
    dir_create(file->dir);

    /* Build path for old file */
    old_ptr = old_file + strcpy2(old_file, old_dir, "/");

    /* Build path for new directory */
    new_ptr = new_file + strcpy2(new_file, file->dir, "/");

    for (dir=readdir(dp), file->num = 0; dir != NULL; dir=readdir(dp)) 
    {
	    /* If file name begins with '.', do not put it on the list */
	    if (dir->d_name[0] == '.') continue;

	    strcpy(file->name[file->num++], dir->d_name);
	    
	    /* Build complete names for file copy */
	    strcpy(old_ptr, dir->d_name);
	    strcpy(new_ptr, dir->d_name);
	    file_copy(old_file, new_file);
    } /* end for */

    closedir(dp);
} /* end files_copy */
