/****************************************************************************
 *
 * Function:	read_global_params
 *
 * Description:	Reads global parameters and global driver files.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <strings.h>
#include <string.h>
#include "global_defines.h"
#include "close_down.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "server_params.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "m2server_paths.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include "strcpy.h"
#include "mti.h"

/* Holds Email address and login of database administrator */
extern DBADMIN_INFO admin;
void read_global_params(void)
{
    extern char service_password[];
    extern char ret_password[];
    extern char mso_password[];
    extern char bin_dir[];
    extern char global_email[];
    extern char admin_email[];
    extern char group_email[];
    extern char cert7db[];
     int cnt = 0;
    FILE *fp;
    char file_name[FILE_SIZE];

    if ((fp = fopen(GLOBAL_PARAMS, "r")) == NULL)
    {
    strcpy(admin.err_email, "mdsEvents@nxp.com");
    close_down1m(rd_open_err, GLOBAL_PARAMS, RD_OPEN_ERR);
    }
    fscanf(fp, "%s %s %s %s %s %s %s %s %s %d %d %s %s  %s %s %d",
     ret_password, mso_password, admin.from_email, admin.reg_email, admin.err_email, admin.login1, admin.login2,
    ip.master_site, ip.master_addr, &global_file_num, &ip.dsgn_file_num,
    /* Information about archive server */
    bin_dir, cert7db, service_password, server.machine, server.address, &server.socket);
    /* Get the carriage return */
    getc(fp);
line_get(fp, global_email);
    fclose(fp);
    strcpy(admin.name1, "MDS");
    strcpy(admin.name2, "Team");
    strcpy(admin.phone, "");
    strcpy(admin.program, "MDS Database");
    /* Read in the audit mail list */
    if ((fp = fopen("db_admin/audit_mail", "r")) == NULL)
    {
    /* Assign default values */
    strcpy(admin_email, "mdsEvents@nxp.com");
    strcpy(group_email, "mdsEvents@nxp.com ");
    } /* End if for file not found */
    else
    {
    line_get(fp, admin_email);
    line_get(fp, group_email);
    fclose(fp);
    }
    /* Read in the .db filelist files */
    if ((fp = fopen(DB_FILELIST, "r")) == NULL)
    close_down1m(rd_open_err, DB_FILELIST, RD_OPEN_ERR);
    /* Read in .db file names */
     while(fscanf(fp, "%d %s", &db_program[cnt], file_name) == 2)
    {
    /* Allocate memory */
    db_filelist[cnt] = malloc(strlen(file_name) +1);
    strcpy(db_filelist[cnt++],  file_name);
    } /* end while */
     db_filelist[cnt] = NULL;
    fclose(fp);
    /* Read in top level file names */
    if ((fp = fopen(FILELIST, "r")) == NULL)
    close_down1m(rd_open_err, FILELIST, RD_OPEN_ERR);
    /* Set cnt back to 0 for next loop */
    cnt = 0;
     while(fscanf(fp, "%d %s", &m2program[cnt], file_name) == 2)
    {
    /* Allocate memory */
    m2filelist[cnt] = malloc(strlen(file_name) +1);
    strcpy(m2filelist[cnt++],  file_name);
    } /* end while */
     m2filelist[cnt] = NULL;
    fclose(fp);
    /* Read in names for global fab files */
    /* These files are copied from global_files to fab_files f*/
    /* when a new fab is created */
    if ((fp = fopen(GLOBAL_FAB_FILE, "r")) == NULL)
    {
    global_fab_files[0] = "machine";
    global_fab_files[1] = "timing";
    global_fab_files[2] = "exp";
    global_fab_files[3] = "pelicle";
        global_fab_files[4] = NULL;
    }
    else
    {
        char table[32];
    int cnt = 0;
    while (fscanf(fp, "%s", table) == 1)
    {
        global_fab_files[cnt] = malloc(strlen(table) +1);
    strcpy(global_fab_files[cnt++], table);
    } /* End while */
    /* Point last pointer at null */
    global_fab_files[cnt] = NULL;
    fclose(fp);
    } /* End else */
    help_msg_print(help_msg);
} /* End read_global_params */

/* Read in the file list */
void m2filelist_read()
{
    FILE *fp;
    int cnt = 0;
    char file_name[32];

    /* Build name for filelist file */
    strcpy2(file_name, mti.delta_dir, "/m2filelist");
    /* Open the file list file */
    if ((fp = fopen(file_name, "r")) == NULL)
    close_down1m(rd_open_err, file_name, RD_OPEN_ERR);
    /* Scan in names */
    while (fscanf(fp, "%d %s", &m2program[cnt], file_name) == 2)
    {
    m2filelist[cnt] = malloc(strlen(file_name) +1);
    strcpy(m2filelist[cnt++], file_name);
    } /* End while */
    m2filelist[cnt] = NULL;
    fclose(fp);
} /* End m2filelist_read */
