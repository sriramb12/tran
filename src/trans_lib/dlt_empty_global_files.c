/****************************************************************************
 *
 * Function:	dlt_empty_global_files
 *
 * Delete global files with 0 length.
 *
 ***************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
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
#include "global_debug.h"


int dlt_empty_global_files(char *file_path)
{
    int dlt;
    int disk_index;
    char file_type;
    int num = 0;
    int cnt;
    int fd;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();
    char data_file[FILE_SIZE];
    char history_file[FILE_SIZE];
    char delta_file[FILE_SIZE];
    char *delta_ptr;
    char *history_ptr;

    DBG();

    if ((dp = opendir(file_path)) == NULL)
        close_down2d(open_dir_err, errno, file_path, OPEN_DIR_ERR);

    /* Initialize first part of file paths */
    delta_ptr = strcpy1(delta_file, GLOBAL_FILE_PATH) + delta_file;
    history_ptr = strcpy1(history_file, HISTORY_FILE_PATH) + history_file;

    while ((dir = readdir(dp)) != NULL)
    {
	    dlt = DBNO;

	    /* Only work with the files built from 9 character key */
	    if (strlen(dir->d_name) != 9) continue;

	    /* Build the global file name */
	    strcpy(delta_ptr, dir->d_name);
	    if ((fd = open(delta_file, O_RDONLY)) == EOF) continue;

	    /* Read in data from the file */
	    cnt = read(fd, bf.buffer, 1000);
	    close(fd);

	    /* If got no data, delete the file */
	    if ((cnt < 2) || (bf.buffer[2] < '0') || (bf.buffer[2] > '9') || (sscanf(bf.buffer, "%c %d", &file_type, &disk_index) != 2))
        {
	        dlt = DBYES;
        }
	    else 
	    {
	        strcpy3(data_file, disk.path[disk_index], dir->d_name, ".gz");
	        if (access(data_file, 0) != 0)
	            dlt = DBYES;
	    } /* End else */

	    if (dlt == DBYES)
	    {
	        unlink(delta_file);

	        /* put entry in history file if it exists */
	        strcpy2(history_ptr, dir->d_name, ".history");
	        
	        if (access(history_file, 0) == 0)
	            log_history_file(dir->d_name);

	        ++num;
	    } /* End if */
    } /* end while */

    closedir(dp);
    
    return(num);
} /* End dlt_empty_global_files */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
