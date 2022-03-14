/****************************************************************************
 *
 * Functions:	mask_file functions
 *
 * Description:	Operates on mask files.
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
#include <alloca.h>
#include <string.h>
#include <unistd.h>
#include "dir.h"
#include "close_down.h"
#include "files.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "m2mask_files.h"
#include "m2req_types.h"
#include "m2server_arrays.h"
#include "m2server_globals.h"
#include "mti.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"

int tmp_data_log(char *log_file, char *string)
{
    FILE *log_ptr;

    if ((log_ptr = fopen(log_file, "a")) == NULL)
    {
	printf("Could not log message '%s' in file '%s'\n", string, log_file);
	return(FAILURE);
    }
    fprintf(log_ptr, "%s\n", string);
    fflush(log_ptr);
    fclose(log_ptr);
    return(SUCCESS);
} /* End data_log */
/* Makes a list of subdirectories with version equal to latest version of data from state file */
int subdir_version_list(char *directory, char *dirlist[])
{
    int mask_version_check(char *dir, float version);
    float version_num;
    struct dirent *dir, *readdir();
    char delta_file[FILE_SIZE];
    char *delta_ptr = delta_file + strcpy2(delta_file, mti.delta_dir, "/.dirlist/");
    DIR *dp;
    DIR *opendir();
    int dirlist_cnt = 0;
    char dirlist_name[FILE_SIZE];

    /* Now read in a list of directories */
    /* Build directory name */
    strcpy2(dirlist_name, directory,  "/.dirlist");
    /* Try opening the directory */
    /* If cannot open the directory, simply return */
    if ((dp = opendir(dirlist_name)) == NULL)
    {
	dirlist[0] = NULL;
	return(0);
    }
    /* Set the version number */
    if (mask_msg.req_type == RETRIEVE) version_num = st.app_version;
    else version_num = st.vnum;
    for (dir=readdir(dp);  dir != NULL; dir=readdir(dp)) 
    {
	if ((strcmp(dir->d_name, ".") == 0)
	|| (strcmp(dir->d_name, "..") == 0)) continue;
	/* Check for right version */
	strcpy(delta_ptr, dir->d_name);
	if (mask_version_check(delta_file, version_num) !=  SUCCESS) continue;
	    dirlist[dirlist_cnt] = malloc(strlen(dir->d_name)+1);
	    strcpy(dirlist[dirlist_cnt++], dir->d_name);
    }
    dirlist[dirlist_cnt] = NULL;
    return(dirlist_cnt);
} /* End subdir_version_list */

int subdir_prior_version_list(char *directory, char *dirlist[], char *version)
{
    struct dirent *dir, *readdir();
    char delta_file[FILE_SIZE];
    char *delta_ptr = delta_file + strcpy2(delta_file, mti.delta_dir, "/.dirlist/");
    DIR *dp;
    DIR *opendir();
    int dirlist_cnt = 0;
    char dirlist_name[FILE_SIZE];

    /* Now read in a list of directories */
    /* Build directory name */
    strcpy2(dirlist_name, directory,  "/.dirlist");
    /* Try opening the directory */
    /* If cannot open the directory, simply return */
    if ((dp = opendir(dirlist_name)) == NULL)
    {
	dirlist[0] = NULL;
	return(0);
    }
    for (dir=readdir(dp);  dir != NULL; dir=readdir(dp)) 
    {
	if ((strcmp(dir->d_name, ".") == 0)
	|| (strcmp(dir->d_name, "..") == 0)) continue;
	/* Check for right version */
	strcpy(delta_ptr, dir->d_name);
    if (mask_prior_version_read(delta_file, mask_msg.par1) == SUCCESS)
	{
	    dirlist[dirlist_cnt] = malloc(strlen(dir->d_name)+1);
	    strcpy(dirlist[dirlist_cnt++], dir->d_name);
	} /* End if for version found */
    } /* End for */
    dirlist[dirlist_cnt] = NULL;
    return(dirlist_cnt);
} /* End subdir_prior_version_list */
/* Send subdirectories to server */
void subdir_send(char *dirlist[])
{
    int cnt;
    char *files[M2TOTAL_FILES];
    char *dir_ptr = mti.data_dir+ strlen(mti.data_dir);
    char *delta_dir_ptr = mti.delta_dir + strlen(mti.delta_dir);
    /* Send other directories if present */
    for (cnt = 0; dirlist[cnt] != NULL; ++cnt)
    {
	put_string(dirlist[cnt]);
	/* Build full directory path */
	strcpy2(dir_ptr, "/.dir/", dirlist[cnt]);
	/* Build full delta directory path */
	strcpy2(delta_dir_ptr, "/.dir/", dirlist[cnt]);
	/* Make a list of files */
	mask_files_version_list(mti.data_dir, files, mti.delta_dir, DBNO);
	/* Send the files */
	files_send(mti.data_dir, files);
	}
    /* Tell client all directories sent */
    put_short(SUCCESS);
    /* Restore delta directory */
    delta_dir_ptr[0] = '\0';
} /* End subdir_send */

