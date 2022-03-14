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

/* Version the new file being created during append */
void new_append_file_version(char *local_file)
{
    char file_name[FILE_SIZE];
    char string[80];

    sprintf(string, "Version %2.3f %s %s %s",
    st.vnum+.001, user.login, req_array[mask_msg.req_type], log_date);
    /* build name of file to append to */
    strcpy3(file_name, mti.delta_dir, "/", local_file);
    data_log(file_name, string);
} /* End new_append_file_version */

void mask_filelist_add(int level, char *name, char *filelist[], int program[])
{
    int ret, cnt, cnt1, cnt2, total_cnt;
    FILE *fp;
    char file_name[FILE_SIZE];

    /* go through array until correct place to put file found */
    for (cnt = 0; filelist[cnt] != NULL; ++cnt)
    {
	if ((ret = strcmp(name, filelist[cnt])) < 0) break;
	if (ret == 0) return;
    }
    /* At this point cnt is pointing at place to put the file */
    /* Find out how many elements are in the array */
    for (total_cnt = cnt; filelist[total_cnt] != NULL; ++total_cnt);
    /* Now move everything down until reach place where new file will be placed */
    for (cnt1 = total_cnt,cnt2 = total_cnt-1; cnt1 > cnt; --cnt1, --cnt2)
    {
	filelist[cnt1] = filelist[cnt2];
	program[cnt1] = program[cnt2];
    }
    /* now have an opening for the new name */
    filelist[cnt] = malloc(strlen(name)+1);
    strcpy(filelist[cnt], name);
    program[cnt] = mask_msg.program;
    /* Point the new last element at NULL */
    filelist[total_cnt+1] = NULL;
    /* Update the file on disk */
    /* Build the name for the file */
    if (level == 0)
    strcpy2(file_name, mti.delta_dir, "/local_filelist");
    else strcpy2(file_name, mti.delta_dir, "/.db/local_filelist");
    if ((fp = fopen(file_name, "w")) == NULL)
    close_down1m(wr_open_err, file_name, WR_OPEN_ERR);
    /* Write out changes to the file */
    for (cnt = 0; filelist[cnt] != NULL; ++cnt)
    {
    fprintf(fp, "%d %s\n", program[cnt], filelist[cnt]);
    }
    fclose(fp);
} /* End mask_filelist_add */

int mask_file_version_same(char *file, float version)
{
    float local_version;
    FILE *fp;

    /* First check to see if this tool owns the data */
    if ((fp = fopen(file, "r")) == NULL) return(DBNO);
    if (fscanf(fp, "%*s %f", &local_version) != 1) 
    {
	fclose(fp);
	return(DBNO);
    }
    fclose(fp);
    if (local_version == version) return(DBYES);
    else return(DBNO);
} /* mask_file_version_same */

/* Read in latest version number from delta file */
float mask_version_read(char *delta_file)
{
    FILE *fp;
    float version;

    if ((fp = fopen(delta_file, "r")) == NULL) return(0);
    if (fscanf(fp, "%*s %f", &version) != 1) 
    version = 0;
    fclose(fp);
    return(version);
} /* End mask_version_read */

/* See if this data was checked in at specified version number */
int mask_version_check(char *delta_file, float version)
{
    int fnd = FAILURE;
    char line[260];
    FILE *fp;
    float local_version;

    if ((fp = fopen(delta_file, "r")) == NULL) return(FAILURE);
    while (line_get(fp, line) != EOF)
    {
    if (sscanf(line, "%*s %f", &local_version) != 1)  continue;
	if (version == local_version)
	{
	fnd = SUCCESS;
	break;
	}
    } /* End while */
    fclose(fp);
    return(fnd);
} /* End mask_version_read */

int mask_prior_version_read(char *delta_file, char *version)
{
    char line[250];
    char local_version[8];
    FILE *fp;

    if ((fp = fopen(delta_file, "r")) == NULL) return(0);
    while (line_get(fp, line) != EOF)
    {
	if (sscanf(line, "%*s %s", local_version) != 1) continue;
	    if (strcmp(local_version, version) == 0)
    {
	fclose(fp);
	return(SUCCESS);
	} /* End if for version found */
    } /* End for */
    fclose(fp);
    /* If reach this point, version not found */
    return(FAILURE);
} /* End mask_prior_version_read */

