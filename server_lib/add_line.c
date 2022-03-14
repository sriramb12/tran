/****************************************************************************
 *
 * Function:	add_line
 *
 * Description:	Adds new line to a file keeping the file
 *		in alphabetical order.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *    char *file_name - Complete path name of file entry being added to
 *    char *entry     - Name of entry to be added 
 ***************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "user_params.h"
#include "rtn.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities


int add_line(char *file_name, char *entry)
{
    FILE *fopen();
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
	    return(SUCCESS);
    }

    /* Construct the temporary file name */
#ifdef LINUX
    strcpy7(bf.tmp_file, "tmp_files/", user.login, "_", user.mach, "_", tr.upd_time_string, "_tmp_file");
#else
    strcpy6(bf.tmp_file, user.login, "_", user.mach, "_", tr.upd_time_string, "_tmp_file");
#endif

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    /* Close file already opened */
	    fclose(fi);
	    rtn.len += strcpy5(rtn.msg+rtn.len, wr_open_err, 
	    RET2TAB, "'", bf.tmp_file, "'");
	    
	    return(WR_OPEN_ERR);
    }

    done = DBNO;

    while (1)
    {
	    if (((cnt = line_get(fi, bf.buffer)) == EOF) || (cnt == 0))
	    /* If reached end of file, put in new entry */
	    {
	        fprintf(fo, "%s\n", entry);
	        done = DBYES;
	        break;
	    }
	
	    /* Put null on end of string */
	    bf.buffer[cnt] = '\0';

	    /* If buffer entry less than new entry, write out the buffer entry */
	    if ((ret = strcmp(entry, bf.buffer)) > 0)
	    {
	        fprintf(fo, "%s\n", bf.buffer);
	    }
	    /* Otherwise write out new entry followed by bf.buffer entry */
	    else if (ret < 0)
	    {
	        fprintf(fo, "%s\n", entry);
	        fprintf(fo, "%s\n", bf.buffer);
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
	        
	        return(DUP_ENTRY);
	    } /* end else */
    } /* end while */
    
    /* Write remaining entries out to the temporary file */
    if (done == DBNO)
    {
	    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BLOCK_SIZE, fi)) != 0)
        {
            bf.buffer[ret] = 0;
	        fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
        }
    } /* end if */

    /* Close both files */
    fclose(fi);
    fclose(fo);

    /* Write the temporary file over the old file */
    rename(bf.tmp_file, file_name);

    return(SUCCESS);
} /* End add_line */
