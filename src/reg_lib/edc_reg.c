/****************************************************************************
 *
 * Function:	edc_entry_add, edc_entry_lookup
 *
 * Description:
 *  Adds new entry to the site registration table.
 *  Add new entry with key being unique.
 *	when a new user registers to use the DMI database.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include "close_down.h"
#include "customer_table.h"
#include "edc_defines.h"
#include "edc_prototypes.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "reg_globals.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "line.h"
#include "registry_prototypes.h"
#include "dbreg_errs.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "x500_defines.h"

#include "global_debug.h" // Mazieri's debug facilities

int current_status;

int edc_unique_entry_add(char *file_name, char *entry, char *key)
{
    FILE *fi;
    FILE *fo;
    register int done;
    register int ret;
    int len = strlen(key);

    DBG("file_name='%s', entry='%s', key='%s'", file_name, entry, key);

    /* Open the file entry being added to */
    if ((fi = fopen(file_name, "r")) == NULL)
    {
	    /* Create the file and add the entry */
	    data_log(file_name, entry);
	    return(SUCCESS);
    }

    /* Construct the temporary file name */
    strcpy6(bf.tmp_file, file_name, ".", user.login, "_", user.mach, ".tmp_File");

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    /* Close the file already opened */
	    fclose(fi);
	    strcpy7(rtn.msg, wr_open_err, RET2TAB, "'", bf.tmp_file, "'",  DOUBLE_RET, HELP_MSG);
	    return(WR_OPEN_ERR);
    }

    done = DBNO;

    while (done == DBNO)
    {
	    ret = line_get(fi, bf.buffer);

	    /* If end of file reached put the new entry in */
    	if (ret == EOF)
    	{
    	    fprintf(fo, "%s\n", entry);
    	    done = DBYES;
    	    break;
    	}

    	/* If buffer entry less than new entry, write out the buffer entry */
    	if ((ret = strncasecmp(key, bf.buffer, len)) > 0)
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
    	/* Otherwise replace old entry with new entry */
    	/* At this point ret will equal 0 */
    	else
    	{
    	    fprintf(fo, "%s\n", entry);
    	    break;
    	} /* end else */

    } /* end while */

    /* Write remaining entries out to the temporary file */
    if (done == DBNO)
    {
	    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BLOCK_SIZE, fi)) != 0)
	        fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* end if */

    /* Close both files */
    fclose(fi);
    fclose(fo);

    /* Write the temporary file over the old file */
    if ( rename(bf.tmp_file, file_name) != SUCCESS)
        close_down2m(rename_err, bf.tmp_file, file_name, 3);

    return(SUCCESS);
} /* end edc_unique_entry_add */


int is_gui(void)
{
    DBG();

    if ((strcmp(gu.machine_type, "Windows GUI") == 0) || (strcmp(gu.machine_type, "Linux GUI") == 0) || (strcmp(gu.machine_type, "TransGUI") == 0))
        return(DBYES);
    else 
        return(DBNO);
} /* end is_gui */


int is_windows(void)
{
    DBG();

    if ((strncmp(gu.machine_type, "Windows ", 7) == 0) || (strcmp(gu.machine_type, "TransGUI") == 0))
        return(DBYES);
    else 
        return(DBNO);
} /* end is_gui */



#ifdef EXTERNAL
void time_string_build(char *time_string, unsigned long expirationtime)
{
    DBG("time_string='%s' and expirationtime='%lu'", time_string, expirationtime);

    char *time_ptr;
    time_ptr = ctime(&expirationtime);

    strncpy(time_string, time_ptr+4, 7);
    strncpy(time_string+7, time_ptr+20, 4);

    time_string[11] = '\0';
}  /* end time_string_build */

int is_trusted_sender(char *user)
{
    FILE *fp;
    int trusted = DBNO;
    char line[260];
    char trusted_sender[32];
    int status;
    time_t expirationtime;
    char *ptr;

    DBG("user='%s' TRUSTED_SENDER_TABLE=[%s]", user, TRUSTED_SENDER_TABLE);

    data_log("trusted", user);
    data_log("trusted", TRUSTED_SENDER_TABLE);

    if ((fp = fopen(TRUSTED_SENDER_TABLE, "r")) == NULL)
        return(DBNO);

    while (line_get(fp, line) != EOF)
    {
	    sscanf(line, "%[^;]%*c%d%*c%lu", trusted_sender, &status, &expirationtime);
	    if (strcasecmp(user, trusted_sender) == 0)
	    {
            data_log("trusted", "found");

	        /* If customer approved and approval not expired, trust sender */
	        if ((status == APPROVED) && (expirationtime > tr.upd_time))
	            trusted = DBYES;

	        break;
	    } /* end if */
    } /* end while */

    fclose(fp);
    return(trusted);
} /* end is_trusted_sender */


