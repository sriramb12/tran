/****************************************************************************
 *
 * Function:	trans_get_request and supporting functions
 *
 * Description:	Retrieve data from the Transcend/InterCom repository.
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
#include "client_dir.h"
#include "global_client_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "transrc.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "ssl_client_lib.h"

#include "global_debug.h" // Mazieri's debug facilities

#define HELP_MSG "For assistance, contact Intercom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"


extern TRANS_PARAMS *trans[];
void trans_alternate_dir_name_build(char *alternate_dir);


/* Set a get request */
int trans_get_request_set(int index, REQ_TYPE req_type, char transfer_type, char *keyword)
{

    int ret;

    DBG();

    /* Verify valid keyword entered */
    if ((ret = trans_is_keyword(keyword)) < 0) return(ret);
    trans[index]->transaction_type = GET_REQUEST;
    trans[index]->req_type = req_type;
    strcpy(trans[index]->keyword, keyword);
    /* Get info about repository data stored in */
    trans_repository_find(trans[index]->keyword[8]);
    /* Set the transfer type */
    return(trans_get_transfer_type_set(index, transfer_type));
} /* end trans_get_request_set */



static int trans_get_transfer_type_set_SERIAL(int index)
{
    int is_delete = IS_REQUEST_GET_DELETE(trans[index]->req_type);
    int is_zipped = IS_REQUEST_GET_ZIPPED(trans[index]->req_type);
    int serial    =  is_delete ? DBGETDLT_SERIAL : DBGET_SERIAL;
    if (is_zipped)
    {
        serial    =  is_delete ? DBGETDLT_SERIAL_ZIPPED : DBGET_SERIAL_ZIPPED;
    }
    DBG("current transfer type is %d returning %d", trans[index]->req_type, serial);
    return serial;
}


static int trans_get_transfer_type_set_PARALLEL(int index)
{
     int is_delete = IS_REQUEST_GET_DELETE(trans[index]->req_type);
     int parallel  = is_delete ? DBGETDLT_PARALLEL: DBGET_PARALLEL;
     DBG("current transfer type is %d returning %d", trans[index]->req_type, parallel);
     return parallel;
}


static int trans_get_transfer_type_set_SEPARATE(int index)
{
     int is_delete = IS_REQUEST_GET_DELETE(trans[index]->req_type);
     int separate  = is_delete ? DBGETDLT_SEPARATE : DBGET_SEPARATE;
     DBG("current transfer type is %d returning %d", trans[index]->req_type, separate);
     return separate;
}


static int trans_get_transfer_type_set_ZIPPED(int index)
{
    int is_delete = IS_REQUEST_GET_DELETE(trans[index]->req_type);
    int zipped    = is_delete ? DBGETDLT_ZIPPED : DBGET_ZIPPED;
    DBG("current transfer type is %d returning %d", trans[index]->req_type, zipped);
    return zipped;
}



/* Set the transfer type */
int trans_get_transfer_type_set(int index, char transfer_type)
{
    extern int gl_is_web;

    DBG("index='%d', gl_is_web=%d transfer_type='%c'", index, gl_is_web, transfer_type);

    /* Verify this is a get request */
    if (trans[index]->transaction_type != GET_REQUEST)
    {
	    strcpy(rtn.msg, get_req_err);
        DBG("GET_REQ_ERR");
        return(GET_REQ_ERR);
    }

    switch(transfer_type)
    {
      case 'l':
         trans[index]->req_type = trans_get_transfer_type_set_PARALLEL(index);
      break;
      case 's':
         trans[index]->req_type = trans_get_transfer_type_set_SERIAL(index);
      break;
      case   0:
         // nothing to do, old clients such as 9.3.3 or older used to set  DBGET_SERIAL
         // that is why paralell get used to not work
      break;
      case 'r':
         trans[index]->req_type = trans_get_transfer_type_set_SEPARATE(index);
      break;
      case 'z':
	    /* Indicate data will not be uncompressed */
	    trans[index]->uncompress_during_transfer = DBNO;
        trans[index]->req_type  = trans_get_transfer_type_set_ZIPPED(index);
      break;
      default:
      {
        DBG("invalid transfer_type=%d", transfer_type);
	    strcpy(rtn.msg, invalid_transfer_type);
        DBG("INVALID_TRANSFER_TYPE");
        return(INVALID_TRANSFER_TYPE);
      }
      break;
    } /* end switch */
    if (gl_is_web)
    {
        trans[index]->req_type = trans_get_transfer_type_set_SERIAL(index);
    }
    DBG("SUCCESS");
    return(SUCCESS);
} /* end trans_get_transfer_type_set */

/* Retrieve file with alternate file name */
int trans_alternate_file_name_set(int index, char *file_name)
{
	trans_field_allocate(&trans[index]->alternate_file, file_name);
    DBG("SUCCESS");
    return(SUCCESS);
} /* end trans_alternate_file_name_set */

