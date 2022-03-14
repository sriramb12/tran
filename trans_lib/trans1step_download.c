/****************************************************************************
 *
 * Function:	trans1step_download
 *
 * Description:	Routines to perform 1 step downloads
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include "errno.h"
#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_defines.h"
#include "trans_server_defines.h"
#include "trans_server_paths.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_arrays.h"
#include "trans_lib_prototypes.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

/* Decide if file needs to be unzipped */
void trans_unzip_untar_data(char *unzip, char *untar)
{
    DBG();

    /* Set default for untar to not needed */
    strcpy(untar, "untar=no");
    /* If file type is directory or files, it needs to be unzipped and untarred */
    if ((fs.file_type == DIRECTORY) || (fs.file_type == FILES))
    {
	strcpy(untar, "tar=yes");
	strcpy(unzip, "unzip=yes");
    } /* end if */
    /* If original and compressed size different, it needs to be unzipped */
    else if (fs.original_size != fs.compressed_size)
    strcpy(unzip, "unzip=yes");
    /* If original and compressed size the same, does not need to be unzipped */
    else strcpy(unzip, "unzip=dbno");
} /* end trans_unzip_untar_data */

/* Process request to perform 1 step download from client */
void trans1step_download(char *keyword)
{
     extern TRANS_FILE_STAT  fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    int ret;
    FILE *fp;
    char cwd[PATH_SIZE];
    char unzip[10];
    char untar[10];

    DBG();

    /* Open the global file to get needed info */
    fp = read_big_file(keyword, lines, &fs);
     /* Close file no longer needed */
    fclose(fp);
    lock_status_check();
    /* If data needs to be deleted, set delete flag */
    if ((gu.req_type == DBGETDLT_DATA)
    && (fs.dlt_flag != 3))
    fs.dlt_flag = DBYES;
    /* Get the current working directory */
    ret = getcwd(cwd, PATH_SIZE);
    /* Determine if data needs to be uncompressed and untarred */
    trans_unzip_untar_data(unzip, untar);
    /* Tell client ready to send file name */
    put_short(SUCCESS);
    sprintf(rtn.msg, "%s;%s/data_files%d/%s;%s;%s",
    fs.real_name, cwd, fs.disk_index, gu.key1, unzip, untar);
    /* Send the return code */
    put_success_msg(keyword);
} /* end 1step_download */

/* Indicate 1 step download complete */
void trans1step_download_complete(char *keyword)
{
    FILE *fp;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
     extern TRANS_FILE_STAT  fs;

     DBG();

    /* Open the global file to get needed info */
    fp = read_big_file(keyword, lines, &fs);
     /* Close file no longer needed */
    fclose(fp);
    put_short(SUCCESS);
    /* Send the return code */
    get_big_file_ack(keyword);
} /* end 1step_download_complete */
