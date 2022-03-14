#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	dbput_big_file
 *
 * Description:	Sends the Transcend/Intercom server a big file.
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "client_disk_usage.h"
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"
#include "transrc.h"
#include "rtn.h"
#include "strcpy.h"
#include "transapi_defines.h"

#include "global_debug.h" // Mazieri's debug facilities


/* See if parallel transfer  will be done */
int parallel_type_set(int req_type, char *file_name, long long size, TRANS_FILE_TYPE file_type)
{
    extern char tar_option;
    int parallel_transfer;

/* Parallel data transfer not supported for Windows */
#ifdef WINDOWS_NT4
    return(req_type);
#endif
/* Parallel data transfer not supported for AIX */
#ifdef AIX
    return(req_type);
#endif
    /* If serial transfer specified, it overwrites any other tar_options */
    if (tar_option == 's') return(SERIAL);
#ifdef INTERCOM
	else if (is_zipped_file(file_name) == DBNO)
    {
	if (tar_option == 'r')
	printf("Retry option supported only for zipped files.\n");
	else if (tar_option == 'l')
    PRINTF(ll_zip_wrn);
	return(req_type);
    }
#endif
#ifdef TRANSCEND
    else if (file_type == DIRECTORY)
    {
	if (tar_option == 'r')
	printf(retry_dir_wrn);
	else if (tar_option == 'l')
	printf(ll_dir_wrn);
	return(req_type);
    }
#endif /* for TRANSCEND */
    /* At this point we know we have a zipped file */
    if (size < 5000000)
    {
	if (tar_option == 'l')
	    PRINTF(ll5mb_wrn);
	if (tar_option == 'r')
	    PRINTF(retry5mb_wrn);
	    return(req_type);
	}
    if (tar_option == 'l')
    {
        /* Specify large parallel for everything */
        /* Server then decides if large parallel or parallel used */
        return(DBPUT_LARGE_PARALLEL);
    } /* end if */
    else if (tar_option == 'r')
    {
        return(DBPUT_SEPARATE);
    }
    else if (size > transrc.parallel_zip_size)
    {
        return(DBPUT_LARGE_PARALLEL); // always force LARGE
#if 0   // disabled
        if (size > 8000000)
        {
            return(DBPUT_LARGE_PARALLEL);
        }
        else
        {
            return(DBPUT_PARALLEL);
        }
#endif
    } /* end if */
    else return(req_type);
} /* End parallel_type_set */