/* Does not include . .. .fab, .db or .tmpdb in directory list */
/* Returns the number of files found */
int mask_files_list(char *directory, char *files[])
{
    int cnt = 0;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();
    char full_file_name[FILE_SIZE];

    if ((dp = opendir(directory)) == NULL)
    close_down2d(open_dir_err, errno, directory, OPEN_DIR_ERR);
    for (dir=readdir(dp);  dir != NULL; dir=readdir(dp)) 
    {
	/* If file is '.' '..' '.db' or '.tmpdb' skip it */
	if ((strcmp(dir->d_name, ".") == 0)
	|| (strcmp(dir->d_name, "..") == 0)
	|| (strcmp(dir->d_name, ".tmpdb") == 0)
	|| (strcmp(dir->d_name, ".db") == 0)
	|| (strcmp(dir->d_name, ".fab") == 0)
	|| (strcmp(dir->d_name, ".dir") == 0)) continue;
	if (strcmp(dir->d_name, "core") == 0)
	{
	    /* Delete the core file */
	    strcpy2(full_file_name, directory, "/core");
	    unlink(full_file_name);
	    continue;
    }
	files[cnt] = malloc(strlen(dir->d_name)+1);
	strcpy(files[cnt++], dir->d_name);
    } /* end for */
    closedir(dp);
    /* Poiht last element at 0 */
    files[cnt] = NULL;
    /* Return the number of files found */
    return(cnt);
} /* end mask_files_list */

int mask_files_version_list(char *directory, char *files[], char *delta_dir, int block_history)
{
    int cnt = 0;
    DIR *dp;
    DIR *opendir();
    char full_file_name[FILE_SIZE];
    char delta_file[FILE_SIZE];
     char *delta_ptr = delta_file + strcpy2(delta_file, delta_dir, "/");
    struct dirent *dir, *readdir();

    if ((dp = opendir(directory)) == NULL)
    close_down2d(open_dir_err, errno, directory, OPEN_DIR_ERR);
    for (dir=readdir(dp);  dir != NULL; dir=readdir(dp)) 
    {
	/* If file is '.' '..' '.db' or '.tmpdb' skip it */
	if ((strcmp(dir->d_name, ".") == 0)
	|| (strcmp(dir->d_name, "..") == 0)
	|| (strcmp(dir->d_name, ".tmpdb") == 0)
	|| (strcmp(dir->d_name, ".db") == 0)
	|| (strcmp(dir->d_name, ".fab") == 0)
	|| (strcmp(dir->d_name, ".dir") == 0)) continue;
	if ((strcmp(dir->d_name, "core") == 0)
	|| (strcmp(dir->d_name, "query") == 0)
	|| (strncmp(dir->d_name, ".dbErrors.09", 12) == 0))
	{
	    /* Delete the file */
	    strcpy2(full_file_name, directory, dir->d_name);
	    unlink(full_file_name);
	    continue;
    }
	if ((block_history == DBYES)
	&& (strcmp(dir->d_name, "History") == 0)) continue;
	strcpy(delta_ptr, dir->d_name);
	/* Make sure this file has the latest version */
	/* Does not need latest version if it is the rb_out.gdsii file */
	if ((strcmp(dir->d_name, "rb_out.gdsii") != 0)
	&& (strcmp(dir->d_name, "create") != 0)
	&& (mask_version_read(delta_file) !=  st.vnum)) continue;
	files[cnt] = malloc(strlen(dir->d_name)+1);
	strcpy(files[cnt++], dir->d_name);
    } /* end for */
    closedir(dp);
    delta_ptr[0] = '\0';
    /* Poiht last element at 0 */
    files[cnt] = NULL;
    /* Return the number of files found */
    return(cnt);
} /* end mask_files_version_list */

