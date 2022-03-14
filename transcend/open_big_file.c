/****************************************************************************
 *
 * Function:	get_big_file
 *
 * Description:	Opens big file and returns stream pointer.
 *		Sends user error message and exits if file not found.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_server_macros.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h"


FILE *open_big_file(char *key)
{
    DBG("key='%s'", key);

    FILE *fp;
    int len;

    /* Build file name */
    len = strcpy2(bf.file_name, GLOBAL_FILE_PATH, key);

    if ((fp = fopen(bf.file_name, "r+")) == NULL)
    {
        history_lookup(key);
	    return_non_err(key);
    } /* End if for global file not present */

    return(fp);
} /* End open_big_file */


/* Read big file */
FILE *read_big_file(char *key, char line[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs)
{
    DBG("key='%s'", key);

    extern int secure_keyword_fnd;
    char tmp_key[10];
    register short cnt;
    FILE *fp;
    char *ptr;

    strcpy(tmp_key, key);
    
    /* If the key is found, it is returned in variable 'key' */
    secure_keyword_fnd = keyword_find(tmp_key, key);
    
    /* Open global file */
    fp = open_big_file(key);
    
    /* Read in the data */
    /* Read in TRANS_LINE_NUM lines of text */
    for (cnt = 0; cnt <TRANS_LINE_NUM; ++cnt)
        line_get(fp, line[cnt]);

    /* Scan in parameters from first line */
    strcpy(fs->tar_type, "0");
    sscanf(line[0], EXTEND_FORMAT, &fs->file_type, &fs->disk_index, &fs->dlt_time, fs->own_id, &fs->email_flag, &fs->extension, &fs->dlt_flag, &fs->lock_flag, &fs->mode, &fs->compression, fs->tar_type);

    /*
     * Carlos Mazieri, commented this, do not touch email/coreid
    if ((ptr = strchr(fs->own_id, '@')) != NULL)
        ptr[0] = '\0';
     */

    if (fs->file_type == DIRECTORY)
        strcpy(fs->title, "directory");
    else if ((fs->file_type == FILES) || (fs->file_type == HUGE_FILES))
        strcpy(fs->title, "files");
    else 
        strcpy(fs->title, "file");

    /* Pull out the real file name */
    strcpy(fs->real_name, line[1]+13);

    /* Initialize the file size */
    /* it is on line 3 */
    DBG("BEFORE sscanf()\nline[2]='%s'\nfs->original_size='%d'", line[2], fs->original_size);
    sscanf(line[2], "%*s %*S %lld", &fs->original_size);
    DBG("AFTER sscanf()\nline[2]='%s'\nfs->original_size='%d'", line[2], fs->original_size);
    sscanf(line[3], "%*S %lld", &fs->compressed_size);

    /* Reset the compression type for a double check */
    if (fs->original_size == fs->compressed_size) fs->compression = DBNO;
    
    /* copy in the key */
    strcpy(fs->key, key);
    
    /* Convert iCAP class to abbreviation */
    icap_class_rtv(fs->icap_class, line[TRANS_LINE_NUM-2]+13, line[TRANS_LINE_NUM-3]+13);
    
    /* Return the file pointer */
    return(fp);
} /* End read_big_file */


/* Write out modified contents of big file */
void write_big_file(FILE *fp, char line[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs)
{
    extern USER_PARAMS user;
    int cnt;
    char own_id[EMAIL_LEN];

    if (gu.email1[0] != 0)
    {
        strcpy(own_id, gu.email1);
    }
    else if (user.email[0] != 0)
    {
        strcpy(own_id, user.email);
    }
    else
    {
         strcpy(own_id, user.login);
    }
    DBG("own_id=%s", own_id);


    /* Write changed data to line 1 */
    sprintf(line[0], EXTEND_FORMAT, fs->file_type, fs->disk_index, fs->dlt_time, own_id, fs->email_flag, fs->extension, fs->dlt_flag, fs->lock_flag, fs->mode, fs->compression, fs->tar_type);

    /* Go back to beginning of file */
    rewind(fp);

    /* Now write out data */
    for (cnt = 0; cnt <TRANS_LINE_NUM; ++cnt)
        fprintf(fp, "%s\n", line[cnt]);
    
    fclose(fp);
    DBG("living write_big_file()");
} /* End write_big_file */
