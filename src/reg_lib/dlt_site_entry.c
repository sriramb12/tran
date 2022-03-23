/****************************************************************************
 *
 * Function:	dlt_site_entry
 *
 * Description:	Deletes entry from the site registration table.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *new_site
 ***************************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "close_down.h"
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "user_params.h"
#include "line.h"
#include "registry_prototypes.h"
#include "rtn.h"

#include "global_debug.h" // Mazieri's debug facilities

int dlt_site_entry(char *new_site)
{
    int index;
    int len = strlen (new_site) + 1;
    FILE *fi;
    FILE *fo;
    char file_name[24];
    register int done = DBNO;
    register int ret = 0;
    char entry[260];
	char new_entry[260];

	DBG("new_site='%s'", new_site);

	sprintf (new_entry, "%s ", new_site);

    /* Build name of registration file */
    reg_name_build(file_name, new_site);

    /* Lock the file so no other process can do a simultaneous update */
    /* Lock registry file by creating lock file */
    index = lock_file_set(file_name);

    /* Open the file entry being deleted from */
    if ((fi = fopen(file_name, "r")) == NULL)
        close_down1m(rd_open_err, file_name, RD_OPEN_ERR);

    /* Construct the temporary file name */
    sprintf(bf.tmp_file, "registry/tmp_files/%s_%s_%ld.reg",
    user.email, user.mach, tr.upd_time);

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    fclose(fi);
	    close_down1m(wr_open_err, bf.tmp_file, WR_OPEN_ERR);
    }

    while (done == DBNO)
    {
	    ret = line_get(fi, entry);

	    /* If end of file reached return failure */
	    if (ret <= 0)
	    {
	        fclose(fi);
	        fclose(fo);
	        unlink(bf.tmp_file);
	        sprintf(rtn.msg, usr_not_fnd, new_site);
	        shut_down(rtn.msg, USR_NOT_FND);
	    }

	    /* If old entry less than new entry, write out the old entry */
	    if ((ret = strncmp(new_entry, entry, len)) > 0)
		    fprintf(fo, "%s\n", entry);
	    /* If reached this point, entry not found */
	    else if (ret < 0)
	    {
	        fclose(fi);
	        fclose(fo);
	        unlink(bf.tmp_file);
	        sprintf(rtn.msg, usr_not_fnd, new_site);
	        shut_down(rtn.msg, USR_NOT_FND);
	    } /* end else */
	    /* At this point ret will equal 0 */
	    /* If the entry already exist, fall out of loop */
	    else
	    {
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

    /* Unlock the file */
    lock_file_clear(index);

    if (ret == 0) return(DBYES);
    else close_down2m(rename_err, bf.tmp_file, file_name, RENAME_ERR);

    return(DBNO);
} /* End dlt_site_entry */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
