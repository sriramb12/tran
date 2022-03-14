/****************************************************************************
 *
 * Function:	process_request
 *
 * Description:	Processes requests with data type of MASK *
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include "dir.h"
#include "close_down.h"
#include "entry.h"
#include "global_defines.h"
#include "list.h"
#include "mask_server_defines.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2mask_files.h"
#include "m2server_arrays.h"
#include "m2server_globals.h"
#include "m2state_table.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2server_paths.h"
#include "m2req_types.h"
#include "m2server_prototypes.h"
#include "mti.h"
#include "pre_suffix.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "m2msg.h"

void process_off_template(void)
{

    /* If not reading a template, */
    /* Initialize paths needed to perform offofficial template transactions */
    off_template_paths_init();
    switch(mask_msg.req_type)
{
	case REFRESH: rmt_mask_refresh(); break;
	case MARK: rmt_mask_refresh(); break;
	case AUDIT:
	{
	    long audit_time = audit_time_set();
	    template_audit(OFF_FAB_DATA, &audit_time);
	}
	break;
	case RMT_AUDIT: rmt_audit_data(); break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_off_template */

/* Initialize paths needed to perform template transactions */
void off_template_paths_init(void)
{
	    strcpy3(key, mask_msg.key1, "_", mask_msg.key2);
    strcpy5(mti.data_dir, OFF_FAB_DATA, SLASH, mask_msg.key1, SLASH, mask_msg.key2);
    strcpy5(mti.prefix, "Approved copy of fab '", mask_msg.key1, 
    "', technology '", mask_msg.key2, "'");
    strcpy5(mti.suffix, "approved copy of fab '", mask_msg.key1, 
    "', technology '", mask_msg.key2, "'");
    strcpy2(mti.db_data_dir, mti.data_dir, "/.db");
    strcpy2(mti.tmpdb_data_dir, mti.data_dir, "/.tmpdb");
    strcpy5(mti.delta_dir, FAB_DELTA, SLASH, mask_msg.key1, SLASH, mask_msg.key2);
    strcpy2(mti.db_delta_dir, mti.delta_dir, "/.db");
    /* Build the state file name */
    strcpy2(mti.state_file, mti.delta_dir, "/state_file");
} /* End off_template_paths_init */

