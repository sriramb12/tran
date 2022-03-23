
#ifdef NXP
/****************************************************************************
 *
 * Functions:	dlt_old_sites, dlt_x500, site_array_build
 *
 * Description:	Delete registration entries with no
 *		valid X.500 user ID.
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "intercom_server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_macros.h"
#include "trans_lib_prototypes.h"
#include "trans_server_paths.h"
#include "global_debug.h"
#include "ldapfsl.h"

void dlt_old_sites(char *site_array[])
{
    char fnd;
    int site_cnt;
    int num = 0;
    int len;
    char line[260];
    char site[64];
     char site_file[32];
    char tmp_site_file[32];
    char *site_ptr;
    char *tmp_site_ptr;
    FILE *fi;
    FILE *fo;
    FILE *fp;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG();

    if ((dp = opendir("registry")) == NULL)
    close_down2d(open_dir_err, errno, "registry", OPEN_DIR_ERR);
    /* Initialize first part of file paths */
    site_ptr = strcpy1(site_file, "registry/") +site_file;
    tmp_site_ptr = strcpy1(tmp_site_file, "tmp_registry/") +tmp_site_file;
    /* Open the file to hold old entries */
    if ((fp = fopen("tmp_registry/old_entries", "w")) == NULL)
    close_down1f(wr_open_err, "tmp_registry/old_entries", WR_OPEN_ERR);
    while ((dir = readdir(dp)) != NULL)
    {
	/* All site files are 11 characters in length */
	/* Ignore all other files */
	if (strlen(dir->d_name) != 11) continue;
	/* If file prefix is not 'site_reg', ignore it */
	if (strncmp(dir->d_name, "site_reg.", 9) != 0) continue;
	/* Build the site file name */
	strcpy(site_ptr, dir->d_name);
	strcpy(tmp_site_ptr, dir->d_name);
	if ((fi = fopen(site_file, "r")) == NULL)
	close_down1f(rd_open_err, site_file, RD_OPEN_ERR);
	if ((fo = fopen(tmp_site_file, "w")) == NULL)
	close_down1f(wr_open_err, tmp_site_file, WR_OPEN_ERR);
	/* Read in data from the file */
	while (line_get(fi, line) > 0)
	{
	    /* Scan in email address */
	    if (sscanf(line, "%s", site) != 1)
	    {
		fprintf(fp, "%s\n", line);
		++num;
		continue;
	    }
	    len = strlen(site);
	    if (len < 8)
	    {
		fprintf(fp, "%s\n", line);
		++num;
		continue;
	    }
	    /* Look for user in defined sites */
	    fnd = DBNO;
    for (site_cnt = 0; site_array[site_cnt] != NULL; ++site_cnt)
	    {
		if (strcmp(site_array[site_cnt], site) == 0)
		{
		    fnd = DBYES;
		    break;
		}
	    }
	    if (fnd != DBYES)
	    {
		fprintf(fp, "%s\n", line);
		++num;
		continue;
	    }
	    else fprintf(fo, "%s\n", line);
	} /* End while for getting lines */
	fclose(fi);
	fclose(fo);
    } /* end while */
    closedir(dp);
    fclose(fp);
    if (num > 0)
    sprintf(rtn.msg, "Found %d registration entries to delete.\nRevised data is in directory 'tmp_registry'.\nDeleted entries are in file old_entries.", num);
    else strcpy(rtn.msg, "No registration entries found to delete.");
    put_short(SUCCESS);
    put_success_msg(NONE);
} /* End dlt_old_sites */

