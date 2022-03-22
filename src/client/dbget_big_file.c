#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	get_big_file_init
 *
 * Description:	Retrieve a big file from the Transcend/InterCom database.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "global_client_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "transrc.h"
#include "rtn.h"
#include "strcpy.h"
#include "transapi_defines.h"
#include "trans_prototypes.h"
#include "client_lib.h"

/* Print warning if parallel type requested but not performed */
void parallel_warning_print(TRANS_FILE_TYPE file_type, unsigned long long size)
{
}
#ifdef garbage
    if (file_type == DIRECTORY)
    PRINTF(llrtv_dir_wrn); 
    else if (file_type == FILES)
    PRINTF(llrtv_files_wrn); 
    else if (size < 1024*1024*5)
    PRINTF(llrtv_5mb_wrn);
    else PRINTF(ll_wrn);
} /* end parallel_warning_print */
#endif

/* See if file already exists */
/* if it already exists, ask user if he wants to overwrite it */
int get_delete_check(char *file_name, char *file_dir, char *alternate_file, TRANS_FILE_TYPE file_type)
{
    char c_string[8];
    char *file_ptr; 
    char full_file_name[PATH_SIZE];

    /* Build the complete file name */
    alternate_file_name_bld(full_file_name, file_name, file_dir, alternate_file);
    /* Point to the root file name */
    file_ptr = root_find(full_file_name, '/');
    if (access(full_file_name, 0) == 0) 
    {
	if (file_type != DIRECTORY)
	printf("File '%s' already exists.  Overwrite this file, y/n?  ", file_ptr);
	else PRINTF("Directory '%s' already exists.  Overwrite this directory, y/n?  ", file_ptr);
	fflush(stdout);
	scanf("%s", c_string);
	fflush(stdin);
	printf("\n");
	if (( c_string[0] == 'y') || (c_string[0] == 'Y')) 
	{
	    /* Delete the file and indicate files have been overwritten */
	    unlink(full_file_name);
	} /* End if */
	else
	{
	    /* Tell server unable to create file */
	    put_short(BAD_CREATE);
	    close(sock);
	    PRINTF("Retrieval of '%s' aborted.\n\n", file_ptr);
	exit(FAILURE);
	}
    } /* End if for file already present */
    return(SUCCESS);
} /* End get_delete_check */

int auto_get_delete_check(char *file_name, TRANS_FILE_TYPE file_type)
{
    extern int global_dlt;

    if (access(file_name, 0) == 0) 
    {
    if (global_dlt == DBYES)
	{
	    /* Delete the file and indicate files have been overwritten */
#ifndef IRIX
	    unlink(file_name);
#else
	strcpy2(buffer, "rm -r -f ", file_name);
	system(buffer);
#endif /* For not IRIX */
	return(SUCCESS);
	} /* End if */
	else
	{
	    /* Tell server unable to create file */
	    put_short(BAD_CREATE);
	    close(sock);
	sprintf(rtn.msg, already_present_err, file_name);
	return(ALREADY_PRESENT_ERR);
	}
    } /* End if for file already present */
    return(SUCCESS);
} /* End auto_get_delete_check */

int get_big_file_init(
    REQ_TYPE req_type,
    REQ_TYPE parallel_type,
    char *primary_key,		/* Key needed to retrieve the big file */
    char *secondary_key,		/* Key needed to retrieve the big file */
    char *buffer,
    char *file_dir,
    char *alternate_file)
{
    char separate_size_string[12];
    char parallel_type_string[4];
    int ret;
    int ret_init;

    /* Make sure valid keyword entered */
#ifdef TRANSWEB
    if (is_transweb_keyword(req_type, primary_key, secondary_key) < 0) return(-1);
#else
    if (is_keyword(req_type, primary_key) < 0) return(-1);
#endif
    /* Get info about repository data stored in */
    repository_find(primary_key[8]);
    /* Connect to the Transcend/InterCom database */
    sprintf(separate_size_string, "%llu", transrc.separate_block_size);
    sprintf(parallel_type_string, "%d", parallel_type);
    if ((ret_init = init_trans_socket(req_type, primary_key, NONE, NONE, 0, 0, 
    UNKNOWN, secondary_key, file_dir, alternate_file, NONE, separate_size_string, parallel_type_string)) < SUCCESS) return(ret_init);
    /* Get the file name, size, mode, and file type  */
    ret = get_string(buffer);
    /* If 0 length file name, give error */
    /* User not allowed to retrieve file with 0 length file name */
    if (ret == 0)
    {
	sprintf(rtn.msg, "%s%s%s\n", zero_len_file_err, DOUBLE_RET, HELP_MSG);
	return(ZERO_LEN_FILE_ERR);
    }
    /* At this point we have the file name, mode and file type */
    return(ret_init);
} /* end get_big_file_init */
 

