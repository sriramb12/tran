/****************************************************************************
 *
 * Function:	files_rcv
 *
 * Description:	Receives files from the client or remote server.
 *		Puts names of files received in array 'files'.
 *		Creates the directory where files are put. 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
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
int old_files_rcv(DB_FILES *file)
{
    DBG();

    extern TMP_BUFFER bf;
    short ret;
    short fd;
    char *file_ptr;	/* Points to end of path name */

    /* Build directory name where files will be placed */
    file_ptr = bf.file_name + strcpy2(bf.file_name, file->dir, "/");

    /* Make the directory where the files will be placed */
    /* if it does not already exist */
    if (access(file->dir, 0) != 0)
        dir_create(file->dir);

    file->num =0;

    /* Indicate ready to receive files */
    /* put_short(SUCCESS); */

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
	    /* Tell client ready to get text */
	    put_short(SUCCESS);

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
	    strcpy(file->name[file->num], file_ptr);
	    ++file->num;
    } /* end while */

    /* Restore file name to original state */
    file_ptr[0] = 0;

    return(file->num);
} /* end files_rcv */
