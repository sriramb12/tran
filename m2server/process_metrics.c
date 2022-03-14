/****************************************************************************
 *
 * Function:	process_metrics
 *
 * Description:	Processes requests with data type of METRICS *
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include "create.h"
#include "dir.h"
#include "close_down.h"
#include "global_defines.h"
#include "global_params.h"
#include "list.h"
#include "mask_entry.h"
#include "mask_server_defines.h"
#include "m2globals.h"
#include "m2server_globals.h"
#include "m2req_types.h"
#include "server_lib.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2server_prototypes.h"
#include "user_params.h"
#include "rtn.h"
#include "strcpy.h"
#include "m2msg.h"

void event_log(void)
{
    char log_file[24];
    char log_string[BUFFER_SIZE];
    char data_string[BUFFER_SIZE];

    data_string[0] = '\0';
    /* Tell client ready to get log data */
    put_short(SUCCESS);
    if (get_string(data_string) == 0)
    strcpy(data_string, "None");
    else if (data_string[0] > '127') strcpy(data_string, "None");
    /* Build log name */
    strcpy2(log_file, "event_", tr.log_file);
    /* If correct user ID not passed in, try to generate it */
    if (strcmp(user.email, "n/a") == 0)
    site_map2x500();
    /* If this is not a member of our group, log the transaction */
    if (metrics_dmi() == DBNO)
    {
	/* Build string to log */
	sprintf(log_string, "%s;%s;%s;%ld;%s;%s", mask_msg.key1,
	mask_msg.key2, mask_msg.key3, mask_msg.transaction_time, user.email, data_string);
	/* Log the data */
	data_log(log_file, log_string);
	/* If this is reticle builder, make entry in reticlebld table */
	/* Only make entry for mask set runs */
	if ((strcmp(mask_msg.key2, "Reticle_Builder") == 0)
	&& (is_mask(mask_msg.key1) == DBYES))
	/* Mask set in key1 and state in key3 */
	sql_reticle_bld_insert(mask_msg.key1, mask_msg.key3, user.email);
    } /* End if */
    /* Tell user transaction successful */
    return_success("Data logged in event log", DBNO);
} /* End event_log */

/* Determine if this request is from a member of the DMI team */
int metrics_dmi(void)
{
    if ((strncmp(user.email, "rxfh80", 6) == 0)
    || (strncmp(user.email, "rrbv10", 6) == 0)
    || (strncmp(user.email, "akx850", 6) == 0)
    || (strncmp(user.email, "rmn720", 6) == 0)
    || (strncmp(user.email, "rlvw20", 6) == 0)
    || (strncmp(user.email, "rzcr20", 6) == 0)
    || (strncmp(user.email, "rrnj60", 6) == 0)
    || (strncmp(user.email, "rna020", 6) == 0)
    || (strncmp(user.email, "rpfh80", 6) == 0)
    || (strncmp(user.email, "rcbf90", 6) == 0)
    || (strncmp(user.email, "recc80", 6) == 0))
    return(DBYES);
    else return(DBNO);
} /* End metrics_dmi */

/* Map user's site to his X.500 user ID */
/* The user's X.500 ID is returned in x500 parameter */
/* If it is not found, his login name is returned */
void site_map2x500(void)
{
    int len;
    int fnd;

    /* Build the user's site */
    len = strcpy1(tr.user_site, user.login);
    get_site(user.addr, tr.user_site+len);
    /* Look up the user in the site table */
    fnd = reg_lookup();
    /* If found return his user ID */
    if (fnd == DBYES) strcpy(user.email, tr.user_email);
    /* Otherwise return his login name */
    else strcpy(user.email, user.login);
} /* End site_map2x500 */

void process_metrics(void)
{

    /* Make sure data is being logged to wacc */
    if (strcmp(ip.master_site, "wacc") != 0)
    m2server_send("wacc");
    switch(mask_msg.req_type)
    {
	case EVENT_LOG:  event_log(); break;
	case EVENT_LOG_METRICS: mettracks_send("event_log"); break;
	case MARS_METRICS: mettracks_send("contract_tables/mask_log"); break;
	case REPORT_LIST:
	{
	    report_usage(1, mask_msg.key2, mask_msg.key3, mask_msg.key1); 
	}
	break;
	case REPORT_JOIN:
	{
	    report_usage(2, mask_msg.key2, mask_msg.key3, mask_msg.key1); 
	}
	break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_metrics */
