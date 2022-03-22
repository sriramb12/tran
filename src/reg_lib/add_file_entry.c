/****************************************************************************
 *
 * Function:	add_file_entry
 *
 * Description:	Adds a new entry to specified global table.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *path
 *  char *file_name - Complete path name of file entry being added to
 *  char *entry - Name of entry to be added 
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "user_params.h"
#include "rtn.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

int add_file_entry(char *path, char *file_name, char *entry)
{
    FILE *fi;
    FILE *fo;
    register int done;
    register int ret;

    DBG("path='%s', file_name='%s' and entry='%s'", path, file_name, entry);

    /* Open the file entry being added to */
    if ((fi = fopen(file_name, "r")) == NULL)
    {
	    /* Create the file and add the entry */
	    data_log(file_name, entry);

	    /* Warn the DB admin the global table was not present */
	    //sprintf(rtn.msg, no_glbl_table_cau, file_name);
	    //data_log(tr.log_file, rtn.msg);
	    
        DBG("entry='%s' added to path='%s' and file_name='%s'", entry, path, file_name);
	    return(SUCCESS);
    }

    /* Construct the temporary file name */
    strcpy8(bf.tmp_file, path, "tmp_files/",  user.email, "_", user.mach, "_", "test", "_bf.tmp_file");

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    strcpy7(rtn.msg, wr_open_err, RET2TAB, "'", bf.tmp_file, "'",  DOUBLE_RET, help_msg);
	    return(WR_OPEN_ERR);
    }

    done = DBNO;
    while (done == DBNO)
    {
	    ret = fscanf(fi, "%s", bf.buffer);

	    /* If end of file reached put the new entry in */
	    if (ret == EOF)
	    {
	        fprintf(fo, "%s\n", entry);
	        done = DBYES;
	        break;
	    }

	    /* If buffer entry less than new entry, write out the buffer entry */
	    if ((ret = strcmp(entry, bf.buffer)) > 0)
	    {
	        fprintf(fo, "%s\n", bf.buffer);
	    }
	    /* Otherwise write out new entry followed by buffer entry */
	    else if (ret < 0)
	    {
	        fprintf(fo, "%s\n", entry);
	        fprintf(fo, "%s\n", bf.buffer);
	        break;
	    } /* end else */
	    /* Otherwise inform user entry already exists */
	    /* At this point ret will equal 0 */
	    else
	    {
	        fclose(fi);
	        fclose(fo);

	        /* Delete the temporary file */
	        unlink(bf.tmp_file);
	        strcpy5(rtn.msg, dup_entry, entry, RET_TAB, IN_TABLE, file_name);
	        data_log(tr.log_file, rtn.msg);

            DBG("entry='%s' is duplicated for the path='%s' and file_name='%s'", entry, path, file_name);
	        return(DUP_ENTRY);
	    } /* end else */
    } /* end while */

    /* Write remaining entries out to the temporary file */
    if (done == DBNO)
    {
	    /* Skip over extra carriage return */
	    getc(fi);
	    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
	        fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* end if */

    /* Close both files */
    fclose(fi);
    fclose(fo);

    /* Write the temporary file over the old file */
    if (rename(bf.tmp_file, file_name) != SUCCESS)
    {
	    strcpy7(bf.msg, rename_err, RET2TAB, "'", bf.tmp_file, RET_TO, file_name, "'");
	    return(RENAME_ERR);
    } else {
        return(SUCCESS);
    }
} /* End add_file_entry */