/* Trust this user for external transfers */
void edc_trust_sender(char *email, char *req_type, char *expirationtime_string)
{
    char entry[260];
    char time_string[30];
    time_t expirationtime;

    DBG("email='%s', req_type='%s' and expirationtime_string='%s'", email, req_type, expirationtime_string);

    expirationtime = atol(expirationtime_string);

    /* Build English representation of time string */
	time_string_build(time_string, expirationtime);

    /* Verify time stamp not lessthan current time */
    if (expirationtime <= tr.upd_time)
    {
	    sprintf(rtn.msg, timestamp_err, tr.upd_time, expirationtime);
	    shut_down(rtn.msg, TIMESTAMP_ERR);
    }

    /* Build entry to add to the trusted sender table */
    /* Put the entry in lower case */
    str_lower(email);
    strcpy3(entry, email, ";3;", expirationtime_string);
    edc_unique_entry_add(TRUSTED_SENDER_TABLE, entry, email);

    put_short(SUCCESS);
    strcpy5(rtn.msg, "Trusted sender ", email, " approved until ", time_string, ".");

    return_success(rtn.msg, DBNO);
} /* end edc_trust_sender */


int edc_trusted_entry_revoke(char *entry, char *email)
{
    FILE *fi;
    FILE *fo;
    char file_name[64] = TRUSTED_SENDER_TABLE;
    register int done;
    register int ret;
    int len = strlen(email);

    DBG("entry='%s' and email='%s'", entry, email);

    /* Open the file entry being added to */
    if ((fi = fopen(file_name, "r")) == NULL)
    {
	    /* Create the file and add the entry */
	    data_log(file_name, entry);
	    return(SUCCESS);
    }

    /* Construct the temporary file name */
    strcpy6(bf.tmp_file, file_name, ".", user.login, "_", user.mach, ".tmp_File");

    /* Open the temporary file */
    if ((fo = fopen(bf.tmp_file, "w")) == NULL)
    {
	    /* Close the file already opened */
	    fclose(fi);
	    strcpy7(rtn.msg, wr_open_err, RET2TAB, "'", bf.tmp_file, "'",  DOUBLE_RET, HELP_MSG);
	    return(WR_OPEN_ERR);
    }

    done = DBNO;

    while (done == DBNO)
    {
	    ret = line_get(fi, bf.buffer);

	    /* If end of file reached put the new entry in */
	    if (ret == EOF)
	    {
	        fprintf(fo, "%s\n", entry);
	        done = DBYES;
	        break;
	    }
	    /* If buffer entry less than new entry, write out the buffer entry */
	    if ((ret = strncasecmp(email, bf.buffer, len)) > 0)
	    {
	        fprintf(fo, "%s\n", bf.buffer);
	    }
	    /* Otherwise write out new entry followed by buffer entry */
	    else if (ret < 0)
	    {
	        fprintf(fo, "%s\n", entry);
	        fprintf(fo, "%s\n", bf.buffer);
	    } /* end else */
	    /* Otherwise replace old entry with new entry */
	    /* At this point ret will equal 0 */
	    else
	    {
	        fprintf(fo, "%s\n", entry);
	        break;
	    } /* end else */

    } /* end while */

    /* Write remaining entries out to the temporary file */
    if (done == DBNO)
    {
	    while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BLOCK_SIZE, fi)) != 0)
	        fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);
    } /* end if */

    /* Close both files */
    fclose(fi);
    fclose(fo);

    /* Write the temporary file over the old file */
    if ( rename(bf.tmp_file, file_name) != SUCCESS)
        close_down2m(rename_err, bf.tmp_file, file_name, 3);

    return(SUCCESS);
} /* end edc_trusted_entry_revoke */


void edc_trusted_sender_revoke(char *email, char *req_type, char *expirationtime_string)
{
    char entry[64];

    DBG("email='%s', req_type='%s' and expirationtime_string='%s'", email, req_type, expirationtime_string);

    /* Build entry to revoke */
    strcpy2(entry, email, ";1;0");

    /* Change status for user */
    edc_trusted_entry_revoke(entry, email);

    /* Tell user transaction revoked */
    strcpy3(rtn.msg, "Trusted sender ", email, " revoked.");

    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
}
#endif