/* Retrieve specified mask files */
int mask_files_rcv(char *dir, char *files[], int level)
{
    char access_list[M2TOTAL_FILES];
    int fd;
    char buffer[BUFFER_SIZE];
    char full_file_name[FILE_SIZE];
    short ret;
    int cnt = 0;
    char *file_ptr;	/* Points to end of path name */

    /* Build directory name where files will be placed */
    file_ptr = full_file_name + strcpy2(full_file_name, dir, "/");
    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0)
    dir_create(dir);
    /* Get list of files that will be sent */
    list_put(files);
    /* go through list to see which should be checked in */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* Make sure this tool can update this file */
	if (filelist_check(file_ptr, level)!= M2OWNER)
	/* Indicate this file should not be sent */
	    access_list[cnt] = DBNO;
	else access_list[cnt] = DBYES;
    } /* End while */
    /* Put null at end of access list */
    access_list[cnt] = '\0';
    /* Send list of files to retrieve to client */
    put_string(access_list);
    /* Now get the specified files */
    /* Set counter back to 0 for next loop */
  cnt = 0;
    while ((ret = get_string(file_ptr)) > 0)
    {
	/* Open file for writing */
	if ((fd = open(full_file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
	close_down1m(rd_open_err, full_file_name,  RD_OPEN_ERR);
	/* Read in blocks of text and write them out to the file */
	while((ret = get_string(buffer)) > 0)
	{
	    /* Write the text out to the file */
	    write(fd, buffer, ret);
    } /* End while */
	/* Close the file just created */
	close(fd);
	/* Allocate space for file name */
	files[cnt] = malloc(strlen(file_ptr) +1);
	/* Copy file name into file array */
	strcpy(files[cnt++], file_ptr);
    } /* end while */
    files[cnt] = NULL;
    /* Restore file name to original state */
    file_ptr[0] = '\0';
    if (ret < 0)
    {
	get_string(rtn.msg);
	close(sock);
	data_log(tr.log_file, rtn.msg);
	exit(0);
    } /* End if */
    return(cnt);
} /* end mask_files_rcv */

int mask_files_append(char *new_dir, char *old_dir)
{
    int ccc;
    int fd;
    char buffer[BUFFER_SIZE];
    char new_file_name[FILE_SIZE];
    char old_file_name[FILE_SIZE];
    short ret;
    char *old_file_ptr;	/* Points to end of path name */
    char *new_file_ptr;	/* Points to end of path name */

    /* Build directory name where files will be placed */
    new_file_ptr = new_file_name + strcpy2(new_file_name, new_dir, "/");
    old_file_ptr = old_file_name + strcpy2(old_file_name, old_dir, "/");
    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(new_dir, 0) != 0)
    dir_create(new_dir);
    /* Get the file */
    ret = get_string(new_file_ptr);
        /* Open file for create */
        fd = open(new_file_name, O_WRONLY|O_CREAT, 0600);
    if (fd == EOF)
    close_down1m(rd_open_err, new_file_name,  RD_OPEN_ERR);
    /* Tell client ready to get text */
    put_short(SUCCESS);
    /* Read in blocks of text and write them out to the file */
    while((ret = get_string(buffer)) > 0)
    {
	/* Write the text out to the file */
	write(fd, buffer, ret);
    } /* End while */
    /* Close the file just created */
    close(fd);
    if (ret < 0)
    {
	get_string(rtn.msg);
	close(sock);
	data_log(tr.log_file, rtn.msg);
	exit(0);
    } /* End if */
    /* Scan in data from this file and store in SQL database */
    if (strcmp(new_file_ptr, "send2maskshop_log") == 0)
    {
	ccc = DBNO;
	send2maskshop_log_scan(new_file_name, DBNO);
    } else {
	ccc = DBYES;
    	ccc_scan(new_file_name, mask_msg.key1);
    } /* End else */
    /* Build name for existing log file */
	strcpy(old_file_ptr, new_file_ptr);
    /* If send2maskshop_log file already present, append to new data retrieved */
	if (access(old_file_name, 0) == 0) 
	file_append( old_file_name, new_file_name);
    /* If this is the first log, build a version file for it */
	else new_append_file_version( new_file_ptr);
    /* Now move file to permanent location */
    file_copy(new_file_name, old_file_name);
    unlink(new_file_name);
    return(ccc);
} /* end mask_files_append */


/* Version new files just retrieved from client */
/* also do necessary replacements */
int mask_files_version(char *new_data_dir, char *old_data_dir, 
    char *new_delta_dir, char *old_delta_dir, char *files[])
{
    char command[PATH_SIZE];
    char string[80];
    int cnt;
    char new_data_file[FILE_SIZE];
    char old_data_file[FILE_SIZE];
    char new_delta_file[FILE_SIZE];
    char old_delta_file[FILE_SIZE];
    char *new_data_ptr;
    char *old_data_ptr;
    char *new_delta_ptr;
    char *old_delta_ptr;

    /* Build the version string */
    sprintf(string, "Version %2.3f %s %s %s",
    st.vnum, user.login, req_array[mask_msg.req_type], log_date);
    /* Set up pointers used to access the files */
    new_data_ptr = new_data_file +strcpy2(new_data_file, new_data_dir, "/");
    old_data_ptr = old_data_file +strcpy2(old_data_file, old_data_dir, "/");
    new_delta_ptr = new_delta_file +strcpy2(new_delta_file, new_delta_dir, "/");
    old_delta_ptr = old_delta_file +strcpy2(old_delta_file, old_delta_dir, "/");
    /* Go through list of files doing versioning as needed */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	strcpy(new_data_ptr, files[cnt]);
	strcpy(old_data_ptr, files[cnt]);
	strcpy(new_delta_ptr, files[cnt]);
	strcpy(old_delta_ptr, files[cnt]);
	/* First create the new delta file */
	data_log(new_delta_file, string);
	/* If old data file not present, no versioning needed */
	/* Also if this is the send2maskshop_log file, no versioning needed */
	if ((access(old_data_file, 0) == 0)
	&& (strcmp(files[cnt], "send2maskshop_log") != 0)
    && (strcmp(files[cnt], "rb_out.gdsii") != 0))
	{
	    /* Must build the deltas */
	    /* build command to create deltas */
	    strcpy6(command, "diff -e ", new_data_file, " ", 
	    old_data_file, " >>", new_delta_file);
	    /* Execute command */
	    system(command);
	} /* end if for old data file present */
	/* If old delta file present,  must append it */
	if (access(old_delta_file, 0) == 0)
	file_append(old_delta_file, new_delta_file);
	/* Now we are ready to do the replacements */
    /* do not overwrite the send2maskshop_log file */
    if (strcmp(files[cnt], "send2maskshop_log") != 0)
    {
	unlink(old_data_file);
	file_rename(new_data_file, old_data_file);
    }
	else unlink(new_data_file);
	unlink(old_delta_file);
	file_rename(new_delta_file, old_delta_file);
    } /* end for */
    /* Point first element at NULL */
    files[0] = NULL;
    return(cnt);
} /* end mask_files_version */

