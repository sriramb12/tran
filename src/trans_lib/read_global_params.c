/****************************************************************************
 *
 * Function:	read_global_params
 *
 * Description:	Read in the remote server machine, address and socket.
 *		Also read in list of registered users.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rmt_server.h"
#include "trans_defines.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_server_prototypes.h"
#include "global_debug.h" // Mazieri's debug facilities

/* Holds number of disks for Transcend files */
#ifdef TRANSCEND
extern char disk_partition[3][64];
#endif
extern int disk_cnt;
extern int deletion_days;
extern int pending_timeout;
extern char edc_database[];
extern char dt_database[];
int cnt;

/* Holds login and Email for database administrator */
DBADMIN_INFO admin;
extern char time_zone[];
extern char dtms_db[];
extern char keyword_link_extension[];
extern char keyword_extension[];
extern char secure_keyword_extension[];
extern TRANS_DISK disk;
extern int timeout_extension;

void read_global_params(char *file_name)
{
    char line[260];
    char keyword[260];
    char value[260];
    FILE *fp;

    DBG("file_name='%s'", file_name);

    /* Assign default values */
    default_values_assign();

    /* Open the global file */
    if ((fp = fopen(file_name, "r")) == NULL) return;

    /* Scan in lines looking for values to assign */
    while (line_get(fp, line) != EOF)
    {
	    /* If this is a comment line, skip it */
	    if (line[0] == '#') continue;
	
	    if (line_split(line, keyword, value) == FAILURE) continue;
	
	    DBG("keyword='%s', value='%s'", keyword, value);

	    value_assign(keyword, value);
    } /* End while */

	fclose(fp);
} /* End read_global_params */


int line_split(char *line, char *keyword, char *value)
{
    DBG();
    char *ptr;
    if ((ptr = index(line, ':')) == NULL) return(FAILURE);
    /* Increment pointer and put null at end of keyword string */
    ++ptr;
    ptr[0] = '\0';
    strcpy(keyword, line);
    /* Look for first non white space */
    ++ ptr;
    while ((ptr[0] == ' ') || (ptr[0] == '\t')) ++ptr;
    strcpy(value, ptr);
    return(SUCCESS);
} /* End line_split */

