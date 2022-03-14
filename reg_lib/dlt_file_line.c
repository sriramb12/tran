/****************************************************************************
 *
 * Function:	dlt_file_line
 *
 * Description:
 *  Looks for a line that matches specified entry.
 *	If line is found, it is deleted.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *file_name - Complete path name of file entry being deleted from
 *  char *entry
 *
 ***************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "user_params.h"
#include "rtn.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

int dlt_file_line(char *file_name, char *entry)
{
    FILE *fi;
    FILE *fo;
    register int cnt;
    register int ret;

    DBG("file_name='%s', entry='%s'", file_name, entry);

    /* Open the file entry being deleted from */
    if ((fi = fopen(file_name, "r")) == NULL)
        close_down1q(rd_open_err, file_name, RD_OPEN_ERR);

    /* Construct the temporary file name */
    strcpy6(bf.tmp_file, user.email, "_", user.mach, "_", "test", "_tmp_file");

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    strcpy7(rtn.msg, wr_open_err, RET2TAB, "'", bf.tmp_file, "'",  DOUBLE_RET, help_msg);
	    return(WR_OPEN_ERR);
    }

    /* Pull in text and write it out until right line found */
    while (1)
    {
	    /* Pull in a line of text and write it out */
        cnt = 0;

	    while (((bf.buffer[cnt] = getc(fi)) != '\n') && (bf.buffer[cnt] != EOF) && (cnt < 260))
	        ++cnt;

	    /* If reached end of file, leave lloop */
	    if (cnt == 0) 
	    {
	        /* If didn't find the entry, just keep the original file */
	        /* Close both files */
	        fclose(fi);
	        fclose(fo);

	        /* Get rid of the temporary file */
	        unlink(bf.tmp_file);

	        /* Return to the calling function */
	        return(SUCCESS);
        }

	    bf.buffer[cnt] = '\0';

	    if (strcmp(bf.buffer, entry) == 0) break;

        /* Write line out to temporary file */
	    fprintf(fo, "%s\n", bf.buffer);
    } /* end while */

    /* Write remaining entries out to file */
    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
        fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);

    /* Close both files */
    fclose(fi);
    fclose(fo);

    /* Write the temporary file over the old file */
    rename(bf.tmp_file, file_name);

    return(SUCCESS);
} /* End dlt_file_line */
