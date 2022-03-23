/****************************************************************************
 *
 * Functions:	dir_archive_dlt, dir_create, dir_delete, dir_remove, dir_rename
 *
 * Description:	Delete specified directory
 *		If there is a problem deleting the directory,
 *		a caution is logged, but the transaction continues
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "close_down.h"
#include "dir.h"
#include "files.h"
#include "global_caution_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_debug.h" // Mazieri's debug facilities
#include "rtn.h"
#include "strcpy.h"

int dir_archive_dlt(char *directory, char *mask)
{
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();
    char *file_ptr;
    char state_file[16];
    char crt_file[16];
    char data_log_file[16];
    int len;

    DBG("directory='%s' and mask='%s'", directory, mask);

    /* If directory null string, return immediately */
    /* Copy directory into file_name */
    if ((len = strcpy2(bf.file_name, directory, "/")) < 2)
    {
	    data_log(tr.log_file, invalid_dir_name);
	    return(INVALID_DIR_NAME);
    }

    /* Open directory */
    if ((dp = opendir(directory)) == NULL)
    {
	    log3(invalid_dir_open, directory, "'");
	    return(INVALID_DIR_OPEN);
    } /* End if */

    /* Point file_ptr at end of directory name */
    file_ptr = bf.file_name + len;

    /* Build state file name */
    strcpy2(state_file, mask, ".state");

    /* Build crt file name */
    strcpy2(crt_file, mask, ".crt");

    /* Build data log file name */
    strcpy2(data_log_file, mask, ".data_log");

    /* Delete all files in the directory */
    for (dir=readdir(dp); dir != NULL; dir=readdir(dp))
    {
	    if ((dir->d_name[0] == '.') && ((dir->d_name[1] == '.') || (dir->d_name[1] == 0))) continue;

	    /* Also do not delete the state file */
	    if (strcmp(dir->d_name, state_file) == 0) continue;

        /* In addition do not delete the 'crt' file */
	    if (strcmp(dir->d_name, crt_file) == 0) continue;

	    /* Finally do not delete the data log file */
	    if (strcmp(dir->d_name, data_log_file) == 0) continue;

	    /* Build the file name to be deleted */
	    strcpy(file_ptr, dir->d_name);

	    if (unlink(bf.file_name) != SUCCESS)
	    {
	        log3(invalid_unlink, bf.file_name, "'");
	        return(INVALID_UNLINK);
	    } /* end if for unsuccessful delete */
    } /* end for statement */

    closedir(dp);

    return(SUCCESS);
} /* end dir_archive_dlt */


/* Creates new directory */
/* If directory already present, deletes and re-creates */
void dir_create(char *dir)
{
# if !defined(PATH_MAX)
#define PATH_MAX   1600
#endif
    char mydir[PATH_MAX];
    DIR *dir_exist = NULL;
    char *slash  = NULL;
    int ret = SUCCESS;

    DBG("dir='%s'", dir);

    strcpy(mydir, dir);
    // try to create all directories in the path
    slash = strchr(mydir, '/');
    while (ret == SUCCESS && slash != NULL)
    {
        *slash = 0;
        dir_exist = opendir(mydir);
        if (dir_exist == NULL)
        {
            ret = mkdir(mydir, 0770);
        }
        else
        {
            closedir(dir_exist);
        }
        *slash = '/';
        slash = strchr(slash + 1, '/'); // next subdir
    }
    if (mkdir(dir, 0770) != SUCCESS)
    {
	    dir_remove(dir);
	    if (mkdir(dir, 0770) != SUCCESS)
	    {
	        close_down2d(mkdir_err, errno,  dir, MKDIR_ERR);
        }
    }
} /* End dir_create */




