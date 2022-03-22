/****************************************************************************
 *
 * Function:	add_file_line
 *
 * Description:	Adds new line to specified file.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *file_name - Complete path name of file entry being added to
 *  char *entry - Name of entry to be added 
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "user_params.h"
#include "rtn.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

int add_file_line(char *file_name, char *entry)
{
    char buffer[BUFFER_SIZE];
    FILE *fi;
    FILE *fo;
    register int cnt;
    register int done;
    register int ret;

    DBG("file_name='%s' and entry='%s'", file_name, entry);

    /* Open the file entry being added to */
    if ((fi = fopen(file_name, "r")) == NULL)
    {
	    /* Create the file and add the entry */
	    data_log(file_name, entry);

        DBG("entry='%s' added to file_name='%s'", entry, file_name);
	    return(SUCCESS);
    }

    /* Construct the temporary file name */
    strcpy6(bf.tmp_file, user.email, "_", user.mach, "_", "test", "_tmp_file");

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    strcpy7(rtn.msg, wr_open_err, RET2TAB, "'", bf.tmp_file, "'",  DOUBLE_RET, help_msg);
	    return(WR_OPEN_ERR);
    }

    done = DBNO;

    while (1)
    {
	    /* Pull in a line of text */
	    /* Point to the first character on the line */
        cnt = 0;
	    while (((buffer[cnt] = getc(fi)) != '\n') && (buffer[cnt] != EOF) && (cnt < 260)) {
	        ++cnt;
        }

	    /* If reached end of file, put in new entry */
	    if (cnt == 0) 
	    {
	        fprintf(fo, "%s\n", entry);
	        done = DBYES;
	        break;
	    }

	    /* Put null on end of string */
	    buffer[cnt] = '\0';

	    /* If buffer entry less than new entry, write out the buffer entry */
	    if ((ret = strcmp(entry, buffer)) > 0)
	    {
	        fprintf(fo, "%s\n", buffer);
	    }
	    /* Otherwise write out new entry followed by buffer entry */
	    else if (ret < 0)
	    {
	        fprintf(fo, "%s\n", entry);
	        fprintf(fo, "%s\n", buffer);
	        break;
	    } /* end else */
	    /* Otherwise inform user entry already exists */
	    /* At this point ret will equal 0 */
	    /* If entry already exists, return */
	    else
	    {
	        fclose(fi);
	        fclose(fo);

	        /* Delete the temporary file */
	        unlink(bf.tmp_file);

            DBG("entry='%s' is duplicated for the file_name='%s'", entry, file_name);
	        return(DUP_ENTRY);
	    } /* end else */
    } /* end while */

    /* Write remaining entries out to the temporary file */
    if (done == DBNO)
    {
	    while ((ret = fread(buffer, sizeof(*buffer), BUFFER_SIZE, fi)) != 0)
        {
            buffer[ret] = 0;
	        fwrite(buffer, sizeof(*buffer), ret, fo);
        }
    } /* end if */

    /* Close both files */
    fclose(fi);
    fclose(fo);

    /* Write the temporary file over the old file */
    rename(bf.tmp_file, file_name);

    return(SUCCESS);
} /* End add_file_line */
