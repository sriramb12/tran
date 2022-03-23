/****************************************************************************
 *
 * Function:	info_big_file
 *
 * Description:	Retrieves info on big file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_lib_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_server_macros.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h"

void info_big_file(char *key)
{
    long long total_size;
    extern TRANS_FILE_STAT fs;
     int cnt;
    char *msg = rtn.msg;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char tmp_key[12];

    DBG();

    strcpy(tmp_key, key);
    /* If the key is found, it is returned in variable 'key' */
    read4info_big_file(key, &fs, DBNO);
    /* Put data into rtn.msg */
    for (cnt = 1; cnt < TRANS_LINE_NUM; ++cnt)
    {
       msg += strcpy2(msg, lines[cnt], "\n");
    }
    /* Get the lock status of the file */
    if (fs.lock_flag == 'l')
    {
	/* Parse out the total site */
	sscanf(lines[2], "%*s %*s %lld", &total_size);
	/* Add transfer status to message */
	transfer_status_big_file(key, msg, total_size);
    }
    /* If the file is locked for retrieval, let user know */
    else if (fs.lock_flag == 'L')
    sprintf(msg, "Status:      Lock for retrieval\n");
    /* Indicate data will be sent */
    put_short(SUCCESS);
    /* Send the return code */
    put_success_msg(key);
} /* End info_big_file */
/*es.files.fileuse*/
/*is.files.fileuse*/