/* Set directory where file retrieved */
int trans_alternate_dir_set(int index, char *dir_name)
{
	trans_field_allocate(&trans[index]->alternate_dir, dir_name);
    DBG("SUCCESS");
    return(SUCCESS);
} /* end trans_alternate_dir_set */

/* Print warning if parallel type requested but not performed */
void parallel_warning_print(TRANS_FILE_TYPE file_type, unsigned long long size, int index)
{
    DBG();

    /* If standard out suppressed, simply return */
    if (trans[index]->display_transfer_status == 0) return;
    if (file_type == DIRECTORY)
    PRINTF(ll_dir_wrn); 
    else if (file_type == FILES)
    PRINTF(ll_files_wrn); 
    else if (size < 1024*1024*5)
    PRINTF(ll_5mb_wrn);
    else PRINTF(ll_wrn);
} /* end parallel_warning_print */

/* See if file already exists */
/* if it already exists, ask user if he wants to overwrite it */
int get_delete_check(char *file_name, char *alternate_dir, char *alternate_file, TRANS_FILE_TYPE file_type)
{
    DBG();

    char c_string[8];
    char *file_ptr; 
    char full_file_name[PATH_SIZE];
    int ret;

    /* Build the complete file name */
    alternate_file_name_bld(full_file_name, file_name, alternate_dir, alternate_file);
    /* Point to the root file name */
    file_ptr = root_find(full_file_name, '/');
    if (access(full_file_name, 0) == 0) 
    {
	if (file_type != DIRECTORY)
	printf("File '%s' already exists.  Overwrite this file, y/n?  ", file_ptr);
	else PRINTF("Directory '%s' already exists.  Overwrite this directory, y/n?  ", file_ptr);
	fflush(stdout);
	scanf("%s", c_string);
	printf("\n");
	if (( c_string[0] == 'y') || (c_string[0] == 'Y')) 
	{
	    /* Delete the file and indicate files have been overwritten */
	if (file_type != DIRECTORY)
	    unlink(full_file_name);
	else dir_dlt(full_file_name);
	} /* End if */
	else
	{
	    /* Tell server unable to create file */
	    put_ssl_short(BAD_CREATE);
	ret = get_ssl_short();
	get_ssl_string(rtn.msg);
	    close(sock);
	return(-1);
	}
    } /* End if for file already present */
    return(SUCCESS);
} /* End get_delete_check */

int auto_get_delete_check(int index)
{
    DBG();

    if (access(trans[index]->file_name, 0) == 0) 
    {
    if (trans[index]->overwrite_files == DBYES)
	{
	    /* Delete the file and indicate files have been overwritten */
#ifndef IRIX
	    unlink(trans[index]->file_name);
#else
	strcpy2(buffer, "rm -r -f ", trans[index]->file_name);
	system(buffer);
#endif /* For not IRIX */
	return(SUCCESS);
	} /* End if */
	else
	{
	    /* Tell server unable to create file */
	    put_ssl_short(BAD_CREATE);
	    close(sock);
	sprintf(rtn.msg, already_present_err, trans[index]->file_name);
	return(ALREADY_PRESENT_ERR);
	}
    } /* End if for file already present */
    return(SUCCESS);
} /* End auto_get_delete_check */

/* build alternate directory name */
void trans_alternate_dir_name_build(char *alternate_dir)
{
    DBG();

    /* If this is a full path, replace current path with new path */
    if (alternate_dir[0] == '/')
        strcpy(user.cwd, alternate_dir);
    else strcpy2(user.cwd+strlen(user.cwd), "/", alternate_dir);
}