/* Get files in other directories */
/* Returns list of directories and files retrieves */
/* offset holds offset for each file list returned */
/* The directory where the files are placed is initialized before the function is called */
int subdir_get(char *dir_name, char *dirlist[], char *files[], int file_offset[])
{
    int offset = 0;
    int cnt;
    int ret;
    char *dir_ptr = dir_name+strlen(dir_name);

    for (cnt = 0; ((ret = get_string(dir_ptr)) > 0); ++cnt)
    {
	/* Get the files in this directory */
	/* Copy name into dirlist array */
	dirlist[cnt] = malloc(strlen(dir_ptr) + 1);
	strcpy(dirlist[cnt], dir_ptr);
	/* Point to first name on list */
    file_offset[cnt] = offset;
	offset += 1 + files_rcv(dir_name, files+offset);
    } /* End for */
    dirlist[cnt] = NULL;
    if (ret == 0) return(cnt);
    else return(cnt);
} /* End subdir_get */

/* Version the directory list */
int subdir_version(char *dirlist[], char *files[], char *new_data_dir, char *new_delta_dir, int file_offset[])
{
    char tmp_file [FILE_SIZE]; 
    int local_offset;
    char old_data_dir[FILE_SIZE];
    char *old_data_ptr = old_data_dir + strcpy2(old_data_dir, mti.data_dir, "/.dir/");
    char *new_data_ptr = new_data_dir + strlen(new_data_dir);
    char old_delta_dir[FILE_SIZE];
    char *old_delta_ptr = old_delta_dir + strcpy2(old_delta_dir, mti.delta_dir, "/.dir/");
	char *new_delta_ptr = new_delta_dir +  strlen(new_delta_dir);
    char version_string[260];
    int cnt;
    char dirlist_file[FILE_SIZE];
    char *dirlist_ptr = dirlist_file + strcpy2(dirlist_file, mti.delta_dir, "/.dirlist/");

    /* Need to create a unique name for the tmp file */
    strcpy2(tmp_file, new_data_dir, "/tmp_file");
    /* Need to put the / part on new data_dir and new_delta_dir */
    new_data_ptr += strcpy1(new_data_ptr, "/");
    new_delta_ptr += strcpy1(new_delta_ptr, "/");
    /* Build the version string */
    sprintf(version_string, "Version %2.3f %s %s %s",
    st.vnum, user.login, req_array[mask_msg.req_type], log_date);
/* Go through each directory name and add to the appropriate file */
    for (cnt = 0; dirlist[cnt] != NULL; ++cnt)
    {
	/* Build file to version */
	strcpy(dirlist_ptr, dirlist[cnt]);
	if (access(dirlist_file, 0) != 0) data_log(dirlist_file, version_string);
	else
	{
	    /* Write version string to tmp file */
	    data_log(tmp_file, version_string);
	    /* Append contents of original file */
	    file_append(dirlist_file,  tmp_file);
	    /* Move the file back to its original place */
	    file_rename(tmp_file, dirlist_file);
	} /* End else */
	/* Just got through versioning the directory name */
	/* Now need to version the files within each directory */
	/* Call function to perform the versioning */
	strcpy(new_delta_ptr, dirlist[cnt]);
	/* Create this directory */
	dir_create(new_delta_dir);
	strcpy(old_delta_ptr, dirlist[cnt]);
	strcpy(old_data_ptr, dirlist[cnt]);
	strcpy(new_data_ptr, dirlist[cnt]);
    local_offset = file_offset[cnt];
	/* Make final directories if not laready present */
	if (access(old_data_dir, 0) != 0)
	dir_create(old_data_dir);
	if (access(old_delta_dir, 0) != 0)
	dir_create(old_delta_dir);
	mask_files_version(new_data_dir, old_data_dir, new_delta_dir, old_delta_dir, files+local_offset);
    } /* End for */
} /* End subdir_version */
