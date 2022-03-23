#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	get_gzip_file
 *
 * Description:	Retrieve a compressed file from the Transcend database.
 *		Also retrieves history of big files.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "hash.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "client_lib.h"
#include "transapi_defines.h"
#include <zlib.h>

/* Allows passing in a directory name and alternate file name */
void alternate_file_name_bld(char *full_file_name, char *file_name, char *file_dir, char *alternate_file)
{

	if (strcmp(file_dir, NONE) != 0)
	{
	if (strcmp(alternate_file, NONE) != 0)
	    strcpy3(full_file_name, file_dir, "/", alternate_file);
	    else strcpy3(full_file_name, file_dir, "/", file_name);
    }
    else strcpy(full_file_name, file_name);
} /* end alternate_file_name_bld */

int uncompressed_file_get(FILE *fp,
    char *file_name,
    long long fileSize,
    long mode,
    TRANS_FILE_TYPE file_type)
{
    extern int size_printed;
    int ret;
    gzFile file;

    size_printed = DBNO;
    if ((file = trans_open("r", fileSize, 1, file_type)) == NULL)
    {
	strcpy(rtn.msg, gz_sockopen_err);
	close(sock);
#ifdef do_tar
	if (file_type == DIRECTORY) pclose(fp);
	else
#endif
	return(GZ_SOCKOPEN_ERR);
    }
    /* Retrieve all file data */
    if ((ret = block_retrieve(file, fp, file_type, file_name, DBNO)) != SUCCESS)
    return(ret);
    /* Write any remaining output to file */
    trans_close(file, 0);
    /* Send final ack */
    put_short(SUCCESS);
#ifdef do_tar
    if (file_type == DIRECTORY) pclose(fp);
    else fclose(fp);
#endif
    /* Now set the file mode */
    if (file_type != DIRECTORY) {
        chmod(file_name, mode);
    }

    if (size_printed == DBYES) PRINTF("\n");
    return(get_ret_code());
} /* end uncompressed_file_get */

