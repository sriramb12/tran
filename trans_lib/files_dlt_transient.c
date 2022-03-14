/****************************************************************************
 *
 * Function:	files_list
 *
 * Description:	Lists files in specified directory
 *              putting names in file array 
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "close_down.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "files.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


/* The directory name passed in must not have an ending slash */
int files_dlt_transient(char *directory)
{
    unsigned long dlt_time;
    int file_cnt;
    char file_name[FILE_SIZE];
    char *ptr;
    struct stat buf;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG();

    if ((dp = opendir(directory)) == NULL)
        close_down1q(open_dir_err, directory, OPEN_DIR_ERR);

    /* Calculate the delete time */
    dlt_time = tr.upd_time-86400;

    /* Point to place where root file name is placed */
    ptr = file_name;
    ptr+= strcpy2(file_name, directory, "/");

    for (dir=readdir(dp), file_cnt = 0; dir != NULL; dir=readdir(dp)) 
    {
	    /* If this is the '.' or '..' file do not put it on the list */
	    if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0))
	        continue;
	
	    /* Do a stat on the file to get file size */
	    /* Build file name */
	    strcpy(ptr, dir->d_name);

	    if (stat(file_name, &buf) != 0) continue;
	
	    /* If modify time older than 24 hours, delete the file */
	    if (buf.st_mtime < dlt_time) 
	    {
	        unlink(file_name);
	        ++file_cnt;
	    } /* End if for deleting */
    } /* end for */

    closedir(dp);

    /* Return the number of files deleted */
    return(file_cnt);
} /* End files_dlt_transient */

