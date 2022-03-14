#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	disk_usage
 *
 * Description:	Calculates size of specified directory tree
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef WINDOWS_NT4
#include <alloca.h>
#endif
#include <dirent.h>
#include <sys/stat.h>
#include <strings.h>
#include <string.h>
#include "global_defines.h"
#include "client_disk_usage.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "transapi_hidden_prototypes.h"

/* The directory name passed in must not have an ending slash */
unsigned long long ldisk_usage(char *directory, struct DIR_INFO *dir_ptr, int local_dir_cnt)
{
    extern long long global_compressed_size;
    extern int global_dir_offset;
    extern char tmp_transdirlist[];
    extern int global_file_cnt;
    extern int global_dir_cnt;
    extern int log_files;
    float ratio;
    char file_name[PATH_SIZE];
    char string[FILE_SIZE];
    long long file_size;
    struct stat buf;
    DIR *dp;
    struct dirent *dir; 

    if ((dp = opendir(directory)) == NULL)
    return(512);
	if (log_files == DBYES)
    ++local_dir_cnt;
    if (local_dir_cnt > global_dir_cnt) global_dir_cnt = local_dir_cnt;
    for (dir=readdir(dp), file_size = 0; dir != NULL; dir=readdir(dp)) 
    {
    /* Ignore '..' file */
	if (strcmp(dir->d_name, "..") == 0) continue;
	/* Build complete file name */
	strcpy3(file_name, directory, "/", dir->d_name);
	if (stat(file_name, &buf) != SUCCESS) continue;
	/* If this is current directory or a file simply add size */
	if ((strcmp(dir->d_name, ".") == 0) || (S_ISDIR(buf.st_mode) < 1))
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
	if ((log_files == DBYES)
	&& (strcmp(dir->d_name, ".") != 0))
    {
	sprintf(string, "%s %lld", file_name+global_dir_offset, buf.st_size);
	trans_data_log(tmp_transdirlist, string);
	++global_file_cnt;
    } /* end if */
#ifdef NXP 
#ifdef INTERCOM
	if (strcmp(dir->d_name, ".") != 0)
	trans_maskshop_file_log(file_name, &buf, DBNO);
#endif
#endif
	} /* end if */
	/* Otherwise this is a directory */
	else
	{
	    /* See if this directory has been traversed */
	    if (dir_info_traverse(dir_ptr, buf.st_ino, buf.st_dev) == DBNO)
	    {
		dir_ptr = dir_info_add(dir_ptr, buf.st_ino, buf.st_dev);
		file_size += ldisk_usage(file_name, dir_ptr, local_dir_cnt);
	    } /* End if for not traversed */
	} /* End else */
    } /* End for */
    closedir(dp);
    /* Return total file size */
    return(file_size);
} /* enld disk_usage */

/* Adds new entry to directory info */
struct DIR_INFO *dir_info_add(struct DIR_INFO *dir_info, long st_ino, long st_dev)
{
    struct DIR_INFO *dir_ptr;

    /* Allocate a new structure */
    dir_ptr = malloc(sizeof(struct DIR_INFO));
    dir_ptr->st_ino = st_ino;
    dir_ptr->st_dev = st_dev;
    dir_ptr->next = dir_info;
    return(dir_ptr);
} /* End dir_info_add */

/* Traverse directory info */
int dir_info_traverse(struct DIR_INFO *dir_info, long st_ino, long st_dev)
{
    struct DIR_INFO *dir_ptr = dir_info;

    while (1)
    {
	if ((dir_ptr->st_ino == st_ino) && (dir_ptr->st_dev == st_dev)) 
	return(DBYES);
	/* If have traversed everything, return failure */
	if (dir_ptr->next == NULL) return(DBNO);
	/* Point to the next node */
	dir_ptr = dir_ptr->next;
    } /* End while */
} /* End dir_info_traverse */
#endif /* For not TRANSAPI */
