/****************************************************************************
 *
 * Function:	list_dtms_test_files
 *
 * Description:	List DTMS files that do not expire
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "strcpy.h"
#include "trans_server_errs.h"
#include "trans_server_macros.h"
#include "trans_server_globals.h"
#include "trans_lib_prototypes.h"
#include "trans_server_paths.h"
#include "user_params.h"
#include "global_debug.h"

int list_dtms_test_files(void)
{
    DBG();

    int cnt = 0;
    int ret;
    long dlt_time;
    FILE *fp;
    DIR *dp;
    struct dirent *dir;
    char delta_file[FILE_SIZE];
    char *list[200];
    char *delta_ptr;

    /* Make sure user authorized to retrieve list */
    if (sql_is_dtms_tester(user.email) == DBNO)
        shut_down("You are not authorized to see the list of test files", -1);

    if ((dp = opendir("global_data")) == NULL)
        close_down2d(open_dir_err, errno, "global_data", OPEN_DIR_ERR);

    /* Initialize first part of file paths */
    delta_ptr = strcpy1(delta_file, GLOBAL_FILE_PATH) + delta_file;

    while ((dir = readdir(dp)) != NULL)
    {
	    /* Only work with the files built from 9 character key */
	    if (strlen(dir->d_name) != 9) continue;

	    /* Build the global file name */
	    strcpy(delta_ptr, dir->d_name);

        if ((fp = fopen(delta_file, "r")) == NULL) continue;
	    
	    /* Read in data from the file */
        ret = fscanf(fp, "%*c %*d %d", &dlt_time);
        fclose(fp);

        /* If data not found, skip this file */
        if (ret != 1) continue;
	    
	    /* If this is a file that does not expire, put it on the list */
	    if (dlt_time == 0)
	    {
	        list[cnt] = malloc(10);
	        strcpy(list[cnt++], dir->d_name);
	    } /* end if for file found */
    } /* end while */

    list[cnt] = NULL;
    closedir(dp);

    /* If no files found, send error code */
    if (cnt == 0)
        shut_down("No keywords found", -1);
    else
    {
	    /* Send indication that data will be transferred */
	    put_short(cnt);
	    list_get(list);
	
	    /* Send return code */
	    return_success("List of keywords retrieved.", DBNO);
    } /* End else for list sent */

} /* End list_dtms_test_files */