short trans_get_init(int index, char *alternate_dir)
{
    extern float compression_ratio;
    char tmp_file_type;
    /*int compression;*/
    int secondary_file_type;
    int num_args;
    char separate_size_string[12];
    short ret;
    int parallel_ret;

    DBG();

    /* Make sure valid keyword entered */
    /* Connect to the Transcend/InterCom database */
    sprintf(separate_size_string, "%llu", transrc.separate_block_size);
    if ((parallel_ret = trans_socket_init(index)) < SUCCESS) 
        return(parallel_ret);

    /* Get the file name, size, mode, and file type  */
    ret = get_ssl_string(rtn.msg);
    /* If 0 length file name, give error */
    /* User not allowed to retrieve file with 0 length file name */
    if (ret == 0)
    {
	    sprintf(rtn.msg, "%s%s%s\n", zero_len_file_err, DOUBLE_RET, HELP_MSG);
        DBG("ZERO_LEN_FILE_ERR");
        return(ZERO_LEN_FILE_ERR);
    }
    /* At this point we have the file name, mode and file type */
    /* Scan out data */
    num_args = sscanf(rtn.msg, "%s %llu %lu %c %d %d %f %d %s", 
    trans[index]->file_name, &trans[index]->original_size, &trans[index]->file_mode, &tmp_file_type, &trans[index]->uncompress_during_transfer, 
    &trans[index]->req_type, &compression_ratio, &secondary_file_type, trans[index]->icap_class);
    /* If alternate directory name specified, use it */
    if (trans[index]->alternate_dir != NULL)
    trans_alternate_dir_name_build(trans[index]->alternate_dir);
    /* If alternate file name specified, use it */
    if (trans[index]->alternate_file != NULL)
    strcpy(trans[index]->file_name, trans[index]->alternate_file);
    /* Put spaces back in file name */
    strexc(trans[index]->file_name, '`', ' ');
    /* Make sure compression ratio is not set to 0 */
    if ((compression_ratio == 0)  || (compression_ratio > 20))
    compression_ratio = 1;
    trans[index]->file_type = (TRANS_FILE_TYPE) tmp_file_type;
    /* If secondary file type not scanned in, assign it */
    if (num_args <= 6) secondary_file_type = trans[index]->file_type;
    if (num_args < 7) strcpy(trans[index]->icap_class, "fcp");
    /* Check to see if file already exists */
    if (trans[index]->overwrite_files == DBNO)
	ret = get_delete_check(trans[index]->file_name, user.cwd,  trans[index]->alternate_file, trans[index]->file_type);
    /* If file present, delete without prompting user */
	else ret = auto_get_delete_check(index);
    if (ret < 0)
    {
         DBG("auto_get_delete_check() returned ret='%d'", ret);
         return(ret);
    }
    else
    {
        return(parallel_ret);
    }
} /* end trans_get_init */
 

int trans_get_data(int index, int parallel_ret)
{
    extern char tar_option;
    extern float compression_ratio;
    int parallel_type;
    int ret;

    DBG();

    /* Get the file */
    /* Give warning if parallel transfer denied */
    if ((parallel_type == DBGET_PARALLEL) && (parallel_ret == 0))
    parallel_warning_print(trans[index]->file_type, trans[index]->original_size, index);
/* Currently not supporting directory transfer for windows */
#ifdef WINDOWS_NT4
#ifdef do_tar
    /* If this is a directory or group of files, retrieve it with libarchive */
    if ((tr->file_type == DIRECTORY) || (tr->file_type == FILES))
    ret = get_archive(tr->file_name, tr->original_size);
    else
#endif /* for do_tar */
#endif /* for Windows */
/* currently not supporting parallel transfer for Windows */
#ifndef WINDOWS_NT4
    /* If 1 returned, the server is returning parallel compressed data */
    /* The client can retrieve all blocks in parallel or one block at a time */
    if (parallel_ret == PARALLEL) 
    {
	/* Server does not distinguish between parallel blocks and serial blocks */
	if (tar_option == 'r')
	ret = trans_get_separate_compressed_data(index);
	else ret = trans_get_parallel_compressed(index);
    } /* End if */
    else if (ret == DBGET_RANDOM_PARALLEL)
    ret = trans_get_parallel_uncompressed_data(index);
    else 
#endif /* for WINDOWS_NT4 */
    /* Sending secondary file type so will be opened correctly */
ret = trans_file_get(index);
#ifdef tar_problem
    if (trans[index]->file_type == DIRECTORY)
    {
	 rtn.len = strlen(rtn.msg);
	sprintf(rtn.msg+rtn.len, nt_get_wrn, trans[index]->file_name);
    }
    else if (trans[index]->file_type == FILES)
    {
	 rtn.len = strlen(rtn.msg);
	sprintf(rtn.msg+rtn.len, files_get_wrn, trans[index]->file_name);
    }
#endif /* For tar problem */
    DBG("ret='%d'", ret);
    return(ret);
} /* end trans_get_data */

/* Perform a get request */
int trans_get_request(int index)
{
    DBG();

     int ret = trans_get_init(index, NONE);
	if (ret < 0) return(ret);
#ifndef WINDOWS_NT4
    /* See if transfer will be done in background */
    if (trans[index]->perform_background_transfer == DBYES)
    {
	/* Initialize the shared memory */
	int mem_key = shared_status_create(trans[index]->keyword);
	/* Allocate memory */
	shared_status_allocate(mem_key);
	/* Indicate not finished and 0 bytes transferred */
	shared_status_write(1, 0);
	/* If this is the parent, simply return */
	if (fork() != 0)  return(SUCCESS);
    } /* end if for performing background transfer */
#endif
	    ret = trans_get_data(index, ret);
#ifndef WINDOWS_NT4
    if (trans[index]->perform_background_transfer == DBYES)
    {
	shared_status_finish(0);
	/* Release the shared memory */
	shared_status_release();
    }
#endif
    DBG("ret='%d'", ret);
    return(ret);
} /* end trans_get_request */

/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