/* Assign value based on the keyword */
void value_assign(char *keyword, char *value)
{
    DBG();
    extern unsigned long long large_parallel_transfer_size;
    extern char expire_date[];
    extern char expire_version[10][8];
    extern char unsupported_version[10][8];
     extern char key_file[]; 
    extern char ca_list[];
    extern unsigned long long parallel_configured_size;
    extern unsigned long long separate_configured_size;
    extern char cert7db[];
    extern char external_cert7db[];
    extern char service_password[];
    extern int global_disk_usage;
    extern int core_id_timeout;
    extern char global_default_site;
    extern char dtms_mail_list[];
extern char root_dir[];
    extern char dtms_db[];
    extern char parallel_version[];
#ifdef TRANSWEB
    extern char fss_admin[];

    if (strcmp(keyword, "FSS admin:") == 0) 
    {
	sscanf(value, "%s", fss_admin);
    }
#endif /* for TransWeb */


    if (strcmp(keyword, "Lock count:") == 0) sscanf(value, "%d", &global_lock_cnt);
    else if (strcmp(keyword, "File timeout:") == 0) sscanf(value, "%d", &deletion_days);
    else if (strcmp(keyword, "Pending timeout:") == 0) 
    sscanf(value, "%d", &pending_timeout);
    else if (strcmp(keyword, "Extension timeout:") == 0) sscanf(value, "%d", &timeout_extension);
    else if (strcmp(keyword, "Admin name:") == 0) sscanf(value, "%s %s", admin.name1, admin.name2);
    else if (strcmp(keyword, "Admin email:") == 0) sscanf(value, "%s", admin.from_email);
    else if (strcmp(keyword, "Admin phone:") == 0) sscanf(value, "%s", admin.phone);
    else if (strcmp(keyword, "Error log email:") == 0) sscanf(value, "%s", admin.err_email);
    else if (strcmp(keyword, "Authorized register site1:") == 0) { sscanf(value, "%s", admin.login1); }
    else if (strcmp(keyword, "Authorized register site2:") == 0) sscanf(value, "%s", admin.login2);
    else if (strcmp(keyword, "Site name:") == 0) sscanf(value, "%s", server.site);
    else if (strcmp(keyword, "Server address:") == 0) sscanf(value, "%s", server.address);
    /* this value is used when logging transfer times in dt_transfertimes */
    else if (strcmp(keyword, "Server site:") == 0) sscanf(value, "%s", server.site_name);
    else if (strcmp(keyword, "Keyword extension:") == 0) sscanf(value, "%s", keyword_link_extension);
    else if (strcmp(keyword, "DTMS DB:") == 0) sscanf(value, "%s", dtms_db);
    else if (strcmp(keyword, "Root directory:") == 0) sscanf(value, "%s", root_dir);
    else if (strcmp(keyword, "Site extension:") == 0) sscanf(value, "%s", keyword_extension);
    else if (strcmp(keyword, "Secure site extension:") == 0) sscanf(value, "%s", secure_keyword_extension);
    else if (strcmp(keyword, "Time zone:") == 0) sscanf(value, "%s", time_zone);
    else if (strcmp(keyword, "From company:") == 0) sscanf(value, "%s", gu.from_company);
    else if (strcmp(keyword, "Cert7db:") == 0) 
	sscanf(value, "%s", cert7db);
    else if (strcmp(keyword, "External cert7db:") == 0) 
	sscanf(value, "%s", external_cert7db);
    else if (strcmp(keyword, "DTMS mail list:") == 0)
	{
	    char *ptr= value;
	    while ((ptr[0] == ' ') || (ptr[0] == '\t')) ++ptr;
	strcpy(dtms_mail_list, ptr);
	}
    else if (strcmp(keyword, "Disk usage:") == 0) sscanf(value, "%d", &global_disk_usage);
#ifdef TRANSCEND
    else if (strcmp(keyword, "Disk partition1:") == 0) sscanf(value, "%s", disk_partition[0]);
    else if (strcmp(keyword, "Disk partition2:") == 0) sscanf(value, "%s", disk_partition[1]);
    else if (strcmp(keyword, "Disk partition3:") == 0) sscanf(value, "%s", disk_partition[2]);
#endif
    else if (strcmp(keyword, "Core ID timeout:") == 0) sscanf(value, "%d", &core_id_timeout);
    else if (strcmp(keyword, "Large transfer size:") == 0) sscanf(value, "%llu", &large_parallel_transfer_size);
    else if (strcmp(keyword, "Default site:") == 0) sscanf(value, "%c", &global_default_site);
    else if (strcmp(keyword, "partition0:") == 0) sscanf(value, "%s %ld %ld", disk.path[0], &disk.lower_size[0], &disk.upper_size[0]);
    else if (strcmp(keyword, "partition1:") == 0) sscanf(value, "%s %ld %ld", disk.path[1], &disk.lower_size[1], &disk.upper_size[1]);
    else if (strcmp(keyword, "partition2:") == 0) sscanf(value, "%s %ld %ld", disk.path[0], &disk.lower_size[2], &disk.upper_size[2]);
    else if (strcmp(keyword, "partition3:") == 0) sscanf(value, "%s %ld %ld", disk.path[3], &disk.lower_size[3], &disk.upper_size[3]);
    else if (strcmp(keyword, "partition4:") == 0) sscanf(value, "%s %ld %ld", disk.path[4], &disk.lower_size[4], &disk.upper_size[4]);
    else if (strcmp(keyword, "partition5:") == 0) sscanf(value, "%s %ld %ld", disk.path[5], &disk.lower_size[5], &disk.upper_size[5]);
    else if (strcmp(keyword, "partition6:") == 0) sscanf(value, "%s %ld %ld", disk.path[6], &disk.lower_size[6], &disk.upper_size[6]);
    else if (strcmp(keyword, "partition7:") == 0) sscanf(value, "%s %ld %ld", disk.path[7], &disk.lower_size[7], &disk.upper_size[7]);
    else if (strcmp(keyword, "partition8:") == 0) sscanf(value, "%s %ld %ld", disk.path[8], &disk.lower_size[8], &disk.upper_size[8]);
    else if (strcmp(keyword, "partition9:") == 0) sscanf(value, "%s %ld %ld", disk.path[9], &disk.lower_size[9], &disk.upper_size[9]);
    else if (strcmp(keyword, "Encrypted info:") == 0) sscanf(value, "%s", service_password);
    else if (strcmp(keyword, "Parallel version:") == 0) sscanf(value, "%s", parallel_version);
    else if (strcmp(keyword, "Parallel size:") == 0) sscanf(value, "%llu", &parallel_configured_size);
    else if (strcmp(keyword, "Separate size:") == 0) sscanf(value, "%llu", &separate_configured_size);
    else if (strcmp(keyword, "Expire date:") == 0) strcpy(expire_date, value);
    else if (strcmp(keyword, "Expire version:") == 0) 
    {
    int i = sscanf(value, "%s %s %s %s %s %s %s %s %s %s", 
	expire_version[0], expire_version[1], expire_version[2], expire_version[3], expire_version[4],
	expire_version[5], expire_version[6], expire_version[7], expire_version[8], expire_version[9]);
	expire_version[i][0]= '\0';
    } /* end if */
    else if (strcmp(keyword, "Unsupported version:") == 0) 
    {
    int i = sscanf(value, "%s %s %s %s %s %s %s %s %s %s", 
	unsupported_version[0], unsupported_version[1], unsupported_version[2], unsupported_version[3], unsupported_version[4], 
	unsupported_version[5], unsupported_version[6], unsupported_version[7], unsupported_version[8], unsupported_version[9]);
	unsupported_version[i][0] = '\0';
    } /* end if */
    /* currently value in ssl_serer takes presidence over this value */
    else if (strcmp(keyword, "Key file:") == 0) sscanf(value, "%s", key_file);
    else if (strcmp(keyword, "CA list:") == 0) sscanf(value, "%s", ca_list);
#ifdef TRANS_FTP
#endif
    else if (strcmp(keyword, "EDC database:") == 0) sscanf(value, "%s", edc_database);
    else if (strcmp(keyword, "DT database:") == 0) sscanf(value, "%s", dt_database);
} /* End value_assign */
    /* Assign default values */
