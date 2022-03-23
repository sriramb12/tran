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
     #include <unistd.h>
     #include <sys/uio.h>
#include <netinet/in.h>
#include <string.h>
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "rtn.h"
#include "strcpy.h"
#include "server_lib.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_defines.h"
#include "user_params.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

/* build string to send to the client */


/* Send one file to the client */
int send_file(TRANS_FILE_STAT *fs, char *key, int dtms)
{
    DBG();

    extern char transfertype[];
    int tar_option = DBNO;
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */
#ifdef TRANSWEB
    char *real_file_ptr;
    char *ptr;
#endif

    /* Build complete name of file */
    if (strcmp(server.site, DTMS) == 0)
    strcpy4(bf.file_name, disk.path[fs->disk_index], key, ".gz/", fs->real_name);
    else strcpy3(bf.file_name, disk.path[fs->disk_index], key, ".gz");
    /* Obtain a file descriptor to file */
    fd = open(bf.file_name, O_RDONLY);
    if (fd == EOF)
    return_non_err(key);
    /* Indicate that file wil be sent */
    put_short(SUCCESS);
#ifdef TRANSCEND
    /* Build string to send to client */
    file_string_build(NONE, bf.buffer);
    put_string(bf.buffer);
#endif
#ifdef TRANSWEB
    /* Build string to send to client */
    /* Exchange spaces for ` */
    real_file_ptr =  malloc(strlen(fs->real_name) +5);
    strcpy_ex(real_file_ptr, fs->real_name, '`', ' ');
    if (fs->file_type == FILES)
    {
	    ptr = strchr(real_file_ptr, '`');
	if (ptr != NULL) ptr[0] = '\0';
	strcat(real_file_ptr, ".tar");
    } /* End if for files */
    else if (fs->file_type == DIRECTORY)
    strcat(real_file_ptr, ".tar");
    if (fs->compression == 0) fs->compression = DBYES;
    /* If sending the file as a zipped file */
    if (gu.req_type == DBGET_ZIPPED)
    strcat(real_file_ptr, ".gz");
     sprintf(bf.buffer, "%s %lld %ld %c %d %s", real_file_ptr, fs->original_size, fs->mode, fs->file_type, fs->compression, fs->tar_type);
    free(real_file_ptr);
    put_string(bf.buffer);
#endif /* for TransWeb */
    /* Get acknowledgement */
    if ((ret = get_short()) != SUCCESS)
    {
	/* If unsuccessful, log failure, clean-up and exit */
	close(fd);
	strcpy3(rtn.msg, "Retrieval of '", fs->real_name, "' aborted.");
	put_success_msg(NONE);
	exit(0);
    }
    /* Calculate the starttime */
    start_transfertime_calc();
    /* Read text from the file and write it to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0) 
     put_binary_string(bf.buffer, ret);
    /* Indicate all data sent */
    put_short(SUCCESS);
    /* Calculate the end time */
    end_transfertime_calc();
    /* Initialize transfer type for logging purposes */
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
    DBG("name='%s', len='%d'", name, len);

    int extension = 0;
    int cnt;
    char tmp_name[FILE_SIZE];

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
/*ts.files.fileuse*/
