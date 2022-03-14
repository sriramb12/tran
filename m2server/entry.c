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
#include <strings.h>
#include <time.h>
     #include <unistd.h>
#include "close_down.h"
#include "lock_file.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "m2req_types.h"
#include "m2server_globals.h"
#include "m2server_errs.h"
#include "m2server_paths.h"
#include "mti.h"
#include "rtn.h"
#include "server_caution_errs.h"
#include "strcpy.h"
#include "user_params.h"

/* Verify user authorized to build/approve templates */
int entry_find(char *file_name, char *entry)
{
    FILE *fp;
    int len = strlen(entry);
    char file_entry[64];


    /* Open the specified file */
    if ((fp = fopen(file_name, "r")) == NULL)
    close_down1m(rd_open_err, file_name, RD_OPEN_ERR);
    /* Scan in names until match found or end of file reached */
    while (fscanf(fp, "%s", file_entry) == 1)
    {
	if (  strncmp(file_entry, entry, len) == 0) 
	{
	    fclose(fp);
	    return(SUCCESS);
	} /* End if */
    } /* End while */
    /* Close file no longer needed */
    fclose(fp);
    /* If reached this point, entry not found */
    return(FAILURE);
} /* End entry_find */

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

    /* Open with the stream pointer */
    if ((fi = fopen(file1, "r")) == NULL)
    {
	/* If the file does not exist, create it */
	data_log(file1, entry);
	/* Let the database administrator know the file does not exist */
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
		/* Log a caution message */
		strcpy5(bf.buffer, dup_entry, key, RET_TAB, IN_TABLE, "mask.sets");
		data_log(tr.log_file, bf.buffer);
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
int entry_find_write(char *file1, char *file2, char *entry, FILE **fi, FILE **fo, char *line)
{
    int index;
    int ret;
    int len;

    /* Open and lock the first file  */
    if ((*fi = fopen(file1, "r")) == NULL)
    return(FAILURE);
    /* Open the temporary new file */
    if ((*fo = fopen(file2, "w")) == NULL)
    close_down1m(wr_open_err, file2, WR_OPEN_ERR);
    index = lock_file_set(file1);
     /* Look for specified entry */
    len = strlen(entry);
    while (line_get(*fi, line) > 0)
    {
	/* If old entry less than new entry, just write it out */
	if ((ret = strncmp(line, entry, len)) < 0)
	fprintf(*fo, "%s\n", line);
	/* found place for new entry */
	else if (ret == 0)
	{
	    return(SUCCESS);
	} else {
	    /* Replace existing entry with new entry */
	    return(FAILURE);
	}
    } /* End while */
    return(index);
} /* End file_entry_find */

/* Write out the rest of the file */
void entry_write(int index, char *file1, char *file2, FILE *fi, FILE *fo, char fnd)
{
    int ret;
    char tmp_file[FILE_SIZE];

    if (fnd == DBYES)
    {
	while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
	fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* End if for data still left in file */
    /* Close the files */
    fclose(fi);
    fclose(fo);
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
    /* Unlock the file */
    lock_file_clear(index);
} /* End entry_write */

void entry_close(int index, char *file1, char *file2, FILE *fi, FILE *fo)
{

    /* Close the files */
    fclose(fi);
    fclose(fo);
    unlink(file2);
    lock_file_clear(index);
} /* End entry_close */
void entry_approve(char *file1, char *version)
{
    char entry[260];
     int fnd = DBNO;
    int len;
    char file2[FILE_SIZE];
    char tmp_file[FILE_SIZE];
    char line[260];
    FILE *fi;
    FILE *fo;
    extern TMP_BUFFER bf;
    register int ret;

    /* Build entry to add */
    sprintf(entry, "+  %s %s %s (%s) %s",
    version, "None", user.name, user.email, ctime(&tr.upd_time));
    /* Open with the stream pointer */
    if ((fi = fopen(file1, "r")) == NULL)
    {
	/* If the file does not exist, create it */
	/* Take off trailing carriage return */
	entry[strlen(entry)-1] = '\0';
	data_log(file1, entry);
	return;
    }
    /* Open and lock the first file */
    strcpy2(file2, file1, ".bak");
    /* Open the temporary new file */
    if ((fo = fopen(file2, "w")) == NULL)
    close_down1m(wr_open_err, file2, WR_OPEN_ERR);
    /* Now lock the file */
    /* Look for the place to put the new entry */
    len = strlen(version);
    while (line_get(fi, line) != EOF)
    {
	if (strncmp(line+3, version, len) == 0)
	{
	    /* Approve data */
	    line[0] = '+';
	        fnd = DBYES;
	    fprintf(fo, "%s\n", line);
	        break;
	}
	else fprintf(fo, "%s\n", line);
    } /* End while */
    /* If reached the end of the file without finding a place for the version */
    /* Put it at the end of the file */
    if (fnd == DBNO) fprintf(fo, "%s", entry);
    else
    {
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
    close_down2m(rename_err, file2, file1, RENAME_ERR);
    }
    /* Otherwise delete the back-up file */
    else unlink(tmp_file);
} /* End entry_approve */

void entry_unapprove(char *file1)
{
     int fnd = DBNO;
    char file2[FILE_SIZE];
    char tmp_file[FILE_SIZE];
    char line[260];
    FILE *fi;
    FILE *fo;
    extern TMP_BUFFER bf;
    register int ret;

    /* Open with the stream pointer */
    if ((fi = fopen(file1, "r")) == NULL)
    {
	if (mask_msg.req_type == UNAPPROVE)
	close_down1f(no_approved_version, mti.prefix, NO_APPROVED_VERSION);
	else return;
    } /* End if */
    /* Open and lock the first file */
    strcpy2(file2, file1, ".bak");
    /* Open the temporary new file */
    if ((fo = fopen(file2, "w")) == NULL)
    close_down1m(wr_open_err, file2, WR_OPEN_ERR);
    /* Now lock the file */
    /* Look for the approved version */
    while ((line_get(fi, line) != EOF) && (fnd == DBNO))
    {
    if (line[0] == '+')
	{
	    fnd = DBYES;
	    line[0] = '*';
	} /* End if for approved version found */
	fprintf(fo, "%s\n", line);
    } /* End while */
    /* If approved version not found close files and give error */
    if (fnd == DBNO)
    {
	fclose(fi); 
	fclose(fo);
	unlink(file2);
	if (mask_msg.req_type == UNAPPROVE)
	close_down1f(no_approved_version, mti.prefix, NO_APPROVED_VERSION);
	else return;
    }
    /* Now write out last line retrieved */
    fprintf(fo, "%s\n", line);
    /* Write out remainder of the data */
    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
    fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
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
    close_down2m(rename_err, file2, file1, RENAME_ERR);
    }
    /* Otherwise delete the back-up file */
    else unlink(tmp_file);
} /* End entry_unapprove */

/* Return version number for approved version */
void entry_approved_version_find(char *version_file, char *version)
{
    FILE *fp;
    char line[260];

    if ((fp = fopen(version_file, "r")) == NULL)
    close_down1f(no_approved_version, mti.prefix, NO_APPROVED_VERSION);
    while (line_get(fp, line) != EOF)
    {
    if (line[0] == '+')
	{
	    sscanf(line+3, "%s", version);
	    fclose(fp);
	    return;
	} /* End if */
    } /* End while */
    /* If reached this point, no approved version present */
    fclose(fp);
    close_down1f(no_approved_version, mti.prefix, NO_APPROVED_VERSION);
} /* End entry_approved_version_find */

void entry_alternate_name_find(char *alternate_name, char *version)
{
    char version_file[FILE_SIZE];
    FILE *fp;
    char line[260];

    /* Build version file name where alternate name stored */
    strcpy2(version_file, mti.data_dir, "/version_list");
    if ((fp = fopen(version_file, "r")) == NULL)
    close_down2p(no_version_fnd, version, mti.prefix, NO_VERSION_FND);
    while (line_get(fp, line) != EOF)
    {
	if (strncmp(line+3, version, 3) == 0)
	{
	    sscanf(line+6, "%s", alternate_name);
	    fclose(fp);
	    return;
	} /* End if */
    } /* End while */
    /* If reached this point, no approved version present */
    fclose(fp);
    close_down2p(no_version_fnd, version, mti.prefix, NO_VERSION_FND);
} /* End entry_alternate_name_find */
