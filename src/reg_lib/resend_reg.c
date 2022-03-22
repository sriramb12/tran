/****************************************************************************
 *
 * Function:	process_resend_reg
 *
 * Description:	Once an hour, the server is sent a  request  from  a  client
		through a cron job to resend any registration 
 *		requests it could not send to remote servers 
 *		because of  inaccessibility  problems.  This
		  function  attempts to resend all registration 
 *		requests that have not been sent to remote 
 *		servers.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "user_params.h"
#include "rmt_server.h"
#include "rtn.h"
#include "registry_prototypes.h"
#include "server_lib.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

void resend_reg()
{
    int len;
    register int line_cnt;
    FILE *fp;
    register int cnt;
    short ret;
    char tmp_password[24];
    char tmp_resend_file[FILE_SIZE];

    DBG();

    /* Tell the client the data has been processed */
    put_short(SUCCESS);
    put_short(SUCCESS);

    put_string("Remote registration entries resent.");
    put_string("0 0 0 0");

    close(sock);

    data_log(tr.log_file, "remote registration entries resent.");

    /* Build the password and encrypt it */
    /* Mask Prep uses a different password than Transcend and Mercado */
    strcpy2(tmp_password, user.email, RMT_PWD_KEY);
    tmp_password[12] = '\0';

    /* Encrypt password and put into first 12 characters of buffer */
    password_encrypt(bf.buffer, tmp_password, PWD_LEN);

    /* Build first part of header to send to remote server */
    sprintf(bf.buffer+12, " %c %c %c %c %c l 1.000 l 1.000 %s %s %s ",
    /* Specific request type, general request type, type of data, etc */
    DBRMT_MAN_REG, REGISTER_REQ, GLBL, DBNO,  DBNO_REQ,
    /* No versioning done, but parameters still needed */
    user.email, user.mach, user.addr);

    /* Send registration request to remote servers */
    /* Must calculate after password to avoid NULL's in the password */
    len = strlen(bf.buffer+12)+12;

    /* Go through the resend file for each server */
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	    /* Build the temporary file name */
	    strcpy3(tmp_resend_file, "resend_data/tmp_", rmt_mpd.site[cnt], ".reg");

	    /* Build name for permanant resend file */
	    strcpy3(bf.file_name, "resend_data/", rmt_mpd.site[cnt], ".reg");

	    if ((fp = fopen(tmp_resend_file, "r")) != NULL)
	    {
	        /* Pull in lines until end of file reached */
	        while (1)
	        {
	    	    /* Data put on end of bf.buffer so header can be sent as is */
	    	    /* Each line holds user who registered, site, email, */
	    	    /* badge, two place holders for PIN's */
	    	    /* first name, last name, phone, LOC, mail drop */
	    	    /* Point to the end of the header */
	    	    line_cnt = len;
	    	    while (((bf.buffer[line_cnt] = getc(fp)) != '\n') && (bf.buffer[line_cnt] != EOF))
	    	        ++line_cnt;

	            if (line_cnt == len)
	                break;
	    	
	    	    /* Put NULL at end of string */
	    	    bf.buffer[line_cnt] = '\0';
	    	
	    	    /* Open the socket for the server */
	    	    ret = init_rmt_socket(bf.buffer, &rmt_mpd, cnt);
                if (ret == 0) 
                    ret = get_rmt_short();
	    	
	    	    get_rmt_string(rtn.msg);
	    	    close(sock);
	    	
	    	    data_log(tr.log_file,  rtn.msg);

	    	    if (ret == SUCCESS) dlt_file_line(bf.file_name, bf.buffer+len);
	    	    /* Otherwise add the line */
	    	    else add_file_line(bf.file_name, bf.buffer+len);
	        } /* End while */
	        
	        fclose(fp);

	        /* Get rid of the temporary file no longer needed */
	        unlink(tmp_resend_file);
	    } /* End if for temporary file exists */

	    /* Now resend the data in the original file */
	    if (access(bf.file_name, 0) != 0) continue;

	    /* Move the original file to the temporary file */
	    rename(bf.file_name, tmp_resend_file);

	    if ((fp = fopen(tmp_resend_file, "r")) == NULL)
	    {
	        strcpy3(rtn.msg, rd_open_err, tmp_resend_file, "'");
	        data_log(tr.log_file, rtn.msg);
	        continue;
	    }

	    /* Process all of the entries in the table */
	    /* Pull in lines until end of file reached */
	    while (1)
	    {
	        /* Data put on end of bf.buffer so header can be sent as is */
	        /* Each line holds user who registered, site, email, */
	        /* badge, two place holders for PIN's */
	        /* first name, last name, phone, LOC, mail drop */
	        /* Point to the end of the header */
	        line_cnt = len;
	        while (((bf.buffer[line_cnt] = getc(fp)) != '\n') && (bf.buffer[line_cnt] != EOF)) 
	            ++line_cnt;

	        if (line_cnt == len)
	            break;

	        /* Put NULL at end of string */
	        bf.buffer[line_cnt] = '\0';

	        /* Open the socket for the server */
	        ret = init_rmt_socket(bf.buffer, &rmt_mpd, cnt);

            if (ret == 0) 
                ret = get_rmt_short();

	        get_rmt_string(rtn.msg);
	        close(sock);

	        data_log(tr.log_file,  rtn.msg);
	        if (ret != SUCCESS)
	         add_file_line(bf.file_name, bf.buffer+len);
	    } /* End while */

	    fclose(fp);

	    /* Get rid of the temporary file no longer needed */
	    ret = unlink(tmp_resend_file);
    } /* End big for */
} /* End process_resend_reg */
