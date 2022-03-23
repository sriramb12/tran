/****************************************************************************
 *
 * Function:	email_dir_rtv
 *
 * Description:	Maps encrypted keyword to real keyword
 *		name and  machine name as keywords and the  time 
 *		stamp as the seed for the encryption.
 *		Builds the doubly encrypted keyword using the
 *		receiving user's Email address as the encryption key.
 *		Sends the encrypted keyword to the receiving user.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

     #include <sys/types.h>
     #include <sys/stat.h>
     #include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "company.h"
#include "dir.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "keyword.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "reg_globals.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_lib_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h"

/* Retrieve email directory and send email notification to users */
void email_dir_rtv(void)
{
    char tar_file[64];
    char command[256];
    char old_file_name[FILE_SIZE];
    char new_file_name[FILE_SIZE];
    char *old_ptr = old_file_name + strcpy1(old_file_name, "email_dir/");
    char *new_ptr = new_file_name + strcpy1(new_file_name, "email_dir.send/");
    int cnt;
    char *files[1000];

    DBG();

    sprintf(tar_file, "email_dir%lu.tar", tr.upd_time);
    /* Make list of all files in the directory */
    if (dir_list("email_dir", files) < 1)
    shut_down(no_email, NO_EMAIL);
    /* Make the temporary directory */
    dir_create(new_file_name);
    /* Go through list moving the files */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	/* build file names */
	strcpy(old_ptr, files[cnt]);
	strcpy(new_ptr, files[cnt]);
	/* do the move */
	file_copy(old_file_name, new_file_name);
	unlink(old_file_name);
    }
    /* Tar up the directory */
    strcpy3(command, "tar cf ", tar_file, " email_dir.send");
    system(command);
    /* Make sure the tar file was created */
    if (access(tar_file, 0) != 0)
    close_down1f(non_err, tar_file, NON_ERR);
    put_short(SUCCESS);
    /* Send the file */
    get_tar_file(tar_file, tar_file);
    /* Delete the temp directory */
    new_ptr[0] = '\0';
    dir_remove(new_file_name);
    unlink(tar_file);
    /* Send the server the return code */
    sprintf(rtn.msg, "Sent email notification to %d customers.", cnt);
    put_success_msg("");
} /* End email_dir_rtv */

void get_tar_file(char *file_name, char *root_file)
{
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */

    DBG();
    /* Obtain a file descriptor to file */
    fd = open(file_name, O_RDONLY);
    if (fd == EOF)
    close_down1f(non_err, file_name, NON_ERR);
    /* Indicate that file wil be sent */
    put_short(SUCCESS);
    /* Send name of file */
    /* Pass root name only without full path */
    put_string(root_file);
    /* Get acknowledgement */
    if ((ret = get_short()) != SUCCESS)
    /* If unsuccessful, log failure, clean-up and exit */
    shut_down(bad_ack, BAD_ACK);
    /* Read text from the file and write them to the socket */
    while ((ret = read(fd, bf.buffer, BLOCK_SIZE))  > 0)
    put_binary_string(bf.buffer, ret);
    /* Tell client all data for this file sent */
    put_short(SUCCESS);
    /* Close the file just sent */
    close(fd);
} /* end get_tar_file */
/*es.files.fileuse*/
/*is.files.fileuse*/
