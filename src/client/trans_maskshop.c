
/****************************************************************************
 *
 * Function:	mask shop functions
 *
 * Description:	mask shop functions
 *		for specified company.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include "ssl_client_lib.h"
#include "client_maskshop.h"
#include "global_client_errs.h"
#include "client_prototypes.h"
#include "scan_prototypes.h"
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "global_params.h"
#include "server_params.h"
#include "global_defines.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "company.h"
#include "transapi_defines.h"
#include "client_dir.h"
#include "client_files.h"

#include "global_debug.h" // Mazieri's debug facilities

#define M2TOTAL_FILES 4100

extern int global_file_cnt;
char remote_mask[8];
FILE *maskshop_fp;

int trans_list_get(char *files[])
{
    DBG();
    int cnt;

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	put_ssl_string(files[cnt]);
    free(files[cnt]);
    } /* End for */
    /* Send indication all list retrieved */
    put_ssl_short(SUCCESS);
    return(cnt);
} /* End trans_list_get */

/* See if user wants to abort transaction */
/* Resets to put verify if data going to mask shop */
void trans_maskshop_req_type_reset(int index)
{
    DBG();
    /* No action required if request type already set */
    if (trans[index]->req_type == DBPUT_DUPLICATE_FILES)
    {
        return;
    }
    if (trans[index]->is_maskshop == DBYES)
    {
        if (IS_REQUEST_PUT_DELETE(trans[index]->req_type))
        {
            trans[index]-> req_type = DBPUTDLT_VERIFY_FILES;
        }
        else
        {
            trans[index]->req_type = DBPUT_VERIFY_FILES;
        }
    }
} /* End trans_maskshop_req_type_reset */

/* Sends list of files being transferred to the maskshop */
int trans_maskshop_filelist_send(void)
{
    DBG();
    int ret;
    char *files[M2TOTAL_FILES];

    /* First scan in list of files */
    trans_maskshop_files_find(files, MASKSHOP_LOG);
    trans_list_get(files);
    /* Get return code from server */
    if ((ret = get_ssl_short()) < SUCCESS)
    {
	get_ssl_string(rtn.msg);
	return(ret);
    }
    else return(SUCCESS);
} /* end trans_maskshop_filelist_send */

/* Pulls file names out of sendtomaskshop log */
int trans_maskshop_files_find(char *files[], char *file_name)
{
    DBG();
    char *ptr;
    char line[260];
    int cnt = 0;
    char tmp_file[256];
    FILE *fp;

    if ((fp = fopen(MASKSHOP_LOG, "r")) == NULL)
    {
	files[0] = NULL;
	return(0);
    }
    /* Read in file names */
    cnt = 0;
    while (line_get(fp, line) != EOF)
    {
	if (sscanf(line, "%s", tmp_file) != 1) break;
	/* find leaf of file name */
	if ((ptr = strrchr(tmp_file, '/')) != NULL)
	++ptr;
	else ptr = tmp_file;
	files[cnt] = malloc(strlen(ptr) +1);
	strcpy(files[cnt], ptr);
	++cnt;
    } /* end while */
    files[cnt] = NULL;
    /* Close file */
    fclose(fp);
    return(cnt);
} /* end trans_maskshop_files_find */

/* Determine mask set name */
/* If can deduce mask set name from file name, simply use it */
/* If not, prompt user for mask set name */
int trans_mask_name_set( char *file_name, char *mask)
{
    DBG();

    /* copy first four characters of file name into mask name */
    strncpy(mask, file_name, 4);
    mask[4] = '\0';
    str_lower(mask);
    if (trans_is_mask(mask) == DBYES) 
	{
	strcpy2(file_name+strlen(file_name), ",", mask);
	    return(SUCCESS);
	} /* end if for valid mask */
    else
    {
	/* Get mask set name from user */
	PRINTF("Please enter the 4 character mask set name.\n>  ");
	scanf("%s", mask);
	mask[4] = '\0';
	/* Put mask in lower case */
	str_lower(mask);
	if (trans_is_mask(mask) == DBYES) 
	{
	strcpy2(file_name+strlen(file_name), ",", mask);
	    return(SUCCESS);
	} /* end if for valid mask */
	else
	{
	/* Indicate invalid mask set name entered */
	sprintf(rtn.msg, invalid_mask_name, mask);
	return(INVALID_MASK_NAME);
	} /* end else for mask set name entered invalid */
    } /* End else for mask set name not determined from file name */
    return(SUCCESS);
} /* end trans_mask_name_set */