int dbput_big_file(
    REQ_TYPE req_type,
     TRANS_FILE_TYPE file_type,
#ifdef TRANSCEND_GUI
    char *description,
#endif
#ifdef JAVA
    char *description,
#endif
    int argc,
    char **argv)
{
    extern char icap_class[];
    extern int global_dir_offset;
    extern int log_files;
    extern long long global_compressed_size;
    void strip_control_chars(char *new_name, char *old_name);
    char new_name[FILE_SIZE];
    struct DIR_INFO *dir_ptr  = NULL;
    int is_tar;
    REQ_TYPE parallel_type;
    struct stat buf;
    char *root_ptr; /* Points to root file name sent to database */
     char *c_option_ptr;
    short len;
    short ret;
    long long size;

    /* Verify that a file name was passed in */
    if (argc < 3)
    {
	printf(no_filename_err);
	return(-1);
    }
    /* See if data classification specified */
    if ((argc >= 4)
    && (argv[2][0] == '-'))
    {
	strcpy(icap_class, argv[2]+1);
	argv += 1;
	argc -= 1;
    }
#ifdef NXP 
    else strcpy(icap_class, "fcp");
#else
    else strcpy(icap_class, "cp");
#endif
    /* Get the length of the file name */
    len = strlen(argv[2]);
    /* If there is a slash or backslash on the end of the name, get rid of it */
    if ((argv[2][len-1] == '/') || (argv[2][len-1] == '\\')) 
    argv[2][len-1] = '\0';
    /* Find the root file name */
    root_ptr = root_find(argv[2], '/');
    /* If found '/' and '/' is not the first character of path, point to where space goes */
    if ((root_ptr != argv[2]) 
    && (root_ptr-1 != argv[2]))
    c_option_ptr = root_ptr-1;
    else c_option_ptr = NULL;
    /* Calculate the global offset used when writing data to the transdirlist file */
    global_dir_offset = root_ptr - argv[2];
    strip_control_chars(new_name, root_ptr);
    /* Determine if this is a file or directory */
    ret = stat(argv[2], &buf);
     if (ret == -1)
    {
		    char *err_ptr = strerror(errno);
		    char err_string[8];
		    sprintf(err_string, "%d", errno);
	rtn.len = strcpy4(rtn.msg, bad_rd_open, RET2TAB_QUOTE,
	argv[2], "'\n\n");
		    strcpy6(rtn.msg+rtn.len,"Error message: ", err_ptr,
		    "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
	return(BAD_RD_OPEN);
    }
#ifdef TRANSCEND_GUI
    if (file_type == DIRECTORY)
    {
	is_tar = DBNO;
	len = strlen(new_name);
	if (strcmp(new_name+len-4, ".tar") == 0)
	new_name[len-4] = '\0';
    }
    else
#endif
    /* Set tar option based on file type */
    if (S_ISDIR(buf.st_mode)) 
    {
	is_tar = DBYES;
	/* Add the first node */
	file_type = DIRECTORY;
    }
    /* If this is not a directory, set request type back to PUT_DATA */
    else 
    {
	is_tar = DBNO;
	/* Indicate not creating the transdirlist file */
	/* It will be created dynamicly on the server if needed */
	log_files = DBNO;
    }
    /* If using the -t option, add first node for size calculation */
    if ((req_type == DBPUT_FILES) || (req_type == DBPUTDLT_FILES))
	dir_ptr = dir_info_add(dir_ptr, buf.st_ino, buf.st_dev);
    global_compressed_size = 0l;
    /* If this is a directory must calculate the size */
    if (is_tar == DBYES)
    {
	size = dir_size_calc(argv[2], dir_ptr);
    } /* End if */
    else size = buf.st_size;
    /* Decide if parallel type will be done based on size and file type */
    parallel_type = parallel_type_set(req_type, argv[2],  size, file_type);
     if ((parallel_type == DBPUT_PARALLEL) && (argv[1][2] == 'r')) 
     parallel_type = DBPUT_SEPARATE; 
    /* Send the file compressing it as it is sent */
	return(put_gzip_file(req_type, parallel_type, (long long)size, buf.st_mode, file_type, argv[2], 
	new_name, c_option_ptr, 
#ifdef TRANSCEND_GUI
    description,
#endif
	argc, argv, is_tar));
} /* End dbput_big_file */

/* Takes unreadable characters out of file name */
void strip_control_chars(char *new_name, char *old_name)
{

    int new_cnt = 0;
    int old_cnt;

    /* Go through each character in file name */
    for (old_cnt = 0; old_name[old_cnt] != '\0'; ++old_cnt)
    {
	/* If this is a control character do not copy it */
	if (old_name[old_cnt] <= 26) continue;
    /* Also get rid of spaces */
/*     if (old_name[old_cnt] == ' ') continue; */
	/* Otherwise just copy the character */
	else new_name[new_cnt++] = old_name[old_cnt];
    } /* End for */
    /* Put null at end of name */
    new_name[new_cnt] = '\0';
} /* End strip_control_chars */

/* Assign the file type */
char file_type_get(char type)
{

    switch(type)
    {
	case 'a':
	case 'A':
	{
	    return(ASCII);
	}
	case 'b':
	case 'B':
	{
	    return(BINARY);
	}
	default: return(UNSPECIFIED);
    } /* End switch */
} /* end file_type_get */
#endif /* For not TRANSAPI */
