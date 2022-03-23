/****************************************************************************
 *
 * Function:	mod_lock_cnt
 *
 * Description:	Increments the lock count.
 *		If lock count is 7 or greater, users cannot
 *		access the external site.  The lock_cnt is incremented
 *		each time the user enters an invalid keyword.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "registry_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "trans_lib_prototypes.h"
#include "user_params.h"
#include "rtn.h"
#include "strcpy.h"
#include "global_debug.h"


/* No longer keeping track of lock count */
int mod_lock_cnt(int increment)
{
    DBG();
    return(0);
}

/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
