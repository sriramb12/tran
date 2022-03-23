#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	trans_dirlist_rtv
 *
 * Description:	Retrieve a trans_dirlist.txt file from Transcend
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "ssl_client_lib.h"

#include "global_debug.h" // Mazieri's debug facilities


    extern char tmp_transdirlist[];
extern int global_dir_offset;
extern int global_file_cnt;
extern int global_dir_cnt;
    extern int log_files;
    extern char global_keyword[];

/* Retrieve the trans_dirlist.txt file and write output to a file */
int trans_dirlist_file_rtv(int index)
{
    DBG();
    int ret;

    /* Verify this is a valid keyword */
    if ((ret = trans_is_keyword(trans[index]->keyword )) != SUCCESS)
    return(ret);
    /* Connect to the Transcend database */
    if ((ret = trans_socket_init(index)) < SUCCESS)
    return(ret);
    /* Get the file name */
    ret = get_ssl_string(trans[index]->file_name);
    /* If transaction failed,  return mesage is initialized */
    if (ret < 0)
	return(ret);
	/* If file already present, delete it */
    	if (access(trans[index]->file_name, 0) == 0) 
	unlink(trans[index]->file_name);
	/* Get the file */
	return(custom_file_get(index));
} /* end trans_dirlist_file_rtv */

/* Retrieve the trans_dirlist.txt file and write output to standard out */
int trans_dirlist_rtv(int index)
{
    DBG();
    int ret;
    char buffer[BUFFER_SIZE];

    /* Verify this is a valid keyword */
    if ((ret = trans_is_keyword(trans[index]->keyword )) != SUCCESS)
    return(ret);
    /* Connect to the Transcend database */
    if ((ret = trans_socket_init(index)) < SUCCESS)
    return(ret);
    /* Get the file name */
    ret = get_ssl_string(buffer);
    /* If transaction failed,  return mesage is initialized */
    if (ret < 0)
	return(ret);
    /* Tell server ready to retrieve data */
    put_ssl_short(SUCCESS);
    while ( 0 != (ret = get_ssl_string(buffer)) )
    PRINTF(buffer);
    ret = trans_ret_code_get();
#ifdef TRANSCEND_GUI
    PRINTF("Operation completed with errors.\n");
#endif
    /* Need to exit so last message is not printed */
    exit(ret);
} /* end trans_dirlist_rtv */

