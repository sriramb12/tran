#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	trans_file_get
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
#include "trans_hash.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "rtn.h"
#include "strcpy.h"
#include "ssl_client_lib.h"
#include "gzio.h"
#include "zlib.h"

#include "global_debug.h"

#define HELP_MSG "For assistance, contact Intercom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"

extern int gl_is_web;  // special flag for transweb

/* Get a file and uncompress it during transfer */
int trans_uncompress_get( FILE *fp, int index)
{
    DBG();

    extern int size_printed;
    int ret;
    gzFile file;

    size_printed = DBNO;
    if ((file = tfile_open("r", 1, index)) == NULL)
    {
	strcpy(rtn.msg, gz_sockopen_err);
	close(sock);
#ifdef do_tar
	if (trans[index]->file_type == DIRECTORY) pclose(fp);
	else
#endif
	return(GZ_SOCKOPEN_ERR);
    }
    /* Retrieve all file data */
    if ((ret = trans_block_retrieve(file, fp, trans[index]->file_type, trans[index]->file_name, DBNO, index)) != SUCCESS)
    return(ret);
    /* Write any remaining output to file */
    tfile_close(file, 0);
    /* Send final ack */
    put_ssl_short(SUCCESS);
#ifdef do_tar
    if (trans[index]->file_type == DIRECTORY) pclose(fp);
    else fclose(fp);
#endif
    /* Now set the file mode */
    if (trans[index]->file_type != DIRECTORY) {
        DBG("chmod(trans[index]->file_name='%s', trans[index]->file_mode='%i')", trans[index]->file_name, trans[index]->file_mode);
       /* Quick workaround to make sure permission is set when downloading file originally uploaded with Transweb, because 
        * Transweb is losting the file permission during the upload. The following chmod sets owner permissions to read, write, and execute, 
        * group permissions to read and execute, and other permissions to read. -  in 7/8/15 
        */ 
        if (trans[index]->file_mode == 0) {
            chmod(trans[index]->file_name, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
        } else {
            chmod(trans[index]->file_name,  trans[index]->file_mode);
        }
    }

    if (size_printed == DBYES) PRINTF("\n");
    return(trans_ret_code_get());
} /* end uncompressed_file_get */

/* Retrieves a file without uncompressing it */
int trans_compress_get(FILE *fp, int index)
{
    DBG();

    extern int size_printed;
    extern char tar_option;
    int ret;
    char buffer[BLOCK_SIZE];

    size_printed = DBNO;
    tfile_hash_init(trans[index]->original_size, UNCOMPRESSED, "r", SERIAL, index);
  /* Read in data and write to buffer until all data sent */
    while ((ret = get_ssl_string(buffer)) > 0)
    {
		if (fwrite(buffer, 1, ret, fp)!= ret)
		{
		    char *err_ptr = strerror(errno);
		    char err_string[8];
		    sprintf(err_string, "%d", errno);
		    rtn.len = strcpy5(rtn.msg, bad_write, RET2TAB, "'", 
		    trans[index]->file_name, "'.");
		    strcpy7(rtn.msg+rtn.len,DOUBLE_RET, "Error message:  ", err_ptr,
		    "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
		    put_ssl_short(BAD_WRITE);
		    close(sock);
	#ifdef do_tar
	if (tar_option == 'z') fclose(fp);
		    else if (trans[index]->file_type == DIRECTORY) pclose(fp);
		    else
	#endif
		    fclose(fp);
		        unlink(trans[index]->file_name);
		    return(BAD_WRITE);
	} /* End if for write failed */
	/* See if output needs to be printed */
	tfile_hash(ret, 0, index);
    } /* End while */
    /* Print final size */
    if (gl_is_web == 0)
    {
        size_printed = DBYES;
    }
    tfile_hash_close();
    /* Close the file */
#ifdef do_tar
    /* If pulling down as a compressed directory */
    if (tar_option == 'z') 
    fclose(fp);
    else if ((trans[index]->file_type == DIRECTORY) 
    || (trans[index]->file_type == FILES))
    pclose(fp);
    else
#endif
    fclose(fp);
    if (ret < 0) return(ret);
    /* Tell server transaction successful */
    put_ssl_short(SUCCESS);
    if (size_printed == DBYES) PRINTF("\n");
    return(trans_ret_code_get());
} /* end trans_compress_get */


/* Retrieve a file specified with the GET option */
int trans_file_get(int index)
{
    DBG();

    extern char tar_option;
    int len = 0;
    short ret;
    FILE *fp = 0;
    char full_file_name[PATH_SIZE];

	/* If directory specified, prepend to file name */
    /* also use alternate file name if specified */
    alternate_file_name_bld(full_file_name, trans[index]->file_name, trans[index]->alternate_dir, trans[index]->alternate_file);
    if ((trans[index]->file_type == BINARY)
            || (trans[index]->file_type == UNSPECIFIED)
            || (tar_option == 'z')
            || (trans[index]->file_type == UNCOMPRESSED)
            || (trans[index]->file_type == UNKNOWN))
    {
        fp = gl_is_web ? stdout  : fopen(full_file_name, "wb");
    }
#ifdef do_tar
    else
    {
        char tar_command[64];
#ifdef WINDOWS_NT4
        if (strcmp(MACHINE_TYPE, "Windows GUI") == 0)
            len = strcpy1(tar_command, "tar.exe zxf -");
        else len = strcpy1(tar_command, "tar.exe xf -");
#else
        if (strcmp(trans[index]->tar_type, "gtar") == 0)
            len = whereis_gunzip_tar(tar_command);
        else len = strcpy1(tar_command, "gunzip --stdout | tar xf -");
#endif
        /* If directory to retrieve data in specified, add it to tar command */
        if (trans[index]->alternate_dir != NULL)
            strcpy3(tar_command+len, " -C \"", trans[index]->alternate_dir, "\"");
        fp = popen(tar_command, "w");
    } /* End else */
#endif
    /* If still having a problem with tar on NT, build temporary tar file */
#ifdef tar_problem
    else
    {
        /* Make sure file name ends with .tar suffix */
        if (strcmp(full_file_name+strlen(full_file_name)-4, ".tar") != 0)
            strcat(full_file_name, ".tar");
        fp = gl_is_web ? stdout  : fopen(full_file_name, "wb");
    } /* End else */
#endif
    if (fp == NULL)
    {
        char *err_ptr = strerror(errno);
        char err_string[8];
#ifdef WINDOWS_NT4
        if (strlen(user.cwd) <= 3)
            sprintf(rtn.msg, root_dir_err, user.from_company, trans[index]->file_name, user.from_company);
        else
#endif
            /* Following braces here for compatibility with Windows NT */
        {
            sprintf(err_string, "%d", errno);
            rtn.len = strcpy4(rtn.msg, bad_wr_open, RET2TAB_QUOTE,
                              trans[index]->file_name, "'\n\n");
            rtn.len += strcpy6(rtn.msg+rtn.len,"Error message: ", err_ptr,
                               "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
        }
        /* Send error message to server */
        put_ssl_short(BAD_WR_OPEN);
        close(sock);
        return(BAD_WR_OPEN);
    } /* End if for failure to open file */
    put_ssl_short(SUCCESS);
    /* At this point, we have finally opened the file for write */
    /* Now ready to get the actual data */
    if ((trans[index]->uncompress_during_transfer == DBNO) || (tar_option == 'z')
            || (trans[index]->file_type == DIRECTORY)
            || (trans[index]->file_type == FILES))
    {
        ret = trans_compress_get(fp, index);
    }
    else
    {
        ret = trans_uncompress_get(fp, index);
    }
    if (ret < 0) return(ret);
    /* If sent with gtar, but retrieved with tar, give warning */
/* Currently not geting used */
#ifdef OLD
    if ((strcmp(trans[index]->tar_type, "gtar") == 0)
    && (len == 3))
    strcat(rtn.msg, gtar_wrn);
#endif
    return(ret);
} /* end trans_file_get */

/* routine for everything except Transcend GUI */
int custom_file_get(int index)
{
    DBG();

    extern TRANS_TOOL tool;
    short ret;
    FILE *fp;
    char buffer[BUFFER_SIZE];

    fp = gl_is_web ? stdout  : fopen(trans[index]->file_name, "w");
     if (fp == NULL)
    {
	/* Tell server unable to open file */
	put_ssl_short(BAD_CREATE);
	trans_ret_code_get();
	sprintf(rtn.msg, bad_create, errno, trans[index]->file_name, tool.name, HELP_MSG);
		return(BAD_CREATE);
    } /* End if for failure to open file */
    put_ssl_short(SUCCESS);
    while ((ret = get_ssl_string(buffer)) > 0)
    {
	if (fwrite(buffer, 1, ret, fp) != ret)
	{
	    strcpy7(rtn.msg, bad_write, RET2TAB, "'", trans[index]->file_name, "'.", DOUBLE_RET, HELP_MSG);
	    put_ssl_short(BAD_WRITE);
	    close(sock);
    fclose(fp);
	        unlink(trans[index]->file_name);
	    return(BAD_WRITE);
	}
	} /* end while */
    fclose(fp);

    /* Set the file mode */
    if ((trans[index]->file_type != DIRECTORY)
    && (trans[index]->file_mode != 0))
    chmod(trans[index]->file_name, trans[index]->file_mode);
    ret = trans_ret_code_get();
    if (trans[index]->req_type != DBHIST_DATA)
  PRINTF("\n");
    return(ret);
} /* end custom_file_get */

#endif
