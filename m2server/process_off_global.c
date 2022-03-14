/****************************************************************************
 *
 * Function:	process_off_global_
 *
 * Description:	Processes requests to access single global tables
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <unistd.h>
#include "dir.h"
#include "close_down.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "mask_server_defines.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2mask_files.h"
#include "m2server_globals.h"
#include "m2state_table.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2server_paths.h"
#include "m2req_types.h"
#include "mti.h"
#include "server_lib.h"
#include "user_params.h"
#include "rtn.h"
#include "m2msg.h"
#include "m2server_prototypes.h"
#include "strcpy.h"
#include "pre_suffix.h"

/* Initialize paths needed to perform transaction */
void off_global_paths_init(void)
{
	    strcpy(key, mask_msg.key1);
    strcpy1(mti.data_dir, OFF_GLOBAL_DATA);
    strcpy1(mti.delta_dir, GLOBAL_DELTA);
    strcpy1(mti.tmp_data_dir, TMP_GLOBAL_DATA);
    strcpy1(mti.tmp_delta_dir, TMP_GLOBAL_DELTA);
    strcpy3(mti.prefix, "Approved global table '", mask_msg.key1, "'");
    strcpy3(mti.suffix, "approved global table '", mask_msg.key1, "'");
    /* Build the state file name */
    strcpy4(mti.state_file, GLOBAL_DELTA, SLASH, mask_msg.key1, ".state");
} /* End global_paths_init */

void process_off_global()
{

    /* Initialize paths needed to perform global transaction */
     off_global_paths_init();
    switch(mask_msg.req_type)
{
    /* Create a new global file by porting it from the user's disk */
	/* Force shadow of global data to all database machines */
	case SHADOW: mask_shadow(global_refresh); break;
	case REFRESH: rmt_refresh_global(); break;
	case MARK: rmt_global_refresh(); break;
	case AUDIT:  
	{
	    long audit_time = audit_time_set();
	    global_audit(&audit_time);
	}
	break;
	case RMT_AUDIT:  rmt_audit_global(); break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* End process_off_global */