/* Version other files present */
void other_files_version(int level, char *delta_dir, char *filelist[], float old_version)
{
    char *files[M2TOTAL_FILES];
     char tmp_file[FILE_SIZE];
    char *delta_ptr;
    char delta_file[FILE_SIZE];
    char string[80];
    int cnt;

    /* Make list of all files in data directory */
    if (level == 0)
    mask_files_list(mti.data_dir, files);
    else mask_files_list(mti.db_data_dir, files);
    /* Build name of temporary file */
    strcpy2(tmp_file, "tmp_file", tr.pid_string);
    /* Build the version string */
    sprintf(string, "Version %2.3f %s %s %s",
    st.vnum, user.login, req_array[mask_msg.req_type], log_date);
    delta_ptr = delta_file + strcpy2(delta_file, delta_dir, "/");
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* If this file does not belong to this tool, version it */
	if ((mask_file_version_same(files[cnt] , old_version) == DBNO)
	&& (filelist_check(files[cnt], level) != M2OWNER))
	{
	    /* See if file is present  */
	    strcpy(delta_ptr, files[cnt]);
	    if (access(delta_file, 0) == 0)
	    {
		/* Create temporary file name */
		/* Log new version number */
		data_log(tmp_file, string);
		/* Append original file */
		file_append(delta_file, tmp_file);
		/* Delete original file */
		unlink(delta_file);
		/* Move new file to permanant location */
		file_rename(tmp_file, delta_file);
	    } /* End if for delta file present */
	} /* end if for owned by different program */
     free(files[cnt]);
    } /* End for */
} /* end other_files_version */

/* Moves all files with current version to the new version */
/* This function is called when mask_append is done */
void all_files_version(char *data_dir, char *delta_dir)
{
     char tmp_file[64];
    char *delta_ptr;
    char delta_file[FILE_SIZE];
    char string[80];
    char *files[M2TOTAL_FILES];
    int cnt;

    /* Make list of all files in data directory at current version  */
    mask_files_version_list(data_dir, files, delta_dir, DBNO);
    /* Build name of temporary file */
    strcpy(tmp_file, "xx");
    /* Build the version string */
    sprintf(string, "Version %2.3f %s %s %s",
    st.vnum+.001, user.login, req_array[mask_msg.req_type], log_date);
    delta_ptr = delta_file + strcpy2(delta_file, delta_dir, "/");
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    /* See if file is present  */
	    strcpy(delta_ptr, files[cnt]);
	    if (access(delta_file, 0) == 0)
	    {
		/* Create temporary file name */
		/* Log new version number */
		data_log(tmp_file, string);
		/* Append original file */
		file_append(delta_file, tmp_file);
		/* Delete original file */
		unlink(delta_file);
		/* Move new file to permanant location */
		file_rename(tmp_file, delta_file);
	    } /* End if for delta file present */
     free(files[cnt]);
    } /* End for */
} /* end all_files_version */

