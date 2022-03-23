/****************************************************************************
 *
 * Function:	dlt_history_files
 *
 * Description:	Delete history files which have expired.
 *		        History files expire 2 weeks after the real file does.
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/file.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include "close_down.h"
#include "dir.h"
#include "strcpy.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_macros.h"
#include "trans_lib_prototypes.h"
#include "trans_server_paths.h"
#include "global_debug.h"

#define HISTORY 0
#define DIRLIST 1


void dlt_history_files(int num[2])
{
    long dlt_time;
    int file_type;
    int ret;
    FILE *fp;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();
    char data_file[FILE_SIZE];
    char *data_ptr;

    DBG();

    /* Indicate that not history and dirlist files deleted */
    num[0] = num[1] = 0;

    if ((dp = opendir(HISTORY_FILE_DIR)) == NULL)
        close_down2d(open_dir_err, errno, GLOBAL_FILE_DIR, OPEN_DIR_ERR);
        
    /* Initialize first part of file paths */
    data_ptr = strcpy2(data_file, HISTORY_FILE_DIR, "/") + data_file;

    while ((dir = readdir(dp)) != NULL)
    {
	    /* All history files are  17 characters in length */
	    /* Ignore all other files */
	    if (strlen(dir->d_name) != 17) continue;

	    /* If file suffix is not '.history' ignore it */
	    if (strcmp(".history", dir->d_name+9) == 0) file_type = HISTORY;
	    else if (strcmp(".dirlist", dir->d_name+9) == 0) file_type = DIRLIST;
	    else continue;

	    /* Build the history file name */
	    strcpy(data_ptr, dir->d_name);

	    if ((fp = fopen(data_file, "r")) == NULL) continue;

	    /* Read in data from the file */
	    ret = fscanf(fp, "%ld", &dlt_time);
	    
	    /* Close the file */
	    fclose(fp);
	    
	    if (ret != 1)
	    {
	        /* If unable to read file, delete it */
	        unlink(data_file);
	        ++num[file_type];
	        continue;
	    } /* End if for unable to read history file */

	    /* If delete time earlier to or equal to current time, delete file */
        /* Do not delete if this is a file that does not expire */
	    if ((dlt_time <= tr.upd_time) && (dlt_time != 0))
	    {
	        unlink(data_file);
	        ++num[file_type];
	    } /* End if */
    } /* end while */

    closedir(dp);
} /* End dlt_history_files */


int user_keys_delete(void)
{
    int keyword_cnt = 0;
    char *files[20000];
    FILE *fi;
    FILE *fo;
    char history_file[64];
    char *history_ptr = history_file + strcpy1(history_file, "history_data/");
    char file_name[128];
    char tmp_file[64];
    char *file_ptr = file_name + strcpy1(file_name, "user_keys/");
    char key[64];
    int change;
    int data_fnd;
    int cnt;

    DBG();

    /* Read in all file names in the directory */
    dir_list("user_keys", files);

    /* For each file check to see if history files present for the entries */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    /* Build complete file name */
	    strcpy(file_ptr, files[cnt]);

	    /* Open the file */
	    if ((fi = fopen(file_name, "r")) == NULL) continue;

	    /* Build temporary file name */
	    strcpy2(tmp_file, file_name, ".bak");

	    if ((fo = fopen(tmp_file, "w")) == NULL) 
        {
	        fclose(fi);
	        continue;
        }

	    /* Read in keywords and check for their existence */
	    change = data_fnd = DBNO;

	    while (fscanf(fi, "%s", key) != EOF)
	    {
	        /* build file name */
	        strcpy2(history_ptr, key, ".history");
	        if (access(history_file, 0) == 0)
	        {
	            data_fnd = DBYES;
	            fprintf(fo, "%s\n", key);
	        }
	        else
	        {
	    	    change = DBYES;
	    	    ++keyword_cnt;
	        } /* End else */
	    } /* End while */

	    fclose(fi);
	    fclose(fo);

	    /* If file changed, write out new file */
        if (data_fnd == DBNO)
        {
	        unlink(file_name);
	        unlink(tmp_file);
        }
        else if (access(tmp_file, 0) == 0)
        {
	        if (change == DBYES) file_rename(tmp_file, file_name);
	        else unlink(tmp_file);
        } /* End else */

    } /* End for for reading files */
    return(keyword_cnt);
}
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
