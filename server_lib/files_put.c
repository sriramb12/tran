/****************************************************************************
 *
 * Function:	files_put
 *
 * Description:	Basic function to send one or more files to the database client.
 *		Does give final ACK indicating all files sent.
 *
 *		Protocol is as follows.
 *		Sends file name followed by file data followed by ACK.
 *		Repeats this process until all files sent.
 *		Sends extra ACK after last file.
 *		Gets ACK at end to see if remote server or client
 *		received all of the files.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "strcpy.h"
#include "server_lib.h"
#include "global_debug.h" // Mazieri's debug facilities

/* Send a group of files to the client */
int files_put(char *files[], char *dir)
{
    extern TMP_BUFFER bf;
    char *file_ptr;	/* Points to first empty character in file path */
    register int i;
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */

    DBG();

    /* Initialize the file path */
    file_ptr = bf.file_name + strcpy2(bf.file_name, dir, "/");

    /* Open the file to be sent back to the client */
    for (i = 0; files[i] != NULL; ++i)
    {
	    /* Build complete file name to be opened */
	    strcpy(file_ptr, files[i]);
	
	    /* Obtain a file descriptor to file */
	    fd = open(bf.file_name, O_RDONLY);
        
        if (fd == EOF) close_down1m(rd_open_err, bf.file_name,  RD_OPEN_ERR);
	
	    /* Send name of file */
	    /* Pass root name only without full path */
	    put_string(files[i]);

	    /* Read text from the file and write it to the socket */
	    while ((ret = read(fd, bf.buffer, MSG_SIZE)) > 0) 
	    {
	        put_binary_string(bf.buffer, ret);
	    } /* End while for sending one file */

	
	    /* Tell client all data for this file sent */
	    put_short(SUCCESS);

	    /* Close the file just sent */
	    close(fd);
    } /* end for statement */

    /* Send final ACK indicating all files sent */
    put_short(SUCCESS);

    /* Get ack from the remote server or client */
    return(get_short());
} /* end files_put */
