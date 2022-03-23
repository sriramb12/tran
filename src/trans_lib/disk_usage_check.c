/****************************************************************************
 *
 * Function:	disk_usage_check
 *
 * Description:	Check amount of disk space available. If usage over 
 *              percent specified send message to system administrator
 *		        Calling program sends email.
 *
 ***************************************************************************/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_server_macros.h"
#include "trans_server_errs.h"
#include "trans_lib_prototypes.h"
#include "trans_server_paths.h"
#include "global_debug.h"


int disk_usage_check(char *msg)
{
    extern int global_disk_usage;
    int cnt;
    FILE *fp;
    int len = 0;
    char line[260];
    int percent = 0;
    int new_percent = 0;
    char partition[32];

    DBG();

    /* Open the disk usage file for reading */
    if ((fp = fopen(TRANS_DISK_FILE, "r")) == NULL)
        close_down1m(rd_open_err, TRANS_DISK_FILE, RD_OPEN_ERR);

    for (cnt = 0; cnt < 10; ++cnt)
    {
	    line_get(fp, line);
	    if (sscanf(line, "%s %d", partition, &new_percent) != 2) continue;

	    if (new_percent > global_disk_usage)
	    {
	        len += sprintf(msg+len, "%s %d\n", partition, new_percent);
	    } /* End if */
	    
	    if (new_percent > percent) percent = new_percent;
    } /* End for */

    /* Close file no longer needed */
    fclose(fp);

    return(percent);
} /* End disk_usage_check */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