/* Retrieves an uncompressed file */
int compressed_file_get(FILE *fp,
    char *file_name,
    long long fileSize,
    long mode,
    TRANS_FILE_TYPE file_type)
{
    extern int size_printed;
    extern char tar_option;
    int ret;
    char buffer[BLOCK_SIZE];

    size_printed = DBNO;
    trans_hash_init(fileSize, UNCOMPRESSED, "r", SERIAL);
  /* Read in data and write to buffer until all data sent */
    while ((ret = get_string(buffer)) > 0)
    {
		if (fwrite(buffer, 1, ret, fp)!= ret)
		{
		    char *err_ptr = strerror(errno);
		    char err_string[8];
		    sprintf(err_string, "%d", errno);
		    rtn.len = strcpy5(rtn.msg, bad_write, RET2TAB, "'", 
		    file_name, "'.");
		    strcpy7(rtn.msg+rtn.len,DOUBLE_RET, "Error message:  ", err_ptr,
		    "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
		    put_short(BAD_WRITE);
		    close(sock);
	#ifdef do_tar
	if (tar_option == 'z') fclose(fp);
		    else if (file_type == DIRECTORY) pclose(fp);
		    else
	#endif
		    fclose(fp);
		        unlink(file_name);
		    return(BAD_WRITE);
	} /* End if for write failed */
	/* See if output needs to be printed */
	trans_hash(ret, 0);
    } /* End while */
    /* Print final size */
    size_printed = DBYES;
    trans_hash_close();
    /* Close the file */
#ifdef do_tar
    /* If pulling down as a compressed directory */
    if (tar_option == 'z') 
    fclose(fp);
    else if ((file_type == DIRECTORY) 
    || (file_type == FILES))
    pclose(fp);
    else
#endif
    fclose(fp);
    if (ret < 0) return(ret);
    /* Tell server transaction successful */
    put_short(SUCCESS);
    if (size_printed == DBYES) PRINTF("\n");
    return(get_ret_code());
} /* End compressed_file_get */


int get_gzip_file(
    char *file_name,
    long long fileSize,
    long mode,
    TRANS_FILE_TYPE file_type,
    int do_uncompression,
    char *tar_type,
    char *file_dir, char *alternate_file)
{
    extern char tar_option;
    int len = 0;
    short ret;
    FILE *fp = 0;
    char full_file_name[PATH_SIZE];

	/* If directory specified, prepend to file name */
    /* also use alternate file name if specified */
    alternate_file_name_bld(full_file_name, file_name, file_dir, alternate_file);
    if ((file_type == BINARY)
    || (file_type == UNSPECIFIED)
    || (tar_option == 'z')
    || (file_type == UNCOMPRESSED)
    || (file_type == UNKNOWN))
	    fp = fopen(full_file_name, "wb");
    else if (file_type == ASCII)
        fp = fopen(full_file_name, "w");
#ifdef do_tar
    else
	{
	char tar_command[64];
#ifdef WINDOWS_NT4
	if (strcmp(MACHINE_TYPE, "Windows GUI") == 0)
    len = strcpy1(tar_command, "tar.exe zxf -");
    else len = strcpy1(tar_command, "tar.exe xf -");
#else
    if (strcmp(tar_type, "gtar") == 0) 
	len = whereis_gunzip_tar(tar_command);
	else len = strcpy1(tar_command, "gunzip --stdout | tar xf -");
#endif
    /* If directory to retrieve data in specified, add it to tar command */
    if (strcmp(file_dir, NONE) != 0)
    strcpy3(tar_command+len, " -C \"", file_dir, "\"");
	fp = popen(tar_command, "w");
    } /* End else */
#endif
/* If still having a problem with tar on NT, build temporary tar file */
#ifdef tar_problem
    else
    {
	/* Make sure file name ends with .tar suffix */
	if (strcmp(file_name+strlen(file_name)-4, ".tar") != 0)
	strcat(file_name, ".tar");
	fp = fopen(file_name, "wb");
	} /* End else */
#endif
    if (fp == NULL)
    {
		    char *err_ptr = strerror(errno);
		    char err_string[8];
#ifdef WINDOWS_NT4
	if (strlen(user.cwd) <= 3)
	sprintf(rtn.msg, root_dir_err, user.from_company, file_name, user.from_company);
else
#endif
      /* Following braces here for compatibility with Windows NT */
      {
		    sprintf(err_string, "%d", errno);
	rtn.len = strcpy4(rtn.msg, bad_wr_open, RET2TAB_QUOTE,
	file_name, "'\n\n");
		    rtn.len += strcpy6(rtn.msg+rtn.len,"Error message: ", err_ptr,
		    "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
      }
	/* Send error message to server */
	put_short(BAD_WR_OPEN);
	close(sock);
	return(BAD_WR_OPEN);
    } /* End if for failure to open file */
    put_short(SUCCESS);
    /* At this point, we have finally opened the file for write */
#ifdef WINDOWS_NT4
#ifndef TRANSCEND_GUI
    return(get_uncompressed_file(fp, file_name, fileSize, mode, file_type));
#endif
#endif
    /* Now ready to get the actual data */
    if ((do_uncompression == DBNO) || (tar_option == 'z') 
    || (file_type == DIRECTORY)
    || (file_type == FILES))
    ret = compressed_file_get(fp, file_name, fileSize, mode, file_type);
    else ret = uncompressed_file_get(fp, file_name, fileSize, mode, file_type);
    if (ret < 0) return(ret);
    /* If sent with gtar, but retrieved with tar, give warning */
/*
    if ((strcmp(tar_type, "gtar") == 0)
    && (len == 3))
    strcat(rtn.msg, gtar_wrn);
*/
    return(ret);
} /* end get_gzip_file */
#endif /* For not TRANSAPI */