#ifdef TRANSCEND
/* Delete entries with no valid X.500 user ID */
void dlt_x500()
{
    char *ptr;
    int num = 0;
    int len;
    char line[260];
    char email_addr[256];
     char site_file[32];
    char tmp_site_file[32];
    char *site_ptr;
    char *tmp_site_ptr;
    FILE *fi;
    FILE *fo;
    FILE *fp;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG();

    if ((dp = opendir("registry")) == NULL)
    close_down2d(open_dir_err, errno, "registry", OPEN_DIR_ERR);
    /* Initialize first part of file paths */
    site_ptr = strcpy1(site_file, "registry/") +site_file;
    tmp_site_ptr = strcpy1(tmp_site_file, "tmp_registry/") +tmp_site_file;
    /* Open the file to hold old entries */
    if ((fp = fopen("tmp_registry/old_entries", "w")) == NULL)
    close_down1f(wr_open_err, "tmp_registry/old_entries", WR_OPEN_ERR);
    while ((dir = readdir(dp)) != NULL)
    {
	/* All site files are 11 characters in length */
	/* Ignore all other files */
	if (strlen(dir->d_name) != 11) continue;
	/* If file prefix is not 'site_reg', ignore it */
	if (strncmp(dir->d_name, "site_reg.", 9) != 0) continue;
	/* Build the site file name */
	strcpy(site_ptr, dir->d_name);
	strcpy(tmp_site_ptr, dir->d_name);
	if ((fi = fopen(site_file, "r")) == NULL)
	close_down1f(rd_open_err, site_file, RD_OPEN_ERR);
	if ((fo = fopen(tmp_site_file, "w")) == NULL)
	close_down1f(wr_open_err, tmp_site_file, WR_OPEN_ERR);
	/* Read in data from the file */
	while (line_get(fi, line) > 0)
	{
	    /* Scan in email address */
	    if (sscanf(line, "%*s %*d %s", email_addr) != 1)
	    {
		fprintf(fp, "%s\n", line);
		++num;
		continue;
	    }
	    len = strlen(email_addr);
	    if (len < 6)
	    {
		fprintf(fp, "%s\n", line);
		++num;
		continue;
	    }
	    if ((ptr = index(email_addr, '@')) == NULL)
	    {
		fprintf(fp, "%s\n", line);
		++num;
		continue;
	    }
	   ptr[0] = '\0';
	    /* convert address to lower case */
/*
	    str_lower(email_addr);
*/

       if (ldapfsl_exists_account_by_coreid(email_addr) != DBYES &&
           ldapfsl_exists_account_by_email(email_addr) != DBYES)
	    {
           fprintf(fp, "%s\n", line);
           ++num;
           continue;
	    }
	    else fprintf(fo, "%s\n", line);
	} /* End while for getting lines */
	fclose(fi);
	fclose(fo);
    } /* end while */
    closedir(dp);
    fclose(fp);
    if (num > 0)
    sprintf(rtn.msg, "Found %d entries to delete.\nRevised data is in directory 'tmp_registry'.", num);
    else strcpy(rtn.msg, "No registration entries found to delete.");
    put_short(SUCCESS);
    put_success_msg(NONE);
} /* End dlt_x500 */
#endif

/* Builds array with site entries from specified year */
void site_array_build(char *site_array[], char *year)
{
    int array_cnt;
    char line[260];
    char site[64];
     char log_file[32];
    char *log_ptr;
    FILE *fi;
    FILE *fo;
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG();

    if ((dp = opendir("log")) == NULL)
    close_down2d(open_dir_err, errno, "log", OPEN_DIR_ERR);
    /* Open the temporary site file */
    if ((fo = fopen("log/out", "w")) == NULL)
    close_down1f(wr_open_err, "log/out", WR_OPEN_ERR);
    /* Initialize first part of file paths */
    log_ptr = strcpy1(log_file, "log/") +log_file;
    while ((dir = readdir(dp)) != NULL)
    {
	/* See if this file is for the right year */
	if (strcmp(dir->d_name+strlen(dir->d_name)-2, year) != 0) continue;
	/* Build the log file name */
	strcpy(log_ptr, dir->d_name);
	if ((fi = fopen(log_file, "r")) == NULL)
	close_down1f(rd_open_err, log_file, RD_OPEN_ERR);
	/* Read in data from the file */
	while (line_get(fi, line) > 0)
	{
	    /* See if this is a site line */
	    if (strncmp(line, "Site", 4) == 0)
	    {
		if (sscanf(line, "%*s %s", site) == 1)
		fprintf(fo, "%s\n", site);
	    } /* End if */
	} /* End while for getting lines */
	fclose(fi);
    } /* end while */
    closedir(dp);
    /* Close the temporary file */
    fclose(fo);
    /* Sort the file and remove duplicates */
    system("sort <log/out >log/outt");
    rmv_dups("log/outt", "log/out");
    /* Sorted output in file out */
    /* Open this file and read in entries */
    if ((fi = fopen("log/out", "r")) == NULL)
    close_down1f(rd_open_err, "log/out", RD_OPEN_ERR);
    for (array_cnt = 0; fscanf(fi, "%s", line) > 0; ++array_cnt)
    {
	if (array_cnt >= 99999)
	shut_down("not enough memory", -1);
	site_array[array_cnt] = malloc(strlen(line) +1);
	get_site(line, site_array[array_cnt]);
    } /* End for */
    /* Close the file */
    fclose(fi);
    site_array[array_cnt] = NULL;
} /* End site_array_build */