int dir_delete(char *directory)
{
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();
    char *file_ptr;
    int len;

    DBG("directory='%s'", directory);

    /* If directory null string, return immediately */
    /* Copy directory into file_name */
    if ((len = strcpy2(bf.file_name, directory, "/")) < 2)
    {
	    data_log(tr.log_file, invalid_dir_name);
	    return(INVALID_DIR_NAME);
    }

    /* Open directory to be deleted */
    if ((dp = opendir(directory)) == NULL)
    {
	    /* If this is really a file, just delete it */
	    if (unlink(directory) == 0)
	        return(SUCCESS);
	    else
	    {
	        log3(invalid_dir_open, directory, "'");
	        return(INVALID_DIR_OPEN);
	    } /* End else */
    }

    /* Point file_ptr at end of directory name */
    file_ptr = bf.file_name + len;

    /* Delete all files in the directory */
    for (dir=readdir(dp); dir != NULL; dir=readdir(dp))
    {
	    if ((dir->d_name[0] == '.') && ((dir->d_name[1] == '.') || (dir->d_name[1] == 0))) continue;

	    /* Build the file name to be deleted */
	    strcpy(file_ptr, dir->d_name);
	    if (unlink(bf.file_name) != SUCCESS)
	    {
	        log3(invalid_unlink, bf.file_name, "'");
	        return(INVALID_UNLINK);
	    } /* end if for unsuccessful delete */
    } /* end for statement */

    closedir(dp);

    /* Now delete the directory */
    if (rmdir(directory) != SUCCESS)
    {
	    log3(invalid_rmdir, directory, "'");
	    return(INVALID_RMDIR);
    }

    /* else if everything has been successful, return SUCCESS to client */
    return(SUCCESS);
} /* end dir_delete */


int dir_recursive_dlt(char *directory)
{
    char file_name[PATH_SIZE];
    struct stat buf;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();
    char *file_ptr;
    int len;

    DBG("directory='%s'", directory);

    /* If directory null string, return immediately */
    /* Copy directory into file_name */
    if ((len = strcpy2(file_name, directory, "/")) < 2)
    {
	    data_log(tr.log_file, invalid_dir_name);
	    rtn.len += strcpy1(rtn.msg+rtn.len, invalid_dir_name);

	    return(INVALID_DIR_NAME);
    }

    /* Open directory to be deleted */
    if ((dp = opendir(directory)) == NULL)
    {
	    /* If this is really a file, just delete it */
	    if (unlink(directory) == 0)
	        return(SUCCESS);
	    else
	    {
	        log3(invalid_dir_open, directory, "'");
	        rtn.len += strcpy3(rtn.msg+rtn.len, invalid_dir_open, directory, "'");
	        return(INVALID_DIR_OPEN);
	    } /* End else */
    }

    /* Point file_ptr at end of directory name */
    file_ptr = file_name + len;

    /* Delete all files in the directory */
    for (dir=readdir(dp); dir != NULL; dir=readdir(dp))
    {
	    if ((dir->d_name[0] == '.') && ((dir->d_name[1] == '.') || (dir->d_name[1] == 0))) continue;

	    /* Build the file name to be deleted */
	    strcpy(file_ptr, dir->d_name);

	    if (((stat(file_name, &buf)) == 0) && (S_ISDIR(buf.st_mode)))
	        dir_recursive_dlt(file_name);
	    else if (unlink(file_name) != SUCCESS)
	    {
	        log3(invalid_unlink, file_name, "'");
	        rtn.len += strcpy3(rtn.msg ,invalid_unlink, file_name, "'");
	    } /* end if for unsuccessful delete */
    } /* end for statement */

    closedir(dp);

    /* Now delete the directory */
    if (rmdir(directory) != SUCCESS)
    {
	    log3(invalid_rmdir, directory, "'");
	    rtn.len += strcpy3(rtn.msg+rtn.len, invalid_rmdir, directory, "'");
	    return(INVALID_RMDIR);
    }
    /* else if everything has been successful, return SUCCESS to client */
    return(SUCCESS);
} /* end dir_recursive_dlt */


