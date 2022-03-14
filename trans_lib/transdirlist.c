/****************************************************************************
 *
 * Function:	transdirlist_rtv
 *
 * Description:	Retrieves transdirlist of a big file from the Transcend database.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <dirent.h>
     #include <sys/types.h>
     #include <sys/stat.h>
     #include <fcntl.h>
#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
     #include <unistd.h>
     #include <sys/uio.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_server_globals.h"
#include "trans_defines.h"
#include "rtn.h"
#include "strcpy.h"
#include "server_lib.h"
#include "line.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h"

    char line[TRANS_LINE_NUM][TRANS_LINE_LEN];
/* Send a dynamicly created dirlist to the client */
int trans_dynamic_dirlist_rtv(char *key)
{
    int len;
    int ret;
    extern TRANS_FILE_STAT fs;
    char buffer[BUFFER_SIZE];

    DBG("key='%s'", key);

    /*  build the first part of the file */
    len = sprintf(buffer, "# Keyword:  %s\n", fs.key);
    if (fs.file_type == DIRECTORY)
    {
        len += sprintf(buffer+len, "# Number dir levels:  unknown\n");
        len += sprintf(buffer+len, "# Number of files:  unknown\n");
    }
    else if (fs.file_type == FILES)
    {
        len += sprintf(buffer+len, "# Number dir levels:  0\n");
        len += sprintf(buffer+len, "# Number of files:  unknown\n");
    }
    else
    {
        len += sprintf(buffer+len, "# Number dir levels:  0\n");
        len += sprintf(buffer+len, "# Number of files:  1\n");
    } /* end else */
    len += sprintf(buffer+len, "# Trans user:  %s\n#\n", user.name);
    len += sprintf(buffer+len, "%s %lld\n", fs.real_name, fs.original_size);
    /* Indicate that file wil be sent */
    put_short(SUCCESS);
    /* Send name of file */
    /* Pass root name only without full path */
    put_string(TRANS_DIRLIST);
    /* Get acknowledgement */
    if ((ret = get_short()) != SUCCESS)
    {
        data_log(tr.log_file, get_err);
        exit(1);
    }
    /* Send the data */
    put_binary_string(buffer, len);
    fs.original_size = len;
    /* Tell client all data for this file sent */
    put_short(SUCCESS);
    /* Send return code */
    strcpy(rtn.msg, "File 'trans_dirlist.txt' retrieved");
    put_success_msg(key);
} /* end trans_dynamic_dirlist_rtv */

void transdirlist_rtv(char *key)
{
    extern char global_secure_keyword[];
    extern TRANS_FILE_STAT fs;
    short ret;
    int total_len;
    FILE *fp;

    DBG("key='%s'", key);

    strcpy(global_secure_keyword, key);
    /* If the file key is found, it is returned in variable 'key' */
    /* Read the info file to get needed information */
    read_big_file(key, line, &fs);
    /* Send the transdirlist file to the user */
    /* Build complete name of file */
    strcpy3(bf.file_name, HISTORY_FILE_PATH, key, ".dirlist");

    fp = fopen(bf.file_name, "r");
    if (fp != NULL)
    {
        /* Indicate that file wil be sent */
        put_short(SUCCESS);
        /* Send name of file */
        /* Pass root name only without full path */
        put_string(TRANS_DIRLIST);
        /* Get acknowledgement */
        if ((ret = get_short()) == SUCCESS)
        {
            /* Throw away the first line */
            /* It just has the time stamp */
            line_get(fp, bf.buffer);
            /* Read text from the file and write them to the socket */
            while ((ret = fread(bf.buffer, sizeof(*bf.buffer), 4096, fp)) > 0)
            {
                put_binary_string(bf.buffer, ret);
                total_len += ret;
            }
            /* Tell client all data for this file sent */
            put_short(SUCCESS);
        } /* end if */
        /* Close the file just sent */
        fclose(fp);
        /* Put length of file in structure for correct reporting */
        fs.original_size = total_len;
        /* Send the return code */
        strcpy(rtn.msg, "File 'trans_dirlist.txt' retrieved");
        put_success_msg(key);
    }
    else
    {
        trans_dynamic_dirlist_rtv(key);
    }
} /* End transdirlist_rtv */

