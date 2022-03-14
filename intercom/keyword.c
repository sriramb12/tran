/****************************************************************************
 *
 * Function:	Keyword functions
 *
 * Description:	Adds, deletes and modifies keyword entries
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "close_down.h"
#include "customer_table.h"
#include "dir.h"
#include "mm_email_msg.h"
#include "files.h"
#include "external_intercom_prototypes.h"
#include "ftp_info.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "reg_globals.h"
#include "registry_prototypes.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h"


/* Delete keyword entries no longer needed */
int dlt_keyword_entries(void)
{
    char keyword_file[32];
    char *files[10000];
    int dlt_cnt = 0;
    int cnt;
    char decrypted_keyword[12];
    char history_file[32];
    char *history_ptr;
    FILE *fi;

    DBG();

    /* Build prefix for global file */
    history_ptr = strcpy1(history_file, HISTORY_FILE_PATH) + history_file;
     /* List keyword files */
    dir_list(KEYWORD_DIR, files);
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
        /* build name of keyword file */
        strcpy2(keyword_file, KEYWORD_DIR, files[cnt]);
        if ((fi = fopen(keyword_file, "r")) == NULL) continue;
        if (fscanf(fi, "%*s %s", decrypted_keyword) != 1)
        {
            fclose(fi);
            continue;
        }
        fclose(fi);
        /* Build the global file name */
        strcpy2(history_ptr, decrypted_keyword, ".history");
        /* See if history file exists */
        if (access(history_file, 0) != 0)
        {
            unlink(keyword_file);
            ++dlt_cnt;
        } /* end if */
    } /* end for */
    /* Return the number of enntries deleted */
    return(dlt_cnt);
} /* End dlt_keyword_entries */
