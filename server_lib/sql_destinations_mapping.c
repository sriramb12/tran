#include <mysql.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "close_down.h"
#include "global_server_params.h"
#include "strcpy.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "user_params.h"
#include "ldapfsl.h"
#include "company.h"
#include "rtn.h"
#include "global_debug.h"

static const char *MYSQL_DATE_INTERVAL_45_DAYS =  "DATE_SUB(CURDATE(), INTERVAL 45 DAY) <= timestamp";

// global data
TRANS_COMPANY  gl_site_destinations[MAX_SITE_DESTINATIONS];
int            gl_site_destinations_counter = 0;


void populate_destinations_on_server()
{
    char query[1024];
    int ret = FAILURE;
    MYSQL_COLUMN_RETRIEVE fields_info;
    fields_info.fields_counter = 12;
    fields_info.field[0] = gl_site_destinations[0].name;
    fields_info.field[1] = gl_site_destinations[0].abbreviation;
    fields_info.field[2] = gl_site_destinations[0].machine;
    fields_info.field[3] = gl_site_destinations[0].address;
    fields_info.field[4] = gl_site_destinations[0].socket;
    fields_info.field[5] = gl_site_destinations[0].server_type;
    fields_info.field[6] = gl_site_destinations[0].email;
    fields_info.field[7] = gl_site_destinations[0].is_foundry;
    fields_info.field[8] = gl_site_destinations[0].is_maskshop;
    fields_info.field[9] = gl_site_destinations[0].compression;
    fields_info.field[10] = gl_site_destinations[0].extension;
    fields_info.field[11] = gl_site_destinations[0].secure_extension;

    sprintf(query, "SELECT name, abbreviation, machine, address, socket, "
                   "server_type, email, is_foundry, is_maskshop, compression, extension, "
                   "secure_extension"
                   " FROM data_transfer.dt_destinations WHERE active limit %d", MAX_SITE_DESTINATIONS);
    ret = sql_generic_columm_retrieve_into_structure_array(query, &fields_info,
                                                           sizeof(gl_site_destinations[0]),
                                                           sizeof(gl_site_destinations)/sizeof(gl_site_destinations[0]));
    if (ret != FAILURE)
    {
        int counter = 0;
        gl_site_destinations_counter = ret;
        for (; counter < gl_site_destinations_counter; ++counter)
        {
            DBG("[%02d] %s %s %s %s %s %s %s %s %s %s %s %s",
                    counter,
                    gl_site_destinations[counter].name,
                    gl_site_destinations[counter].abbreviation,
                    gl_site_destinations[counter].machine,
                    gl_site_destinations[counter].address,
                    gl_site_destinations[counter].socket,
                    gl_site_destinations[counter].server_type,
                    gl_site_destinations[counter].email,
                    gl_site_destinations[counter].is_foundry,
                    gl_site_destinations[counter].is_maskshop,
                    gl_site_destinations[counter].compression,
                    gl_site_destinations[counter].extension,
                    gl_site_destinations[counter].secure_extension);
        }
    }
    DBG("ret=%d gl_site_destinations_counter=%d", ret, gl_site_destinations_counter);
    return ret;
}


/*!
  * host_mapping_lookup() returns the host and port where a keyword was placed
  *
  * \return SUCCESS when found or FAILURE if not
  */
int host_mapping_lookup(char *file_keyword,      /* IN */
                        char *destination_site,  /* OUT */
                        char *destination_host,  /* OUT */
                        char *destination_addr,  /* OUT */
                        char *destination_port)  /* OUT */
{
    char query[1024];
    int ret = FAILURE;

    // repositories differ from common companies such as TSMC because they have 'extension' and 'secure_extension'
    //--
    sprintf(query, "SELECT  dt_destinations.name,"
                   "        dt_destinations.machine,"
                   "        dt_destinations.address,"
                   "        dt_destinations.socket"
                   "   FROM data_transfer.dt_transfertimes, data_transfer.dt_destinations"
                   "   WHERE file_keyword = '%s' "
                   "   AND   dt_destinations.active "
                   "   AND   dt_destinations.extension is not NULL "
                   "   AND   dt_destinations.secure_extension is not NULL "
                   "   AND   transfertype in ('Serial put','Parallel put','Separate block put') "
                   "   AND   dt_destinations.machine = dt_transfertimes.destination_ip "
                   "   ORDER BY trans_index asc limit 1"
                  ,file_keyword);

    /* will SUCCESS or FAILURE */
    ret = sql4column_rtv(query, destination_site, destination_host, destination_addr, destination_port);
    DBG("ret=%d", ret);
    return ret;
}


void convert_destinations_into_csv_values_in_memory()
{
    extern RTN rtn;
    int counter = 0;
    rtn.len = 0;
    for (; counter < gl_site_destinations_counter; ++counter)
    {
        // format the line to make life easy for clients
        if (gl_site_destinations[counter].email[0] == '\0')
        {
            gl_site_destinations[counter].email[0] = '0';
            gl_site_destinations[counter].email[1] = 0;
        }
        rtn.len += sprintf(&rtn.msg[rtn.len] , "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
                gl_site_destinations[counter].name,
                gl_site_destinations[counter].abbreviation,
                gl_site_destinations[counter].machine,
                gl_site_destinations[counter].address,
                gl_site_destinations[counter].socket,
                gl_site_destinations[counter].server_type,
                gl_site_destinations[counter].email,
                gl_site_destinations[counter].is_foundry,
                gl_site_destinations[counter].is_maskshop,
                gl_site_destinations[counter].compression,
                gl_site_destinations[counter].extension,
                gl_site_destinations[counter].secure_extension);
    }
    DBG("rtn.len=%d msg=%s",  rtn.len, rtn.msg);
}
