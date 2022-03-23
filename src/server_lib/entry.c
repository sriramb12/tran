/****************************************************************************
 *
 * Function:	add_entry
 *
 * Description:	Adds a new entry to specified global table.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "close_down.h"
#include "errno.h"
#include "global_defines.h"
#include "global_req_types.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "user_params.h"
#include "rtn.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities


/* Adds new entry to table of contents when a new mask, template, */
/* wafer line, or flow is added */
int add_entry(char *file_name, char *entry)
{
    FILE *fi;
    FILE *fo;
    register int done;
    register int ret;
    extern TMP_BUFFER bf;

    DBG("file_name='%s' and entry='%s'", file_name, entry);

    /* Open the file entry being added to */
    if ((fi = fopen(file_name, "r")) == NULL)
    {
	    /* Create the file and add the entry */
	    data_log(file_name, entry);
	    return(SUCCESS);
    }

    /* Construct the temporary file name */
    strcpy6(bf.tmp_file, user.login, "_", user.mach, "_", tr.upd_time_string, "_tmp_file");

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    /* Close the file already opened */
	    fclose(fi);
	    strcpy7(rtn.msg, wr_open_err, RET2TAB, "'", bf.tmp_file, "'",  DOUBLE_RET, HELP_MSG);
	    return(WR_OPEN_ERR);
    }

    done = DBNO;
    
    while (done == DBNO)
    {
	    ret = line_get(fi, bf.buffer);
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
	        strcpy5(bf.buffer, dup_entry, entry, RET_TAB, IN_TABLE, file_name);
	        data_log(tr.log_file, bf.buffer);

	        return(SUCCESS);
	    } /* end else */
    } /* end while */

    /* Write remaining entries out to the temporary file */
    if (done == DBNO)
    {
	    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BLOCK_SIZE, fi)) != 0)
	        fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* end if */

    /* Close both files */
    fclose(fi);
    fclose(fo);

    /* Write the temporary file over the old file */
    if ( rename(bf.tmp_file, file_name) != SUCCESS)
        close_down2m(rename_err, bf.tmp_file, file_name, 3);

    return(SUCCESS);
}


/*    char *file_name - Complete path name of file entry being deleted from */
/*    char *entry     - Name of entry to be deleted */
int dlt_entry(char *file_name, char *entry)
{
    FILE *fi;
    FILE *fo;
    register int ret;

    DBG("file_name='%s' and entry='%s'", file_name, entry);

    /* Open the file entry being deleted from */
    if ((fi = fopen(file_name, "r")) == NULL)
    {
	    strcpy7(rtn.msg, rd_open_err, RET2TAB, "'", file_name, "'",  DOUBLE_RET, HELP_MSG);
	    return(RD_OPEN_ERR);
    }

    /* Construct the temporary file name */
    strcpy6(bf.tmp_file, user.login, "_", user.mach, "_", "test", "_tmp_File");

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    /* Close file already opened */
	    fclose(fi);
	    strcpy7(rtn.msg, wr_open_err, RET2TAB, "'", bf.tmp_file, "'",  DOUBLE_RET, HELP_MSG);
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
	        strcpy5(bf.buffer, bad_toc, entry, RET_TAB, IN_TABLE, file_name);
	        data_log(tr.log_file, bf.buffer);
	        break;
	    }
	    /* If string scanned in from file not equal to the entry to be, */
	    /* deleted, write the string to the temporary file */
	    if (strcmp(entry, bf.buffer) != 0)
	        fprintf(fo, "%s\n", bf.buffer);

	    /* Otherwise fall out of the loop */
	    else break;
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
    rename(bf.tmp_file, file_name);

    return(SUCCESS);
} /* End dlt_entry */


/* Looks for entire entry */
int entry_find(char *file_name, char *entry)
{
    FILE *fp;
    char line[260];

    DBG("file_name='%s' and entry='%s'", file_name, entry);

    if ((fp = fopen(file_name, "r")) == NULL) return(FAILURE);

    while (line_get(fp, line) != EOF)
    {
	    if (strcasecmp(line, entry) == 0)
	    {
	        fclose(fp);
	        return(SUCCESS);
	    } /* End if */
    } /* End while */

    fclose(fp);

    return(FAILURE);
} /* End entry_find */


/* Looks for partial entry */
int entry_lookup(char *file_name, char *entry)
{
    FILE *fp;
    char line[260];
    int len;

    DBG("file_name='%s' and entry='%s'", file_name, entry);

    if ((fp = fopen(file_name, "r")) == NULL) return(FAILURE);

    len = strlen(entry);
    while (line_get(fp, line) != EOF)
    {
	    if (strncasecmp(line, entry, len) == 0)
	    {
	        fclose(fp);
	        return(SUCCESS);
	    } /* End if */
    } /* End while */

    fclose(fp);

    return(FAILURE);
} /* End entry_lookup */


/* Looks for partial entry and returns entry found */
int entry_return(char *file_name, char *entry, char *line)
{
    FILE *fp;
    int len;

    DBG("file_name='%s', entry='%s' and line='%s'", file_name, entry, line);

    if ((fp = fopen(file_name, "r")) == NULL) return(FAILURE);

    len = strlen(entry);
    while (line_get(fp, line) != EOF)
    {
	    if (strncasecmp(line, entry, len) == 0)
	    {
	        fclose(fp);
	        return(SUCCESS);
	    } /* End if */
    } /* End while */

    fclose(fp);

    return(FAILURE);
} /* End entry_lookup */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
