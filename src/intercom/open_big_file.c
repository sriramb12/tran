/****************************************************************************
 *
 * Function:	get_big_file
 *
 * Description:	Opens big file and returns stream pointer.
 *		Sends user error message and exits if file not found.
 *
 ***************************************************************************/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "close_down.h"
#include "customer_table.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "rtn.h"
#include "strcpy.h"
#include "user_params.h"
#include "trans_server_macros.h"
#include "global_debug.h"


/* See if valid version of Intercom being used */
void maskshop_version_check()
{
    /* Check to see if valid version is being used */
    if (strncmp(user.client_version, "6.", 2) != 0)
        return;

    /* If reached this point, using an invalid version */
    shut_down(invalid_trans_version, INVALID_TRANS_VERSION);
} /* end maskshop_version_check */


/* Determines if specified company is a defined mask shop */
int old_is_maskshop(char *company)
{
    char maskshop[64];
    char extra_string[64];
    FILE *fp;
    int len1 = strlen(company);
    int len2;
    int is_maskshop = DBNO;

    DBG("company=%s", company);

    if ((fp = fopen(COMPANY_FILE, "r")) == NULL)
        return(DBNO);

    while (fscanf(fp, "%[^;]%*c%[^;]%*c%*c",maskshop, extra_string) == 2)
    {
	    len2 = strlen(maskshop);
	    
	    /* Use shortest length for maskshop or company */
	    if (len1 < len2) len2 = len1;
	    
	    if (strncmp(company, maskshop, len2) == 0)
	    {
	        is_maskshop = DBYES;
	        break;
	    } /* End if */
    } /* End while */

    fclose(fp);

    return(is_maskshop);
} /* End is_maskshop */


FILE *open_big_file(char *keyword)
{
    FILE *fp;
    extern char global_secure_keyword[];
    int is_secure_key;

    DBG("keyword=%s gu.req_type=%d", keyword, gu.req_type);

    /* Copy secure keyword for later use */
    strcpy(global_secure_keyword, keyword);

    /* First map keyword */
    is_secure_key  = keyword_find(global_secure_keyword, keyword);

    if ((customer_type == EXTERNAL_CUSTOMER) && (is_secure_key == DBNO)
        && (gu.req_type != DBPUT_ACK) && (gu.req_type != DBGET_ACK))
        shut_down(fss_auth_err, FSS_AUTH_ERR);

    /* If this is external TransWeb, file and secure keyword must match */
    else if (customer == EXTERNAL_TRANSWEB)
    {
	    if (strcmp(keyword, gu.par1) != 0)
        close_down2p(key_mismatch_err, global_secure_keyword, gu.par1, KEY_MISMATCH_ERR);
    }

    /* Build file name */
    strcpy2(bf.file_name, GLOBAL_FILE_PATH, keyword);
    if ((fp = fopen(bf.file_name, "r+")) == NULL)
    {
	    /* If history present, send this to user */
	    history_lookup(keyword);
        return_non_err(keyword);
    } /* End if for global file not present */
    else
        return(fp);
} /* End open_big_file */