/* Read names of files defined for this mask set */
void other_files_read(char *files[], int program[])
{
    char file_name[FILE_SIZE];
    int cnt;
    FILE *fp;

    /* Build file name */
    strcpy2(file_name, mti.delta_dir, "/filelist");
    /* Open this file for read */
    if ((fp = fopen(file_name, "r")) == NULL)
    {
	files[0] = NULL;
	return;
    } /* End if */
    for (cnt = 0; fscanf(fp, "%s %d", file_name, &program[cnt]) == 2; ++cnt)
    {
	/* Allocate space for file */
	files[cnt] = malloc(strlen(file_name) +1);
	strcpy(files[cnt], file_name);
    } /* End for */
    files[cnt] = NULL;
    fclose(fp);
} /* End other_files_read */

/* Verify mask file is at the latest version */
int mask_file_is_latest(char *root_file)
{
    FILE *fp;
    float version_num;
    char file_name[FILE_SIZE];

    /* Build full file name */
    strcpy3(file_name, mti.delta_dir, "/", root_file);
    if ((fp = fopen(file_name, "r")) == NULL)
    return(DBNO);
    /* Scan in version number */
    if (fscanf (fp, "%*s %f", &version_num) != 1)
    return(DBNO);
    /* Close file no longer needed */
    fclose(fp);
    if (version_num == st.vnum) return(DBYES);
    else return(DBNO);
} /* End mask_file_is_latest */

/* See if present on file list */
int filelist_check(char *name, int level)
{
    int ret;

    /* If this is not mask data, do not make the check */
    if (mask_msg.data_type != MASK_SET) return(M2OWNER);
  /* Do check for level 0 or mask files */
  if (level == 0)
  {
    if ((ret = mask_filelist_check(m2filelist, m2program, name)) != M2NOT_FOUND)
    return(ret);
    else if ((ret = mask_filelist_check(local_m2filelist, local_m2program, name)) != M2NOT_FOUND)
    return(ret);
    /* If no tool owns the file, make current tool the owner */
    else 
    {
	mask_filelist_add(level, name, local_m2filelist, local_m2program);
	return(M2OWNER);
    }
  /* Make check for files in .db directory */
  } else {
	/* If this is an error file, throw it away */
/*
	if (strncmp(name, ".dbErrors.", 10) == 0) return(M2NOT_FOUND);
*/
    if ((ret = mask_filelist_check(db_filelist, db_program, name)) != M2NOT_FOUND)
    return(ret);
    else if ((ret = mask_filelist_check(local_db_filelist, local_db_program, name)) != M2NOT_FOUND)
    return(ret);
    /* If no tool owns the file, make current tool the owner */
    else 
    {
	mask_filelist_add(level, name, local_db_filelist, local_db_program);
	return(M2OWNER);
    }
  } /* End big else */
} /* End filelist_check */

/* Knowing the file list check for name in that file list */
int mask_filelist_check(char *filelist[], int program[],  char *name)
{
    int cnt;
    int ret;

    for (cnt = 0; filelist[cnt] != NULL; ++cnt)
    {
	if ((ret = strcmp(name, filelist[cnt])) > 0) continue;
	else if (ret == 0)
	{
	    if (mask_msg.program == program[cnt]) return(M2OWNER);
	    else return(M2NOT_OWNER);
	} /* End if */
	/* At this point name was not found */
	else break;
    }
    /* If reached this point, file was not found */
    return(M2NOT_FOUND);
} /* End mask_filelist_check */


