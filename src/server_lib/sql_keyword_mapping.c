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
#include "global_debug.h"

/* Look up the file keyword in the keyword mapping table */
int keyword_mapping_lookup(char *secure_key, char *file_key, char *reg_email)
{
    char query[1024];

    DBG();

    sprintf(query, "select file_keyword, to_email from data_transfer.dt_keyword_mapping WHERE secure_keyword = '%s'  order by trans_index desc limit 1",
                   secure_key);

    return(sql2column_rtv(query, file_key, reg_email));
} /* end keyword_mapping_lookup */



/* Add entry to keyword mapping table */
int sql_keyword_mapping_insert(char *file_keyword, char *secure_keyword,
char *from_email, char *to_email, char *description, long upload_time)
{
    char err_msg[BUFFER_SIZE];
    char query[FILE_SIZE];
    char starttime_string[48];
    int ret = FAILURE;

    /* Convert the start time to SQL format */
    sql_time_convert(starttime_string, tr.upd_time, NULL);
    /* Take quotes out of description */
    strexc(description, '"', ' ');
    sprintf(query, "insert into data_transfer.dt_keyword_mapping \
    (secure_keyword, file_keyword, description, from_email, to_email,  \
upload_date, download_date) \
values(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\")",
    secure_keyword, file_keyword, description, from_email, to_email,
    starttime_string, "");
    if (sql_insert_data(query, err_msg) != SUCCESS)
    {
        data_log("dt_keyword_mapping", query);
    }
    else
    {
        ret = SUCCESS;
    }
    DBG("ret=%d", ret);
    return ret;
} /* end sql_keyword_mapping_insert */




/* Update entry to keyword mapping table */
int sql_keyword_mapping_update(void)
{
    char err_msg[BUFFER_SIZE];
    extern char global_secure_keyword[];
    char query[FILE_SIZE];
    char starttime_string[48];

    DBG();

    /* Convert the start time to SQL format */
    sql_time_convert(starttime_string, tr.upd_time, NULL);
    sprintf(query, "update data_transfer.dt_keyword_mapping \
 set download_date = '%s' \
 where secure_keyword = '%s'",
    starttime_string, global_secure_keyword);
    if (sql_insert_data(query, err_msg) != SUCCESS)
    {
        data_log("mapping", err_msg);
    data_log("mapping", query);
    }
    return(SUCCESS);
} /* end sql_keyword_mapping_update */
