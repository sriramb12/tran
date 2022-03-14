/****************************************************************************
 *
 * Function:	add_email_entry
 *
 * Description:
 *  Adds new entry in a global table when a new user, 
 *	is registered to use Transcend.
 *	The name of the global file is registry/local.reg.
 *	This routine is used only by the registration software.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *entry - Name of entry to add
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "close_down.h"
#include "global_server_prototypes.h"
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "user_params.h"
#include "rtn.h"
#include "strcpy.h"
#include "line.h"

#include "global_debug.h" // Mazieri's debug facilities

int add_email_entry(char *entry)
{
    int len;
    FILE *fi;
    FILE *fo;
    register int cnt;
    register int done;
    register int ret;
    char tmp_email_file[FILE_SIZE];

    DBG("entry='%s'", entry);

    /* Build name of Email file */
    if ((fi = fopen("registry/local.reg", "r")) == NULL)
    {
	    /* Create the file and add the entry */
	    data_log("registry/local.reg", entry);

        DBG("Parameter entry='%s' has been registered to use Transcend", entry);
	    return(SUCCESS);
    }

    /* Construct the temporary file name */
    sprintf(tmp_email_file, "registry/tmp_files/%s_%s_%ld.reg", user.email, user.mach, tr.upd_time);

    /* Open the temporary file */
    if ((fo = fopen(tmp_email_file, "w")) == NULL)
    {
	    close_down1m(wr_open_err, tmp_email_file, WR_OPEN_ERR);
    }

    done = DBNO;

	/* Get the length of the Email address */
    len = strfind_char(entry, ' ', 32);

	/* Comparison is done with the Email address */
    /* If an entry with the same Email address is found, the old entry */
    /* is replaced with the new entry */
    while (1)
    {
	    /* Pull in a line of text */
        cnt = line_get(fi, bf.buffer);

	    /* If reached end of file, put in new entry */
	    if ((cnt <= 0) || ( cnt >= 260)) 
	    {
	        fprintf(fo, "%s\n", entry);
	        done = DBYES;
	        break;
	    }

	    /* If buffer entry less than new entry, write out the bf.buffer entry */
	    if ((ret = strncmp(entry, bf.buffer, len)) > 0)
	    {
	        fprintf(fo, "%s\n", bf.buffer);
	    }

	    /* Otherwise write out new entry followed by buffer entry */
	    else if (ret < 0)
	    {
	        fprintf(fo, "%s\n", entry);
	        fprintf(fo, "%s\n", bf.buffer);
	        break;
	    } /* end else */
	    /* At this point ret will equal 0 */
	    /* Replace existing entry with new entry */
	    else
	    {
	        fprintf(fo, "%s\n", entry);
	        break;
	    } /* end else */
    } /* end while */

    /* Write remaining entries out to the temporary file */
    if (done == DBNO)
    {
	    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
        {
            bf.buffer[ret] = 0;
	        fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
        }
    } /* end if */

    /* Close both files */
    fclose(fi);
    fclose(fo);
    
    /* Write the temporary file over the old file */
    ret = rename(tmp_email_file, "registry/local.reg");

    if (ret != SUCCESS)
    {
	    strcpy5(rtn.msg, rename_err, tmp_email_file, "' to '", "registry/local.reg", "'");
	    email_send(admin.err_email, admin.err_email, "Error from jdcd", rtn.msg);
    }

    /* Add log entry indicating that user has been registered to use Transcend */
    DBG("Parameter entry='%s' has been registered to use Transcend", entry);
    log3("Registration entry added for:\n", entry, "\n");

    return(ret);
} /* End add_email_entry */