int trans_is_mask(char *mask)
{
    DBG();

    if ((mask[0] < 'a') || (mask[0] > 'z')) return(DBNO);
    else if ((mask[1] < '0') || (mask[1] > '9')) return(DBNO);
    else if ((mask[2] < '0') || (mask[2] > '9')) return(DBNO);
    else if ((mask[3] < 'a') || (mask[3] > 'z')) return(DBNO);
    else return(DBYES);
} /* end trans_is_mask */

/* Open file with information about files being sent to the maskshop */
int trans_maskshop_file_open(void)
{
    DBG();
    if ((maskshop_fp = fopen(MASKSHOP_LOG, "w")) != NULL)
    return (SUCCESS);
    else
    {
	strcpy3(rtn.msg, bad_wr_open, " ", MASKSHOP_LOG);
	return(BAD_WR_OPEN);
    }
} /* end trans_maskshop_file_open */

/* Begin building the file to return to the data base */
int trans_maskshop_buffer_build(int index, char *buffer)
{
    DBG();
    int len ;
    long current_time = time(0);
    len = sprintf(buffer, DASHES);
    len += sprintf(buffer+len, "               Send To Mask Shop History For Mask Set '%s'\n\n", trans[index]->mask);
    len += sprintf(buffer+len, "Date:  %s", ctime(&current_time));
    len += sprintf(buffer+len, "Sender's name:  %s\n", user.name);
    len += sprintf(buffer+len, "Mask shop:  %s\n", server.site);
    len += sprintf(buffer+len, "Tool used for transfer:  InterCom\n");
    len += sprintf(buffer+len, "\nFile names:\n\n");
    return(len);
} /* end trans_maskshop_buffer_build */

int trans_maskshop_header_build(int index, char *buffer, int plate_cnt, int processstep_cnt, int file_cnt)
{
    DBG();
    int len ;
    long current_time = time(0);
    len = sprintf(buffer, DASHES);
    len += sprintf(buffer+len, "               Send To Mask Shop History For Mask Set '%s'\n\n", trans[index]->mask);
    len += sprintf(buffer+len, "Date:  %s", ctime(&current_time));
    len += sprintf(buffer+len, "Sender's name:  %s\n", user.name);
    len += sprintf(buffer+len, "Mask shop:  %s\n", server.site);
    len += sprintf(buffer+len, "Tool used for transfer:  Mask Designer\n");
    len += sprintf(buffer+len, "Number of plates:  %d\n", plate_cnt);
    len += sprintf(buffer+len, "Number of process steps:  %d\n", processstep_cnt);
    return(len);
} /* end trans_maskshop_header_build */

/* Close the file with the mask set info */
void trans_maskshop_file_close(void)
{
    DBG();

    if (maskshop_fp == NULL) return;
    /* Write final message to file */
	fprintf(maskshop_fp, "\nNumber of files: %d\n\n", global_file_cnt);
    /* Close file */
    fclose(maskshop_fp);
} /* end trans_maskshop_file_close */

/* Send maskshop file to Intercom server */
void trans_maskshop_file_send(void)
{
    DBG();

    /* If the file is not present, just return */
    if (access(MASKSHOP_LOG, 0) != 0) return;
#ifdef SOLARIS
    if (mask_log_append(remote_mask, MASKSHOP_LOG, MASKSHOP_LOG, NONE)  == SUCCESS)
    strcpy3(rtn.msg, "\nTransfer log for mask set '", remote_mask, 
         "' updated in the Mask Designer database.");
    /* Do not print the error message returned */
	else rtn.msg[0] = '\0';
#endif
} /* trans_maskshop_file_send */

/* Log file name in maskshop file */
void trans_maskshop_file_log(char *file_name, struct stat  *buf, int get_buf)
{
    DBG();
    
    if (maskshop_fp != NULL)
    {
	    if (get_buf == DBYES)
	    {
	        if (stat(file_name, buf) != 0) return;
	    } /* End if for needed to get file info */

	    /* Increment number of files found */
	    /* Print info about file to log */
	    fprintf(maskshop_fp, "%s/%s - Size %lld bytes, Last Update: %s", user.cwd, file_name, (unsigned long long)buf->st_size, ctime(&buf->st_mtime));
    } /* end if  for log file present */

} /* End trans_maskshop_file_log */