/* Remove specified directory */
/* If delete fails, client sent error message */
int dir_remove(char *directory)
{
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();
    char *file_ptr;
    int len;

    DBG("directory='%s'", directory);

    /* If directory null string, return immediately */
    if ((len = strlen(directory)) < 1)
        shut_down(bad_dir_name, BAD_DIR_NAME);

    /* Open directory to be deleted */
    if ((dp = opendir(directory)) == NULL)
    {
	    /* If this is really a file, just delete it */
	    if (unlink(directory) == 0)	return(SUCCESS);
	    else
	    {
	        close_down2d(open_dir_err, errno, directory, OPEN_DIR_ERR);
	    } /* End else */
    }

    directory[len] = '/';

    /* Point file_ptr at end of directory name */
    file_ptr = directory + len+1;

    /* Delete all files in the directory */
    for (dir=readdir(dp); dir != NULL; dir=readdir(dp))
    {
	    if ((dir->d_name[0] == '.') && ((dir->d_name[1] == '.') || (dir->d_name[1] == 0))) continue;

	    /* Build the file name to be deleted */
	    strcpy(file_ptr, dir->d_name);

	    if (unlink(directory) != SUCCESS)
	    {
	        sprintf(rtn.msg, dlt_dir_err, errno, directory);
	        shut_down(rtn.msg, DLT_ERR);
	    } /* end if for unsuccessful delete */
    } /* end for statement */

    closedir(dp);

    /* Restore directory name to former value */
    directory[len] = '\0';

    /* Now delete the directory */
    if (rmdir(directory) != SUCCESS)
    {
	    /* Try renaming the directory */
	    /* Build temporary directory name */
	    strcpy2(bf.dir_name, directory, ".b");
	    if (rename(directory, bf.dir_name) == SUCCESS)
	    {
	        sprintf(rtn.msg, dlt_dir_cau, directory, bf.dir_name);
	        data_log(tr.log_file, rtn.msg);
	        return(SUCCESS);
	    } else {
	        sprintf(rtn.msg, dlt_dir_err, errno, directory);
	        shut_down(rtn.msg, DLT_DIR_ERR);
	    } /* End else */
    }

    /* If able to delete the directory, log a caution message */
    /* indicating an unexpected directory was deleted */
    log3(extra_dir, directory, "'");

    /* Tell calling function, deletion occurred successfully */
    return(SUCCESS);
} /* end dir_remove */


void dir_rename(
    char *dir1,	/* Old directory */
    char *dir2)	/* New directory */
{
    DBG("dir1='%s' and dir2='%s'", dir1, dir2);

    DB_FILES files;

    if (rename(dir1, dir2) != SUCCESS)
    {
	    dir_delete(dir2);
	    if (rename(dir1, dir2) != SUCCESS)
	    {
	        if (errno == EXDEV)
	        {
		        /* Make list of files in directory */
		        /* First copy in directory name needed by copy function */
		        strcpy(files.dir, dir1);
		        files_list(&files);

		        /* Copy in new directory name */
		        strcpy(files.dir, dir2);

		        /* Copy the files to the new directory */
		        files_copy(&files, dir1);

		        /* Delete the old directory */
		        dir_remove(dir1);
	        } /* End if for copying directory across file systems */
	        else
	            close_down2q(rename_err, dir1, dir2, RENAME_ERR);
	    } /* End if for rename unsuccessful */
    } /* End if for first attempt unsuccessful  */
} /* End dir_rename */


void dir_recursive_rename(
    char *dir1,	/* Old directory */
    char *dir2)	/* New directory */
{
    DBG("dir1='%s' and dir2='%s'", dir1, dir2);

    DB_FILES files;

    if (rename(dir1, dir2) != SUCCESS)
    {
	    dir_recursive_dlt(dir2);
	    if (rename(dir1, dir2) != SUCCESS)
	    {
	        if (errno == EXDEV)
	        {
		        /* Make list of files in directory */
		        /* First copy in directory name needed by copy function */
		        strcpy(files.dir, dir1);
		        files_list(&files);

		        /* Copy in new directory name */
		        strcpy(files.dir, dir2);

		        /* Copy the files to the new directory */
		        files_copy(&files, dir1);

		        /* Delete the old directory */
		        dir_recursive_dlt(dir1);
	    } /* End if for copying directory across file systems */
	    else
	        close_down2q(rename_err, dir1, dir2, RENAME_ERR);
	    } /* End if for rename unsuccessful */
    } /* End if for first attempt unsuccessful  */
} /* End dir_recursive_rename */


