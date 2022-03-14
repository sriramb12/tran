/****************************************************************************
 *
 * Function:	file_copy
 *
 * Description:	Copies old file into new file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "close_down.h"
#include "dir.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "global_debug.h" // Mazieri's debug facilities


void file_copy(char *old_file, char *new_file)
{
    int fi;
    int fo;
    register int done;
    int i;

    DBG("old_file='%s' and new_file='%s'", old_file, new_file);

    /* Open the old file for reading */
    if ((fi = open(old_file, O_RDONLY)) < 0)
    { 
        sprintf(rtn.msg, "Error no is %d\n", errno);
	    data_log(tr.log_file, rtn.msg);
        close_down1m(rd_open_err, old_file,  RD_OPEN_ERR);
    }

    /* Delete the new file if it already exists */
    unlink(new_file);

    /* Open the new file for writing */
    if ((fo = open(new_file,  O_WRONLY | O_CREAT, 0664)) < 0)
        close_down1m(wr_open_err, new_file,   WR_OPEN_ERR);

    done = DBNO;
    
    while (done == DBNO)
    {
	    i = read(fi, bf.buffer, BLOCK_SIZE);
	    if (i > 0)
	    {
	        bf.buffer[i] = 0;
	        write(fo, bf.buffer, i);
	    }
	    else done = DBYES;
    }

    close(fi);
    close(fo);
} /* End file_copy*/


void file_copy777(char *old_file, char *new_file)
{
    int fi;
    int fo;
    register int done;
    int i;

    DBG("old_file='%s' and new_file='%s'", old_file, new_file);

    /* Open the old file for reading */
    if ((fi = open(old_file, O_RDONLY)) < 0)
    { 
        sprintf(rtn.msg, "Error no is %d\n", errno);
	    data_log(tr.log_file, rtn.msg);
        close_down1m(rd_open_err, old_file,  RD_OPEN_ERR);
    }

    /* Delete the new file if it already exists */
    if (access(new_file, 0) == 0)
        unlink(new_file);

    /* Open the new file for writing */
    if ((fo = open(new_file,  O_WRONLY | O_CREAT, 0777)) < 0)
        close_down1m(wr_open_err, new_file,   WR_OPEN_ERR);

    done = DBNO;
    while (done == DBNO)
    {
	    i = read(fi, bf.buffer, BLOCK_SIZE);
	    if (i > 0)
	    {
	        bf.buffer[i] = 0;
	        write(fo, bf.buffer, i);
	    }
	    else done = DBYES;
    }

    close(fi);
    close(fo);
} /* End file_copy*/


void file_copy770(char *old_file, char *new_file)
{
    int fi;
    int fo;
    register int done;
    int i;

    DBG("old_file='%s' and new_file='%s'", old_file, new_file);

    /* Open the old file for reading */
    if ((fi = open(old_file, O_RDONLY)) < 0)
    { 
        sprintf(rtn.msg, "Error no is %d\n", errno);
	    data_log(tr.log_file, rtn.msg);
        close_down1m(rd_open_err, old_file,  RD_OPEN_ERR);
    }

    /* Delete the new file if it already exists */
    if (access(new_file, 0) == 0) unlink(new_file);

    /* Open the new file for writing */
    if ((fo = open(new_file,  O_WRONLY | O_CREAT, 0770)) < 0)
        close_down1m(wr_open_err, new_file,   WR_OPEN_ERR);

    done = DBNO;
    while (done == DBNO)
    {
	    i = read(fi, bf.buffer, BLOCK_SIZE);
	    if (i > 0)
	    {
	        bf.buffer[i] = 0;
	        write(fo, bf.buffer, i);
	    }
	    else done = DBYES;
    }

    close(fi);
    close(fo);
} /* End file_copy*/