/* Read big file */
/* Reads in data from big file */
FILE *read_big_file(char *key, char line[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs)
{
    extern char global_mask[];
    register short cnt;
    FILE *fp;
    int counter = 0;

    /* Open global file */
    fp = open_big_file(key);

    /* Read in the data */
    /* Read in 17 lines of text */
    for (cnt = 0; cnt <TRANS_LINE_NUM; ++cnt)
        line_get(fp, line[cnt]);

    /* Scan in parameters from first line */
    /* Set default value for tar type */
    strcpy(fs->tar_type, NONE);
    counter = sscanf(line[0], ICOM_EXTEND_FORMAT,
        &fs->file_type, &fs->disk_index, &fs->dlt_time, fs->own_id, &fs->email_flag,
        &fs->extension, &fs->dlt_flag, &fs->lock_flag, &fs->mode, &fs->compression, global_mask, fs->tar_type);

    DBG("counter=%d line[0]=%s", counter, line[0]);
    DBG("key=%s fs->own_id=%s fs->email_flag=%d fs->extension=%d fs->dlt_flag=%d fs->lock_flag=%d fs->mode=%ld fs->compression=%d global_mask=%s fs->tar_type=%s",
        key, fs->own_id, fs->email_flag, fs->extension, fs->dlt_flag, fs->lock_flag, fs->mode, fs->compression, global_mask, fs->tar_type);

    if (fs->file_type == DIRECTORY)
        strcpy(fs->title, "directory");
    else if (fs->file_type == FILES)
        strcpy(fs->title, "files");
    else 
        strcpy(fs->title, "file");

    /* Pull out the real file name */
    strcpy(fs->real_name, line[1]+13);
    
    /* Scan in the original size */
    sscanf(line[2], "%*s %*s %lld", &fs->original_size);
    sscanf(line[3], "%*s %lld", &fs->compressed_size);
    
    /* Scan in the key */
    sscanf(line[4], "%*s %s", fs->key);
    
    /* Convert iCAP class to abbreviation */
    icap_class_rtv(fs->icap_class, line[TRANS_LINE_NUM-2]+13, line[TRANS_LINE_NUM-4]+13);
    
    /* Return the file pointer */
    return(fp);
} /* End read_big_file */


/* Reads in owner of big file */
int tmp_is_trusted_owner(char *key)
{
    char owner[32];
    char line[260];
    char *ptr;
    FILE *fp;

    DBG("key=%s", key);

    /* Open global file */
    fp = open_big_file(key);

    if (fp == NULL) return(DBNO);

    /* Read in the first line of the file */
    line_get(fp, line);
    fclose(fp);

    /* Scan in parameters from first line */
    if (sscanf(line, "%*c %*d %*ld %s", owner) < 1)
        return(DBNO);

    return(is_trusted_sender(owner));
} /* end is_trusted_owner */


/* Write out modified contents of big file */
void write_big_file(FILE *fp, char line[TRANS_LINE_NUM][TRANS_LINE_LEN], TRANS_FILE_STAT *fs)
{
    extern char global_mask[];
    int cnt;

    DBG("fs->own_id=%s fs->email_flag=%d fs->mode=%ld fs->compression=%d", fs->own_id, fs->email_flag, fs->mode, fs->compression);

    /* Write changed data to line 1 */
    sprintf(line[0], ICOM_EXTEND_FORMAT, 
        fs->file_type, fs->disk_index, fs->dlt_time, fs->own_id,
        fs->email_flag, fs->extension, fs->dlt_flag, fs->lock_flag, fs->mode, 
        fs->compression, global_mask, fs->tar_type);

    /* Go back to beginning of file */
    if ( fseek(fp, 0L, SEEK_SET) != 0)
    {
	    strcpy5(rtn.msg, seek_err, RET2TAB, "'global_data/", fs->key, "'");
	    shut_down(rtn.msg, SEEK_ERR);
    }
    /* Now write out data */
    for (cnt = 0; cnt <TRANS_LINE_NUM; ++cnt)
        fprintf(fp, "%s\n", line[cnt]);

    fclose(fp);
    DBG("living write_big_file()");
} /* End write_big_file */


int check_company_auth(char *user_specified_company, char *real_company, int len)
{
    DBG();

    if (strncasecmp(user_specified_company, real_company, len) == 0)
        return(0);
    else if ((strncmp(user_specified_company, "External", 8) == 0) && (strncmp(real_company, "External", 8) == 0))
        return(0);
    else 
        return (-1);
} /* end check_company_auth */


FILE *read4info_big_file(char *key,  TRANS_FILE_STAT *fs, int close_flag)
{
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN]; 
    extern int customer_type;
    extern int customer;
    extern char global_secure_keyword[];
    FILE *fp;

    DBG();

    fp = read_big_file(key, lines, fs);

    /* If calling function does not want pointer, close it */
    if (close_flag == DBYES)
    {
	    /* Close file no longer needed */
	    fclose(fp);
	    fp = NULL;
    }

    /* If this is a NXP customer, he can perform anything */
    if (customer_type == INTERNAL_CUSTOMER)
        return(fp);
    /* If user owns the data, he can perform anything */
    else if (keyword_auth_check(fs->own_id) == 0)
        return(fp);
    else
        return(fp);
} /* end read4info_big_file */
/*es.files.fileuse*/
/*is.files.fileuse*/
