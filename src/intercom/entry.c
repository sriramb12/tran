/****************************************************************************
 *
 * Function:	entry_find, entry_add entry_delete
 *
 * Description:	Finds, adds, deletes  * entries from file containing 
 *		entries separated by carriage returns
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
#include "close_down.h"
#include "lock_file.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "user_params.h"
#include "global_debug.h"

/* Verify user authorized to build/approve templates */

/* Adds new entry to specified file */
/* If entry already present, replace old entry with new entry */
int entry_add(char *file1, char *entry, char *key)
{
    int index;
     int fnd = DBNO;
    int len;
    char file2[FILE_SIZE];
    char tmp_file[FILE_SIZE];
    char line[260];
    FILE *fi;
    FILE *fo;
    extern TMP_BUFFER bf;
    register int ret;

    DBG();

    /* Open with the stream pointer */
    if ((fi = fopen(file1, "r")) == NULL)
    {
	/* If the file does not exist, create it */
	data_log(file1, entry);
	return(SUCCESS);
    }
    /* Open and lock the first file */
    strcpy2(file2, file1, ".bak");
    /* Open the temporary new file */
    if ((fo = fopen(file2, "w")) == NULL)
    close_down1m(wr_open_err, file2, WR_OPEN_ERR);
    /* Now lock the file */
    index = lock_file_set(file1);
    /* Look for the place to put the new entry */
    len = strlen(key);
    while (line_get(fi, line) > 0)
    {
	/* If old entry less than new entry, just write it out */
	if ((ret = strncmp(line, key, len)) < 0)
	fprintf(fo, "%s\n", line);
	/* found place for new entry */
	else if (ret > 0)
	{
	    /* Write out new entry followed by old entry */
	    fprintf(fo, "%s\n", entry);
	    fprintf(fo, "%s\n", line);
	    fnd = SUCCESS;
	    break;
	} else {
	    /* Replace existing entry with new entry */
	    fprintf(fo, "%s\n", entry);
	    fnd = DUP_ENTRY;
	    break;
	}
    } /* End while */
    /* If reached the end of the file without finding a place for the mask */
    /* Put it at the end of the file */
    if (fnd == DBNO) 
    {
	fprintf(fo, "%s\n", entry);
	fnd = SUCCESS;
    } else {
	while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
	fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* End else for data still left in file */
    /* Close the files */
    fclose(fi);
    fclose(fo);
    /* Build name for temporary file */
    strcpy2(tmp_file, file1, ".tmp");
    /* Move the original global mask file to a back-up location */
    rename(file1, tmp_file);
    /* Move the temporary mask file to its permanant location */
    if (file_rename(file2, file1) != SUCCESS)
    {
	file_rename(tmp_file, file1);
	lock_file_clear(index);
    close_down2m(rename_err, file2, file1, RENAME_ERR);
    }
    /* Otherwise delete the back-up file */
    else unlink(tmp_file);
    lock_file_clear(index);
    return(fnd);
} /* End entry_add */

/* Delete entry from specified file */
int entry_delete(char *file1, char *entry)
{
    int index;
     char file2[FILE_SIZE];
     int len;
    char fnd = DBNO;
    int ret;
    char line[260];
    FILE *fi;
    FILE *fo;

    DBG();

    /* Open with stream pointer */
    if ((fi = fopen(file1, "r")) == NULL)
    close_down1m(rd_open_err, file1, RD_OPEN_ERR);
     /* build name for back up file */
    strcpy2(file2, file1, ".bak");
    /* Open the temporary file */
    if ((fo = fopen(file2, "w")) == NULL)
    close_down1m(wr_open_err, file2, WR_OPEN_ERR);
    /* Lock the file */
    index = lock_file_set(file1);
    len = strlen(entry);
    /* Go through file looking for entry to delete */
    while (line_get(fi, line) > 0)
    {
	if ((ret = strncmp(line, entry, len)) < 0) 
	fprintf(fo, "%s\n", line);
	else if (ret == 0) 
	{
	    fnd = DBYES;
	    break;
	}
	/* If reached this point, entry not found */
	/* Thus do not need to bother writing out any more entries to new file */
	else break;
    } /* End while */
    /* If entry found, write out rest of entries */
    if (fnd == DBYES)
    {
	while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
	fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* End else for data still left in file */
    fclose(fi);
    fclose(fo);
    if (fnd == DBYES)
    file_rename(file2, file1);
    /* Otherwise delete new file no longer needed */
    else unlink(file2);
    lock_file_clear(index);
    return(fnd);
} /* End entry_delete */

/* Returns SUCCESS or FAILURE */
/*es.files.fileuse*/
/*is.files.fileuse*/
