/****************************************************************************
 *
 * Function:	send_file
 *
 * Description:	Basic function to send one or more files to the database client.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_defines.h"
#include "user_params.h"
#include "global_debug.h"

/* Send one file to the client */
int send_file(TRANS_FILE_STAT *fs)
{
    extern char transfertype[];
    int global_cnt;
    int tar_option = DBNO;
    char *real_name_ptr;
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */

    DBG();

    /* Build complete name of file */
    strcpy3(bf.file_name, disk.path[fs->disk_index], fs->key, ".gz");
    /* Obtain a file descriptor to file */
    fd = open(bf.file_name, O_RDONLY);
    if (fd == EOF)
    return_non_err(fs->key);
    /* Indicate that file wil be sent */
    put_short(SUCCESS);
#ifdef INTERCOM
    /* Build string to send to client */
    /* Exchange spaces for ` */
    real_name_ptr =  malloc(strlen(fs->real_name) +1);
    strcpy_ex(real_name_ptr, fs->real_name, '`', ' ');
    file_string_build(NONE, bf.buffer);
    put_string(bf.buffer);
#endif
#ifdef TRANS_FTP
    /* Build string to send to client */
    /* Exchange spaces for ` */
    real_name_ptr =  malloc(strlen(fs->real_name) +1);
    strcpy_ex(real_name_ptr, fs->real_name, '`', ' ');
    sprintf(bf.buffer, "%s %lld %ld %c %d %s",
    real_name_ptr, 
    fs->original_size, fs->mode, fs->file_type, fs->compression, fs->tar_type);
    put_string(bf.buffer);
#endif
#ifdef TRANSWEB
    /* Build string to send to client */
    /* Exchange spaces for ` */
    real_name_ptr =  malloc(strlen(fs->real_name) +5);
    strcpy_ex(real_name_ptr, fs->real_name, '`', ' ');
    if (fs->file_type == FILES)
    {
    char *ptr;
	    ptr = strchr(real_name_ptr, '`');
	if (ptr != NULL) ptr[0] = '\0';
	strcat(real_name_ptr, ".tar");
    } /* End if for files */
    else if (fs->file_type == DIRECTORY)
    strcat(real_name_ptr, ".tar");
    if (fs->compression == 0) fs->compression = DBYES;
     sprintf(bf.buffer, "%s %lld %ld %c %d %s", real_name_ptr, fs->original_size, fs->mode, fs->file_type, fs->compression, fs->tar_type);
    free(real_name_ptr);
    put_string(bf.buffer);
#endif /* for TransWeb */
    /* Get acknowledgement */
    if ((ret = get_short()) != SUCCESS)
    /* If unsuccessful, log failure, clean-up and exit */
    shut_down(bad_ack, BAD_ACK);
    /* Calculate the start time */
    start_transfertime_calc();
    /* Read text from the file and write it to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0) 
    {
    ++global_cnt;
     put_binary_string(bf.buffer, ret);
    }
    /* Indicate all data sent */
    put_short(SUCCESS);
    /* Calculate the end time */
    end_transfertime_calc();
    /* Initialize transfer type for loggin purposes */
    strcpy(transfertype, "Serial get");
    /* Close the file just sent */
    close(fd);
    /* Get final ack from client */
    if (get_short() != SUCCESS)
    {
	sprintf(rtn.msg, abort_err, fs->real_name);
	shut_down(rtn.msg, ABORT_ERR);
    }
    return(tar_option);
} /* end send_file */

/* Shorten name of file to 32 characters */
void name_shorten(char *name, int len)
{
    int extension = 0;
    int cnt;
    char tmp_name[FILE_SIZE];

    DBG();

    /* See if name has an extension */
    for (cnt = 2; cnt <6; ++cnt)
    {
	if (name[len-cnt] == '.') 
	{
	extension = cnt;
	    break;
	} /* End if for extension found */
    } /* End for */
    if (extension > 0)
    {
	strcpy(tmp_name, name);
	strcpy(name+32-extension, tmp_name+len-extension);
    } /* End if */
    name[32] = '\0';
}
/*es.files.fileuse*/
/*is.files.fileuse*/
