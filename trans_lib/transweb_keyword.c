/****************************************************************************
 *
 * Function:	process_request
 *
 * Description:	Retrieves list of secure keywords sent to specified user.ain routine called to process database requests
 *		from the database client.  Calls other functions
 *		to perform specific transactions.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "close_down.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_server_arrays.h"
#include "trans_defines.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_paths.h"
#include "reg_globals.h"
#include "trans_server_macros.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "x500_defines.h"
#include "global_debug.h"

/* Delete all user keys */
void user_keywords_delete()
{
    char file_name[128];
    char delta_file[32];
    char data_file[32];
    char *delta_ptr;
    char *data_ptr;
    char index[4];
    char key[10];
    int cnt = 0;
    FILE *fp;
    FILE *fo;

    DBG();
    format_user_keys_file_name(file_name);

    if ((fp = fopen(file_name, "r")) != NULL)
    {
    delta_ptr = strcpy1(delta_file, "global_data/") + delta_file;
    data_ptr = strcpy1(data_file, "data_files") + data_file;
    while (fscanf(fp, "%s", key) == 1)
    {
	strcpy(delta_ptr, key);
	if ((fo = fopen(delta_file, "r")) == NULL) continue;
	if (fscanf(fo, "%*s %s", index) == 1)
	{
	    strcpy4(data_ptr, index, "/", key, ".gz");
	    unlink(data_file);
	} /* End if for found index */
	fclose(fo);
	unlink(delta_file);
	++cnt;
    } /* End while */
    } /* End if for able to open file */
    if (cnt == 0) sprintf(rtn.msg, "No data present to delete for '%s' on the %s repository", user.email, server.site);
    else if (cnt == 1)
    sprintf(rtn.msg, "Deleted data for 1 user keyword on the %s repository.", server.site);
    else sprintf(rtn.msg, "Deleted data for %d user keywords on the %s repository.", cnt, server.site);
    put_short(SUCCESS);
    put_success_msg(NONE);
} /* End user_keywords_delete */
