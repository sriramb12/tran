/****************************************************************************
 *
 * Function:	process_mask
 *
 * Description:	Processes requests with data type of SQL_QUERY *
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_sql_lib.h"
#include "m2mask_prototypes.h"
#include "m2globals.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "m2req_types.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h"

#define DTMS_SIGNOFF_ERR -1
#define DTMS_NOT_NEEDED -741

int sql_dtms_status_rtv(char *one, char *two, char *three)
{
    return(SUCCESS);
}

/* Retrieve list of projects associated with the mask set */
void mask_projects_rtv(void)
{
    char *projects[2];

    /* Read in the projects */
    projects[0] = "DTMS not needed";
    projects[1] = NULL;
    /* Tell client ready to send project names */
    put_short(1);
    list_get(projects);
    /* Send return code */
    return_success("Project list retrieved.", DBNO);
} /* end mask_projects_rtv */

void process_sql(void)
{
    char mwr_num[12];
    DBG("mask_msg.req_type=%d", mask_msg.req_type);
    switch(mask_msg.req_type)
    {
    case MASK_PROJECTS_RTV: mask_projects_rtv(); break;
    case DTMS_STATUS_RTV:
    {
        char project_date[32];
        int ret;

        /* Run query to see if DTMS has completed */
        if ((ret = sql_dtms_status_rtv(mask_msg.key1, mwr_num, project_date)) == SUCCESS)
        {
            sprintf(rtn.msg, "dtms_signoff_done", mwr_num, project_date);
            put_short(SUCCESS);
            return_success(rtn.msg, DBNO);
        } /* end if */
        else if (ret == DTMS_NOT_NEEDED)
        {
            strcpy(rtn.msg, "dtms_not_needed");
            return_success(rtn.msg, DBNO);
        } /* end if */
        else close_down1f("dtms_signoff_err", mwr_num, DTMS_SIGNOFF_ERR);
    }
        break;
        /* Update the die per shot in reticle_die table */
    case RETICLE_LAYOUT_UPDATE:
    {
        reticle_die_update(mask_msg.key1, mask_msg.key2, mask_msg.key3, mask_msg.par1, mask_msg.num);
        put_short(SUCCESS);
        return_success("Reticle_die table updated", DBNO);
    }
        break;
    default:
    {
        DBG("bad BAD_REQ_TYPE, exiting ...");
        /* Return failure code to client */
        close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* end default */
    } /* end switch */
} /* end process_sql */
