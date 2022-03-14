/****************************************************************************
 *
 * Function:	files_get
 *
 * Description:	Receives files from the client or remote server.
 *		Creates directory where files are placed 
 *		if it is not already present.
 *		Puts names of files received in array 'files'.
 *
 *		Protocol is as follows.
 *		Retrieves file name followed by file data followed by SUCCESS.
 *		Repeats process until all files received.
 *		Gets extra SUCCESS after last file.
 *		Sends ACK at end to indicate all files received.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "dir.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "files.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities


/* Basic routine used to retrieve files */
int files_get(char *files[], char *dir)
{
    int num;
    extern TMP_BUFFER bf;
    short ret;
    short fd;
    char *file_ptr;	/* Points to end of path name */
    short get_string();

    DBG();

    /* Build directory name where files will be placed */
    file_ptr = bf.file_name + strcpy2(bf.file_name, dir, "/");

    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(dir, 0) != 0) dir_create(dir);

    num = 0;
    
    /* Retrieve the files in a while loop */
    while (1)
    {
	    /* First get file name */
	    /* If no more files to get, get out of loop */
	    if ((ret = get_string(file_ptr)) == 0) break;

	    /* Open file for writing */
	    if ((fd = open(bf.file_name, O_WRONLY|O_CREAT, 0600)) == EOF)
        {
	        close_down1m(rd_open_err, bf.file_name,  RD_OPEN_ERR);
        }

	    /* Read in blocks of text and write them out to the file */
	    while (1)
	    {
	        /* Read a block from the file */
	        ret = get_string(bf.buffer);
	        if (ret == 0) break;
	        
	        /* Write the text out to the file */
	        write(fd, bf.buffer, ret);
	    } /* end while */

	    /* Close the file just created */
	    close(fd);

	    /* Copy file name into file array */
	    strcpy(files[num], file_ptr);
	    ++num;
    } /* end while */

    /* If reached this point, received all files */
    /* Let server or client know received all data */
    put_short(SUCCESS);

    /* Restore file name to original state */
    file_ptr[0] = 0;

    return(num);
} /* end files_get */