void reg_restore()
{
     char command[260];
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG();

    if ((dp = opendir("registry")) == NULL)
    close_down2d(open_dir_err, errno, "registry", OPEN_DIR_ERR);
    while ((dir = readdir(dp)) != NULL)
    {
	/* All site files are 11 characters in length */
	/* Ignore all other files */
	if (strlen(dir->d_name) != 11) continue;
	/* If file prefix is not 'site_reg', ignore it */
	if (strncmp(dir->d_name, "site_reg.", 9) != 0) continue;
	/* Build the sort command */
	strcpy6(command, "sort -u registry/", dir->d_name, " registry/.delta/", dir->d_name, " -o registry/", dir->d_name);
	system(command);
    } /* End while */
    /* close the directory */
    closedir(dp);
} /* End reg_restore */

/* Verify registry has not been corrupted */
void reg_check()
{
    struct stat buf;
    struct stat delta_buf;
    char site_file[64];
    char delta_file[64];
     char *site_ptr;
     char *delta_ptr;
     char command[260];
    DIR *dp;
    DIR *opendir();
    struct dirent *dir, *readdir();

    DBG();

    if ((dp = opendir("registry")) == NULL)
    close_down2d(open_dir_err, errno, "registry", OPEN_DIR_ERR);
     /* Initialize paths needed in following  loop */
     site_ptr = strcpy1(site_file, "registry/") +site_file;
     delta_ptr = strcpy1(delta_file, "registry/.delta/") +delta_file;
    while ((dir = readdir(dp)) != NULL)
    {
	/* All site files are 11 characters in length */
	/* Ignore all other files */
	if (strlen(dir->d_name) != 11) continue;
	/* If file prefix is not 'site_reg', ignore it */
	if (strncmp(dir->d_name, "site_reg.", 9) != 0) continue;
	/* Build site file name */
	strcpy(site_ptr, dir->d_name);
	/* do a stat on the file */
	if (stat(site_file, &buf) != 0) buf.st_size = 0;
	/* Build delta file name */
	strcpy(delta_ptr, dir->d_name);
	/* Do a stat on this file */
	if (stat(delta_file, &delta_buf) != 0) delta_buf.st_size = 0;
	/* Compare sizes */
	/* If site file has a greater size, copy the site file */
	if (buf.st_size > delta_buf.st_size)
	{
	    strcpy4(command, "cp registry/", dir->d_name, " registry/.delta/", dir->d_name);
	    system(command);
	} /* End if */
	/* If site file smaller, merge two files */
	else if (buf.st_size < delta_buf.st_size)
	{
	    /* Build the sort command */
	    strcpy6(command, "sort -u registry/", dir->d_name, " registry/.delta/", dir->d_name, " -o registry/", dir->d_name);
	    system(command);
	    /* Send email */
	    sprintf(rtn.msg+rtn.len, reg_problem_err, dir->d_name);
	    email_send("", admin.err_email, "Registration file error", rtn.msg+rtn.len);
	    rtn.len += strlen(rtn.msg+rtn.len);
	} /* End else */
    } /* End while */
    /* close the directory */
    closedir(dp);
} /* End reg_check */
#endif
/*es.files.fileuse*/
/*is.files.fileuse*/
