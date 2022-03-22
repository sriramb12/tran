/****************************************************************************
 *
 * Function:	query_user
 *
 * Description:	Send info back about specified user
 *		Has same format as old user command.
 *		Returns name, Email address, phone, department, 
 *		mail drop, location code, and sites.
 *
 ***************************************************************************/

/* This function is only implemented for NXP */
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "close_down.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "server_lib.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_prototypes.h"
#include "trans_server_globals.h"
#include "trans_lib_prototypes.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "line.h"
#include "strcpy.h"
#include "ldapfsl.h"
#include "global_debug.h"


/* Get user info and return in rtn.msg */
static int get_user_info(char *user_id)
{
     int ret = DBNO;
     DBG("user_id='%s'", user_id);
     LDAP_RETURN_DATA * ldap_ret = ldap_ret = ldapfsl_lookupByCoreId(user_id);
     if (GOOD_LDAP_RETURN_DATA(ldap_ret))
     {
          /* Put formatted message in rtn_msg */
          rtn.len = strcpy6(rtn.msg, "Name: ", ldap_ret->info[0]->name,
                                     "\nfriendly email: ", ldap_ret->info[0]->friendly_email,
                                     "\nSite: ", ldap_ret->info[0]->site);
          /* Indicate entry found */        
          ret = DBYES;
     }
     ldapfsl_free_return_data(ldap_ret);
     return ret;
} /* End get_user_info */



void query_user(int req_type, char *user_id)
{
    char fnd = DBNO;
    short ret;
    int len;
    int fd;

    DBG("req_type='%d' and user_id='%s'", req_type, user_id);

    /* Build temporary file name */
    sprintf(bf.file_name, "/tmp/%s_%d.icom.users", user.email, tr.pid);

    /* Open the temporary file */
    if ((fd = open(bf.file_name, O_RDWR | O_CREAT, 0600)) == EOF)
        close_down1m(wr_open_err, bf.file_name, WR_OPEN_ERR);

    /* Put user ID in lower case */
    str_lower(user_id);

    /* Retrieve info using whobu */
    if (get_user_info(user_id) == DBYES)
    {
	    /* Look for data in site table */
        if (req_type != DBFAST_QUERY_USER)
        {
            /* This function puts data found in rtn.msg */
	        sql_dt_registry_sites_lookup(user_id);
        }
	    /* Write data to the file */
	    write(fd, rtn.msg, rtn.len);
	    fnd = DBYES;
	    close(fd);
    } /* End if for able to execute whobu command */

     /* If no entries found, tell user */
    if (fnd == DBNO)
    {
	    unlink(bf.file_name);
	    strcpy3(rtn.msg, "Registration entry not found for '", user_id, "'");
	    shut_down(rtn.msg, FAILURE);
    }

     /* Open the output file again */
    if ((fd = open(bf.file_name, O_RDONLY,  0600)) == EOF)
        close_down1m(wr_open_err, bf.file_name, WR_OPEN_ERR);

    /* Indicate that file wil be sent */
    put_short(SUCCESS);

    /* Send name of file */
#ifdef INTERCOM
    put_string("icom.users");
#else
    put_string("trans.users");
#endif

    /* Get acknowledgement */
    if ((ret = get_short()) != 0)
        shut_down(bad_ack, BAD_ACK);

    /* Grab the first 1000 characters for the return message */
    len = read(fd, rtn.msg, 1000);
    put_binary_string(rtn.msg, len);

    /* Read additional text from the file and write them to the socket */
    while ((ret = read(fd, bf.buffer, 1024)) > 0) 
        put_binary_string(bf.buffer, ret);

    /* Tell client all data for this file sent */
    put_short(SUCCESS);

    /* Close the file just sent */
    close(fd);

    /* Send the return code */
#ifdef INTERCOM
    strcpy1(rtn.msg+rtn.len, "\nInterCom user info saved in file 'icom.users'");
#endif
#ifdef TRANSCEND
    strcpy1(rtn.msg+rtn.len, "\nTranscend user info saved in file 'trans.users'");
#endif

    put_success_msg(NONE);
    
    /* Delete temporary file no longer needed */
    unlink(bf.file_name);
} /* End query_user */


void trans_site_read(char *user_id)
{
    char line[2096];
    int site_cnt;
    FILE *po;
    char command[80];
    char par1[32], par2[32];

    DBG("user_id='%s'", user_id);

    /* Find sites where user is registered */
    /* Build command to find the sites */
    strcpy3(command,  "egrep -ih ", user_id, " registry/site*");

    if ((po = popen(command, "r")) != NULL)
    {
	    site_cnt = 0;
	    while (long_line_get(po, line) != EOF)
        {
            sscanf(line, "%[^;]%*c%[^;]", par1, par2);
	
	        /* If all sites specified, change value */
	        if (strcmp(par2, NONE) == 0) strcpy(par2, "All sites");
    	        if (site_cnt <= 0)
    	        {
    	            ++site_cnt;
    	            rtn.len += strcpy5(rtn.msg +rtn.len, "\nSite:   ", par1, " - ",  par2, "\n");
    	        } /* End if */
    	    else 
	        {
	            rtn.len += strcpy5(rtn.msg+rtn.len, "        ", par1, " - ",  par2, "\n");
	        } /* end else */
        } /* End while for reading in sites */
       
        pclose(po);
    } /* End if for opening stream */

    if (site_cnt <= 0)
        rtn.len += strcpy1(rtn.msg+rtn.len, "\nSite:   Not found in the server registry\n");
} /* End trans_site_read */