/* Receive a trans_dirlist file from the client */
void transdirlist_put(char *keyword)
{
    int ret;
    char file_name[64];
    int fd;

    DBG("keyword='%s'", keyword);

    /* build the name for the trans_dirlist file */
    strcpy3(file_name, "history_data/", keyword, ".dirlist");
    /* Open this file for writing */
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0666)) < 0)
    close_down1m(wr_open_err, file_name,  WR_OPEN_ERR);
    /* Tell client ready to get data */
    put_short(SUCCESS);
    /* Write the timestamp to the first line of the file */
    sprintf(bf.buffer, "%ld\n", time(0) + TWO_WEEKS);
    write(fd, bf.buffer, strlen(bf.buffer));
    /* Read in blocks of text and write them out to the file */
    while ((ret = get_string(bf.buffer)) > 0)
    ret = write(fd, bf.buffer, ret);
   /* Tell client data retrieved as a warning message */
    strcpy(rtn.msg, "");
    put_success_msg(keyword);
} /* end transdirlist_put */

/* Build a transdirlist on the server */
/* this is done when the data is transferred as a zipped tarred directory */
void transdirlist_build(char *file_keyword, char *real_name, int disk_index)
{
    extern char transdirlist[];
    extern int global_file_cnt;
    extern int global_dir_cnt;
    int ret;
    char dir_name[FILE_SIZE];
    char tmp_file[64];
    char buffer[BUFFER_SIZE];

    DBG("file_keyword='%s', real_name='%s' and disk_index='%d'", file_keyword, real_name, disk_index);

    global_dir_cnt = global_file_cnt = 0;
    /* Build name for dirlist file */
    /* Must use the .. becauase server is running in data_files directory */
    strcpy3(transdirlist, "history_data/", file_keyword, ".dirlist");
    /* Build name for directory being listed */
    sprintf(dir_name, "data_files%d/%s.gz/%s", disk_index, file_keyword, real_name);
    transdir_list(dir_name);
    /* Now build the header for the file */
    ret = sprintf(buffer, "%ld\n# Keyword:  %s\n", time(0) + TWO_WEEKS, file_keyword);
    ret += strcpy3(buffer+ret, "Directory name:  ", real_name, "\n");
    ret += sprintf(buffer+ret, "# Number dir levels:  %d\n", global_dir_cnt);
    ret += sprintf(buffer+ret, "# Number of files:  %d\n", global_file_cnt);
    ret += sprintf(buffer+ret, "# Trans user:  %s\n#", user.name);
    /* Now write out the new file */
    strcpy2(tmp_file, transdirlist, ".tmp");
    data_log(tmp_file, buffer);
    /* Now add contents of real file */
    file_append(transdirlist, tmp_file);
    file_rename(tmp_file, transdirlist);
    /* Remove temporary file no longer needed */
    unlink(tmp_file);
} /* End transdirlist_build */

/* The directory name passed in must not have an ending slash */

void transdir_list(char *directory)
{
    extern char transdirlist[];
    extern int global_file_cnt;
    extern int global_dir_cnt;
    char string[FILE_SIZE];
    char file_name[4096];
    struct stat buf;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG("directory='%s'", directory);

    if ((dp = opendir(directory)) == NULL)
    return;
    /* Increment number of directory levels */
    ++global_dir_cnt;
    for (dir=readdir(dp); dir != NULL; dir=readdir(dp)) 
    {
    /* Ignore '..' file */
	if (strcmp(dir->d_name, "..") == 0)continue;
	/* Build complete file name */
	strcpy3(file_name, directory, "/", dir->d_name);
	if (stat(file_name, &buf) != 0)  continue;
	if ((strcmp(dir->d_name, ".") == 0)
	|| ((S_ISDIR(buf.st_mode)) < 1))
	{
	if (strcmp(dir->d_name, ".") != 0)
    {
	/* Add 25 to get rid of Transcend directory name data_files0/keyword */
	sprintf(string, "%s %lld", file_name+25, buf.st_size);
	data_log(transdirlist, string);
	++global_file_cnt;
    } /* end if */
	} /* End if */
	/* Otherwise find out if directory is a link */
	/* Links are not followed */
	else
	{
	    if (lstat(file_name, &buf) != 0) continue;
	    /* If this is a link, ignore it */
	    if (S_ISLNK(buf.st_mode)) continue;
	    else transdir_list(file_name);
	} /* End else */
    } /* End for */
    closedir(dp);
} /* end transdir_list */
