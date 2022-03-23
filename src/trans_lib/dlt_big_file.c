/****************************************************************************
 *
 * Function:	dlt_big_file
 *
 * Description:	Deletes the specified big file.
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_defines.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_server_macros.h"
#include "rtn.h"
#include "strcpy.h"
#include "server_lib.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h"


/* key for data being deleted */
void dlt_big_file(char *key)
{
    extern char program_name[];
    int ret;
    int dlt_file = DBNO;
    FILE *fp;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    extern TRANS_FILE_STAT  fs;

    DBG();

    /* Open the global file to get needed info */
    fp = read_big_file(key, lines, &fs);

    /* Close file no longer needed */
    fclose(fp);

    if ((fs.dlt_flag == 3) && (strcmp(fs.own_id, user.email) != 0))
    {
	    sprintf(rtn.msg, dlt_lock_err, fs.title, fs.real_name, lines[5]+13, fs.own_id);
	    shut_down(rtn.msg, DLT_LOCK_ERR);
    }
    
    /* Delete the data */
    if (unlink(bf.file_name) == 0)
        dlt_file = DBYES;

    /* Now delete the actual big file */
    /* build big file name */
    strcpy3(bf.file_name, disk.path[fs.disk_index], key, ".gz");

    /* If this is DTMS must delete the directory */
    if (strcmp(server.site, DTMS) == 0) ret = dir_recursive_dlt(bf.file_name);
	else ret = unlink(bf.file_name);

    if (ret == 0) dlt_file = DBYES;

    /* If deleted global file or actual data, tell user successful */
    if (dlt_file == DBYES)
    {
	    /* Tell user file was deleted */
	    put_short(SUCCESS);
	
	    if (fs.file_type == DIRECTORY)
	        strcpy5(rtn.msg, "Directory '", fs.real_name, "' deleted from ", program_name, ".\n");
        else if ((fs.file_type == FILES) || (fs.file_type == HUGE_FILES))
	        strcpy5(rtn.msg, "Files '", fs.real_name, "' deleted from ", program_name, ".\n");
	    else
	        strcpy5(rtn.msg, "File '", fs.real_name, "' deleted from ", program_name, ".\n");

	    /* Send the return code */
	    put_success_msg(key);
    } /* End if */

    /* Otherwise send message that file could not be found */
    else return_non_err(key);
} /* End dlt_big_file */

/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
