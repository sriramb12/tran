#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	dbput_zip_dir
 *
 * Description:	routines to transfer groups of files to the Intercom repository.
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "client_dir.h"
#include "client_close_down.h"
#include "global_defines.h"
#include "global_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
extern int global_dir_cnt;

int dbput_zip_dir(
    REQ_TYPE req_type,
    int argc,
    char *argv[])
{
     int ret;
    char *root_ptr;
    char *file_ptr;
    char tar_type[12];
    char old_file[FILE_SIZE];
    char new_file[FILE_SIZE];
    char tmp_dir[FILE_SIZE];
    char tmp_dir2[FILE_SIZE];
     char tar_command[BUFFER_SIZE];
    long long size = 1024;
    struct stat buf;
    FILE *fp;

    /* Verify correct number of arguments specified */
    if (argc < 4)
    {
	printf(not_enough_arguments);
	return(-1);
    }
    /* Build temporary directory name */
    sprintf(tmp_dir, "%s%d", argv[2], time(0));
    /* Make the temporary directory specified */
#ifdef WINDOWS_NT4
    if (mkdir(tmp_dir, 0700) != 0)
#else
    if (mkdir(tmp_dir, 0700) != 0)
#endif
    {
	close_down1f(crt_dir_err, tmp_dir);
	return(CRT_DIR_ERR);
    }
    /* Now make the directory under the temporary directory */
    strcpy3(tmp_dir2, tmp_dir, "/", argv[2]);
#ifdef WINDOWS_NT4
    if (mkdir(tmp_dir2, 0700) != 0)
#else
    if (mkdir(tmp_dir2, 0700) != 0)
#endif
    {
	close_down1f(crt_dir_err, tmp_dir2);
	return(CRT_DIR_ERR);
    }
    global_dir_cnt = 1;
    /* Open file where file names stored */
    if ((fp = fopen(argv[3], "r")) == NULL)
    {
		    char *err_ptr = strerror(errno);
		    char err_string[8];
		    sprintf(err_string, "%d", errno);
	rtn.len = strcpy4(rtn.msg, bad_rd_open, RET2TAB_QUOTE,
	argv[3], "'\n\n");
		    strcpy6(rtn.msg+rtn.len,"Error message: ", err_ptr,
		    "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
	/* Delete temporary directories no longer needed */
    rmdir(tmp_dir2);
	rmdir(tmp_dir);
	return(-1);
    } /* End if */
    /* Initialize place where file names placed */
    file_ptr = strcpy2(new_file, tmp_dir2, "/") +new_file;
    while (fscanf(fp, "%s", old_file) == 1)
    {
	/* First find the root file name */
	root_ptr = root_find(old_file, '/');
	strcpy2(file_ptr, "/", root_ptr);
	/* Now perform the link */
	if ((ret = link( old_file, new_file)) != 0)
	ret = symlink( old_file, new_file);
	if (ret != 0)
	{
	    close_down1f(bad_link, old_file);
	    dir_dlt(tmp_dir2);
	    dir_dlt(tmp_dir);
	    fclose(fp);
	    return(BAD_LINK);
	} /* End if */
	/* Add size of file to total size */
	if (stat(old_file, &buf) != 0)
	{
	    fclose(fp);
	    dir_dlt(tmp_dir2);
	    dir_dlt(tmp_dir);
	sprintf(rtn.msg, bad_stat, errno, old_file);
	    return(BAD_STAT);
	} /* End if */
    /* Size of 1024 added if file size less than 1024 */
    /* because tar makes files less than 1024 bytes 1024 bytes */
    if (buf.st_size >= 1024)
	size += buf.st_size;
    else size += 1024;
#ifdef TRANSCEND
    transdirlist2add(argv[2], root_ptr, buf.st_size);
#endif
#ifdef NXP 
#ifdef INTERCOM
	/* Update the Mask Designer log file */
	maskshop_file_log(old_file, &buf, DBNO);
#endif
#endif
    } /* End while */
    fclose(fp);
    /* See if gtar is present */
     /* Build the tar command */
      /* Presidence order, gtar, gnutar or tar */
        if (whereis_tar(tar_command) <= 4)
    strcpy(tar_type, "tar");
    else strcpy(tar_type, "gtar");
     /* Open command for tar output */
    strcpy4(tar_command+strlen(tar_command), " cfh - -C ", tmp_dir, " ", argv[2]);
    ret = gzip_file(req_type, req_type, size, 33216, DIRECTORY, 
    argv[2], argv[2], 
#ifdef TRANSCEND_GUI
    "None",
#endif
    tar_type, argc-1, argv+1, DBYES, tar_command, tar_command);
    /* Delete temporary directories no longer needed */
    dir_dlt(tmp_dir2);
    dir_dlt(tmp_dir);
    return(ret);
} /* End dbput_zip_dir */

int dbput_command_dir(
    REQ_TYPE req_type,
    int argc,
    char *argv[])
{
    int ret;
    char *root_ptr;
    char *file_ptr;
    char tar_type[12];
    char new_file[FILE_SIZE];
     char tar_command[BUFFER_SIZE];
    long long size = 1024;
    struct stat buf;
    int cnt;
    char tmp_dir[FILE_SIZE];
    char tmp_dir2[FILE_SIZE];

    /* build name for temporary directory */
    sprintf(tmp_dir, "%s%d", argv[2], time(0));
    /* Make the temporary directory specified */
#ifdef WINDOWS_NT4
    if (mkdir(tmp_dir, 0700) != 0)
#else
    if (mkdir(tmp_dir, 0700) != 0)
#endif
    {
	close_down1f(crt_dir_err, argv[2]);
	return(CRT_DIR_ERR);
    }
    /* build name for second temporary directory */
    strcpy3(tmp_dir2, tmp_dir, "/", argv[2]);
    /* Make this directory */
#ifdef WINDOWS_NT4
    if (mkdir(tmp_dir2, 0700) != 0)
#else
    if (mkdir(tmp_dir2, 0700) != 0)
#endif
    {
	close_down1f(crt_dir_err, argv[2]);
	rmdir(tmp_dir);
	return(CRT_DIR_ERR);
    }
    /* Initialize place where file names placed */
    file_ptr = strcpy2(new_file, tmp_dir2, "/") +new_file;
#ifdef TRANSCEND
    global_dir_cnt = 1;
#endif
    for (cnt = 3; cnt < argc; ++cnt)
    {
	/* First find the root file name */
	root_ptr = root_find(argv[cnt], '/');
	strcpy2(file_ptr, "/", root_ptr);
     /* Now perform the link */
    if ((ret = link( argv[cnt], new_file)) != 0)
    ret = symlink( argv[cnt], new_file);
	if (ret != 0)
    {
	close_down1f(bad_link, argv[cnt]);
	    dir_dlt(tmp_dir2);
	    dir_dlt(tmp_dir);
	return(BAD_LINK);
	} /* End if */
	/* Add size of file to total size */
	if (stat(argv[cnt], &buf) != 0)
	{
	    dir_dlt(tmp_dir2);
	    dir_dlt(tmp_dir);
	sprintf(rtn.msg, bad_stat, errno, argv[cnt]);
	    return(BAD_STAT);
	} /* End if */
    if (buf.st_size >= 1024)
	size += buf.st_size;
    else size += 1024;
#ifdef TRANSCEND
    transdirlist2add(argv[2], argv[cnt], buf.st_size);
#endif
#ifdef NXP
#ifdef INTERCOM
	/* Update the Mask Designer log file */
	maskshop_file_log(argv[cnt], &buf, DBNO);
#endif
#endif
    } /* End while */
    /* See if gtar is present */
     /* Build the tar command */
      /* Presidence order, gtar, gnutar or tar */
        if (whereis_tar(tar_command) <= 4)
    strcpy(tar_type, "tar");
    else strcpy(tar_type, "gtar");
     /* Open command for tar output */
    strcpy4(tar_command+strlen(tar_command), " cfh - -C ", tmp_dir, " ", argv[2]);
    ret = gzip_file(req_type, req_type, size, 33216, DIRECTORY, 
    argv[2], argv[2], 
#ifdef TRANSCEND_GUI
    "None",
#endif
    tar_type, 0, argv, DBYES, tar_command, tar_command);
    /* Delete temporary directories no longer needed */
    dir_dlt(tmp_dir2);
    dir_dlt(tmp_dir);
    return(ret);
} /* End dbput_command_dir */
#endif /* For not TRANSAPI */
