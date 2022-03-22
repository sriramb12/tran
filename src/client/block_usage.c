#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	block_usage
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
#include <strings.h>
#include <string.h>
#include "strcpy.h"
#ifndef __USE_BSD
#define __USE_BSD
#endif


/* The directory name passed in must not have an ending slash */

long long block_usage(char *directory)
{
    char file_name[256];
    long long file_size;
    struct stat buf;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    if ((dp = opendir(directory)) == NULL)
    return(2);
    for (dir=readdir(dp), file_size = 2; dir != NULL; dir=readdir(dp)) 
    {
    /* Ignore '..' file */
	if (strcmp(dir->d_name, "..") == 0) continue;
	/* Build complete file name */
	strcpy3(file_name, directory, "/", dir->d_name);
	if (stat(file_name, &buf) != 0)  continue;
	if ((strcmp(dir->d_name, ".") == 0)
	|| ((S_ISDIR(buf.st_mode)) < 1))
	{
	    file_size += buf.st_size/512;
	    /* Round up to the nearest block */
	    if (buf.st_size%512 > 0) file_size += 1;
	} /* End if */
	/* Otherwise find out if directory is a link */
	/* Links are not followed */
	else
	{
	    if (lstat(file_name, &buf) != 0) continue;
	    /* If this is a link, ignore it */
	    if (S_ISLNK(buf.st_mode)) continue;
	    else file_size += block_usage(file_name);
	} /* End else */
    } /* End for */
    /* Return total file size */
    closedir(dp);
    return(file_size);
} /* end block_usage */
#endif /* For not TRANSAPI */