/* Make list of files in directory except for those beginning with '.' */
int dir_list(char *directory, char *files[])
{
    int cnt = 0;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG();

    if ((dp = opendir(directory)) == NULL)
#ifdef OPCTRANS
    {
	    files[0] = NULL;
	    return(0);
     }
#else
#ifdef M2
    {
	    files[0] = NULL;
	    return(0);
     }
#else
        close_down2d(open_dir_err, errno, directory, OPEN_DIR_ERR);
#endif
#endif

    for (dir=readdir(dp);  dir != NULL; dir=readdir(dp))
    {
	    /* If file name begins with '.', do not put it on the list */
	    if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0)) continue;

	    files[cnt] = malloc(strlen(dir->d_name) +1);
	    strcpy(files[cnt++], dir->d_name);
    } /* end for */

    closedir(dp);

    /* Poiht last element at 0 */
    files[cnt] = NULL;

    /* Return the number of files found */
    return(cnt);
} /* end dir_list */


int dir_digit_list(char *directory, char *files[])
{
    int cnt = 0;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG();

    if ((dp = opendir(directory)) == NULL)
    {
	    files[0] = NULL;
	    return(0);
    }

    for (dir=readdir(dp);  dir != NULL; dir=readdir(dp))
    {
	    /* If file name begins with '.', do not put it on the list */
	    if ((strcmp(dir->d_name, ".") == 0)	|| (strcmp(dir->d_name, "..") == 0)	|| (str_digits(dir->d_name) != SUCCESS))
	        continue;

	    files[cnt] = malloc(strlen(dir->d_name) +1);
	    strcpy(files[cnt++], dir->d_name);
    } /* end for */

    closedir(dp);

    /* Poiht last element at 0 */
    files[cnt] = NULL;

    /* Return the number of files found */
    return(cnt);
} /* end dir_list */


/* Makes new directory and copies files from old directory to new directory */
/* Makes list of files in source directory */
/* and copies files to destination directory */
/* List of files copied returned to calling program */
/* The directory name passed in must not have an ending slash */
int dir_copy(char *new_dir, char *old_dir, char *files[])
{
    char *old_ptr;
    char *new_ptr;
    char new_file[FILE_SIZE];
    char old_file[FILE_SIZE];
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();
    int cnt = 0;

    DBG();

    if ((dp = opendir(old_dir)) == NULL)
        close_down2d(open_dir_err, errno, old_dir, OPEN_DIR_ERR);

    /* Create the new directory */
    dir_create(new_dir);

    /* Build path for old file */
    old_ptr = old_file + strcpy2(old_file, old_dir, "/");

    /* Build path for new directory */
    new_ptr = new_file + strcpy2(new_file, new_dir, "/");

    for (dir=readdir(dp) ; dir != NULL; dir=readdir(dp))
    {
	    /* If this is a dot file, check for ones not to copy */
	    if (dir->d_name[0] == '.')
	    {
	        if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0) || (strcmp(dir->d_name, ".db") == 0) || (strcmp(dir->d_name, ".dir") == 0))
	            continue;
	    } /* End if for dot file */

	    /* Build complete names for file copy */
	    strcpy(old_ptr, dir->d_name);
	    strcpy(new_ptr, dir->d_name);

	    file_copy(old_file, new_file);

	    /* Put file name on list */
	    /* Allocate space for file */
	    files[cnt] = malloc(strlen(dir->d_name) +1);
	    strcpy(files[cnt++], dir->d_name);
    } /* end for */

    /* Point end of list to NULL */
    files[cnt] = NULL;

    closedir(dp);

    return(cnt);
} /* end dir_copy */


/* Used to make directories during server initialization */
void dir_make(char *dir)
{
    DBG("dir='%s'", dir);

    if (access(dir, 0) != SUCCESS)
    {
        if (mkdir(dir, 0700) != SUCCESS)
        {
            sprintf(rtn.msg, mkdir_err, errno, dir);
	        printf("%s\n\n", rtn.msg);
            exit(1);
        } /* End if for unable to create directory */

    } /* End if for directory not present */
} /* end dir_make */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
