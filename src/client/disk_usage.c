#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	big_disk_usage
 *
 * Description:	Calculates size of specified directory tree
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
#include "client_disk_usage.h"
#include "global_defines.h"
#include "list.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "user_params.h"
#ifndef __USE_BSD
#define __USE_BSD
#endif


/* The directory name passed in must not have an ending slash */

long long big_disk_usage(char *directory, int local_dir_cnt)
{
    extern int global_dir_cnt;
    extern int log_files;
    char file_name[4096];
    long long file_size;
    struct stat buf;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    if ((dp = opendir(directory)) == NULL)
    return(1024);
    /* Increment number of directory levels */
	if (log_files == DBYES)
    ++local_dir_cnt;
    if (local_dir_cnt > global_dir_cnt) global_dir_cnt = local_dir_cnt;
    for (dir=readdir(dp), file_size = 0; dir != NULL; dir=readdir(dp)) 
    {
    /* Ignore '..' file */
	if (strcmp(dir->d_name, "..") == 0)continue;
	/* Build complete file name */
	strcpy3(file_name, directory, "/", dir->d_name);
	if (stat(file_name, &buf) != 0)  continue;
	if ((strcmp(dir->d_name, ".") == 0)
	|| ((S_ISDIR(buf.st_mode)) < 1))
	{
	    if (buf.st_size < 1024) file_size += 1024;
	    else file_size += buf.st_size;
	    /* If sending data to mask shop, append to log */
#ifndef WINDOWS_NT4
/* Creation of dirlist file not supported for windows */
    trans_dirlist_log(file_name, dir->d_name, buf.st_size);
#ifdef INTERCOM
#ifdef NXP 
	if (strcmp(dir->d_name, ".") != 0)
	trans_maskshop_file_log(file_name, &buf, DBNO);
#endif
#endif
#endif
	} /* End if */
	/* Otherwise find out if directory is a link */
	/* Links are not followed */
	else
	{
	    if (lstat(file_name, &buf) != 0) continue;
	    /* If this is a link, ignore it */
	    if (S_ISLNK(buf.st_mode)) continue;
	    else file_size += big_disk_usage(file_name, local_dir_cnt);
	} /* End else */
    } /* End for */
    /* Return total file size */
    closedir(dp);
    return(file_size);
} /* end big_disk_usage */

/* Calculate directory size */
/* Also calculate estimated compressed size */
long long big_random_disk_usage(char *directory, int local_dir_cnt)
{
    extern unsigned long long global_compressed_size;
    extern int global_dir_cnt;
    extern int log_files;
    char file_name[4096];
    unsigned long long file_size;
    struct stat buf;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();
    float ratio;

    if ((dp = opendir(directory)) == NULL)
    return(4096);
    /* Increment number of directory levels */
    /* Put the extensions in list format */
	if (log_files == DBYES)
    ++local_dir_cnt;
    if (local_dir_cnt > global_dir_cnt) global_dir_cnt = local_dir_cnt;
    for (dir=readdir(dp), file_size = 0; dir != NULL; dir=readdir(dp)) 
    {
    /* Ignore '..' file */
	if (strcmp(dir->d_name, "..") == 0)continue;
	/* Build complete file name */
	strcpy3(file_name, directory, "/", dir->d_name);
	if (lstat(file_name, &buf) != 0)  continue;
	/* Do not follow file or directory links */
	if (S_ISLNK(buf.st_mode)) continue;
	if ((strcmp(dir->d_name, ".") == 0)
	|| ((S_ISDIR(buf.st_mode)) < 1))
	{
	ratio = list_ratio_find(file_name);
	    if (buf.st_size < 1024) 
	{
	    file_size += 1024;
	    global_compressed_size += (1024*ratio);
	} else {
	        file_size += buf.st_size;
	    global_compressed_size += (buf.st_size * ratio);
	} /* End else */
	    /* If sending data to mask shop, append to log */
    trans_dirlist_log(file_name, dir->d_name, buf.st_size);
#ifdef INTERCOM
#ifdef NXP 
	if (strcmp(dir->d_name, ".") != 0)
	trans_maskshop_file_log(file_name, &buf, DBNO);
#endif
#endif
	} /* End if */
	/* Otherwise find out if directory is a link */
	/* Links are not followed */
	else file_size += big_random_disk_usage(file_name, local_dir_cnt);
    } /* End for */
    /* Return total file size */
    closedir(dp);
    return(file_size);
} /* end big_random_disk_usage */

unsigned long long trans_random_disk_usage(char *directory)
{
    extern unsigned long long global_compressed_size;
    extern int global_dir_cnt;
    extern int log_files;
     int dir_cnt = 0;
    int new_dir_cnt;
    char file_name[4096];
    char command[FILE_SIZE];
    char line[FILE_SIZE];
    unsigned long long file_size = 0;
    int offset = strlen(user.cwd)+1;
    struct stat buf;
    FILE *po;

#ifdef SOLARIS
    strcpy2(command, "du -a -k ", directory);
#else
    strcpy2(command, "du -a -b ", directory);
#endif
    po = popen(command, "r");
    global_compressed_size = 0;
    while (fscanf(po, "%*s %s", file_name) != EOF)
    {
	if (lstat(file_name, &buf) != 0)  continue;
	/* Do not follow file or directory links */
	if (S_ISLNK(buf.st_mode)) continue;
	    file_size += buf.st_size;
	/* Count the number of directory levels */
	if ((new_dir_cnt = str_numchar(file_name, '/')) > dir_cnt)
	dir_cnt = new_dir_cnt;
	/* If this is a a directory, do not log it */
	if (S_ISDIR(buf.st_mode))  continue;
    trans_dirlist_log(file_name+offset, buf.st_size);
	/* If sending data to mask shop, append to log */
#ifdef INTERCOM
#ifdef NXP
	trans_maskshop_file_log(file_name, &buf, DBNO);
#endif
#endif
    } /* End while */
    /* Close the pipe command */
    pclose(po);
    /* Set the global directory count */
    global_dir_cnt = dir_cnt - 1 - str_numchar(user.cwd, '/');
    /* Return total file size */
    return(file_size);
} /* end trans_random_disk_usage */
#endif /* For not TRANSAPI */
