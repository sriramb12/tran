/****************************************************************************
 *
 * Function:	dlt_file_entry
 *
 * Description:
 *  Deletes entry from global distribution group table 
 *	when a distribution group is deleted.
 *	Currently only calling function is delete_dist_group.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *file_path - Complete path name of file entry being deleted from
 *  char *entry     - Name of entry to be deleted
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "reg_globals.h"
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

int dlt_file_entry(char *file_path,	char *entry)
{
    FILE *fi;
    FILE *fo;
    register int ret;

    DBG("file_path='%s', entry='%s'", file_path, entry);

    /* Open the file entry being deleted from */
    if ((fi = fopen(file_path, "r")) == NULL)
    {
	    strcpy7(rtn.msg, rd_open_err, RET2TAB, "'", file_path, "'",  DOUBLE_RET, help_msg);
	    return(RD_OPEN_ERR);
    }

    /* Construct the temporary file name */
    strcpy6(bf.tmp_file, user.email, "_", user.mach, "_", "test", "_tmp_File");

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    strcpy7(rtn.msg, wr_open_err, RET2TAB, "'", bf.tmp_file, "'",  DOUBLE_RET, help_msg);
	    return(WR_OPEN_ERR);
    }

    while(1)
    {
	    /* If end of file reached before entry found, */
	    /* Put a caution message in the log file for the database admin */
	    /* Also break out of the loop */
	    ret = fscanf(fi, "%s", bf.buffer);

	    if (ret == EOF)
	    {
	        strcpy5(bf.buffer, bad_toc, entry, RET_TAB, IN_TABLE, file_path);
	        data_log(tr.log_file, bf.buffer);
	        break;
	    }

	    /* If string scanned in from file not equal to the entry to be, */
	    /* deleted, write the string to the temporary file */
	    if (strcmp(entry, bf.buffer) != 0)
	        fprintf(fo, "%s\n", bf.buffer);
	    /* Otherwise fall out of the loop */
	    else
	        break;
    } /* end while */

    /* Skip over extra carriage return */
    getc(fi);

    /* Write remaining entries out to the temporary file */
	while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
	    fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    
    /* Close both files */
    fclose(fi);
    fclose(fo);
    
    /* Write the temporary file over the old file */
    rename(bf.tmp_file, file_path);

    return(SUCCESS);
} /* End dlt_file_entry */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
