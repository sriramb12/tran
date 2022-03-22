#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	dbput_big_files
 *
 * Description:	routines to transfer groups of files
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#ifndef WINDOWS_NT4
#include <alloca.h>
#endif
#include "client_close_down.h"
#include "client_disk_usage.h"
#include "global_defines.h"
#include "global_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "trans_defines.h"

/* Puts multiple files listed in a file into Transcend or InterCom */
int dbput_big_files(
    REQ_TYPE req_type,
    int argc,
    char *argv[])
{
    int local_dir_cnt = 0;
    char *file_ptr;
    long long big_size;
    char done = DBNO;
    int len = 0;
    long long size = 0;
    FILE *fp;
    struct stat buf;
    char tar_type[12];
    char files[4096];
    char file_name[FILE_SIZE];
    char tar_command[8192];

    if (argc < 3)
    {
	printf(not_enough_arguments);
	return(-1);
    }
    /* Open file for reading in file names */
    if ((fp = fopen(argv[2], "r")) == NULL)
    {
	close_down1f(bad_rd_open, argv[2]);
	return(BAD_RD_OPEN);
    }
     /* Build the tar command */
      /* Presidence order, gtar, gnutar or tar */
        if (whereis_tar(tar_command) <= 4)
    strcpy(tar_type, "tar");
    else strcpy(tar_type, "gtar");
    strcat(tar_command, " cf - ");
    file_ptr = tar_command + strlen(tar_command);
    /* Read in file names from file making sure present and adding up size */
    while(fscanf(fp, "%s", file_name) == 1)
    {
	if (stat(file_name, &buf) != 0)
	{
	    fclose(fp);
	    sprintf(rtn.msg, bad_stat, errno, file_name);
	    return(BAD_STAT);
	} /* end if for not present */
	file_ptr += strcpy2( file_ptr, " ", file_name);
	if (done == DBNO)
	{
	    if (len < 40) len += strcpy2(files+len, file_name, " ");
	    else
	    {
		len += strcpy1(files+len, "etc.");
		done = DBYES;
	    } /* End else */
    } /* end big if */
	/* If this is a directory, calculate directory size */
	if (S_ISDIR(buf.st_mode))
	{
	    big_size = big_disk_usage(file_name, local_dir_cnt);
	    size += big_size;
	} else {
	    if (buf.st_size >= 1024)
	    size += buf.st_size;
    else size += 1024;
#ifdef TRANSCEND
    transdirlist_add(file_name, buf.st_size);
#else 
#ifdef WINDOWS_NT4
    transdirlist_add(file_name, buf.st_size);
#endif /* for Windows */
#endif /* for Transcend */
#ifdef NXP
#ifdef INTERCOM
    /* Update log for mask in mask designer */
	maskshop_file_log( file_name, &buf, DBNO);
#endif
#endif
	} /* End else for file */
    } /* end while */
    /* Close file no longer needed */
    fclose(fp);
    return(gzip_file(req_type, req_type, size, 33216, FILES, 
    files, argv[2], 
#ifdef TRANSCEND_GUI
        "None",
#endif
    tar_type, 
    argc, argv,  DBYES, tar_command, tar_command));
} /* End dbput_big_files */

/* Put files from the command line */
int dbput_command_files(
    REQ_TYPE req_type,
    int argc,
    char *argv[])
{
    int local_dir_cnt = 0;
    char done = DBNO;
    int len = 0;
    char tar_type[12];
     char tar_command[10000];
    char files[FILE_SIZE];
    long long size = 0;
    long long big_size;
    struct stat buf;
    char *file_ptr;
    int cnt;

    /* Build first part of tar command */
    /* See if gtar if present */
     /* Build the tar command */
      /* Presidence order, gtar, gnutar or tar */
        if (whereis_tar(tar_command) <= 4)
    strcpy(tar_type, "tar");
    else strcpy(tar_type, "gtar");
    strcat(tar_command, " cf - ");
    file_ptr = tar_command + strlen(tar_command);
    /* Make sure files present and build tar command */
    for (cnt = 2; cnt < argc; ++cnt)
    {
	/* First make sure file is present */
	if (stat(argv[cnt], &buf) != 0)
	{
	    sprintf(rtn.msg, bad_stat, errno, argv[cnt]);
	    return(BAD_STAT);
	}
	/* Add file to tar command */
	file_ptr += strcpy2( file_ptr, " ", argv[cnt]);
	/* Add file name to names sent to server */
	if (done == DBNO)
	{
	    if (len < 40) len += strcpy2(files+len, argv[cnt], " ");
	    else
	    {
		len += strcpy1(files+len, "etc.");
		done = DBYES;
	    } /* End else */
	} /* End if */
	/* Add size of file to total size */
	/* If this is a directory, calculate directory size */
	if (S_ISDIR(buf.st_mode))
	{
	    big_size = big_disk_usage(argv[cnt], local_dir_cnt);
	    size += big_size;
	} else {
	    if (buf.st_size >= 1024)
	    size += buf.st_size;
    else size += 1024;
#ifdef TRANSCEND
    transdirlist_add(argv[cnt], buf.st_size);
#else 
#ifdef WINDOWS_NT4
    transdirlist_add(argv[cnt], buf.st_size);
#endif /* for Windows */
#endif /* for Transcend */
#ifdef NXP
#ifdef INTERCOM
    /* Update log for mask in mask designer */
	maskshop_file_log( argv[cnt], &buf, DBNO);
#endif
#endif
	} /* End else for file */
    } /* End while */
    return(gzip_file(req_type, req_type, size, 33216, FILES, 
    files, argv[2], 
#ifdef TRANSCEND_GUI
    "None",
#endif
    tar_type, 0, argv, DBYES, tar_command, tar_command));
} /* End dbput_command_files */
#endif /* For not TRANSAPI */
