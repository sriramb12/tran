#include <mysql.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "trans_server_arrays.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "ldapfsl.h"
#include "global_debug.h"

void insert_ldap_entry(char *site, char *core_id, char *friendly_email)
{
    DBG("site='%s', core_id='%s', friendly_email='%s'", site, core_id, friendly_email);

    char query[1024];
    char db[16] = "data_transfer";

    sprintf(query, "insert into data_transfer.dt_not_in_ldap(site, core_id, friendly_email, serversite, transaction_cnt) values('%s', '%s', '%s', '%s', 1)",
    site, core_id, friendly_email, server.site);
    data_log("query", query);
    insert_data(DTMS_DB, db, query, DBNO);
} /* End insert_ldap */

void update_ldap_entry(char *site)
{
    DBG("site='%s'", site);

    char db[16] = "data_transfer";
    char query[1024];

    sprintf(query, "update data_transfer.dt_not_in_ldap set transaction_cnt = transaction_cnt + 1 where site = '%s'",
    site);
    data_log("query", query);
    insert_data(DTMS_DB, db, query, DBNO);
} /* end update_ldap */

int ldap_entry_present(char *site)
{
    DBG("site='%s'", site);

    MYSQL_RES *result;
    int num_rows;
    char query[1024];

    sprintf(query, "select site from data_transfer.dt_not_in_ldap where site = '%s'", site);
    data_log("query", query);
    result = result_get("data_transfer", query, NULL);
    /* If no result, indicate user not found */
    if (result == NULL) 
    return(DBNO);
    num_rows = mysql_num_rows(result);
    if(num_rows >= 1) return(DBYES);
    else return(DBNO);
} /* end ldap_entry_present */

/* Add entry to Transcend table */
void sql_trans_insert (char *keyword, char *file_name, char *status, char *rtn_msg)
{
    DBG("keyword='%s', file_name='%s', status='%s', rtn_msg='%s'", keyword, file_name, status, rtn_msg);

    extern char user_sector[];
    long current_time = time(0);
    char query[2048];
    char db[16];
    char starttime_string[48];
    char endtime_string[48];

    strcpy(db, "data_transfer");
    /* Convert start and end times to SQL format */
    sql_time_convert(starttime_string, tr.upd_time, NULL);
    sql_time_convert(endtime_string, current_time, NULL);
    strexc(rtn_msg, '"', '\'');
    sprintf(query, " insert into data_transfer.transweb \
(keyword,core_id,name, business_group,request_type,file_name, \
Transcend_version,from_company,to_company,platform,ip_node_address, \
mask_id,original_size,compressed_size,starttime,endtime, \
transfertime,transferspeed, mask_status,cwd, serversite, tool, icap_classification, transaction_status, return_message) values(\
    '%s', '%s',  '%s',  '%s',  '%s',  '%s', \
    '%s',  '%s',  '%s',  '%s',  '%s', \
    '%s',  %lld,  %lld,  '%s',  '%s', \
     %ld, %d,   '%s', '%s', '%s',  '%s',  '%s', '%s', \"%s\")",
    keyword, user.email, user.name, user_sector,  req_array[gu.req_type],  file_name, 
     user.client_version, gu.from_company, 	gu.to_company,     gu.machine_type, user.addr,
    NONE, fs.original_size,     fs.compressed_size, starttime_string, endtime_string, 
#ifdef TRANSCEND
    current_time-tr.upd_time, 0, status, user.cwd, server.site, "Transcend", gu.icap_class, status, rtn_msg);
#else
    current_time-tr.upd_time, 0, status, user.cwd, server.site, "TransWeb", gu.icap_class, status, rtn_msg);
#endif
    /* Run command to put record in the SQL database */
    data_log("query", query);
#ifndef EXTERNAL_TRANSWEB
    insert_data(DTMS_DB, db, query, DBNO);
#endif
} /* End sql_trans_insert */

/* Update disk usage table */
void sql_disk_usage_insert(int percent)
{
    DBG("percent='%d'", percent);

    extern char disk_partition[3][64];
    int percent1;
    int percent2;
    int percent3;
    char query[256];
    char db[32] = "data_transfer";
    char starttime_string[48];

    /* convert starttime to SQL format */
    sql_time_convert(starttime_string, tr.upd_time, NULL);
    percent1 = disk_partition_usage_calc(disk_partition[0]);
    if (disk_partition[1][0] != '\0')
    percent2 = disk_partition_usage_calc(disk_partition[1]);
    else percent2 = 0;
    if (disk_partition[2][0] != '\0')
    percent3 = disk_partition_usage_calc(disk_partition[2]);
    else percent3 = 0;
    sprintf(query, " insert into dt_disk_usage \
 (percent, percent2, percent3, serversite, starttime, startdate) values (\
    %d, %d, %d, '%s',  '%s',  '%s')",
    percent1, percent2, percent3, server.site, starttime_string, starttime_string);
    /* Run command to put record in the SQL database */
    data_log("query", query);
    insert_data(DTMS_DB, db, query, DBNO);
} /* End sql_disk_usage_insert */