/* Transfer dirlist to repository */
int trans_dirlist_put(int index)
{
    DBG();
    int ret;
    FILE *fp;
    char rtn_buffer[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    DBG();

    /* Save message from the put so it is not overwritten */
    strcpy(rtn_buffer, rtn.msg);
    trans[index]->req_type = TRANSDIRLIST_PUT;
    /* The file keyword becomes the keyword for this transaction */
    strcpy(trans[index]->keyword, trans[index]->file_keyword);
    if ((ret = trans_socket_init(index)) < SUCCESS)
    /* If transaction failed,  return mesage is initialized */
    return(ret);
    /* Now build the header for the file */
    ret = sprintf(buffer, "# Keyword:  %s\n", trans[index]->file_keyword);
    ret += sprintf(buffer+ret, "# Number dir levels:  %d\n", global_dir_cnt);
    ret += sprintf(buffer+ret, "# Number of files:  %d\n", global_file_cnt);
    ret += sprintf(buffer+ret, "# Trans user:  %s\n#\n", user.name);
    put_ssl_binary_string(buffer, ret);
    /* Now must read in the list of files already created */
    if ((fp = fopen(tmp_transdirlist, "r")) != NULL)
    {
    /* Read text from the file and write them to the socket */
    while ((ret = fread(buffer, sizeof(*bf.buffer), BLOCK_SIZE, fp)) > 0) 
    put_ssl_binary_string(buffer, ret);
    fclose(fp);
	/* Get rid of temp file no longer needed */
	unlink(tmp_transdirlist);
    } /* End if for reading in data */
    /* Tell server all data sent */
    put_ssl_short(SUCCESS);
    /* Get the return code */
    ret = trans_ret_code_get();
    /* Now move the message back */
    strcpy(rtn.msg, rtn_buffer);
    return(ret);
} /* end trans_dirlist_put */

/* Add new file name to the temporary trans_dirlist file */
void trans_dirlist_add(char *file_name, unsigned long long size)
{
    DBG();
    char string[FILE_SIZE];

    /* Indicate found one more file */
    ++global_file_cnt;
    /* Build string to log */
	sprintf(string, "%s %llu", file_name, size);
    trans_data_log(tmp_transdirlist, string);
} /* end trans_dirlist_add */

/* Add file name to trans_dirlist and include directory path */
void trans_dirlist2add(char *dir_name, char *file_name, unsigned long long size)
{
    DBG();
    char string[FILE_SIZE];

    /* Indicate found one more file */
    ++global_file_cnt;
    /* Build string to log */
	sprintf(string, "%s/%s %llu", dir_name, file_name, size);
    trans_data_log(tmp_transdirlist, string);
} /* end trans_dirlist2add */

/* build name for trans_dirlist file and set log options */
void trans_dirlist_set(void)
{
    DBG();

    global_file_cnt = 0;
    global_dir_cnt = 0;
    /* Build name for tmp trans dirlist file */
#ifdef WINDOWS_NT4
    sprintf(tmp_transdirlist, "%s.%ld\n", TRANS_DIRLIST, (long)getpid());
    tmp_transdirlist[21] = '\0';
#else
    sprintf(tmp_transdirlist, "/tmp/%s.%ld\n", TRANS_DIRLIST, (long)getpid());
#endif
    /* If the trans_dirlist.txt file present delete it */
    if (access(tmp_transdirlist, 0) == 0)
    unlink(tmp_transdirlist);
    log_files = DBYES;
} /* End trans_dirlist_set */

int trans_dirlist_read(char **files)
{
    DBG();
    int len;
    FILE *fp;
    char file_name[PATH_SIZE];
    int cnt;

    /* Count number of files to scan */
    cnt = trans_dirlist_lines_count(tmp_transdirlist);
    /* Allocate space for file names */
    files = malloc(sizeof(char *) *(cnt+1));
    /* Open the trans_dirlist file */
    if ((fp = fopen(tmp_transdirlist, "r")) == NULL)
    {
	sprintf(rtn.msg, no_files_err);
	return(NO_FILES_ERR);
    } /* end if */
    /* The entire file contains file names and sizes */
    for (cnt = 0; (long_line_get(fp, file_name) > EOF); ++cnt)
    {
	/* Get rid of trailing white space and size */
	len = trans_dirlist_name_clip(file_name);
	files[cnt] = malloc(len + 1);
	strcpy(files[cnt], file_name);
    } /* End for */
    /* Put NULL on last file */
    files[cnt] = NULL;
    unlink(tmp_transdirlist);
    return(SUCCESS);
} /* end trans_dirlist_read */


/* Log to the trans_dirlist file */
void trans_dirlist_log(char *file_name, unsigned long long size)
{
    DBG();
    char string[PATH_SIZE];

	if (log_files == DBYES)
    {
	sprintf(string, "%s %lld", file_name+global_dir_offset, size);
	trans_data_log(tmp_transdirlist, string);
	++global_file_cnt;
    } /* end if */
} /* end trans_dirlist_log */
int trans_dirlist_name_clip(char *file_name)
{
    DBG();
    int len = strlen(file_name);
    int cnt;
    for (cnt = len-1; cnt >= 0; --cnt)
    {
	if (file_name[cnt] == ' ')
	{
	    file_name[cnt] = '\0';
	    len = cnt;
	    break;
	} /* End if */
    } /* end for */
    return(len);
} /* end trans_dirlist_name_clip */

int trans_dirlist_lines_count(char *file_name)
{
    DBG();
    int cnt;
    char command[FILE_SIZE];
    sprintf(command, "wc -l %s", file_name);
    FILE *po = popen(command, "r");
    fscanf(po, "%d", &cnt);
    pclose(po);
    return(cnt);
} /* end trans_dirlist_lines_count */
#endif
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
