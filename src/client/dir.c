/****************************************************************************
 *
 * Functions:	dir_archive_dlt, dir_crt, dir_dlt, dir_remove, dir_rename
 *
 * Description:	Delete specified directory 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <malloc.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#ifndef WINDOWS_NT4
#include <alloca.h>
#endif
#include <unistd.h>
#include "client_dir.h"
#include "global_client_errs.h"
#include "global_defines.h"
#include "rtn.h"
#include "strcpy.h"
#include "transapi_defines.h"
#include "trans_prototypes.h"

#include "global_debug.h"

#define HELP_MSG "For assistance, contact Intercom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"


/* Delete directory */
int dir_dlt(char *directory)
{
    DBG();
    char file_name[FILE_SIZE];
    DIR *dp;
    DIR *opendir(const char *name);
    struct dirent *dir;
    struct dirent *readdir(DIR *dirp);
    char *file_ptr;

    /* Copy directory into file_name */
    /* Open directory to be deleted */
    if ((dp = opendir(directory)) == NULL)
    {
	    /* If this is really a file, just delete it */
	    if (unlink(directory) == 0)
	    return(SUCCESS);
	    else
	    {
	        strcpy6(rtn.msg, dir_open_err, RET2TAB_QUOTE, directory,
	        "'", DOUBLE_RET, HELP_MSG);
	        return(FAILURE);
	    } /* End else */
    }
    /* Point file_ptr at end of directory name */
    file_ptr = file_name + strcpy2(file_name, directory, "/");

    /* Delete all files in the directory */
    for (dir=readdir(dp); dir != NULL; dir=readdir(dp)) 
    {
	    if ((dir->d_name[0] == '.') && ((dir->d_name[1] == '.') 
	    || (dir->d_name[1] == 0))) continue;
	    /* Build the file name to be deleted */
	    strcpy(file_ptr, dir->d_name);
	    if (unlink(file_name) != SUCCESS)
	    {
	        strcpy6(rtn.msg, file_dlt_err, RET2TAB_QUOTE, file_name,
	        "'", DOUBLE_RET, HELP_MSG);
	        return(FAILURE);
	    } /* end if for unsuccessful delete */
    } /* end for statement */
    closedir(dp);
    /* Now delete the directory */
    if (rmdir(directory) != SUCCESS)
    {
	    sprintf(rtn.msg, dlt_dir_err, errno, directory, HELP_MSG);
	    return(FAILURE);
    }
    /* else if everything has been successful, return SUCCESS to client */
    return(SUCCESS);
} /* end dir_dlt */

int dir_remove(char *directory)
{
    DBG();
    char file_name[FILE_SIZE];
    DIR *dp;
    DIR *opendir(const char *name);
    struct dirent *dir;
    struct dirent *readdir(DIR *dirp);
    char *file_ptr;

    /* Open directory to be deleted */
    if ((dp = opendir(directory)) == NULL)
    {
	    /* If this is really a file, just delete it */
	    if (unlink(directory) == 0)
	    return(SUCCESS);
	    else
	    {
	        rtn.len += strcpy6(rtn.msg+rtn.len, dir_open_err, RET2TAB_QUOTE, directory,
	        "'", DOUBLE_RET, HELP_MSG);
	        return(FAILURE);
	    } /* End else */
    }
    /* Point file_ptr at end of directory name */
    file_ptr = file_name + strcpy2(file_name, directory, "/");

    /* Delete all files in the directory */
    for (dir=readdir(dp); dir != NULL; dir=readdir(dp)) 
    {
	    if ((dir->d_name[0] == '.') && ((dir->d_name[1] == '.') 
	    || (dir->d_name[1] == 0))) continue;
	    /* Build the file name to be deleted */
	    strcpy(file_ptr, dir->d_name);
	    if (unlink(file_name) != SUCCESS)
	    {
	    rtn.len += strcpy6(rtn.msg+rtn.len, file_dlt_wrn, RET2TAB_QUOTE, file_name,
	    "'", DOUBLE_RET, HELP_MSG);
	    return(FAILURE);
	} /* end if for unsuccessful delete */
    } /* end for statement */
    closedir(dp);
    /* Now delete the directory */
    if (rmdir(directory) != SUCCESS)
    {
	rtn.len += sprintf(rtn.msg+rtn.len, dlt_dir_wrn, errno, directory, HELP_MSG);
	return(FAILURE);
    }
    /* else if everything has been successful, return SUCCESS to client */
    return(SUCCESS);
} /* end dir_remove */

/* List specified directory */
int dir_list(char *directory, char *files[])
{
    DBG();
    int cnt = 0;
    DIR *dp;
    DIR *opendir(const char *name);
    struct dirent *dir;
    struct dirent *readdir(DIR *dirp);


    if ((dp = opendir(directory)) == NULL)
    {
	strcpy6(rtn.msg, dir_open_err, RET2TAB_QUOTE, directory, 
	"'", DOUBLE_RET, HELP_MSG);
	files[0] = NULL;
	return(FAILURE);
    }
    for (dir=readdir(dp);  dir != NULL; dir=readdir(dp)) 
    {
	/* If file name begins with '.', do not put it on the list */
	if ((strcmp(dir->d_name, ".") == 0)
	|| (strcmp(dir->d_name, "..") == 0)) continue;
	files[cnt] = malloc(strlen(dir->d_name) +1);
	strcpy(files[cnt++], dir->d_name);
    } /* end for */
    closedir(dp);
    /* Poiht last element at 0 */
    files[cnt] = NULL;
    /* Return the number of files found */
    return(cnt);
} /* end dir_list */



int dir_crt(char *dir)
{
    DBG();

    /* If directory already present, simply return */
    if (access(dir, 0) == 0) return(SUCCESS);
    else if (mkdir(dir, 0700) != 0)
    {
	strcpy3(rtn.msg, crt_dir_err, dir, "'");
	return(FAILURE);
    }
    else return(SUCCESS);
} /* end dir_crt */

/* Return pointer to value for current working directory */
/* Memory pointed to is global and does not need to be freed */
char * cwd_get(void)
{
    DBG();

    return(user.cwd);
} /* end cwd_get */

/* Set current working directory */
/* Sets global value used by database client for current working directory */
void cwd_set(char *directory)
{
    DBG();
    strcpy(user.cwd, directory);
} /* End cwd_set */
