/****************************************************************************
 *
 * Function:	dbsend_files
 *
 * Description:	Basic function to send one or more files to the database client.
 *		Files to send are passed in a file array with one or more elements.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities


/* Send a cl.group of files to the client */
void dbsend_files(char *dir, char *file_array[], register short num)
{
    extern TMP_BUFFER bf;
    char *file_ptr;	/* Points to first empty character in file path */
    register int i;
    int fd;		/* File descriptor */
    short ret;		/* Holds return code */

    DBG("dir='%s'", dir);

    /* Initialize the file path */
    file_ptr = bf.file_name + strcpy2(bf.file_name, dir, "/");
    for (i = 0; i < num; ++i)
    {
	    /* Build complete file name to be opened */
	    strcpy(file_ptr, file_array[i]);

	    /* Obtain a file descriptor to file */
	    fd = open(bf.file_name, O_RDONLY);
        if (fd == EOF)
            shut_down(no_data, TRANS_NO_DATA);

	    /* Send name of file */
	    /* Pass root name only without full path */
	    put_string(file_array[i]);

	    /* Get acknowledgement */
        /* If unsuccessful, log failure, clean-up and exit */
        if ((ret = get_short()) != SUCCESS)
            clean_up(bad_ack);

	    /* Read text from the file and write them to the socket */
	    while ((ret = read(fd, bf.buffer, BLOCK_SIZE)) > 0) 
	        put_binary_string(bf.buffer, ret);

	    /* Tell client all data for this file sent */
	    put_short(SUCCESS);

	    /* Close the file just sent */
	    close(fd);
    } /* end for statement */
} /* end dbsend_files */
