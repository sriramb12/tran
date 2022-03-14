#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "user_params.h"
#include "line.h"
#include "registry_prototypes.h"
#include "dbreg_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "x500_defines.h"

#include "global_debug.h"

#define SITE_REG_PATH "registry/site_reg."


/****************************************************************************
 *
 * Function:	add_site_entry
 *
 * Description:
 *  Adds new entry to the site registration table.
 *	when a new user registers to use the DMI database.
 *
 * Parameter:
 *  char *new_site
 *  char *new_email1
 *  char *new_email2
 *
 ***************************************************************************/
int add_site_entry(char *new_site, char *new_email1, char *new_email2)
{
    int index;
    int fnd = DBYES;
	int len_userid = strlen(new_site) + 1;
    char new_entry[261];
    char entry[261];
    FILE *fi;
    FILE *fo;
    char file_name[24];
    register int done;
    register int ret;

    DBG("new_site='%s', new_email1='%s' and new_email2='%s'", new_site, new_email1, new_email2);

    /* Make sure email addresses are in right format */
    if (strchr(new_email1, '@') == NULL)
        strcat(new_email1, EMAIL_SUFFIX);

    if (strcmp(new_email2, "-") == 0)
        strcpy(new_email2, new_email1);
    else if (strchr(new_email2, '@') == NULL)
        strcat(new_email2, EMAIL_SUFFIX);

    /* Build name of file to add entry to */
    reg_name_build(file_name, new_site);

    /* Build lock file name */
    /* build entry to add */
    strcpy5( new_entry, new_site, " 0 ", new_email1, " ", new_email2);

    /* Put the string in lower case */
    str_lower(new_entry);

    /* Lock the file so no other process can do a simultaneous update */
    index = lock_file_set(file_name);

    /* Open the file entry being added to */
    if ((fi = fopen(file_name, "r")) == NULL)
    {
	    /* Create the file and add the entry */
	    data_log(file_name, new_entry);

	    /* Unlock the file */
	    lock_file_clear(index);
	    return(DBYES);
    }

    /* Construct the temporary file name */
    sprintf(bf.tmp_file, "registry/tmp_files/%s_%s_%ld.reg", user.email, user.mach, tr.upd_time);

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
	    close_down1m(wr_open_err, bf.tmp_file, WR_OPEN_ERR);

    done = DBNO;

    while (done == DBNO)
    {
	    ret = line_get(fi, entry);

	    /* If end of file reached put the new entry in */
	    if (ret <= 0)
	    {
	        fprintf(fo, "%s\n", new_entry);
	        done = DBYES;
	        break;
	    }

	    /* If old entry less than new entry, write out the old entry */
	    if ((ret = strncmp(new_entry, entry, len_userid)) > 0)
	    {
	    	    fprintf(fo, "%s\n", entry);
	    }

	    /* Otherwise write out new entry followed by old entry */
	    else if (ret < 0)
	    {
	        fprintf(fo, "%s\n", new_entry);
	        fprintf(fo, "%s\n", entry);
	        break;
	    } /* end else */
	    /* At this point ret will equal 0 */
	    /* If entry already exists, replace old entry with new entry */
	    else
	    {
	        fprintf(fo, "%s\n", new_entry);
	        break;
	    } /* end else */
    } /* end while */

    /* Write remaining entries out to the temporary file */
    if (done == DBNO)
    {
	    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
	        fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* end if */

    /* Close both files */
    fclose(fi);
    fclose(fo);

    /* Write the temporary file over the old file */
    ret = rename(bf.tmp_file, file_name);

    /* Change mode for external Intercom connections */
    /* Unlock the file by deleting the lock file */
    lock_file_clear(index);

    if (ret == 0) return(fnd);
    else close_down2m(rename_err, bf.tmp_file, file_name, RENAME_ERR);
} /* End add_site_entry */

void reg_name_build(char *file_name, char *user_id)
{
    DBG("file_name='%s' and user_id='%s'");

    int ret;

#ifdef M2
    ret = strcpy2(file_name, "dmi_", SITE_REG_PATH);
#else
    ret = strcpy1(file_name, SITE_REG_PATH);
#endif
    file_name[ret++] = user_id[0];
#ifndef OPCTRANS
#ifndef DFM
#ifdef NXP
    file_name[ret++] = user_id[1];
#endif
#endif
#endif
    file_name[ret] = '\0';
} /* End reg_name_build */


/****************************************************************************
 *
 * Function:	add_server_site_entry
 *
 * Description:
 *  Adds new entry to the site registration table at the server side.
 *	when a new user registers to use the DMI database.
 *
 * Parameter:
 *  char *new_site
 *  char *new_email1
 *  char *new_email2
 *
 ***************************************************************************/
int add_server_site_entry(char *new_site, char *new_email1, char *new_email2)
{
    int index;
     int fnd = DBYES;
	int len_userid = strlen(new_site) + 1;
    char new_entry[261];
    char entry[261];
    FILE *fi;
    FILE *fo;
    char file_name[24];
    register int done;
    register int ret;

    DBG();

    /* Make sure email addresses are in right format */
    if (strchr(new_email1, '@') == NULL)
        strcat(new_email1, EMAIL_SUFFIX);

    if (strcmp(new_email2, "-") == 0)
        strcpy(new_email2, new_email1);
    else if (strchr(new_email2, '@') == NULL)
        strcat(new_email2, EMAIL_SUFFIX);

    /* Build name of file to add entry to */
    strcpy(file_name, "registry/serversites");

    /* Build lock file name */
    /* build entry to add */
    strcpy5( new_entry, new_site, " 0 ", new_email1, " ", new_email2);

    /* Lock the file so no other process can do a simultaneous update */
    index = lock_file_set(file_name);

    /* Open the file entry being added to */
    if ((fi = fopen(file_name, "r")) == NULL)
    {
	    /* Create the file and add the entry */
	    data_log(file_name, new_entry);

	    /* Unlock the file */
	    lock_file_clear(index);
	    return(DBYES);
    }

    /* Construct the temporary file name */
    sprintf(bf.tmp_file, "registry/tmp_files/%s_%s_%ld.reg", user.email, user.mach, tr.upd_time);

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
	    close_down1m(wr_open_err, bf.tmp_file, WR_OPEN_ERR);

    done = DBNO;

    while (done == DBNO)
    {
	    ret = line_get(fi, entry);
	    /* If end of file reached put the new entry in */

	    if (ret <= 0)
	    {
	        fprintf(fo, "%s\n", new_entry);
	        done = DBYES;
	        break;
	    }

	    /* If old entry less than new entry, write out the old entry */
	    if ((ret = strncmp(new_entry, entry, len_userid)) > 0)
	    {
	        fprintf(fo, "%s\n", entry);
	    }

	    /* Otherwise write out new entry followed by old entry */
	    else if (ret < 0)
	    {
	        fprintf(fo, "%s\n", new_entry);
	        fprintf(fo, "%s\n", entry);
	        break;
	    } /* end else */
	    /* At this point ret will equal 0 */
	    /* If entry already exists, replace old entry with new entry */
	    else
	    {
	        fprintf(fo, "%s\n", new_entry);
	        fnd = DUP_ENTRY;
	        break;
	    } /* end else */
    } /* end while */

    /* Write remaining entries out to the temporary file */
    if (done == DBNO)
    {
	    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
	        fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* end if */

    /* Close both files */
    fclose(fi);
    fclose(fo);

    /* Write the temporary file over the old file */
    ret = rename(bf.tmp_file, file_name);

    /* Change mode for external Intercom connections */
    /* Unlock the file by deleting the lock file */
    lock_file_clear(index);

    if (ret == 0) return(fnd);
    else close_down2m(rename_err, bf.tmp_file, file_name, RENAME_ERR);
} /* End add_server_site_entry */