int sql_is_dtms_tester(char *core_id)
{
    DBG("core_id='%s'", core_id);

    int num_rows;
    char query[256];
    MYSQL_RES *result;

strcpy3(query,
    "select core_id from dtms_testers where core_id = '",
    core_id, "'");
    result = result_get("data_transfer", query, NULL);
    /* If no result, indicate user not found */
    if (result == NULL) 
    return(DBNO);
    num_rows = mysql_num_rows(result);
    if(num_rows >= 1) return(DBYES);
    else return(DBNO);
} /* end sql_is_dtms_tester */

/* Update table if user not present in LDAP */
void sql_update_not_in_ldap(char *site, char *core_id, char *friendly_email)
{
    DBG("site='%s', core_id='%s', friendly_email='%s'", site, core_id, friendly_email);

    /* First see if entry is already present */
    if (ldap_entry_present(site) == DBYES)
        update_ldap_entry(site);
    else 
        insert_ldap_entry(site, core_id, friendly_email);
}

/* Add entry to trans_history table */
void sql_trans_history_insert( char *keyword, char *file_name, char *core_id, unsigned long long original_size, unsigned long long compressed_size)
{
    DBG("keyword='%s', file_name='%s', core_id='%s', original_size='%llu', compressed_size='%llu'", keyword, file_name, core_id, original_size, compressed_size);

    char *ptr;
    char query[8000];
    FILE *fp;
    char history_file[32];

   /* Transactions logged only for the DTMS site */
    if (strcmp(server.site, DTMS) != 0)
    return;
    strcpy3(history_file, "history_data/", keyword, ".history");
    fp = fopen(history_file, "r");
    line_get(fp, rtn.msg);
    history_info_get(fp);

    /* Strip off trailing e-mail address if present */
    if ((ptr = strchr(core_id, '@')) != NULL)
    {
        LDAP_RETURN_DATA *ret = ldapfsl_lookupByMail(core_id);
        if (BAD_LDAP_RETURN_DATA(ret))
        {
            ldapfsl_free_return_data(ret);
            ret = ldapfsl_lookupByCoreId(core_id);
        }
        if (GOOD_LDAP_RETURN_DATA(ret))
        {
            core_id = ret->info[0]->coreId;
        }
        ldapfsl_free_return_data(ret);
    }

    sprintf(query, "insert into data_transfer.trans_history\
(keyword, core_id, file_name, original_size, compressed_size, history)\
 values('%s', '%s', '%s', %llu, %llu, '%s')",
keyword, core_id, file_name, original_size, compressed_size, rtn.msg);
    data_log("query", query);
    insert_data(DTMS_DB, "data_transfer", query, DBNO);
} /* End sql_trans_history_update */

/* Update existing entry in trans_history table */
void sql_trans_history_update(char *keyword)
{
    DBG("keyword='%s'", keyword);

    char query[8000];
    FILE *fp;
    char history_file[32];

   /* Transactions logged only for the DTMS site */
    if (strcmp(server.site, DTMS) != 0)
    return;
    keyword[9] = '\0';
    strcpy3(history_file, "history_data/", keyword, ".history");
    if ((fp = fopen(history_file, "r")) == NULL) return;
    line_get(fp, rtn.msg);
    history_info_get(fp);
    /* Build query to execute */
    strcpy5(query, "update data_transfer.trans_history set history = '",
    rtn.msg, "' where keyword = '", keyword, "'");
    data_log("query", query);
    insert_data(DTMS_DB, "data_transfer", query, DBNO);
} /* End sql_trans_history_update */

/* Calculate disk space for specified partition */
disk_partition_usage_calc(char *disk_partition)
{
    DBG("disk_partition='%s'", disk_partition);

    FILE *po;
    int percent;
    char command[80];

    /* See how much space is available on specified disk partition */
    /* Build the command to execute */
#ifdef SOLARIS
    strcpy2(command, "df -k ", disk_partition);
#endif
/* This used when compiling on Linux */
#ifdef LINUX
    strcpy2(command, "df ", disk_partition);
#endif
    if ((po = popen (command, "r")) == NULL)
    return(0);
    if (fscanf(po, "%*s %*s %*s %*s %*s %*s %*s %*s %*d %*d %*llu %d",
    &percent)!=1)
    return(0);
    pclose(po);
    return(percent);
}

