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
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_debug.h" // Mazieri's debug facilities

void file_append(char *old_file, char *new_file)
{
    register int fi;
    register int fo;
    register int done;
    int i;

    DBG("old_file='%s' and new_file='%s'", old_file, new_file);

    /* Open the old file for reading */
    if ((fi = open(old_file, O_RDONLY)) < 1)
        close_down1m(rd_open_err, old_file,  RD_OPEN_ERR);

    /* Open the new file for writing */
    if ((fo = open(new_file,  O_WRONLY | O_APPEND, 0600)) < 1)
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
} /* End file_append*/


/* Append info to the message file */
void msg_file_append(char *msg_string, char *old_file, char *new_file, char *last_line)
{
    FILE *fi;
    FILE *fo;
    char line[260];

    DBG("msg_string='%s', old_file='%s', new_file='%s', last_line='%s'", msg_string, old_file, new_file, last_line);

    /* Open the old file for reading */
    if ((fi = fopen(old_file, "r")) == NULL) return;

    /* Open the new file for writing */
    if ((fo = fopen(new_file,  "a")) == NULL)
        close_down1m(wr_open_err, new_file,   WR_OPEN_ERR);

    while (line_get(fi, line) != EOF)
    {
	    fprintf(fo, "%s : %s\n", msg_string, line);
	    strcpy(last_line, line);
    }

    fclose(fi);
    fclose(fo);
} /* End msg_file_append*/