/* Send file to Intercom server */
int trans_intercom_file_send(int first_index, int is_icom)
{
    int index;
    int ret;
    char tmp_rtn_msg[BUFFER_SIZE];

    DBG();

    index = trans_init(DBNO, DBNO, DBNO, DBNO);
    /* Initialize the structure */
trans[index]->req_type = DBMASK_FILELIST_SEND;
    strcpy(trans[index]->keyword,  trans[first_index]->mask);
    strcpy(trans[index]->par1,  trans[first_index]->keyword);
    trans[index]->file_mode = 16895;
    if ((ret = trans_socket_init(index)) != SUCCESS)
    return(ret);
    /* Transfer the file */
    if ((ret = trans_intercom_file_put(first_index, is_icom)) != SUCCESS)
    return(ret);
    /* Get the return code */
    /* Save current return code */
    strcpy(tmp_rtn_msg, rtn.msg);
    ret = trans_ret_code_get();
    /* copy in the new return message */
    strcat(tmp_rtn_msg, rtn.msg);
    strcpy(rtn.msg, tmp_rtn_msg);
    /* Free the structure no longer needed */
    trans_cleanup(index);
    /* Delete maskshop log file no longer needed */
    unlink(MASKSHOP_LOG);
    return(ret);
} /* End trans_intercom_file_send */

/* Upload the Intercom log file */
int trans_intercom_file_put(int first_index, int is_icom)
{
    DBG();

    short ret;
    long long size = 0;
    int fd;
    char buffer[BUFFER_SIZE];

    if ((fd = open(MASKSHOP_LOG, O_RDONLY)) < 0)
    {
	    strcpy7(rtn.msg, bad_rd_open, RET2TAB, "'", MASKSHOP_LOG, "'", 
	    DOUBLE_RET, bad_rd_open_msg);
	    /* Send error message to server */
	    put_ssl_short(BAD_CREATE);
	    close(sock);
	    	return(FAILURE);
    } /* End if for failure to open file */
    /* build first part of file to send */
    /* Build first part of file if this is InterCom */
    if (is_icom == DBYES)
    {
	ret = trans_maskshop_buffer_build(first_index, buffer);
	put_ssl_binary_string( buffer, ret);
    } /* End if */
    /* Now transfer the file ata */
    while ((ret = read(fd, buffer, BLOCK_SIZE)) > 0)
    {
	put_ssl_binary_string(buffer, ret);
	size += ret;
    }
    close(fd);
    /* Tell server all data sent */
    put_ssl_short(SUCCESS);
    put_ssl_longlong(size);
    return(SUCCESS);
} /* end trans_intercom_file_put */

/* Retrieve mask set history */
int maskshop_history(char *mask)
{
    DBG();

    int ret;
    int index;
    char *files[M2TOTAL_FILES];
    char dir[24];
    char c_string[8];

    /* First make sure valid mask entered */
    if (trans_is_mask(mask) == DBNO)
    {
	sprintf(rtn.msg, invalid_mask_name, mask);
	return(INVALID_MASK_NAME);
    }
    /* Make the mask set history directory */
    strcpy2(dir, mask, ".history");
    if (access(dir, 0) == 0)
    {
	PRINTF("Directory '%s' already exists.  Overwrite this file, y/n?  ", dir);
	scanf("%s", c_string);
	if (( c_string[0] == 'y') || (c_string[0] == 'Y')) 
	{
	    /* Delete the file and indicate files have been overwritten */
	    unlink(dir);
	} /* End if */
	else
	{
	    sprintf(rtn.msg, "Retrieval of '%s' aborted.\n\n", dir);
	return(-1);
	}
    } /* End if for file already present */
    /* Initialize socket */
    index = trans_init(DBNO, DBNO, DBNO, DBNO);
    /* Initialize the structure */
trans[index]->req_type = DBMASK_HISTORY;
    strcpy(trans[index]->keyword,  mask);
    if ((ret = trans_socket_init(index)) != SUCCESS)
    return(ret);
    if ((ret = dir_crt(dir)) != SUCCESS)
    return(ret);
    /* Now get the files */
    mask_files_get(dir, files);
    /* Now get the return code */
    ret = trans_ret_code_get();
    /* Free the structure no longer needed */
    trans_cleanup(index);
    return(ret);
} /* End maskshop_history */

/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