/* Makes new directory and copies files from old directory to new directory */
/* Makes list of files in source directory */
/* and copies files to destination directory */
/* List of files copied returned to calling program */
/* The directory name passed in must not have an ending slash */
/* Only latest version of files copied */
int mask_dir_copy(char *new_dir, char *old_dir, char *delta_dir,  char *files[], int mod_flag)
{
    char old_name[64];
    char new_name[64];
    char *old_ptr;
    char *new_ptr;
    char new_file[PATH_SIZE];
    char old_file[PATH_SIZE];
     int cnt;
    int check_name = mod_flag;

    /* Create the new directory */
    dir_create(new_dir);
    /* Make list of files using only latest version */
    cnt = mask_files_version_list(old_dir, files, delta_dir, DBYES);
    /* Build path for old file */
    old_ptr = old_file + strcpy2(old_file, old_dir, "/");
    strcpy1(old_ptr, "scribe_plan");
    /* Build path for new directory */
    new_ptr = new_file + strcpy2(new_file, new_dir, "/");
    /* Initialize names needed for following loop */
    if (check_name == DBYES)
    {
	if (mask_msg.data_type == MASK_SET)
	{
	    /* If copying or creating data, new mask set name substituted */
	    strcpy(new_name, mask_msg.key1);
	    /* If creating from a template, name 'mask' substituted */
	    if (mask_msg.req_type == CREATE) strcpy(old_name, "mask");
	    /* If this is a copy use the old mask set name */
	    else strcpy(old_name, mask_msg.par1);
	} else {
	/* If creating template from template, no conversion needed */
	    if (mask_msg.req_type == CREATE) check_name = DBNO;
	    /* If creating template from mask, mask set name changed to 'mask' */
	        strcpy(new_name, "mask");
	        strcpy(old_name, mask_msg.par1);
	} /* End else */
    } /* end if */
     for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* Build complete names for file copy */
	strcpy1(new_ptr, files[cnt]);
	strcpy1(old_ptr, files[cnt]);
	/* If file name begins with old mask set name, */
	/* replace with new mask set name */
	if ((check_name == DBYES)
	&& (strncmp(new_ptr, old_name, 4) == 0))
	{
	    strncpy(new_ptr, new_name, 4);
	    strncpy(files[cnt], new_name, 4);
	}
	file_copy(old_file, new_file);
    } /* end for */
    return(cnt);
} /* end mask_dir_copy */

/* See if this file should be versioned */
/* If this tool does not own the file and file has old version */
/* It should be versioned */


/* Read in the mask file list for this mask set */
void mask_filelist_read(char *filelist[], int program[], int level)
{
    char file_name[FILE_SIZE];
    int cnt = 0;
    char mask_file[FILE_SIZE];
    FILE *fp;

    /* Build the name for the file */
    if (level == 0)
    strcpy2(mask_file, mti.delta_dir, "/local_filelist");
    else strcpy2(mask_file, mti.delta_dir, "/.db/local_filelist");
    /* Open the file for read */
    if ((fp = fopen(mask_file, "r")) == NULL)
    {
	filelist[0] = NULL;
	return;
    }
    /* Scan in lines until end of file reached */
    while (fscanf(fp, "%d %s", &program[cnt], file_name) == 2)
    {
	filelist[cnt] = malloc(strlen(file_name) +1);
	strcpy(filelist[cnt], file_name);
	++cnt;
    } /* end while */
    filelist[cnt] = NULL;
    /* close the file */
    fclose(fp);
} /* End mask_filelist_read */

int mask_dir_list(char *directory, char *list[], int index)
{
    char file_name[1024];
    struct stat buf;
    DIR *dp;
    struct dirent *dir; 
    char entry[256];

    if ((dp = opendir(directory)) == NULL)
    return(0);
    for (dir=readdir(dp); dir != NULL; dir=readdir(dp) )
    {
    /* Ignore '..' file */
	if ((strcmp(dir->d_name, "..") == 0) 
	|| (strcmp(dir->d_name, ".")) == 0) continue;
	/* Build complete file name */
	sprintf(file_name, "%s/%s", directory, dir->d_name);
	if (stat(file_name, &buf) != 0) continue;
	/* If this is a file add to the list */
	/* If this is a directory add to list and traverse directory */
	if (S_ISDIR(buf.st_mode) < 1)
	{
	    sprintf(entry, "%s %ld", file_name, bld_checksum(file_name));
	list[index] = malloc(strlen(entry)+1);
	    strcpy(list[index], entry);
	    ++index;
	} else {
	    /* Just put directory name on list */
	    list[index] = malloc(strlen(file_name)+1);
	    strcpy(list[index],  file_name);
	    ++index;
	    index = mask_dir_list(file_name, list, index);
	} /* End else */
    } /* End for for reading files and directories */
    closedir(dp);
    /* Return position pointed to */
    return(index);
} /* End mask_dir_list */
