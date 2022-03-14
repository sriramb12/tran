#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	trans_disk_usage
 *
 * Description:	Calculates size of specified directory tree
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <strings.h>
#include <string.h>
#include "client_disk_usage.h"
#include "global_defines.h"
#include "list.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transapi_hidden_prototypes.h"

#include "global_debug.h"

/* Calculate disk usage for directory being transferred */
unsigned long long trans_disk_usage(char *directory, int follow_links, int offset, int dir_levels)
{
    DBG();

    extern unsigned long long global_compressed_size;
    extern int global_dir_cnt;
    int new_dir_cnt;
    char file_name[4096];
    char command[FILE_SIZE];
    unsigned long long file_size = 0;
    struct stat buf;
    FILE *po;

    global_dir_cnt = 0;
#ifdef SOLARIS
    /* Solaris does not support the -b option */
    if (follow_links == DBNO)
        strcpy2(command, "du -a -k ", directory);
    else strcpy2(command, "du -a -k -L ", directory);
#else
    if (follow_links == DBNO)
        strcpy2(command, "du -a -b ", directory);
    else strcpy2(command, "du -a -b -L ", directory);
#endif
    po = popen(command, "r");
    global_compressed_size = 0;

    while (fscanf(po, "%*s %s", file_name) != EOF)
    {
	    /* Use stat so can tell which files are links */
	    if (lstat(file_name, &buf) != 0)  continue;
	
	    /* If not following links and this is a link, ignore it */
	    if ((follow_links == DBNO) && (S_ISLNK(buf.st_mode))) continue;

	    /* If this is a link, need to get file size using stat */
	    /* The file size with lstat is for the link instead of the file */
	    if (S_ISLNK(buf.st_mode))
	    {
	        if (stat(file_name, &buf) != 0)  continue;
	    } /* end if */

	    file_size += buf.st_size;
	
	    /* Count the number of directory levels */
	    if ((new_dir_cnt = str_numchar(file_name, '/')) > global_dir_cnt)
	        global_dir_cnt = new_dir_cnt;
	
	    /* If this is a a directory, do not log it */
	    if (S_ISDIR(buf.st_mode))  continue;
            trans_dirlist_log(file_name+offset, buf.st_size);
	/* If sending data to mask shop, append to log */
#ifdef INTERCOM
#ifdef NXP 
	    trans_maskshop_file_log(file_name, &buf, DBNO);
#endif
#endif
    } /* End while */

    /* Close the pipe command */
    pclose(po);

    /* Set the global directory count */
    global_dir_cnt -= dir_levels;

    /* Return total file size */
    return(file_size);
} /* end trans_disk_usage */

#endif