/* Default values are used if the global_params file is not present */
void default_values_assign(void)
{
    extern unsigned long long large_parallel_transfer_size;
     extern char key_file[]; 
    extern char ca_list[];
    extern unsigned long long parallel_configured_size;
    extern unsigned long long separate_configured_size;
    extern int core_id_timeout;
    extern int global_disk_usage;
    extern char parallel_version[];
    extern char global_default_site;

    DBG();

    deletion_days = 3;
    pending_timeout = 14;
    strcpy(dtms_db, DTMS_DB);
    strcpy(keyword_link_extension, ".cgi");
    strcpy(key_file, "db_admin/crts/combined.pem");
    core_id_timeout = 28800;
    global_disk_usage = 80;
#ifdef TRANSCEND
    disk_partition[0][0] = disk_partition[1][0] = disk_partition[2][0] = '\0';
#endif
    global_lock_cnt = 7;
    server_version = 9.0;
    timeout_extension = 3;
    large_parallel_transfer_size = 100000000;
    strcpy(parallel_version, "10.0");
    strcpy(ca_list, "db_admin/root.pem");
    strcpy(admin.name1, "Transcend");
    strcpy(admin.name2, "Team");
    strcpy(admin.from_email, "transbug@nxp.com");
    strcpy(admin.err_email, "transbug@nxp.com");
    strcpy(admin.login1, "db_admin10.81.199");
    strcpy(admin.login2, "db_admin10.65.24");
    strcpy(server.address, "inv0803.nxdi.us-cdc01.nxp.com");
    strcpy(server.site_name, "Phoenix");
    strcpy(server.site, "Phoenix");
    strcpy(keyword_extension, "p");
    strcpy(secure_keyword_extension, "P");
    strcpy(time_zone, "CDT");
    global_default_site = 'p';
    strcpy(disk.path[0], "data_files0/");
    strcpy(disk.path[1], "data_files1/");
    strcpy(disk.path[2], "data_files2/");
    strcpy(disk.path[3], "data_files3/");
    strcpy(disk.path[4], "data_files4/");
    strcpy(disk.path[5], "data_files5/");
    strcpy(disk.path[6], "data_files6/");
    strcpy(disk.path[7], "data_files7/");
    strcpy(disk.path[8], "data_files8/");
    strcpy(disk.path[9], "data_files9/");
    disk.lower_size[0] = 0;
    disk.lower_size[1] = 5000000;
    disk.lower_size[2] = 15000000;
    disk.lower_size[3] = 25000000;
    disk.lower_size[4] = 35000000;
    disk.lower_size[5] = 45000000;
    disk.lower_size[6] = 55000000;
    disk.lower_size[7] = 999000000;
    disk.upper_size[0] = 5000000;
    disk.upper_size[1] = 15000000;
    disk.upper_size[2] = 25000000;
    disk.upper_size[3] = 35000000;
    disk.upper_size[4] = 45000000;
    disk.upper_size[5] = 55000000;
    disk.upper_size[6] = 999000000;
    disk.upper_size[7] = 2000000000;
    disk_cnt = 10;
    parallel_configured_size = 104857600;
    separate_configured_size = 5242880;
    strcpy(edc_database, "edc");
    strcpy(dt_database, "data_transfer");
} /* End default_values_assign */

/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