int get_big_file(REQ_TYPE req_type, REQ_TYPE parallel_type, char *primary_key, char *secondary_key, 
    char *file_dir, char *alternate_file)
{
#ifdef JAVA
    extern char global_keyword[];
    extern TRANS_FILE_TYPE global_file_type;
#endif
    extern char tar_option;
    extern float compression_ratio;
    extern char icap_class[];
    char tmp_file_type;
    char tar_type[12];
    int compression;
    int secondary_file_type;
    int ret;
    int ret_init;
    int num_args;
    char buffer[300];
    char file_name[FILE_SIZE];
    long long size;
    long mode;
    TRANS_FILE_TYPE file_type;

    /* First verify the path is present */
	    ret_init = get_big_file_init(req_type,parallel_type, primary_key, secondary_key, buffer, NONE, alternate_file);
    /* If error occurred, return at this point */
    if (ret_init < 0) return(ret);
    /* Scan out data */
    num_args = sscanf(buffer, "%s %lld %ld %c %d %s %f %d %s", 
    file_name, &size, &mode, &tmp_file_type, &compression, tar_type, &compression_ratio, &secondary_file_type, icap_class);
    /* Put spaces back in file name */
    strexc(file_name, '`', ' ');
    /* Make sure compression ratio is not set to 0 */
    if ((compression_ratio == 0)  || (compression_ratio > 20))
    compression_ratio = 1;
    file_type = (TRANS_FILE_TYPE) tmp_file_type;
    /* If secondary file type not scanned in, assign it */
    if (num_args <= 6) secondary_file_type = file_type;
    if (num_args < 7) strcpy(icap_class, "fcp");
#ifdef JAVA
    global_file_type = file_type;
    strcpy(global_keyword, file_name);
#endif
    /* Check to see if file already exists */
#ifndef JAVA
    if ((ret = get_delete_check(file_name, file_dir, alternate_file, file_type)) < 0)
    return(ret);
#endif
#ifdef JAVA
	if ((ret = auto_get_delete_check(file_name, file_type)) < SUCCESS)
    return(ret);
#endif
    /* Get the file */
    /* Give warning if parallel transfer denied */
    if ((parallel_type == DBGET_PARALLEL) && (ret_init == 0))
    parallel_warning_print(file_type, size);
#ifdef WINDOWS_NT4
    /* If this is a directory or group of files, retrieve it with libarchive */
    if ((file_type == DIRECTORY) || (file_type == FILES))
    ret = get_archive(file_name, size);
    else
#endif
    /* If 1 returned, the server is returning parallel compressed data */
    /* The client can retrieve all blocks in parallel or one block at a time */
    if ((ret_init == PARALLEL) 
    || (ret_init == DBGET_LARGE_PARALLEL) )
    {
	/* Server does not distinguish between parallel blocks and serial blocks */
	if (tar_option == 'r')
	ret = get_separate_compressed_data(file_name, primary_key, size, mode, file_type);
    else ret = get_parallel_compressed_data(ret_init, file_name, primary_key, size, mode, file_type);
    } /* End if */
    else if (ret_init == DBGET_RANDOM_PARALLEL)
    ret = get_parallel_uncompressed_data(file_name, primary_key, size, mode, file_type);
    /* Sending secondary file type so will be opened correctly */
else ret = get_gzip_file(file_name, size, mode, secondary_file_type, compression, tar_type, file_dir, alternate_file);
#ifdef tar_problem
    if (file_type == DIRECTORY)
    {
	 rtn.len = strlen(rtn.msg);
	sprintf(rtn.msg+rtn.len, nt_get_wrn, file_name);
    }
    else if (file_type == FILES)
    {
	 rtn.len = strlen(rtn.msg);
	sprintf(rtn.msg+rtn.len, files_get_wrn, file_name);
    }
#endif /* For tar problem */
    return(ret);
} /* end get_big_file */

/* Returns the tar type */
char get_options_set(int argc, char **argv, int *dir_position, int *file_position)
{
    char *arg = argv[1];
    int cnt;
    char tar_option = '\0';

    /* Set default values */
    *dir_position = *file_position = 0;
    /* See if extra options are present */
    for (cnt = 2; arg[cnt] != '\0'; ++cnt)
    {
	switch(arg[cnt])
	{
	    case 'f': *file_position = cnt; break;
	    case 'd': *dir_position = cnt; break;
	    default:  tar_option = arg[cnt];
	} /* end switch */
    } /* end for */
    /* At this point we have the tar option, and pointers to directory and/or files names */
    /* Return transfer type */
    return(tar_option);
} /* end get_options_set */
#endif /* For not TRANSAPI */
