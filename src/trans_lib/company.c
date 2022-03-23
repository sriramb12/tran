/****************************************************************************
 *
 * Function: transweb_company_rtv, company_table_rtv, company_table_rtv
 * 
 * Description:	Verifies user can access specified data by
 *              checking to see if his company can  access data
 *		        owned by the other company.
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "server_lib.h"
#include "trans_server_errs.h"
#include "trans_server_prototypes.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_lib_prototypes.h"
#include "trans_server_paths.h"
#include "user_params.h"
#include "global_debug.h"


void transweb_company_rtv(void)
{
    int cnt;
    char file_name[64];
    char *list[1000];

    DBG();

    /* build name for company table */
    strcpy(file_name, TRANSWEB_COMPANY_TABLE);
    DBG("company table is file_name='%s'", file_name);

    cnt = list_read(file_name, list);
    if (cnt <= 0)
        close_down1m(rd_open_err, TRANSWEB_COMPANY_TABLE, RD_OPEN_ERR);

    /* Tell client ready to send data */
    put_short(cnt);

    /* Send the list */
    list_get(list);
    strcpy(rtn.msg, "List of companies retrieved.");

    put_success_msg(NONE);
} /* End transweb_table_rtv */


void transweb_repository_rtv(void)
{
    int cnt;
    char *list[1000];

    DBG();

    cnt = list_read(TRANSWEB_REPOSITORY_TABLE, list);
    if (cnt <= 0)
        close_down1m(rd_open_err, TRANSWEB_REPOSITORY_TABLE, RD_OPEN_ERR);

    /* Tell client ready to send data */
    put_short(cnt);

    /* Send the list */
    list_get(list);
    strcpy(rtn.msg, "List of repositories retrieved.");

    put_success_msg(NONE);
} /* End transweb_repository_rtv */


/* Send latest copy of company file to the client */
void company_table_rtv(void)
{
    convert_destinations_into_csv_values_in_memory();
    put_short(SUCCESS);
    put_success_msg(NONE);
} /* End company_table_rtv */


/*!
 * \brief get_company_info()  returns the complete information from a company
 *
 *       Reads the file db_admin/ssl.company_table
 *
 * \param  the company abbreviation such as "rtc"  or "-rtc"
 *
 * \return the internal address to a local structure TRANS_COMPANY or NULL in case not found
 */
const TRANS_COMPANY * get_company_info(char *abbreviation)
{
    int len =  strlen(abbreviation);
    int counter = 0;

    TRANS_COMPANY * ret = 0;

    if (len > 0)
    {
        str_lower(abbreviation);
        // allow "-rtc" to work as "rtc"
        if (*abbreviation == '-') { ++abbreviation; }
        if (gl_site_destinations_counter == 0)
        {
            close_down1m(rd_open_err, GLOBAL_COMPANY_TABLE, RD_OPEN_ERR);
        }

        for(;counter < gl_site_destinations_counter; ++counter)
        {
            if (strncmp(gl_site_destinations[counter].abbreviation, abbreviation, 3) == 0)
            {
                ret = &gl_site_destinations[counter];
                break;
            }
        }
    }
    return ret;
}

/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
