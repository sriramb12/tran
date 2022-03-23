/****************************************************************************
 *
 * Function:	disk_usage
 *
 * Description:	Calculates size of specified directory tree
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "global_defines.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities

#ifndef __USE_BSD
#define __USE_BSD
#endif


/* The directory name passed in must not have an ending slash */
unsigned long disk_usage(char *directory)
{
    char file_name[FILE_SIZE];
    unsigned long file_size;
    struct stat buf;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG("directory='%s'", directory);

    if ((dp = opendir(directory)) == NULL) return(512);

    for (dir=readdir(dp), file_size = 0; dir != NULL; dir=readdir(dp)) 
    {
        /* Ignore '..' file */
	    if (strcmp(dir->d_name, "..") == 0) continue;

	    /* Build complete file name */
	    strcpy3(file_name, directory, "/", dir->d_name);
	    if (stat(file_name, &buf) != 0)  continue;
	
	    if ((strcmp(dir->d_name, ".") == 0) || ((S_ISDIR(buf.st_mode)) < 1))
	    {
	        if (buf.st_size < 1024) file_size += 1024;
	        else file_size += buf.st_size;
	    } /* End if */
	    /* Otherwise find out if directory is a link */
	    /* Links are not followed */
	    else
	    {
	        if (lstat(file_name, &buf) != 0) continue;

	        /* If this is a link, ignore it */
	        if (S_ISLNK(buf.st_mode)) continue;
	        else file_size += disk_usage(file_name);
	    } /* End else */
    } /* End for */

    /* Return total file size */
    closedir(dp);
    return(file_size);
} /* end disk_usage */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
